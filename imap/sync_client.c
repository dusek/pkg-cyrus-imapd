/* sync_client.c -- Cyrus synchonization client
 *
 * Copyright (c) 1994-2008 Carnegie Mellon University.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The name "Carnegie Mellon University" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For permission or any legal
 *    details, please contact
 *      Carnegie Mellon University
 *      Center for Technology Transfer and Enterprise Creation
 *      4615 Forbes Avenue
 *      Suite 302
 *      Pittsburgh, PA  15213
 *      (412) 268-7393, fax: (412) 268-7395
 *      innovation@andrew.cmu.edu
 *
 * 4. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by Computing Services
 *     at Carnegie Mellon University (http://www.cmu.edu/computing/)."
 *
 * CARNEGIE MELLON UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE
 * FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Original version written by David Carter <dpc22@cam.ac.uk>
 * Rewritten and integrated into Cyrus by Ken Murchison <ken@oceana.com>
 */

#include <config.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <syslog.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

#include <netinet/tcp.h>

#include "global.h"
#include "append.h"
#include "mboxlist.h"
#include "exitcodes.h"
#include "imap/imap_err.h"
#include "mailbox.h"
#include "quota.h"
#include "xmalloc.h"
#include "seen.h"
#include "mboxname.h"
#include "map.h"
#include "imapd.h"
#include "util.h"
#include "prot.h"
#include "message_guid.h"
#include "sync_log.h"
#include "sync_support.h"
#include "cyr_lock.h"
#include "backend.h"
#include "xstrlcat.h"
#include "signals.h"
#include "cyrusdb.h"

/* ====================================================================== */

/* Static global variables and support routines for sync_client */

extern char *optarg;
extern int optind;

static const char *servername = NULL;
static struct backend *sync_backend = NULL;
static struct protstream *sync_out = NULL;
static struct protstream *sync_in = NULL;

static struct namespace   sync_namespace;

static int verbose         = 0;
static int verbose_logging = 0;
static int connect_once    = 0;
static int background      = 0;
static int do_compress     = 0;
static int no_copyback     = 0;

static char *prev_userid;

static struct protocol_t csync_protocol =
{ "csync", "csync", TYPE_STD,
  { { { 1, "* OK" },
      { NULL, NULL, "* OK", NULL,
	CAPAF_ONE_PER_LINE|CAPAF_SKIP_FIRST_WORD,
	{ { "SASL", CAPA_AUTH },
	  { "STARTTLS", CAPA_STARTTLS },
	  { "COMPRESS=DEFLATE", CAPA_COMPRESS },
	  { NULL, 0 } } },
      { "STARTTLS", "OK", "NO", 1 },
      { "AUTHENTICATE", USHRT_MAX, 0, "OK", "NO", "+ ", "*", NULL, 0 },
      { NULL, NULL, NULL },
      { "NOOP", NULL, "OK" },
      { "EXIT", NULL, "OK" } } }
};

static int do_meta(char *user);

static void shut_down(int code) __attribute__((noreturn));
static void shut_down(int code)
{
    in_shutdown = 1;

    seen_done();
    annotatemore_close();
    annotate_done();
    quotadb_close();
    quotadb_done();
    mboxlist_close();
    mboxlist_done();
    cyrus_done();
    exit(code);
}

static int usage(const char *name)
{
    fprintf(stderr,
	    "usage: %s -S <servername> [-C <alt_config>] [-r] [-v] mailbox...\n", name);

    exit(EC_USAGE);
}

EXPORTED void fatal(const char *s, int code)
{
    fprintf(stderr, "Fatal error: %s\n", s);
    syslog(LOG_ERR, "Fatal error: %s", s);
    exit(code);
}

/* ====================================================================== */

/* Routines relevant to reserve operation */

/* Find the messages that we will want to upload from this mailbox,
 * flag messages that are already available at the server end */

static int find_reserve_messages(struct mailbox *mailbox,
				 unsigned last_uid,
				 struct sync_msgid_list *part_list)
{
    struct index_record record;
    uint32_t recno;
    int r;

    for (recno = 1; recno <= mailbox->i.num_records; recno++) {
	r = mailbox_read_index_record(mailbox, recno, &record);

	if (r) {
	    syslog(LOG_ERR,
		   "IOERROR: reading index entry for recno %u of %s: %m",
		   recno, mailbox->name);
	    return IMAP_IOERROR;
	}

	if (record.system_flags & FLAG_UNLINKED)
	    continue;

	/* skip over records already on replica */
	if (record.uid <= last_uid)
	    continue;

	sync_msgid_insert(part_list, &record.guid);
    }

    return(0);
}

static int find_reserve_all(struct sync_name_list *mboxname_list,
			    struct sync_folder_list *master_folders,
			    struct sync_folder_list *replica_folders,
			    struct sync_reserve_list *reserve_guids)
{
    struct sync_name *mbox;
    struct sync_folder *rfolder;
    struct sync_msgid_list *part_list;
    struct mailbox *mailbox = NULL;
    int r = 0;

    /* Find messages we want to upload that are available on server */
    for (mbox = mboxname_list->head; mbox; mbox = mbox->next) {
	/* XXX - now this kinda sucks - we use a write lock here
	 * purely for conversations modseq - but we never actually
	 * USE the value... the whole "add to master folders" actually
	 * looks a bit pointless... */
	r = mailbox_open_iwl(mbox->name, &mailbox);
	if (!r) r = sync_mailbox_version_check(&mailbox);

	/* Quietly skip over folders which have been deleted since we
	   started working (but record fact in case caller cares) */
	if (r == IMAP_MAILBOX_NONEXISTENT) {
	    r = 0;
	    continue;
	}

	if (r) {
	    syslog(LOG_ERR, "IOERROR: Failed to open %s: %s",
		   mbox->name, error_message(r));
	    goto bail;
	}

	/* mailbox is open from here, no exiting without closing it! */

	part_list = sync_reserve_partlist(reserve_guids, mailbox->part);

	sync_folder_list_add(master_folders, mailbox->uniqueid, mailbox->name,
			     mailbox->mbtype,
			     mailbox->part, mailbox->acl, mailbox->i.options,
			     mailbox->i.uidvalidity, mailbox->i.last_uid,
			     mailbox->i.highestmodseq, mailbox->i.synccrcs,
			     mailbox->i.recentuid, mailbox->i.recenttime,
			     mailbox->i.pop3_last_login,
			     mailbox->i.pop3_show_after, NULL);

	rfolder = sync_folder_lookup(replica_folders, mailbox->uniqueid);
	if (rfolder)
	    find_reserve_messages(mailbox, rfolder->last_uid, part_list);
	else
	    find_reserve_messages(mailbox, 0, part_list);

	mailbox_close(&mailbox);
    }

bail:
    mailbox_close(&mailbox);
    return r;
}

static int mark_missing (struct dlist *kin,
			 struct sync_msgid_list *part_list)
{
    struct dlist *kl = kin->head;
    struct dlist *ki;
    struct message_guid tmp_guid;
    struct sync_msgid *msgid;

    /* no missing at all, good */
    if (!kl) return 0;

    if (strcmp(kl->name, "MISSING")) {
	syslog(LOG_ERR, "SYNCERROR: Illegal response to RESERVE: %s",
	       kl->name);
	return IMAP_PROTOCOL_BAD_PARAMETERS;
    }

    /* unmark each missing item */
    for (ki = kl->head; ki; ki = ki->next) {
	if (!message_guid_decode(&tmp_guid, ki->sval)) {
	    syslog(LOG_ERR, "SYNCERROR: reserve: failed to parse GUID %s",
		   ki->sval);
	    return IMAP_PROTOCOL_BAD_PARAMETERS;
	}

	/* afraid we will need this after all */
	msgid = sync_msgid_lookup(part_list, &tmp_guid);
	if (msgid && !msgid->need_upload) {
	    msgid->need_upload = 1;
	    part_list->toupload++;
	}
    }

    return 0;
}

static int reserve_partition(char *partition,
			     struct sync_folder_list *replica_folders,
			     struct sync_msgid_list *part_list)
{
    const char *cmd = "RESERVE";
    struct sync_msgid *msgid = part_list->head;
    struct sync_folder *folder;
    struct dlist *kl = NULL;
    struct dlist *kin = NULL;
    struct dlist *ki;
    int r = 0;

    if (!replica_folders->head)
	return 0; /* nowhere to reserve */

    while (msgid) {
	int n = 0;

	if (!part_list->toupload)
	    goto done; /* got them all */

	kl = dlist_newkvlist(NULL, cmd);
	dlist_setatom(kl, "PARTITION", partition);

	ki = dlist_newlist(kl, "MBOXNAME");
	for (folder = replica_folders->head; folder; folder = folder->next)
	    dlist_setatom(ki, "MBOXNAME", folder->name);

	ki = dlist_newlist(kl, "GUID");
	for (; msgid; msgid = msgid->next) {
	    if (!msgid->need_upload) continue;
	    if (n > 8192) break;
	    dlist_setatom(ki, "GUID", message_guid_encode(&msgid->guid));
	    /* we will re-add the "need upload" if we get a MISSING response */
	    msgid->need_upload = 0;
	    part_list->toupload--;
	    n++;
	}

	sync_send_apply(kl, sync_out);

	r = sync_parse_response(cmd, sync_in, &kin);
	if (r) goto done;

	r = mark_missing(kin, part_list);
	if (r) goto done;

	dlist_free(&kl);
	dlist_free(&kin);
    }

done:
    dlist_free(&kl);
    dlist_free(&kin);
    return r;
}

static int reserve_messages(struct sync_name_list *mboxname_list,
			    struct sync_folder_list *master_folders,
			    struct sync_folder_list *replica_folders,
			    struct sync_reserve_list *reserve_guids)
{
    struct sync_reserve *reserve;
    int r;

    r = find_reserve_all(mboxname_list, master_folders,
			 replica_folders, reserve_guids);
    if (r) return r;

    for (reserve = reserve_guids->head; reserve; reserve = reserve->next) {
	r = reserve_partition(reserve->part, replica_folders, reserve->list);
	if (r) return r;
    }

    return 0;
}

/* ====================================================================== */

static int response_parse(const char *cmd,
			  struct sync_folder_list *folder_list,
			  struct sync_name_list *sub_list,
			  struct sync_sieve_list *sieve_list,
			  struct sync_seen_list *seen_list,
			  struct sync_quota_list *quota_list)
{
    struct dlist *kin = NULL;
    struct dlist *kl;
    int r;

    r = sync_parse_response(cmd, sync_in, &kin);

    /* Unpleasant: translate remote access error into "please reset me" */
    if (r == IMAP_MAILBOX_NONEXISTENT)
	return 0;

    if (r) return r;

    for (kl = kin->head; kl; kl = kl->next) {
	if (!strcmp(kl->name, "SIEVE")) {
	    struct message_guid guid;
	    const char *filename = NULL;
	    const char *guidstr = NULL;
	    time_t modtime = 0;
	    uint32_t active = 0;
	    if (!sieve_list) goto parse_err;
	    if (!dlist_getatom(kl, "FILENAME", &filename)) goto parse_err;
	    if (!dlist_getdate(kl, "LAST_UPDATE", &modtime)) goto parse_err;
	    dlist_getatom(kl, "GUID", &guidstr); /* optional */
	    if (guidstr) {
		if (!message_guid_decode(&guid, guidstr)) goto parse_err;
	    }
	    else {
		message_guid_set_null(&guid);
	    }
	    dlist_getnum32(kl, "ISACTIVE", &active); /* optional */
	    sync_sieve_list_add(sieve_list, filename, modtime, &guid, active);
	}

	else if (!strcmp(kl->name, "QUOTA")) {
	    const char *root = NULL;
	    struct sync_quota *sq;
	    if (!quota_list) goto parse_err;
	    if (!dlist_getatom(kl, "ROOT", &root)) goto parse_err;
	    sq = sync_quota_list_add(quota_list, root);
	    sync_decode_quota_limits(kl, sq->limits);
	}

	else if (!strcmp(kl->name, "LSUB")) {
	    struct dlist *i;
	    if (!sub_list) goto parse_err;
	    for (i = kl->head; i; i = i->next) {
		sync_name_list_add(sub_list, i->sval);
	    }
	}

	else if (!strcmp(kl->name, "SEEN")) {
	    const char *uniqueid = NULL;
	    time_t lastread = 0;
	    uint32_t lastuid = 0;
	    time_t lastchange = 0;
	    const char *seenuids = NULL;
	    if (!seen_list) goto parse_err;
	    if (!dlist_getatom(kl, "UNIQUEID", &uniqueid)) goto parse_err;
	    if (!dlist_getdate(kl, "LASTREAD", &lastread)) goto parse_err;
	    if (!dlist_getnum32(kl, "LASTUID", &lastuid)) goto parse_err;
	    if (!dlist_getdate(kl, "LASTCHANGE", &lastchange)) goto parse_err;
	    if (!dlist_getatom(kl, "SEENUIDS", &seenuids)) goto parse_err;
	    sync_seen_list_add(seen_list, uniqueid, lastread,
			       lastuid, lastchange, seenuids);
	}

	else if (!strcmp(kl->name, "MAILBOX")) {
	    const char *uniqueid = NULL;
	    const char *mboxname = NULL;
	    const char *mboxtype = NULL;
	    const char *part = NULL;
	    const char *acl = NULL;
	    const char *options = NULL;
	    modseq_t highestmodseq = 0;
	    uint32_t uidvalidity = 0;
	    uint32_t last_uid = 0;
	    struct synccrcs synccrcs = { 0, 0 };
	    uint32_t recentuid = 0;
	    time_t recenttime = 0;
	    time_t pop3_last_login = 0;
	    time_t pop3_show_after = 0;
	    struct dlist *al = NULL;
	    struct sync_annot_list *annots = NULL;
	    if (!folder_list) goto parse_err;
	    if (!dlist_getatom(kl, "UNIQUEID", &uniqueid)) goto parse_err;
	    if (!dlist_getatom(kl, "MBOXNAME", &mboxname)) goto parse_err;
	    if (!dlist_getatom(kl, "PARTITION", &part)) goto parse_err;
	    if (!dlist_getatom(kl, "ACL", &acl)) goto parse_err;
	    if (!dlist_getatom(kl, "OPTIONS", &options)) goto parse_err;
	    if (!dlist_getnum64(kl, "HIGHESTMODSEQ", &highestmodseq)) goto parse_err;
	    if (!dlist_getnum32(kl, "UIDVALIDITY", &uidvalidity)) goto parse_err;
	    if (!dlist_getnum32(kl, "LAST_UID", &last_uid)) goto parse_err;
	    if (!dlist_getnum32(kl, "RECENTUID", &recentuid)) goto parse_err;
	    if (!dlist_getdate(kl, "RECENTTIME", &recenttime)) goto parse_err;
	    if (!dlist_getdate(kl, "POP3_LAST_LOGIN", &pop3_last_login)) goto parse_err;
	    /* optional */
	    dlist_getdate(kl, "POP3_SHOW_AFTER", &pop3_show_after);
	    dlist_getatom(kl, "MBOXTYPE", &mboxtype);

	    dlist_getnum32(kl, "SYNC_CRC", &synccrcs.basic);
	    dlist_getnum32(kl, "SYNC_CRC_ANNOT", &synccrcs.annot);

	    if (dlist_getlist(kl, "ANNOTATIONS", &al))
		decode_annotations(al, &annots, NULL);

	    sync_folder_list_add(folder_list, uniqueid, mboxname,
				 mboxlist_string_to_mbtype(mboxtype),
				 part, acl,
				 sync_parse_options(options),
				 uidvalidity, last_uid,
				 highestmodseq, synccrcs,
				 recentuid, recenttime,
				 pop3_last_login,
				 pop3_show_after, annots);
	}
	else
	    goto parse_err;
    }
    dlist_free(&kin);

    return r;

 parse_err:
    dlist_free(&kin);
    syslog(LOG_ERR, "SYNCERROR: %s: Invalid response %s",
	   cmd, dlist_lastkey());
    return IMAP_PROTOCOL_BAD_PARAMETERS;
}

static int do_unuser(const char *userid)
{
    const char *cmd = "UNUSER";
    struct mailbox *mailbox = NULL;
    char buf[MAX_MAILBOX_BUFFER];
    struct dlist *kl;
    int r;

    /* nothing to do if there's no userid */
    if (!userid || !userid[0]) {
	syslog(LOG_WARNING, "ignoring attempt to %s() without userid", __func__);
	return 0;
    }

    /* check local mailbox first */
    (sync_namespace.mboxname_tointernal)(&sync_namespace, "INBOX",
					  userid, buf);
    r = mailbox_open_irl(buf, &mailbox);

    /* only remove from server if there's no local mailbox */
    if (r == IMAP_MAILBOX_NONEXISTENT) {
	kl = dlist_setatom(NULL, cmd, userid);
	sync_send_apply(kl, sync_out);
	dlist_free(&kl);

	r = sync_parse_response(cmd, sync_in, NULL);
	if (r == IMAP_MAILBOX_NONEXISTENT) r = 0;
    }

    mailbox_close(&mailbox);

    return r;
}

static int folder_rename(const char *oldname, const char *newname,
			 const char *partition, unsigned uidvalidity)
{
    const char *cmd = "RENAME";
    struct dlist *kl;

    kl = dlist_newkvlist(NULL, cmd);
    dlist_setatom(kl, "OLDMBOXNAME", oldname);
    dlist_setatom(kl, "NEWMBOXNAME", newname);
    dlist_setatom(kl, "PARTITION", partition);
    dlist_setnum32(kl, "UIDVALIDITY", uidvalidity);

    sync_send_apply(kl, sync_out);
    dlist_free(&kl);

    return sync_parse_response(cmd, sync_in, NULL);
}

static int folder_delete(const char *mboxname)
{
    const char *cmd = "UNMAILBOX";
    struct dlist *kl;
    int r;

    kl = dlist_setatom(NULL, cmd, mboxname);
    sync_send_apply(kl, sync_out);
    dlist_free(&kl);

    r = sync_parse_response(cmd, sync_in, NULL);
    if (r == IMAP_MAILBOX_NONEXISTENT)
	r = 0;

    return r;
}

static int set_sub(const char *userid, const char *mboxname, int add)
{
    const char *cmd = add ? "SUB" : "UNSUB";
    struct dlist *kl;

    if (verbose)
	printf("%s %s %s\n", cmd, userid, mboxname);

    if (verbose_logging)
	syslog(LOG_INFO, "%s %s %s", cmd, userid, mboxname);

    kl = dlist_newkvlist(NULL, cmd);
    dlist_setatom(kl, "USERID", userid);
    dlist_setatom(kl, "MBOXNAME", mboxname);
    sync_send_apply(kl, sync_out);
    dlist_free(&kl);

    return sync_parse_response(cmd, sync_in, NULL);
}

static int folder_setannotation(const char *mboxname, const char *entry,
				const char *userid, const struct buf *value)
{
    const char *cmd = "ANNOTATION";
    struct dlist *kl;

    kl = dlist_newkvlist(NULL, cmd);
    dlist_setatom(kl, "MBOXNAME", mboxname);
    dlist_setatom(kl, "ENTRY", entry);
    dlist_setatom(kl, "USERID", userid);
    dlist_setmap(kl, "VALUE", value->s, value->len);
    sync_send_apply(kl, sync_out);
    dlist_free(&kl);

    return sync_parse_response(cmd, sync_in, NULL);
}

static int folder_unannotation(const char *mboxname, const char *entry,
			       const char *userid)
{
    const char *cmd = "UNANNOTATION";
    struct dlist *kl;

    kl = dlist_newkvlist(NULL, cmd);
    dlist_setatom(kl, "MBOXNAME", mboxname);
    dlist_setatom(kl, "ENTRY", entry);
    dlist_setatom(kl, "USERID", userid);
    sync_send_apply(kl, sync_out);
    dlist_free(&kl);

    return sync_parse_response(cmd, sync_in, NULL);
}

/* ====================================================================== */

static int sieve_upload(const char *userid, const char *filename,
			unsigned long last_update)
{
    const char *cmd = "SIEVE";
    struct dlist *kl;
    char *sieve;
    uint32_t size;

    sieve = sync_sieve_read(userid, filename, &size);
    if (!sieve) return IMAP_IOERROR;

    kl = dlist_newkvlist(NULL, cmd);
    dlist_setatom(kl, "USERID", userid);
    dlist_setatom(kl, "FILENAME", filename);
    dlist_setdate(kl, "LAST_UPDATE", last_update);
    dlist_setmap(kl, "CONTENT", sieve, size);
    sync_send_apply(kl, sync_out);
    dlist_free(&kl);
    free(sieve);

    return sync_parse_response(cmd, sync_in, NULL);
}

static int sieve_delete(const char *userid, const char *filename)
{
    const char *cmd = "UNSIEVE";
    struct dlist *kl;

    kl = dlist_newkvlist(NULL, cmd);
    dlist_setatom(kl, "USERID", userid);
    dlist_setatom(kl, "FILENAME", filename);
    sync_send_apply(kl, sync_out);
    dlist_free(&kl);

    return sync_parse_response(cmd, sync_in, NULL);
}

static int sieve_activate(const char *userid, const char *filename)
{
    const char *cmd = "ACTIVATE_SIEVE";
    struct dlist *kl;

    kl = dlist_newkvlist(NULL, cmd);
    dlist_setatom(kl, "USERID", userid);
    dlist_setatom(kl, "FILENAME", filename);
    sync_send_apply(kl, sync_out);
    dlist_free(&kl);

    return sync_parse_response(cmd, sync_in, NULL);
}

static int sieve_deactivate(const char *userid)
{
    const char *cmd = "UNACTIVATE_SIEVE";
    struct dlist *kl;

    kl = dlist_newkvlist(NULL, cmd);
    dlist_setatom(kl, "USERID", userid);
    sync_send_apply(kl, sync_out);
    dlist_free(&kl);

    return sync_parse_response(cmd, sync_in, NULL);
}

/* ====================================================================== */

static int delete_quota(const char *root)
{
    const char *cmd = "UNQUOTA";
    struct dlist *kl;

    kl = dlist_setatom(NULL, cmd, root);
    sync_send_apply(kl, sync_out);
    dlist_free(&kl);

    return sync_parse_response(cmd, sync_in, NULL);
}


static int update_quota_work(struct quota *client,
			     struct sync_quota *server)
{
    const char *cmd = "QUOTA";
    struct dlist *kl;
    int r;

    r = quota_read(client, NULL, 0);

    /* disappeared?  Delete it*/
    if (r == IMAP_QUOTAROOT_NONEXISTENT)
	return delete_quota(client->root);

    if (r) {
	syslog(LOG_INFO, "Warning: failed to read quotaroot %s: %s",
	       client->root, error_message(r));
	return r;
    }

    if (server) {
	int changed = 0;
	int res;
	for (res = 0 ; res < QUOTA_NUMRESOURCES ; res++) {
	    if (client->limits[res] != server->limits[res])
		changed++;
	}
	if (!changed)
	    return 0;
    }

    kl = dlist_newkvlist(NULL, cmd);
    dlist_setatom(kl, "ROOT", client->root);
    sync_encode_quota_limits(kl, client->limits);
    sync_send_apply(kl, sync_out);
    dlist_free(&kl);

    return sync_parse_response(cmd, sync_in, NULL);
}

static int user_sub(const char *userid, const char *mboxname)
{
    int r;

    r = mboxlist_checksub(mboxname, userid);

    switch (r) {
    case CYRUSDB_OK:
	return set_sub(userid, mboxname, 1);
    case CYRUSDB_NOTFOUND:
	return set_sub(userid, mboxname, 0);
    default:
	return r;
    }
}

static int copy_local(struct mailbox *mailbox, unsigned long uid)
{
    uint32_t recno;
    struct index_record oldrecord;
    int r;
    annotate_state_t *astate = NULL;

    for (recno = 1; recno <= mailbox->i.num_records; recno++) {
	r = mailbox_read_index_record(mailbox, recno, &oldrecord);
	if (r) return r;

	/* found the record, renumber it */
	if (oldrecord.uid == uid) {
	    char *oldfname, *newfname;
	    struct index_record newrecord;

	    /* create the new record as a clone of the old record */
	    newrecord = oldrecord;
	    newrecord.uid = mailbox->i.last_uid + 1;

	    /* copy the file in to place */
	    oldfname = xstrdup(mailbox_message_fname(mailbox, oldrecord.uid));
	    newfname = xstrdup(mailbox_message_fname(mailbox, newrecord.uid));
	    r = mailbox_copyfile(oldfname, newfname, 0);
	    free(oldfname);
	    free(newfname);
	    if (r) return r;

	    /* append the new record */
	    r = mailbox_append_index_record(mailbox, &newrecord);
	    if (r) return r;

	    /* ensure we have an astate connected to the destination
	     * mailbox, so that the annotation txn will be committed
	     * when we close the mailbox */
	    r = mailbox_get_annotate_state(mailbox, newrecord.uid, &astate);
	    if (r) return r;

	    /* Copy across any per-message annotations */
	    r = annotate_msg_copy(mailbox, oldrecord.uid,
				  mailbox, newrecord.uid,
				  NULL);
	    if (r) return r;

	    /* and expunge the old record */
	    oldrecord.system_flags |= FLAG_EXPUNGED;
	    r = mailbox_rewrite_index_record(mailbox, &oldrecord);

	    /* done - return */
	    return r;
	}
    }

    /* not finding the record is an error! (should never happen) */
    return IMAP_MAILBOX_NONEXISTENT;
}

static int fetch_file(struct mailbox *mailbox, unsigned long uid,
		      struct index_record *rp)
{
    const char *cmd = "FETCH";
    struct dlist *kin = NULL;
    struct dlist *kl;
    int r = 0;
    const char *fname = dlist_reserve_path(mailbox->part, &rp->guid);
    struct message_guid *guid = NULL;
    struct stat sbuf;

    /* already reserved? great */
    if (stat(fname, &sbuf) == 0)
	return 0;

    kl = dlist_newkvlist(NULL, cmd);
    dlist_setatom(kl, "MBOXNAME", mailbox->name);
    dlist_setatom(kl, "PARTITION", mailbox->part);
    dlist_setguid(kl, "GUID", &rp->guid);
    dlist_setnum32(kl, "UID", uid);
    sync_send_lookup(kl, sync_out);
    dlist_free(&kl);

    r = sync_parse_response(cmd, sync_in, &kin);
    if (r) return r;

    if (!dlist_tofile(kin->head, NULL, &guid, NULL, NULL)) {
	r = IMAP_MAILBOX_NONEXISTENT;
	goto done;
    }

    if (!message_guid_equal(guid, &rp->guid))
	r = IMAP_IOERROR;

done:
    dlist_free(&kin);
    return r;
}

static int copy_remote(struct mailbox *mailbox, uint32_t uid,
		       struct dlist *kr)
{
    struct index_record record;
    struct dlist *ki;
    int r;
    struct sync_annot_list *annots = NULL;

    for (ki = kr->head; ki; ki = ki->next) {
	r = parse_upload(ki, mailbox, &record, &annots);
	if (r) return r;
	if (record.uid == uid) {
	    /* choose the destination UID */
	    record.uid = mailbox->i.last_uid + 1;

	    /* already fetched the file in the parse phase */

	    /* append the file */
	    r = sync_append_copyfile(mailbox, &record, annots);

	    sync_annot_list_free(&annots);

	    return r;
	}
	sync_annot_list_free(&annots);
    }
    /* not finding the record is an error! (should never happen) */
    return IMAP_MAILBOX_NONEXISTENT;
}

static int copyback_one_record(struct mailbox *mailbox,
			       struct index_record *rp,
			       const struct sync_annot_list *annots,
			       struct dlist *kaction)
{
    int r;

    /* don't want to copy back expunged records! */
    if (rp->system_flags & FLAG_EXPUNGED)
	return 0;

    /* if the UID is lower than master's last_uid,
     * we'll need to renumber */
    if (rp->uid <= mailbox->i.last_uid) {
	/* Ok, now we need to check if it's just really stale
	 * (has been cleaned out locally) or an error.
	 * In the error case we copy back, stale
	 * we remove from the replica */
	if (rp->modseq < mailbox->i.deletedmodseq) {
	    if (kaction)
		dlist_setnum32(kaction, "EXPUNGE", rp->uid);
	}
	else {
	    r = fetch_file(mailbox, rp->uid, rp);
	    if (r) return r;
	    if (kaction)
		dlist_setnum32(kaction, "COPYBACK", rp->uid);
	}
    }

    /* otherwise we can pull it in with the same UID,
     * which saves causing renumbering on the replica
     * end, so is preferable */
    else {
	/* grab the file */
	r = fetch_file(mailbox, rp->uid, rp);
	if (r) return r;
	/* make sure we're actually making changes now */
	if (!kaction) return 0;
	/* append the file */
	r = sync_append_copyfile(mailbox, rp, annots);
	if (r) return r;
    }

    return 0;
}

static int renumber_one_record(struct index_record *mp,
			       struct dlist *kaction)
{
    /* don't want to renumber expunged records */
    if (mp->system_flags & FLAG_EXPUNGED)
	return 0;

    if (kaction)
	dlist_setnum32(kaction, "RENUMBER", mp->uid);

    return 0;
}

static const char *make_flags(struct mailbox *mailbox, struct index_record *record)
{
    static char buf[4096];
    const char *sep = "";
    int flag;

    if (record->system_flags & FLAG_DELETED) {
	snprintf(buf, 4096, "%s\\Deleted", sep);
	sep = " ";
    }
    if (record->system_flags & FLAG_ANSWERED) {
	snprintf(buf, 4096, "%s\\Answered", sep);
	sep = " ";
    }
    if (record->system_flags & FLAG_FLAGGED) {
	snprintf(buf, 4096, "%s\\Flagged", sep);
	sep = " ";
    }
    if (record->system_flags & FLAG_DRAFT) {
	snprintf(buf, 4096, "%s\\Draft", sep);
	sep = " ";
    }
    if (record->system_flags & FLAG_EXPUNGED) {
	snprintf(buf, 4096, "%s\\Expunged", sep);
	sep = " ";
    }
    if (record->system_flags & FLAG_SEEN) {
	snprintf(buf, 4096, "%s\\Seen", sep);
	sep = " ";
    }

    /* print user flags in mailbox order */
    for (flag = 0; flag < MAX_USER_FLAGS; flag++) {
	if (!mailbox->flagname[flag])
	    continue;
	if (!(record->user_flags[flag/32] & (1<<(flag&31))))
	    continue;
	snprintf(buf, 4096, "%s%s", sep, mailbox->flagname[flag]);
	sep = " ";
    }

    return buf;
}

static void log_record(const char *name, struct mailbox *mailbox,
		       struct index_record *record)
{
    syslog(LOG_NOTICE, "SYNCNOTICE: %s uid:%u modseq:" MODSEQ_FMT " "
	  "last_updated:%lu internaldate:%lu flags:(%s)",
	   name, record->uid, record->modseq,
	   record->last_updated, record->internaldate,
	   make_flags(mailbox, record));
}

static void log_mismatch(const char *reason, struct mailbox *mailbox,
			 struct index_record *mp,
			 struct index_record *rp)
{
    syslog(LOG_NOTICE, "SYNCNOTICE: record mismatch with replica: %s %s",
	   mailbox->name, reason);
    log_record("master", mailbox, mp);
    log_record("replica", mailbox, rp);
}

static int compare_one_record(struct mailbox *mailbox,
			      struct index_record *mp,
			      struct index_record *rp,
			      const struct sync_annot_list *mannots,
			      const struct sync_annot_list *rannots,
			      struct dlist *kaction)
{
    int i;
    int r;

    /* if both ends are expunged, then we do no more processing.  This
     * allows a split brain cleanup to not break things forever.  It
     * does mean that an expunged message might not replicate in that
     * case, but the only way to fix this is add ANOTHER special flag
     * for BROKEN and only ignore GUID mismatches in that case, after
     * moving the message up.  I guess we could force UNLINK immediately
     * too... hmm.  Not today. */

    if ((mp->system_flags & FLAG_EXPUNGED) && (rp->system_flags & FLAG_EXPUNGED))
	return 0;

    /* first of all, check that GUID matches.  If not, we have had a split
     * brain, so the messages both need to be fixed up to their new UIDs.
     * After this function succeeds, both the local and remote copies of this
     * current UID will be actually EXPUNGED, so the earlier return applies. */
    if (!message_guid_equal(&mp->guid, &rp->guid)) {
	char *mguid = xstrdup(message_guid_encode(&mp->guid));
	char *rguid = xstrdup(message_guid_encode(&rp->guid));
	syslog(LOG_ERR, "SYNCERROR: guid mismatch %s %u (%s %s)",
	       mailbox->name, mp->uid, rguid, mguid);
	free(rguid);
	free(mguid);
	/* we will need to renumber both ends to get in sync */

	/* ORDERING - always lower GUID first */
	if (message_guid_cmp(&mp->guid, &rp->guid) > 0) {
	    r = copyback_one_record(mailbox, rp, rannots, kaction);
	    if (!r) r = renumber_one_record(mp, kaction);
	}
	else {
	    r = renumber_one_record(mp, kaction);
	    if (!r) r = copyback_one_record(mailbox, rp, rannots, kaction);
	}

	return r;
    }

    /* are there any differences? */
    if (mp->modseq != rp->modseq)
	goto diff;
    if (mp->last_updated != rp->last_updated)
	goto diff;
    if (mp->internaldate != rp->internaldate)
	goto diff;
    if ((mp->system_flags & FLAGS_GLOBAL) != rp->system_flags)
	goto diff;
    if (mp->thrid != rp->thrid)
	goto diff;
    if (diff_annotations(mannots, rannots))
	goto diff;
    for (i = 0; i < MAX_USER_FLAGS/32; i++) {
	if (mp->user_flags[i] != rp->user_flags[i])
	    goto diff;
    }

    /* no changes found, whoopee */
    return 0;

 diff:
    /* if differences we'll have to rewrite to bump the modseq
     * so that regular replication will cause an update */

    /* interesting case - expunged locally */
    if (mp->system_flags & FLAG_EXPUNGED) {
	/* if expunged, fall through - the rewrite will lift
	 * the modseq to force the change to stick */
    }
    else if (rp->system_flags & FLAG_EXPUNGED) {
	/* mark expunged - rewrite will cause both sides to agree
	 * again */
	mp->system_flags |= FLAG_EXPUNGED;
    }

    /* otherwise, is the replica "newer"?  Better grab those flags */
    else {
	if (rp->modseq > mp->modseq &&
	    rp->last_updated >= mp->last_updated) {
	    log_mismatch("more recent on replica", mailbox, mp, rp);
	    /* then copy all the flag data over from the replica */
	    mp->system_flags = (rp->system_flags & FLAGS_GLOBAL) |
			       (mp->system_flags & FLAGS_LOCAL);
	    mp->thrid = rp->thrid;
	    for (i = 0; i < MAX_USER_FLAGS/32; i++)
		mp->user_flags[i] = rp->user_flags[i];
	}
    }

    /* are we making changes yet? */
    if (!kaction) return 0;

    /* even expunged messages get annotations synced */
    r = apply_annotations(mailbox, mp, mannots, rannots, 0);
    if (r) return r;

    /* this will bump the modseq and force a resync either way :) */
    return mailbox_rewrite_index_record(mailbox, mp);
}


static int mailbox_update_loop(struct mailbox *mailbox,
			       struct dlist *ki,
			       uint32_t last_uid,
			       modseq_t highestmodseq,
			       struct dlist *kaction)
{
    struct index_record mrecord;
    struct index_record rrecord;
    uint32_t recno = 1;
    uint32_t old_num_records = mailbox->i.num_records;
    struct sync_annot_list *mannots = NULL;
    struct sync_annot_list *rannots = NULL;
    int r;

    /* while there are more records on either master OR replica,
     * work out what to do with them */
    while (ki || recno <= old_num_records) {

	sync_annot_list_free(&mannots);
	sync_annot_list_free(&rannots);

	/* most common case - both a master AND a replica record exist */
	if (ki && recno <= old_num_records) {
	    r = mailbox_read_index_record(mailbox, recno, &mrecord);
	    if (r) goto out;
	    r = read_annotations(mailbox, &mrecord, &mannots);
	    if (r) goto out;
	    r = parse_upload(ki, mailbox, &rrecord, &rannots);
	    if (r) goto out;

	    /* same UID - compare the records */
	    if (rrecord.uid == mrecord.uid) {
		r = compare_one_record(mailbox,
				       &mrecord, &rrecord,
				       mannots, rannots,
				       kaction);
		if (r) goto out;
		/* increment both */
		recno++;
		ki = ki->next;
	    }
	    else if (rrecord.uid > mrecord.uid) {
		/* record only exists on the master */
		if (!(mrecord.system_flags & FLAG_EXPUNGED)) {
		    syslog(LOG_ERR, "SYNCERROR: only exists on master %s %u (%s)",
			   mailbox->name, mrecord.uid,
			   message_guid_encode(&mrecord.guid));
		    r = renumber_one_record(&mrecord, kaction);
		    if (r) goto out;
		}
		/* only increment master */
		recno++;
	    }
	    else {
		/* record only exists on the replica */
		if (!(rrecord.system_flags & FLAG_EXPUNGED)) {
		    if (kaction)
			syslog(LOG_ERR, "SYNCERROR: only exists on replica %s %u (%s)",
			       mailbox->name, rrecord.uid,
			       message_guid_encode(&rrecord.guid));
		    r = copyback_one_record(mailbox, &rrecord, rannots, kaction);
		    if (r) goto out;
		}
		/* only increment replica */
		ki = ki->next;
	    }
	}

	/* no more replica records, but still master records */
	else if (recno <= old_num_records) {
	    r = mailbox_read_index_record(mailbox, recno, &mrecord);
	    if (r) goto out;
	    /* if the replica has seen this UID, we need to renumber.
	     * Otherwise it will replicate fine as-is */
	    if (mrecord.uid <= last_uid) {
		r = renumber_one_record(&mrecord, kaction);
		if (r) goto out;
	    }
	    else if (mrecord.modseq <= highestmodseq) {
		if (kaction) {
		    /* bump our modseq so we sync */
		    syslog(LOG_NOTICE, "SYNCNOTICE: bumping modseq %s %u",
			   mailbox->name, mrecord.uid);
		    r = mailbox_rewrite_index_record(mailbox, &mrecord);
		    if (r) goto out;
		}
	    }
	    recno++;
	}

	/* record only exists on the replica */
	else {
	    r = parse_upload(ki, mailbox, &rrecord, &rannots);
	    if (r) goto out;

	    if (kaction)
		syslog(LOG_NOTICE, "SYNCNOTICE: only on replica %s %u",
		       mailbox->name, rrecord.uid);

	    /* going to need this one */
	    r = copyback_one_record(mailbox, &rrecord, rannots, kaction);
	    if (r) goto out;

	    ki = ki->next;
	}
    }
    r = 0;

out:
    sync_annot_list_free(&mannots);
    sync_annot_list_free(&rannots);
    return r;
}

static int mailbox_full_update(const char *mboxname)
{
    const char *cmd = "FULLMAILBOX";
    struct mailbox *mailbox = NULL;
    int r;
    struct dlist *kin = NULL;
    struct dlist *kr = NULL;
    struct dlist *ka = NULL;
    struct dlist *kuids = NULL;
    struct dlist *kl = NULL;
    struct dlist *kaction = NULL;
    struct dlist *kexpunge = NULL;
    modseq_t highestmodseq;
    uint32_t uidvalidity;
    uint32_t last_uid;
    struct sync_annot_list *mannots = NULL;
    struct sync_annot_list *rannots = NULL;
    int remote_modseq_was_higher = 0;

    kl = dlist_setatom(NULL, cmd, mboxname);
    sync_send_lookup(kl, sync_out);
    dlist_free(&kl);

    r = sync_parse_response(cmd, sync_in, &kin);
    if (r) return r;

    kl = kin->head;

    if (!kl) {
	r = IMAP_MAILBOX_NONEXISTENT;
	goto done;
    }

    /* XXX - handle the header.  I want to do some ordering on timestamps
     * in particular here - if there's more recent data on the replica then
     * it should be copied back.  This depends on having a nice way to
     * parse the mailbox structure back in to a struct index_header rather
     * than the by hand stuff though, because that sucks.  NOTE - this
     * doesn't really matter too much, because we'll blat the replica's
     * values anyway! */

    if (!dlist_getnum64(kl, "HIGHESTMODSEQ", &highestmodseq)) {
	r = IMAP_PROTOCOL_BAD_PARAMETERS;
	goto done;
    }

    if (!dlist_getnum32(kl, "UIDVALIDITY", &uidvalidity)) {
	r = IMAP_PROTOCOL_BAD_PARAMETERS;
	goto done;
    }

    if (!dlist_getnum32(kl, "LAST_UID", &last_uid)) {
	r = IMAP_PROTOCOL_BAD_PARAMETERS;
	goto done;
    }

    if (!dlist_getlist(kl, "RECORD", &kr)) {
	r = IMAP_PROTOCOL_BAD_PARAMETERS;
	goto done;
    }

    /* we'll be updating it! */
    r = mailbox_open_iwl(mboxname, &mailbox);
    if (!r) r = sync_mailbox_version_check(&mailbox);
    if (r) goto done;

    /* if local UIDVALIDITY is lower, copy from remote, otherwise
     * remote will copy ours when we sync */
    if (mailbox->i.uidvalidity < uidvalidity) {
	syslog(LOG_NOTICE, "SYNCNOTICE: uidvalidity higher on replica %s"
	       ", updating %u => %u",
	       mailbox->name, mailbox->i.uidvalidity, uidvalidity);
	mailbox_index_dirty(mailbox);
	mailbox->i.uidvalidity = uidvalidity;
    }

    if (mailbox->i.highestmodseq < highestmodseq) {
	/* highestmodseq on replica is dirty - we must copy and then dirty
	 * so we go one higher! */
	syslog(LOG_NOTICE, "SYNCNOTICE: highestmodseq higher on replica %s"
	       ", updating " MODSEQ_FMT " => " MODSEQ_FMT,
	       mailbox->name, mailbox->i.highestmodseq, highestmodseq+1);
	mailbox->modseq_dirty = 0;
	mailbox->i.highestmodseq = highestmodseq;
	mailbox_modseq_dirty(mailbox);
	remote_modseq_was_higher = 1;
    }

    r = mailbox_update_loop(mailbox, kr->head, last_uid,
			    highestmodseq, NULL);
    if (r) {
	syslog(LOG_ERR, "SYNCNOTICE: failed to prepare update for %s: %s",
	       mailbox->name, error_message(r));
	goto done;
    }

    /* OK - now we're committed to make changes! */

    kaction = dlist_newlist(NULL, "ACTION");
    r = mailbox_update_loop(mailbox, kr->head, last_uid,
			    highestmodseq, kaction);
    if (r) goto cleanup;

    /* if replica still has a higher last_uid, bump our local
     * number to match so future records don't clash */
    if (mailbox->i.last_uid < last_uid)
	mailbox->i.last_uid = last_uid;

    /* ugly variable reuse */
    dlist_getlist(kl, "ANNOTATIONS", &ka);

    if (ka) decode_annotations(ka, &rannots, NULL);
    r = read_annotations(mailbox, NULL, &mannots);
    if (r) goto cleanup;
    r = apply_annotations(mailbox, NULL, mannots, rannots,
			  !remote_modseq_was_higher);
    if (r) goto cleanup;

    /* blatant reuse 'r' us */
    kexpunge = dlist_newkvlist(NULL, "EXPUNGE");
    dlist_setatom(kexpunge, "MBOXNAME", mailbox->name);
    dlist_setatom(kexpunge, "UNIQUEID", mailbox->uniqueid); /* just for safety */
    kuids = dlist_newlist(kexpunge, "UID");
    for (ka = kaction->head; ka; ka = ka->next) {
	if (!strcmp(ka->name, "EXPUNGE")) {
	    dlist_setnum32(kuids, "UID", dlist_num(ka));
	}
	else if (!strcmp(ka->name, "COPYBACK")) {
	    r = copy_remote(mailbox, dlist_num(ka), kr);
	    if (r) goto cleanup;
	    dlist_setnum32(kuids, "UID", dlist_num(ka));
	}
	else if (!strcmp(ka->name, "RENUMBER")) {
	    r = copy_local(mailbox, dlist_num(ka));
	    if (r) goto cleanup;
	}
    }

    /* we still need to do the EXPUNGEs */
 cleanup:

    sync_annot_list_free(&mannots);
    sync_annot_list_free(&rannots);

    /* close the mailbox before sending any expunges
     * to avoid deadlocks */
    mailbox_close(&mailbox);

    /* only send expunge if we have some UIDs to expunge */
    if (kuids && kuids->head) {
	int r2;
	sync_send_apply(kexpunge, sync_out);
	r2 = sync_parse_response("EXPUNGE", sync_in, NULL);
	if (r2) {
	    syslog(LOG_ERR, "SYNCERROR: failed to expunge in cleanup %s",
		   mboxname);
	}
    }

done:
    if (r && mailbox)
	annotate_state_abort(&mailbox->annot_state);
    mailbox_close(&mailbox);

    dlist_free(&kin);
    dlist_free(&kaction);
    dlist_free(&kexpunge);
    /* kuids points into the tree rooted at kexpunge
     * so we don't need to free it explicitly here */

    return r;
}

/* this should be in sync_support, but that becomes Ellie's
 * problem later */
static int crceq(struct synccrcs a, struct synccrcs b)
{
    if (a.basic && b.basic && a.basic != b.basic) return 0;
    if (a.annot && b.annot && a.annot != b.annot) return 0;
    return 1;
}

static int is_unchanged(struct mailbox *mailbox, struct sync_folder *remote)
{
    /* look for any mismatches */
    unsigned options = mailbox->i.options & MAILBOX_OPTIONS_MASK;

    if (!remote) return 0;
    if (remote->mbtype != mailbox->mbtype) return 0;
    if (remote->last_uid != mailbox->i.last_uid) return 0;
    if (remote->highestmodseq != mailbox->i.highestmodseq) return 0;
    if (remote->uidvalidity != mailbox->i.uidvalidity) return 0;
    if (remote->recentuid != mailbox->i.recentuid) return 0;
    if (remote->recenttime != mailbox->i.recenttime) return 0;
    if (remote->pop3_last_login != mailbox->i.pop3_last_login) return 0;
    if (remote->pop3_show_after != mailbox->i.pop3_show_after) return 0;
    if (remote->options != options) return 0;
    if (strcmp(remote->acl, mailbox->acl)) return 0;

    /* compare annotations */
    {
	struct sync_annot_list *mannots = NULL;
	int r = read_annotations(mailbox, NULL, &mannots);
	if (r) return 0;

	if (diff_annotations(mannots, remote->annots)) {
	    sync_annot_list_free(&mannots);
	    return 0;
	}
	sync_annot_list_free(&mannots);
    }

    /* if we got here then we should force check the CRCs */
    if (!crceq(remote->synccrcs, mailbox_synccrcs(mailbox, /*force*/0)))
	if (!crceq(remote->synccrcs, mailbox_synccrcs(mailbox, /*force*/1)))
	    return 0;

    /* otherwise it's unchanged! */
    return 1;
}

static int update_mailbox_once(struct sync_folder *local,
			       struct sync_folder *remote,
			       struct sync_reserve_list *reserve_guids,
			       int is_repeat)
{
    struct sync_msgid_list *part_list;
    struct mailbox *mailbox = NULL;
    int r = 0;
    struct dlist *kl = dlist_newkvlist(NULL, "MAILBOX");
    struct dlist *kupload = dlist_newlist(NULL, "MESSAGE");
    annotate_state_t *astate = NULL;

    r = mailbox_open_iwl(local->name, &mailbox);
    if (!r) r = sync_mailbox_version_check(&mailbox);
    if (r == IMAP_MAILBOX_NONEXISTENT) {
	/* been deleted in the meanwhile... it will get picked up by the
	 * delete call later */
	r = 0;
	goto done;
    }
    else if (r)
	goto done;

    /* hold the annotate state open */
    mailbox_get_annotate_state(mailbox, ANNOTATE_ANY_UID, &astate);
    /* and force it to hold a transaction while it does stuff */
    annotate_state_begin(astate);

    /* definitely bad if these don't match! */
    if (strcmp(mailbox->uniqueid, local->uniqueid) ||
	strcmp(mailbox->part, local->part)) {
	r = IMAP_MAILBOX_MOVED;
	goto done;
    }

    /* check that replication stands a chance of succeeding */
    if (remote && !is_repeat) {
	if (mailbox->i.deletedmodseq > remote->highestmodseq) {
	    syslog(LOG_NOTICE, "inefficient replication ("
		   MODSEQ_FMT " > " MODSEQ_FMT ") %s",
		   mailbox->i.deletedmodseq, remote->highestmodseq,
		   local->name);
	    r = IMAP_AGAIN;
	    goto done;
	}
	/* need a full sync to fix uidvalidity issues so we get a
	 * writelocked mailbox */
	if (mailbox->i.uidvalidity < remote->uidvalidity) {
	    r = IMAP_AGAIN;
	    goto done;
	}
    }

    /* make sure CRC is updated if we're retrying */
    if (is_repeat) {
	r = mailbox_index_recalc(mailbox);
	if (r) goto done;
    }

    /* nothing changed - nothing to send */
    if (is_unchanged(mailbox, remote))
	goto done;

    part_list = sync_reserve_partlist(reserve_guids, mailbox->part);
    r = sync_mailbox(mailbox, remote, part_list, kl, kupload, 1);
    if (r) goto done;

    /* keep the mailbox locked for shorter time! Unlock the index now
     * but don't close it, because we need to guarantee that message
     * files don't get deleted until we're finished with them... */
    mailbox_unlock_index(mailbox, NULL);

    /* upload in small(ish) blocks to avoid timeouts */
    while (kupload->head) {
	struct dlist *kul1 = dlist_splice(kupload, 1024);
	sync_send_apply(kul1, sync_out);
	r = sync_parse_response("MESSAGE", sync_in, NULL);
	dlist_free(&kul1);
	if (r) goto done; /* abort earlier */
    }

    /* close before sending the apply - all data is already read */
    mailbox_close(&mailbox);

    /* update the mailbox */
    sync_send_apply(kl, sync_out);
    r = sync_parse_response("MAILBOX", sync_in, NULL);

done:
    mailbox_close(&mailbox);

    dlist_free(&kupload);
    dlist_free(&kl);
    return r;
}

static int update_mailbox(struct sync_folder *local,
			  struct sync_folder *remote,
			  struct sync_reserve_list *reserve_guids)
{
    int r = update_mailbox_once(local, remote, reserve_guids, 0);

    /* never retry - other end should always sync cleanly */
    if (no_copyback) return r;

    if (r == IMAP_AGAIN) {
	r = mailbox_full_update(local->name);
	if (!r) r = update_mailbox_once(local, remote, reserve_guids, 1);
    }
    else if (r == IMAP_SYNC_CHECKSUM) {
	syslog(LOG_ERR, "CRC failure on sync for %s, trying full update",
	       local->name);
	r = mailbox_full_update(local->name);
	if (!r) r = update_mailbox_once(local, remote, reserve_guids, 1);
    }

    return r;
}

/* ====================================================================== */


static int update_seen_work(const char *user, const char *uniqueid,
			    struct seendata *sd)
{
    const char *cmd = "SEEN";
    struct dlist *kl;

    /* Update seen list */
    kl = dlist_newkvlist(NULL, cmd);
    dlist_setatom(kl, "USERID", user);
    dlist_setatom(kl, "UNIQUEID", uniqueid);
    dlist_setdate(kl, "LASTREAD", sd->lastread);
    dlist_setnum32(kl, "LASTUID", sd->lastuid);
    dlist_setdate(kl, "LASTCHANGE", sd->lastchange);
    dlist_setatom(kl, "SEENUIDS", sd->seenuids);
    sync_send_apply(kl, sync_out);
    dlist_free(&kl);

    return sync_parse_response(cmd, sync_in, NULL);
}

static int do_seen(char *user, char *uniqueid)
{
    int r = 0;
    struct seen *seendb = NULL;
    struct seendata sd = SEENDATA_INITIALIZER;

    if (verbose)
	printf("SEEN %s %s\n", user, uniqueid);

    if (verbose_logging)
	syslog(LOG_INFO, "SEEN %s %s", user, uniqueid);

    /* ignore read failures */
    r = seen_open(user, SEEN_SILENT, &seendb);
    if (r) return 0;

    r = seen_read(seendb, uniqueid, &sd);

    if (!r) r = update_seen_work(user, uniqueid, &sd);

    seen_close(&seendb);
    seen_freedata(&sd);

    return r;
}

/* ====================================================================== */

static int do_quota(const char *root)
{
    int r = 0;
    struct quota q;

    if (verbose)
	printf("SETQUOTA %s\n", root);

    if (verbose_logging)
	syslog(LOG_INFO, "SETQUOTA: %s", root);

    quota_init(&q, root);
    r = update_quota_work(&q, NULL);
    quota_free(&q);

    return r;
}

static int getannotation_cb(const char *mailbox __attribute__((unused)),
			    uint32_t uid __attribute__((unused)),
			    const char *entry, const char *userid,
			    const struct buf *value, void *rock)
{
    struct sync_annot_list *l = (struct sync_annot_list *) rock;

    sync_annot_list_add(l, entry, userid, value);

    return 0;
}

static int parse_annotation(struct dlist *kin,
			    struct sync_annot_list *replica_annot)
{
    struct dlist *kl;
    const char *entry;
    const char *userid;
    const char *valmap = NULL;
    size_t vallen = 0;
    struct buf value = BUF_INITIALIZER;

    for (kl = kin->head; kl; kl = kl->next) {
	if (!dlist_getatom(kl, "ENTRY", &entry))
	    return IMAP_PROTOCOL_BAD_PARAMETERS;
	if (!dlist_getatom(kl, "USERID", &userid))
	    return IMAP_PROTOCOL_BAD_PARAMETERS;
	if (!dlist_getmap(kl, "VALUE", &valmap, &vallen))
	    return IMAP_PROTOCOL_BAD_PARAMETERS;
	buf_init_ro(&value, valmap, vallen);
	sync_annot_list_add(replica_annot, entry, userid, &value);
    }

    return 0;
}

static int do_getannotation(const char *mboxname,
			    struct sync_annot_list *replica_annot)
{
    const char *cmd = "ANNOTATION";
    struct dlist *kl;
    struct dlist *kin = NULL;
    int r;

    /* Update seen list */
    kl = dlist_setatom(NULL, cmd, mboxname);
    sync_send_lookup(kl, sync_out);
    dlist_free(&kl);

    r = sync_parse_response(cmd, sync_in, &kin);
    if (r) return r;

    r = parse_annotation(kin, replica_annot);
    dlist_free(&kin);

    return r;
}

static int do_annotation(const char *mboxname)
{
    int r;
    struct sync_annot_list *replica_annot = sync_annot_list_create();
    struct sync_annot_list *master_annot = sync_annot_list_create();
    struct sync_annot *ma, *ra;
    int n;

    r = do_getannotation(mboxname, replica_annot);
    if (r) goto bail;

    r = annotatemore_findall(mboxname, 0, "*", &getannotation_cb, master_annot);
    if (r) goto bail;

    /* both lists are sorted, so we work our way through the lists
       top-to-bottom and determine what we need to do based on order */
    ma = master_annot->head;
    ra = replica_annot->head;
    while (ma || ra) {
	if (!ra) n = -1;		/* add all master annotations */
	else if (!ma) n = 1;		/* remove all replica annotations */
	else if ((n = strcmp(ma->entry, ra->entry)) == 0)
	    n = strcmp(ma->userid, ra->userid);

	if (n > 0) {
	    /* remove replica annotation */
	    r = folder_unannotation(mboxname, ra->entry, ra->userid);
	    if (r) goto bail;
	    ra = ra->next;
	    continue;
	}

	if (n == 0) {
	    /* already have the annotation, but is the value different? */
	    if (!buf_cmp(&ra->value, &ma->value)) {
		ra = ra->next;
		ma = ma->next;
		continue;
	    }
	    ra = ra->next;
	}

	/* add the current client annotation */
	r = folder_setannotation(mboxname, ma->entry, ma->userid, &ma->value);
	if (r) goto bail;

	ma = ma->next;
    }

bail:
    sync_annot_list_free(&master_annot);
    sync_annot_list_free(&replica_annot);
    return r;
}

/* ====================================================================== */

static int do_folders(struct sync_name_list *mboxname_list,
	       struct sync_folder_list *replica_folders, int delete_remote)
{
    int r;
    struct sync_folder_list *master_folders;
    struct sync_rename_list *rename_folders;
    struct sync_reserve_list *reserve_guids;
    struct sync_folder *mfolder, *rfolder;

    master_folders = sync_folder_list_create();
    rename_folders = sync_rename_list_create();
    reserve_guids = sync_reserve_list_create(SYNC_MSGID_LIST_HASH_SIZE);

    r = reserve_messages(mboxname_list, master_folders,
			 replica_folders, reserve_guids);
    if (r) {
	syslog(LOG_ERR, "reserve messages: failed: %s", error_message(r));
	goto bail;
    }

    /* Tag folders on server which still exist on the client. Anything
     * on the server which remains untagged can be deleted immediately */
    for (mfolder = master_folders->head; mfolder; mfolder = mfolder->next) {
	if (mfolder->mark) continue;
	rfolder = sync_folder_lookup(replica_folders, mfolder->uniqueid);
	if (!rfolder) continue;
	if (rfolder->mark) continue;
	rfolder->mark = 1;

	/* does it need a rename? partition change is a rename too */
	if (strcmp(mfolder->name, rfolder->name) || strcmp(mfolder->part, rfolder->part)) {
	    sync_rename_list_add(rename_folders, mfolder->uniqueid, rfolder->name,
				 mfolder->name, mfolder->part, mfolder->uidvalidity);
	}
    }

    /* Delete folders on server which no longer exist on client */
    if (delete_remote) {
	for (rfolder = replica_folders->head; rfolder; rfolder = rfolder->next) {
	    if (rfolder->mark) continue;
	    r = folder_delete(rfolder->name);
	    if (r) {
		syslog(LOG_ERR, "folder_delete(): failed: %s '%s'",
		       rfolder->name, error_message(r));
		goto bail;
	    }
	}
    }

    /* Need to rename folders in an order which avoids dependancy conflicts
     * following isn't wildly efficient, but rename_folders will typically be
     * short and contain few dependancies.  Algorithm is to simply pick a
     * rename operation which has no dependancy and repeat until done */

    while (rename_folders->done < rename_folders->count) {
	int rename_success = 0;
	struct sync_rename *item, *item2;

	for (item = rename_folders->head; item; item = item->next) {
	    if (item->done) continue;

	    /* don't skip rename to different partition */
	    if (strcmp(item->oldname, item->newname)) {
		item2 = sync_rename_lookup(rename_folders, item->newname);
		if (item2 && !item2->done) continue;
	    }

	    /* Found unprocessed item which should rename cleanly */
	    r = folder_rename(item->oldname, item->newname,
			      item->part, item->uidvalidity);
	    if (r) {
		syslog(LOG_ERR, "do_folders(): failed to rename: %s -> %s ",
		       item->oldname, item->newname);
		goto bail;
	    }

	    rename_folders->done++;
	    item->done = 1;
	    rename_success = 1;
	}

	if (!rename_success) {
	    /* Scanned entire list without a match */
	    syslog(LOG_ERR,
		   "do_folders(): failed to order folders correctly");
	    r = IMAP_AGAIN;
	    goto bail;
	}
    }

    for (mfolder = master_folders->head; mfolder; mfolder = mfolder->next) {
	if (mfolder->mark) continue;
	/* NOTE: rfolder->name may now be wrong, but we're guaranteed that
	 * it was successfully renamed above, so just use mfolder->name for
	 * all commands */
	rfolder = sync_folder_lookup(replica_folders, mfolder->uniqueid);
	r = update_mailbox(mfolder, rfolder, reserve_guids);
	if (r) {
	    syslog(LOG_ERR, "do_folders(): update failed: %s '%s'",
		   mfolder->name, error_message(r));
	    goto bail;
	}
    }

 bail:
    sync_folder_list_free(&master_folders);
    sync_rename_list_free(&rename_folders);
    sync_reserve_list_free(&reserve_guids);
    return r;
}

static int do_unmailbox(const char *mboxname)
{
    struct mailbox *mailbox = NULL;
    int r;

    r = mailbox_open_irl(mboxname, &mailbox);
    if (r == IMAP_MAILBOX_NONEXISTENT) {
	r = folder_delete(mboxname);
	if (r) {
	    syslog(LOG_ERR, "folder_delete(): failed: %s '%s'",
		   mboxname, error_message(r));
	}
    }
    mailbox_close(&mailbox);

    return r;
}

static int do_mailboxes(struct sync_name_list *mboxname_list)
{
    struct sync_name *mbox;
    struct sync_folder_list *replica_folders = sync_folder_list_create();
    struct dlist *kl = NULL;
    int r;

    if (verbose) {
	printf("MAILBOXES");
	for (mbox = mboxname_list->head; mbox; mbox = mbox->next) {
	    printf(" %s", mbox->name);
	}
	printf("\n");
    }

    if (verbose_logging) {
	for (mbox = mboxname_list->head; mbox; mbox = mbox->next)
	    syslog(LOG_INFO, "MAILBOX %s", mbox->name);
    }

    kl = dlist_newlist(NULL, "MAILBOXES");
    for (mbox = mboxname_list->head; mbox; mbox = mbox->next)
	dlist_setatom(kl, "MBOXNAME", mbox->name);
    sync_send_lookup(kl, sync_out);
    dlist_free(&kl);

    r = response_parse("MAILBOXES", replica_folders, NULL, NULL, NULL, NULL);

    /* we don't want to delete remote folders which weren't found locally,
     * because we may be racing with a rename, and we don't want to lose
     * the remote files.  A real delete will always have inserted a
     * UNMAILBOX anyway */
    if (!r)
	r = do_folders(mboxname_list, replica_folders, /*delete_remote*/0);

    sync_folder_list_free(&replica_folders);

    return r;
}

/* ====================================================================== */

struct mboxinfo {
    struct sync_name_list *mboxlist;
    struct sync_name_list *quotalist;
};

static int do_mailbox_info(void *rock,
			   const char *key, size_t keylen,
			   const char *data __attribute__((unused)),
			   size_t datalen __attribute__((unused)))
{
    struct mailbox *mailbox = NULL;
    struct mboxinfo *info = (struct mboxinfo *)rock;
    char *name = xstrndup(key, keylen);
    int r = 0;

    /* XXX - check for deleted? */

    r = mailbox_open_irl(name, &mailbox);
    if (!r) r = sync_mailbox_version_check(&mailbox);
    /* doesn't exist?  Probably not finished creating or removing yet */
    if (r == IMAP_MAILBOX_NONEXISTENT) {
	r = 0;
	goto done;
    }
    if (r == IMAP_MAILBOX_RESERVED) {
	r = 0;
	goto done;
    }
    if (r) goto done;

    if (info->quotalist && mailbox->quotaroot) {
	if (!sync_name_lookup(info->quotalist, mailbox->quotaroot))
	    sync_name_list_add(info->quotalist, mailbox->quotaroot);
    }

    mailbox_close(&mailbox);

    addmbox(name, 0, 0, info->mboxlist);

done:
    free(name);

    return r;
}

static int do_user_quota(struct sync_name_list *master_quotaroots,
			 struct sync_quota_list *replica_quota)
{
    int r;
    struct sync_name *mitem;
    struct sync_quota *rquota;
    struct quota q;

    /* set any new or changed quotas */
    for (mitem = master_quotaroots->head; mitem; mitem = mitem->next) {
	rquota = sync_quota_lookup(replica_quota, mitem->name);
	if (rquota)
	    rquota->done = 1;
	quota_init(&q, mitem->name);
	r = update_quota_work(&q, rquota);
	quota_free(&q);
	if (r) return r;
    }

    /* delete any quotas no longer on the master */
    for (rquota = replica_quota->head; rquota; rquota = rquota->next) {
	if (rquota->done) continue;
	r = delete_quota(rquota->root);
	if (r) return r;
    }

    return 0;
}

static int do_user_main(const char *user,
			struct sync_folder_list *replica_folders,
			struct sync_quota_list *replica_quota)
{
    int r = 0;
    struct sync_name_list *mboxname_list = sync_name_list_create();
    struct sync_name_list *master_quotaroots = sync_name_list_create();
    struct mboxinfo info;

    info.mboxlist = mboxname_list;
    info.quotalist = master_quotaroots;

    r = mboxlist_allusermbox(user, do_mailbox_info, &info, /*incdel*/1);

    /* we know all the folders present on the master, so it's safe to delete
     * anything not mentioned here on the replica - at least until we get
     * real tombstones */
    if (!r) r = do_folders(mboxname_list, replica_folders, /*delete_remote*/1);
    if (!r) r = do_user_quota(master_quotaroots, replica_quota);

    sync_name_list_free(&mboxname_list);
    sync_name_list_free(&master_quotaroots);

    if (r) syslog(LOG_ERR, "IOERROR: %s", error_message(r));

    return r;
}

static int do_user_sub(const char *userid, struct sync_name_list *replica_subs)
{
    struct sync_name_list *master_subs = sync_name_list_create();
    struct sync_name *msubs, *rsubs;
    int r = 0;

    /* Includes subsiduary nodes automatically */
    r = mboxlist_allsubs(userid, addmbox_sub, master_subs);
    if (r) goto bail;

    /* add any folders that need adding, and mark any which
     * still exist */
    for (msubs = master_subs->head; msubs; msubs = msubs->next) {
	rsubs = sync_name_lookup(replica_subs, msubs->name);
	if (rsubs) {
	    rsubs->mark = 1;
	    continue;
	}
	r = set_sub(userid, msubs->name, 1);
	if (r) goto bail;
    }

    /* remove any no-longer-subscribed folders */
    for (rsubs = replica_subs->head; rsubs; rsubs = rsubs->next) {
	if (rsubs->mark)
	    continue;
	r = set_sub(userid, rsubs->name, 0);
	if (r) goto bail;
    }

 bail:
    sync_name_list_free(&master_subs);
    return r;
}

static int get_seen(const char *uniqueid, struct seendata *sd, void *rock)
{
    struct sync_seen_list *list = (struct sync_seen_list *)rock;

    sync_seen_list_add(list, uniqueid, sd->lastread, sd->lastuid,
		       sd->lastchange, sd->seenuids);

    return 0;
}

static int do_user_seen(const char *user, struct sync_seen_list *replica_seen)
{
    int r;
    struct sync_seen *mseen, *rseen;
    struct seen *seendb = NULL;
    struct sync_seen_list *list;

    /* silently ignore errors */
    r = seen_open(user, SEEN_SILENT, &seendb);
    if (r) return 0;

    list = sync_seen_list_create();

    seen_foreach(seendb, get_seen, list);
    seen_close(&seendb);

    for (mseen = list->head; mseen; mseen = mseen->next) {
	rseen = sync_seen_list_lookup(replica_seen, mseen->uniqueid);
	if (rseen) {
	    rseen->mark = 1;
	    if (seen_compare(&rseen->sd, &mseen->sd))
		continue; /* nothing changed */
	}
	r = update_seen_work(user, mseen->uniqueid, &mseen->sd);
    }

    /* XXX - delete seen on the replica for records that don't exist? */

    sync_seen_list_free(&list);

    return 0;
}

static int do_user_sieve(const char *userid, struct sync_sieve_list *replica_sieve)
{
    int r = 0;
    struct sync_sieve_list *master_sieve;
    struct sync_sieve *mitem, *ritem;
    int master_active = 0;
    int replica_active = 0;

    master_sieve = sync_sieve_list_generate(userid);
    if (!master_sieve) {
	syslog(LOG_ERR, "Unable to list sieve scripts for %s", userid);
	return IMAP_IOERROR;
    }

    /* Upload missing and out of date or mismatching scripts */
    for (mitem = master_sieve->head; mitem; mitem = mitem->next) {
	ritem = sync_sieve_lookup(replica_sieve, mitem->name);
	if (ritem) {
	    ritem->mark = 1;
	    /* compare the GUID if known */
	    if (!message_guid_isnull(&ritem->guid)) {
		if (message_guid_equal(&ritem->guid, &mitem->guid))
		    continue;
		/* XXX: copyback support */
	    }
	    /* fallback to date comparison */
	    else if (ritem->last_update >= mitem->last_update)
		continue; /* changed */
	}

	r = sieve_upload(userid, mitem->name, mitem->last_update);
	if (r) goto bail;
    }

    /* Delete scripts which no longer exist on the master */
    replica_active = 0;
    for (ritem = replica_sieve->head; ritem; ritem = ritem->next) {
	if (ritem->mark) {
	    if (ritem->active)
		replica_active = 1;
	} else {
	    r = sieve_delete(userid, ritem->name);
	    if (r) goto bail;
	}
    }

    /* Change active script if necessary */
    master_active = 0;
    for (mitem = master_sieve->head; mitem; mitem = mitem->next) {
	if (!mitem->active)
	    continue;

	master_active = 1;
	ritem = sync_sieve_lookup(replica_sieve, mitem->name);
	if (ritem && ritem->active)
	    break;

	r = sieve_activate(userid, mitem->name);
	if (r) goto bail;

	replica_active = 1;
	break;
    }

    if (!master_active && replica_active)
	r = sieve_deactivate(userid);

 bail:
    sync_sieve_list_free(&master_sieve);
    return(r);
}

static int do_user(const char *userid)
{
    char buf[MAX_MAILBOX_BUFFER];
    int r = 0;
    struct sync_folder_list *replica_folders = sync_folder_list_create();
    struct sync_name_list *replica_subs = sync_name_list_create();
    struct sync_sieve_list *replica_sieve = sync_sieve_list_create();
    struct sync_seen_list *replica_seen = sync_seen_list_create();
    struct sync_quota_list *replica_quota = sync_quota_list_create();
    struct dlist *kl = NULL;
    struct mailbox *mailbox = NULL;

    if (verbose)
	printf("USER %s\n", userid);

    if (verbose_logging)
	syslog(LOG_INFO, "USER %s", userid);

    /* check local mailbox first */
    (sync_namespace.mboxname_tointernal)(&sync_namespace, "INBOX",
					  userid, buf);
    r = mailbox_open_irl(buf, &mailbox);
    if (r == IMAP_MAILBOX_NONEXISTENT) {
	/* user has been removed, skip */
	r = 0;
	goto done;
    }
    if (r) goto done;

    /* we don't hold locks while sending commands */
    mailbox_close(&mailbox);

    kl = dlist_setatom(NULL, "USER", userid);
    sync_send_lookup(kl, sync_out);
    dlist_free(&kl);

    r = response_parse("USER",
		       replica_folders, replica_subs,
		       replica_sieve, replica_seen, replica_quota);
    /* can happen! */
    if (r == IMAP_MAILBOX_NONEXISTENT) r = 0;
    if (r) goto done;

    r = do_user_main(userid, replica_folders, replica_quota);
    if (r) goto done;
    r = do_user_sub(userid, replica_subs);
    if (r) goto done;
    r = do_user_sieve(userid, replica_sieve);
    if (r) goto done;
    r = do_user_seen(userid, replica_seen);

done:
    sync_folder_list_free(&replica_folders);
    sync_name_list_free(&replica_subs);
    sync_sieve_list_free(&replica_sieve);
    sync_seen_list_free(&replica_seen);
    sync_quota_list_free(&replica_quota);

    return r;
}

/* ====================================================================== */

static int do_meta(char *userid)
{
    struct sync_name_list *replica_subs = sync_name_list_create();
    struct sync_sieve_list *replica_sieve = sync_sieve_list_create();
    struct sync_seen_list *replica_seen = sync_seen_list_create();
    struct dlist *kl = NULL;
    int r = 0;

    if (verbose)
	printf("META %s\n", userid);

    if (verbose_logging)
	syslog(LOG_INFO, "META %s", userid);

    kl = dlist_setatom(NULL, "META", userid);
    sync_send_lookup(kl, sync_out);
    dlist_free(&kl);

    r = response_parse("META", NULL, replica_subs, replica_sieve, replica_seen, NULL);
    if (!r) r = do_user_seen(userid, replica_seen);
    if (!r) r = do_user_sub(userid, replica_subs);
    if (!r) r = do_user_sieve(userid, replica_sieve);
    sync_seen_list_free(&replica_seen);
    sync_name_list_free(&replica_subs);
    sync_sieve_list_free(&replica_sieve);

    return r;
}

/* ====================================================================== */

static void remove_meta(char *user, struct sync_action_list *list)
{
    struct sync_action *action;

    for (action = list->head ; action ; action = action->next) {
	if (!strcmp(user, action->user)) {
	    action->active = 0;
	}
    }
}

/* ====================================================================== */

static int do_sync_mailboxes(struct sync_name_list *mboxname_list,
			     struct sync_action_list *user_list)
{
    int r = 0;

    if (mboxname_list->count) {
	r = do_mailboxes(mboxname_list);
	if (r) {
	    /* promote failed personal mailboxes to USER */
	    int nonuser = 0;
	    struct sync_name *mbox;
	    const char *userid;

	    for (mbox = mboxname_list->head; mbox; mbox = mbox->next) {
		/* done OK?  Good :) */
		if (mbox->mark)
		    continue;

		userid = mboxname_to_userid(mbox->name);
		if (userid) {
		    mbox->mark = 1;

		    sync_action_list_add(user_list, NULL, userid);
		    if (verbose) {
			printf("  Promoting: MAILBOX %s -> USER %s\n",
			       mbox->name, userid);
		    }
		    if (verbose_logging) {
			syslog(LOG_INFO, "  Promoting: MAILBOX %s -> USER %s",
			       mbox->name, userid);
		    }
		}
		else
		    nonuser = 1; /* there was a non-user mailbox */
	    }
	    if (!nonuser) r = 0;
	}
    }

    return r;
}

static int do_sync(sync_log_reader_t *slr)
{
    struct sync_action_list *user_list = sync_action_list_create();
    struct sync_action_list *unuser_list = sync_action_list_create();
    struct sync_action_list *meta_list = sync_action_list_create();
    struct sync_action_list *mailbox_list = sync_action_list_create();
    struct sync_action_list *unmailbox_list = sync_action_list_create();
    struct sync_action_list *quota_list = sync_action_list_create();
    struct sync_action_list *annot_list = sync_action_list_create();
    struct sync_action_list *seen_list = sync_action_list_create();
    struct sync_action_list *sub_list = sync_action_list_create();
    struct sync_name_list *mboxname_list = sync_name_list_create();
    const char *args[3];
    struct sync_action *action;
    int r = 0;

    while (1) {
	r = sync_log_reader_getitem(slr, args);
	if (r == EOF) break;

	if (!strcmp(args[0], "USER"))
	    sync_action_list_add(user_list, NULL, args[1]);
	else if (!strcmp(args[0], "UNUSER"))
	    sync_action_list_add(unuser_list, NULL, args[1]);
	else if (!strcmp(args[0], "META"))
	    sync_action_list_add(meta_list, NULL, args[1]);
	else if (!strcmp(args[0], "SIEVE"))
	    sync_action_list_add(meta_list, NULL, args[1]);
	else if (!strcmp(args[0], "MAILBOX"))
	    sync_action_list_add(mailbox_list, args[1], NULL);
	else if (!strcmp(args[0], "UNMAILBOX"))
	    sync_action_list_add(unmailbox_list, args[1], NULL);
	else if (!strcmp(args[0], "QUOTA"))
	    sync_action_list_add(quota_list, args[1], NULL);
	else if (!strcmp(args[0], "ANNOTATION"))
	    sync_action_list_add(annot_list, args[1], NULL);
	else if (!strcmp(args[0], "SEEN"))
	    sync_action_list_add(seen_list, args[2], args[1]);
	else if (!strcmp(args[0], "SUB"))
	    sync_action_list_add(sub_list, args[2], args[1]);
	else if (!strcmp(args[0], "UNSUB"))
	    sync_action_list_add(sub_list, args[2], args[1]);
	else
	    syslog(LOG_ERR, "Unknown action type: %s", args[0]);
    }

    /* Optimise out redundant clauses */

    for (action = user_list->head; action; action = action->next) {
	/* remove per-user items */
	remove_meta(action->user, meta_list);
	remove_meta(action->user, seen_list);
	remove_meta(action->user, sub_list);
    }

    /* duplicate removal for unuser - we also strip all the user events */
    for (action = unuser_list->head; action; action = action->next) {
	/* remove per-user items */
	remove_meta(action->user, meta_list);
	remove_meta(action->user, seen_list);
	remove_meta(action->user, sub_list);

	/* unuser trumps user */
	remove_meta(action->user, user_list);
    }

    for (action = meta_list->head; action; action = action->next) {
	/* META action overrides any user SEEN or SUB/UNSUB action
	   for same user */
	remove_meta(action->user, seen_list);
	remove_meta(action->user, sub_list);
    }

    /* And then run tasks. */
    for (action = quota_list->head; action; action = action->next) {
	if (!action->active)
	    continue;

	if (do_quota(action->name)) {
	    /* XXX - bogus handling, should be user */
	    sync_action_list_add(mailbox_list, action->name, NULL);
	    if (verbose) {
		printf("  Promoting: QUOTA %s -> MAILBOX %s\n",
		       action->name, action->name);
	    }
	    if (verbose_logging) {
		syslog(LOG_INFO, "  Promoting: QUOTA %s -> MAILBOX %s",
		       action->name, action->name);
	    }
	}
    }

    for (action = annot_list->head; action; action = action->next) {
	if (!action->active)
	    continue;

	/* NOTE: ANNOTATION "" is a special case - it's a server
	 * annotation, hence the check for a character at the
	 * start of the name */
	if (do_annotation(action->name) && *action->name) {
	    /* XXX - bogus handling, should be ... er, something */
	    sync_action_list_add(mailbox_list, action->name, NULL);
	    if (verbose) {
		printf("  Promoting: ANNOTATION %s -> MAILBOX %s\n",
		       action->name, action->name);
	    }
	    if (verbose_logging) {
		syslog(LOG_INFO, "  Promoting: ANNOTATION %s -> MAILBOX %s",
		       action->name, action->name);
	    }
	}
    }

    for (action = seen_list->head; action; action = action->next) {
	if (!action->active)
	    continue;

	if (do_seen(action->user, action->name)) {
	    char *userid = mboxname_isusermailbox(action->name, 1);
	    if (userid && !strcmp(userid, action->user)) {
		sync_action_list_add(user_list, NULL, action->user);
		if (verbose) {
		    printf("  Promoting: SEEN %s %s -> USER %s\n",
			   action->user, action->name, action->user);
		}
		if (verbose_logging) {
		    syslog(LOG_INFO, "  Promoting: SEEN %s %s -> USER %s",
			   action->user, action->name, action->user);
		}
	    } else {
		sync_action_list_add(meta_list, NULL, action->user);
		if (verbose) {
		    printf("  Promoting: SEEN %s %s -> META %s\n",
			   action->user, action->name, action->user);
		}
		if (verbose_logging) {
		    syslog(LOG_INFO, "  Promoting: SEEN %s %s -> META %s",
			   action->user, action->name, action->user);
		}
	    }
	}
    }

    for (action = sub_list->head; action; action = action->next) {
	if (!action->active)
	    continue;

	if (user_sub(action->user, action->name)) {
	    sync_action_list_add(meta_list, NULL, action->user);
	    if (verbose) {
		printf("  Promoting: SUB %s %s -> META %s\n",
		       action->user, action->name, action->user);
	    }
	    if (verbose_logging) {
		syslog(LOG_INFO, "  Promoting: SUB %s %s -> META %s",
		       action->user, action->name, action->name);
	    }
	}
    }

    for (action = mailbox_list->head; action; action = action->next) {
	if (!action->active)
	    continue;

	sync_name_list_add(mboxname_list, action->name);
	/* only do up to 1000 mailboxes at a time */
	if (mboxname_list->count > 1000) {
	    syslog(LOG_NOTICE, "sync_mailboxes: doing 1000");
	    r = do_sync_mailboxes(mboxname_list, user_list);
	    if (r) goto cleanup;
	    sync_name_list_free(&mboxname_list);
	    mboxname_list = sync_name_list_create();
	}
    }

    r = do_sync_mailboxes(mboxname_list, user_list);
    if (r) goto cleanup;

    for (action = unmailbox_list->head; action; action = action->next) {
	if (!action->active)
	    continue;
	r = do_unmailbox(action->name);
	if (r) goto cleanup;
    }

    for (action = meta_list->head; action; action = action->next) {
	if (!action->active)
	    continue;

	r = do_meta(action->user);
	if (r) {
	    if (r == IMAP_INVALID_USER) goto cleanup;

	    sync_action_list_add(user_list, NULL, action->user);
	    if (verbose) {
		printf("  Promoting: META %s -> USER %s\n",
		       action->user, action->user);
	    }
	    if (verbose_logging) {
		syslog(LOG_INFO, "  Promoting: META %s -> USER %s",
		       action->user, action->user);
	    }
	}
    }

    for (action = user_list->head; action; action = action->next) {
	if (!action->active)
	    continue;
	r = do_user(action->user);
	if (r) goto cleanup;
    }

    for (action = unuser_list->head; action; action = action->next) {
	if (!action->active)
	    continue;
	r = do_unuser(action->user);
	if (r) goto cleanup;
    }

  cleanup:
    if (r) {
	if (verbose)
	    fprintf(stderr, "Error in do_sync(): bailing out! %s\n", error_message(r));

	syslog(LOG_ERR, "Error in do_sync(): bailing out! %s", error_message(r));
    }

    sync_action_list_free(&user_list);
    sync_action_list_free(&unuser_list);
    sync_action_list_free(&meta_list);
    sync_action_list_free(&mailbox_list);
    sync_action_list_free(&unmailbox_list);
    sync_action_list_free(&quota_list);
    sync_action_list_free(&annot_list);
    sync_action_list_free(&seen_list);
    sync_action_list_free(&sub_list);
    sync_name_list_free(&mboxname_list);

    return r;
}

static int do_sync_filename(const char *filename)
{
    sync_log_reader_t *slr;
    int r;

    if ((filename == NULL) || !strcmp(filename, "-"))
	slr = sync_log_reader_create_with_fd(0);    /* STDIN */
    else
	slr = sync_log_reader_create_with_filename(filename);

    r = sync_log_reader_begin(slr);
    if (!r)
	r = do_sync(slr);

    sync_log_reader_end(slr);
    sync_log_reader_free(slr);
    return r;
}


/* ====================================================================== */

enum {
    RESTART_NONE = 0,
    RESTART_NORMAL,
    RESTART_RECONNECT
};

static int do_daemon_work(const char *channel, const char *sync_shutdown_file,
		   unsigned long timeout, unsigned long min_delta,
		   int *restartp)
{
    int r = 0;
    time_t session_start;
    time_t single_start;
    int    delta;
    struct stat sbuf;
    sync_log_reader_t *slr;

    *restartp = RESTART_NONE;
    slr = sync_log_reader_create_with_channel(channel);

    session_start = time(NULL);

    while (1) {
	single_start = time(NULL);

	signals_poll();

	/* Check for shutdown file */
	if (sync_shutdown_file && !stat(sync_shutdown_file, &sbuf)) {
	    unlink(sync_shutdown_file);
	    /* Have to exit with r == 0 or do_daemon() will call us again.
	     * The value of r is unknown from calls to sync_log_reader_begin() below.
	     */
	    r = 0;
	    break;
	}

	/* See if its time to RESTART */
	if ((timeout > 0) &&
	    ((single_start - session_start) > (time_t) timeout)) {
	    *restartp = RESTART_NORMAL;
	    break;
	}

	r = sync_log_reader_begin(slr);
	if (r) {
	    /* including specifically r == IMAP_AGAIN */
	    if (min_delta > 0) {
		sleep(min_delta);
	    } else {
		usleep(100000);    /* 1/10th second */
	    }
	    continue;
	}

	/* Process the work log */
	if ((r=do_sync(slr))) {
	    syslog(LOG_ERR,
		   "Processing sync log file %s failed: %s",
		   sync_log_reader_get_file_name(slr), error_message(r));
	    break;
	}

	r = sync_log_reader_end(slr);
	if (r) break;

	delta = time(NULL) - single_start;

	if (((unsigned) delta < min_delta) && ((min_delta-delta) > 0))
	    sleep(min_delta-delta);
    }
    sync_log_reader_free(slr);

    if (*restartp == RESTART_NORMAL) {
	prot_printf(sync_out, "RESTART\r\n");
	prot_flush(sync_out);

	r = sync_parse_response("RESTART", sync_in, NULL);

	if (r) {
	    syslog(LOG_ERR, "sync_client RESTART failed: %s",
		   error_message(r));
	} else {
	    syslog(LOG_INFO, "sync_client RESTART succeeded");
	}
	r = 0;
    }

    return(r);
}

static int get_intconfig(const char *channel, const char *val)
{
    int response = -1;

    if (channel) {
	const char *result = NULL;
	char name[MAX_MAILBOX_NAME]; /* crazy long, but hey */
	snprintf(name, MAX_MAILBOX_NAME, "%s_%s", channel, val);
	result = config_getoverflowstring(name, NULL);
	if (result) response = atoi(result);
    }

    if (response == -1) {
	if (!strcmp(val, "sync_repeat_interval"))
	    response = config_getint(IMAPOPT_SYNC_REPEAT_INTERVAL);
    }

    return response;
}

static const char *get_config(const char *channel, const char *val)
{
    const char *response = NULL;

    if (channel) {
	char name[MAX_MAILBOX_NAME]; /* crazy long, but hey */
	snprintf(name, MAX_MAILBOX_NAME, "%s_%s", channel, val);
	response = config_getoverflowstring(name, NULL);
    }

    if (!response) {
	/* get the core value */
	if (!strcmp(val, "sync_host"))
	    response = config_getstring(IMAPOPT_SYNC_HOST);
	else if (!strcmp(val, "sync_authname"))
	    response = config_getstring(IMAPOPT_SYNC_AUTHNAME);
	else if (!strcmp(val, "sync_password"))
	    response = config_getstring(IMAPOPT_SYNC_PASSWORD);
	else if (!strcmp(val, "sync_realm"))
	    response = config_getstring(IMAPOPT_SYNC_REALM);
	else if (!strcmp(val, "sync_port"))
	    response = config_getstring(IMAPOPT_SYNC_PORT);
	else if (!strcmp(val, "sync_shutdown_file"))
	    response = config_getstring(IMAPOPT_SYNC_SHUTDOWN_FILE);
	else
	    fatal("unknown config variable requested", EC_SOFTWARE);
    }

    return response;
}

static void replica_connect(const char *channel)
{
    int wait;
    sasl_callback_t *cb;
    int timeout;

    cb = mysasl_callbacks(NULL,
			  get_config(channel, "sync_authname"),
			  get_config(channel, "sync_realm"),
			  get_config(channel, "sync_password"));

    /* get the right port */
    csync_protocol.service = get_config(channel, "sync_port");

    for (wait = 15;; wait *= 2) {
	sync_backend = backend_connect(sync_backend, servername,
				       &csync_protocol, "", cb, NULL,
				       (verbose > 1 ? fileno(stderr) : -1));

	if (sync_backend || connect_once || wait > 1000) break;

	fprintf(stderr,
		"Can not connect to server '%s', retrying in %d seconds\n",
		servername, wait);
	sleep(wait);
    }

    free_callbacks(cb);
    cb = NULL;

    if (!sync_backend) {
	fprintf(stderr, "Can not connect to server '%s'\n",
		servername);
	syslog(LOG_ERR, "Can not connect to server '%s'", servername);
	_exit(1);
    }

    if (servername[0] != '/' && sync_backend->sock >= 0) {
	tcp_disable_nagle(sync_backend->sock);
	tcp_enable_keepalive(sync_backend->sock);
    }

#ifdef HAVE_ZLIB
    /* Does the backend support compression? */
    if (CAPA(sync_backend, CAPA_COMPRESS)) {
	prot_printf(sync_backend->out, "COMPRESS DEFLATE\r\n");
	prot_flush(sync_backend->out);

	if (sync_parse_response("COMPRESS", sync_backend->in, NULL)) {
	    if (do_compress) fatal("Failed to enable compression, aborting", EC_SOFTWARE);
	    syslog(LOG_NOTICE, "Failed to enable compression, continuing uncompressed");
	}
	else {
	    prot_setcompress(sync_backend->in);
	    prot_setcompress(sync_backend->out);
	}
    }
    else if (do_compress) fatal("Backend does not support compression, aborting", EC_SOFTWARE);
#endif

    /* links to sockets */
    sync_in = sync_backend->in;
    sync_out = sync_backend->out;

    if (verbose > 1) {
	prot_setlog(sync_in, fileno(stderr));
	prot_setlog(sync_out, fileno(stderr));
    }

    /* Set inactivity timer */
    timeout = config_getint(IMAPOPT_SYNC_TIMEOUT);
    if (timeout < 3) timeout = 3;
    prot_settimeout(sync_in, timeout);

    /* Force use of LITERAL+ so we don't need two way communications */
    prot_setisclient(sync_in, 1);
    prot_setisclient(sync_out, 1);
}

static void replica_disconnect(void)
{
    backend_disconnect(sync_backend);
}

static void do_daemon(const char *channel, const char *sync_shutdown_file,
		      unsigned long timeout, unsigned long min_delta)
{
    int r = 0;
    int restart = 1;

    signal(SIGPIPE, SIG_IGN); /* don't fail on server disconnects */

    while (restart) {
	replica_connect(channel);
	r = do_daemon_work(channel, sync_shutdown_file,
			   timeout, min_delta, &restart);
	if (r) {
	    /* See if we're still connected to the server.
	     * If we are, we had some type of error, so we exit.
	     * Otherwise, try reconnecting.
	     */
	    if (!backend_ping(sync_backend, NULL)) restart = 1;
	}
	replica_disconnect();
    }
}

static int do_mailbox(const char *mboxname)
{
    struct sync_name_list *list = sync_name_list_create();
    int r;

    sync_name_list_add(list, mboxname);

    r = do_mailboxes(list);

    sync_name_list_free(&list);

    return r;
}

static int cb_allmbox(void *rock __attribute__((unused)),
		      const char *key, size_t keylen,
		      const char *val __attribute__((unused)),
		      size_t vallen __attribute__((unused)))
{
    char *mboxname = xstrndup(key, keylen);
    const char *userid;
    int r = 0;

    userid = mboxname_to_userid(mboxname);

    if (userid) {
	/* skip deleted mailboxes only because the are out of order, and you would
	 * otherwise have to sync the user twice thanks to our naive logic */
	if (mboxname_isdeletedmailbox(mboxname, NULL))
	    goto done;

	/* only sync if we haven't just done the user */
	if (strcmpsafe(userid, prev_userid)) {
	    printf("USER: %s\n", userid);
	    r = do_user(userid);
	    if (r) {
		if (verbose)
		    fprintf(stderr, "Error from do_user(%s): bailing out!\n", userid);
		syslog(LOG_ERR, "Error in do_user(%s): bailing out!", userid);
		goto done;
	    }
	    free(prev_userid);
	    prev_userid = xstrdup(userid);
	}
    }
    else {
	/* all shared mailboxes, including DELETED ones, sync alone */
	/* XXX: batch in hundreds? */
	r = do_mailbox(mboxname);
	if (r) {
	    if (verbose)
		fprintf(stderr, "Error from do_user(%s): bailing out!\n", mboxname);
	    syslog(LOG_ERR, "Error in do_user(%s): bailing out!", mboxname);
	    goto done;
	}
    }

done:
    free(mboxname);

    return r;
}

/* ====================================================================== */

static struct sasl_callback mysasl_cb[] = {
    { SASL_CB_GETOPT, (mysasl_cb_ft *) &mysasl_config, NULL },
    { SASL_CB_CANON_USER, (mysasl_cb_ft *) &mysasl_canon_user, NULL },
    { SASL_CB_LIST_END, NULL, NULL }
};

enum {
    MODE_UNKNOWN = -1,
    MODE_REPEAT,
    MODE_USER,
    MODE_ALLUSER,
    MODE_MAILBOX,
    MODE_META
};

int main(int argc, char **argv)
{
    int   opt, i = 0;
    char *alt_config     = NULL;
    char *input_filename = NULL;
    int   r = 0;
    int   exit_rc = 0;
    int   mode = MODE_UNKNOWN;
    int   wait     = 0;
    int   timeout  = 600;
    int   min_delta = 0;
    const char *channel = NULL;
    const char *sync_shutdown_file = NULL;
    char buf[512];
    FILE *file;
    int len;
    int config_virtdomains;
    struct sync_name_list *mboxname_list;
    char mailboxname[MAX_MAILBOX_BUFFER];

    if ((geteuid()) == 0 && (become_cyrus(/*is_master*/0) != 0)) {
	fatal("must run as the Cyrus user", EC_USAGE);
    }

    setbuf(stdout, NULL);

    while ((opt = getopt(argc, argv, "C:vlS:F:f:w:t:d:n:rRumsozOA")) != EOF) {
	switch (opt) {
	case 'C': /* alt config file */
	    alt_config = optarg;
	    break;

	case 'o': /* only try to connect once */
	    connect_once = 1;
	    break;

	case 'v': /* verbose */
	    verbose++;
	    break;

	case 'l': /* verbose Logging */
	    verbose_logging++;
	    break;

	case 'S': /* Socket descriptor for server */
	    servername = optarg;
	    break;

	case 'F': /* Shutdown file */
	    sync_shutdown_file = optarg;
	    break;

	case 'f': /* input_filename used by user and mailbox modes; OR
		     alternate sync_log_file used by single-run repeat mode */
	    input_filename = optarg;
	    break;

	case 'n':
	    channel = optarg;
	    break;

	case 'w':
	    wait = atoi(optarg);
	    break;

	case 't':
	    timeout = atoi(optarg);
	    break;

	case 'd':
	    min_delta = atoi(optarg);
	    break;

	case 'r':
	    background = 1;
	    /* fallthrough */

	case 'R':
	    if (mode != MODE_UNKNOWN)
		fatal("Mutually exclusive options defined", EC_USAGE);
	    mode = MODE_REPEAT;
	    break;

	case 'A':
	    if (mode != MODE_UNKNOWN)
		fatal("Mutually exclusive options defined", EC_USAGE);
	    mode = MODE_ALLUSER;
	    break;

	case 'u':
	    if (mode != MODE_UNKNOWN)
		fatal("Mutually exclusive options defined", EC_USAGE);
	    mode = MODE_USER;
	    break;

	case 'm':
	    if (mode != MODE_UNKNOWN)
		fatal("Mutually exclusive options defined", EC_USAGE);
	    mode = MODE_MAILBOX;
	    break;

	case 's':
	    if (mode != MODE_UNKNOWN)
		fatal("Mutually exclusive options defined", EC_USAGE);
	    mode = MODE_META;
	    break;

	case 'z':
#ifdef HAVE_ZLIB
	    do_compress = 1;
#else
	    fatal("Compress not available without zlib compiled in", EC_SOFTWARE);
#endif
	    break;

	case 'O':
	    /* don't copy changes back from server */
	    no_copyback = 1;
	    break;

	default:
	    usage("sync_client");
	}
    }

    if (mode == MODE_UNKNOWN)
	fatal("No replication mode specified", EC_USAGE);

    /* fork if required */
    if (background && !input_filename && !getenv("CYRUS_ISDAEMON")) {
	int pid = fork();

	if (pid == -1) {
	    perror("fork");
	    exit(1);
	}

	if (pid != 0) { /* parent */
	    exit(0);
	}
    }

    cyrus_init(alt_config, "sync_client",
	       (verbose > 1 ? CYRUSINIT_PERROR : 0),
	       CONFIG_NEED_PARTITION_DATA);

    /* get the server name if not specified */
    if (!servername)
	servername = get_config(channel, "sync_host");

    if (!servername)
	fatal("sync_host not defined", EC_SOFTWARE);

    /* Just to help with debugging, so we have time to attach debugger */
    if (wait > 0) {
	fprintf(stderr, "Waiting for %d seconds for gdb attach...\n", wait);
	sleep(wait);
    }

    /* Set namespace -- force standard (internal) */
    config_virtdomains = config_getenum(IMAPOPT_VIRTDOMAINS);
    if ((r = mboxname_init_namespace(&sync_namespace, 1)) != 0) {
	fatal(error_message(r), EC_CONFIG);
    }

    /* open the mboxlist, we'll need it for real work */
    mboxlist_init(0);
    mboxlist_open(NULL);

    /* open the quota db, we'll need it for real work */
    quotadb_init(0);
    quotadb_open(NULL);

    /* open the annotation db */
    annotate_init(NULL, NULL);
    annotatemore_open();

    signals_set_shutdown(&shut_down);
    signals_add_handlers(0);

    /* load the SASL plugins */
    global_sasl_init(1, 0, mysasl_cb);

    switch (mode) {
    case MODE_USER:
	/* Open up connection to server */
	replica_connect(channel);

	if (input_filename) {
	    if ((file=fopen(input_filename, "r")) == NULL) {
		syslog(LOG_NOTICE, "Unable to open %s: %m", input_filename);
		shut_down(1);
	    }
	    while (fgets(buf, sizeof(buf), file)) {
		/* Chomp, then ignore empty/comment lines. */
		if (((len=strlen(buf)) > 0) && (buf[len-1] == '\n'))
		    buf[--len] = '\0';

		if ((len == 0) || (buf[0] == '#'))
		    continue;

		mboxname_hiersep_tointernal(&sync_namespace, buf,
					    config_virtdomains ?
					    strcspn(buf, "@") : 0);
		if (do_user(buf)) {
		    if (verbose)
			fprintf(stderr,
				"Error from do_user(%s): bailing out!\n",
				buf);
		    syslog(LOG_ERR, "Error in do_user(%s): bailing out!",
			   buf);
		    exit_rc = 1;
		}
	    }
	    fclose(file);
	} else for (i = optind; !r && i < argc; i++) {
	    mboxname_hiersep_tointernal(&sync_namespace, argv[i],
					config_virtdomains ?
					strcspn(argv[i], "@") : 0);
	    if (do_user(argv[i])) {
		if (verbose)
		    fprintf(stderr, "Error from do_user(%s): bailing out!\n",
			    argv[i]);
		syslog(LOG_ERR, "Error in do_user(%s): bailing out!", argv[i]);
		exit_rc = 1;
	    }
	}

	replica_disconnect();
	break;

    case MODE_ALLUSER:
	/* Open up connection to server */
	replica_connect(channel);

	if (mboxlist_allmbox(optind < argc ? argv[optind] : NULL, cb_allmbox, NULL, 0))
	    exit_rc = 1;

	replica_disconnect();
	break;

    case MODE_MAILBOX:
	/* Open up connection to server */
	replica_connect(channel);

	mboxname_list = sync_name_list_create();
	if (input_filename) {
	    if ((file=fopen(input_filename, "r")) == NULL) {
		syslog(LOG_NOTICE, "Unable to open %s: %m", input_filename);
		shut_down(1);
	    }
	    while (fgets(buf, sizeof(buf), file)) {
		/* Chomp, then ignore empty/comment lines. */
		if (((len=strlen(buf)) > 0) && (buf[len-1] == '\n'))
		    buf[--len] = '\0';

		if ((len == 0) || (buf[0] == '#'))
		    continue;

		(*sync_namespace.mboxname_tointernal)(&sync_namespace, buf,
						      NULL, mailboxname);
		if (!sync_name_lookup(mboxname_list, mailboxname))
		    sync_name_list_add(mboxname_list, mailboxname);
	    }
	    fclose(file);
	} else for (i = optind; i < argc; i++) {
	    (*sync_namespace.mboxname_tointernal)(&sync_namespace, argv[i],
						   NULL, mailboxname);
	    if (!sync_name_lookup(mboxname_list, mailboxname))
		sync_name_list_add(mboxname_list, mailboxname);
	}

	if (do_mailboxes(mboxname_list)) {
	    if (verbose) {
		fprintf(stderr,
			"Error from do_mailboxes(): bailing out!\n");
	    }
	    syslog(LOG_ERR, "Error in do_mailboxes(): bailing out!");
	    exit_rc = 1;
	}

	sync_name_list_free(&mboxname_list);
	replica_disconnect();
	break;

    case MODE_META:
	/* Open up connection to server */
	replica_connect(channel);

	for (i = optind; i < argc; i++) {
	    mboxname_hiersep_tointernal(&sync_namespace, argv[i],
					config_virtdomains ?
					strcspn(argv[i], "@") : 0);
	    if (do_meta(argv[i])) {
		if (verbose) {
		    fprintf(stderr,
			    "Error from do_meta(%s): bailing out!\n",
			    argv[i]);
		}
		syslog(LOG_ERR, "Error in do_meta(%s): bailing out!",
		       argv[i]);
		exit_rc = 1;
	    }
	}

	replica_disconnect();

	break;

    case MODE_REPEAT:
	if (input_filename) {
	    /* Open up connection to server */
	    replica_connect(channel);

	    exit_rc = do_sync_filename(input_filename);

	    replica_disconnect();
	}
	else {
	    /* rolling replication */
	    if (!sync_shutdown_file)
		sync_shutdown_file = get_config(channel, "sync_shutdown_file");

	    if (!min_delta)
		min_delta = get_intconfig(channel, "sync_repeat_interval");

	    do_daemon(channel, sync_shutdown_file, timeout, min_delta);
	}

	break;

    default:
	if (verbose) fprintf(stderr, "Nothing to do!\n");
	break;
    }

    shut_down(exit_rc);
}
