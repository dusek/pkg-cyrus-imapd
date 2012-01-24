/* message_guid.c -- GUID manipulation
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
 * $Id: message_guid.c,v 1.9 2010/01/06 17:01:37 murch Exp $
 */

#include <config.h>
#include <string.h>
#include <ctype.h>

#include "assert.h"
#include "global.h"
#include "message_guid.h"
#include "util.h"

#ifdef HAVE_SSL
#include <openssl/sha.h>
#define our_sha1 SHA1
#else
/*
 * sha1.c
 *
 * Originally witten by Steve Reid <steve@edmweb.com>
 * 
 * Modified by Aaron D. Gifford <agifford@infowest.com>
 *
 * NO COPYRIGHT - THIS IS 100% IN THE PUBLIC DOMAIN
 *
 * The original unmodified version is available at:
 *    ftp://ftp.funet.fi/pub/crypt/hash/sha/sha1.c
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR(S) OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* to limit changes to the code below, set up the right types here */

typedef uint32_t sha1_quadbyte; /* 4 byte type */
typedef uint8_t sha1_byte;    /* single byte type */

#define SHA1_BLOCK_LENGTH   64
#define SHA1_DIGEST_LENGTH  20

/* The SHA1 structure: */
typedef struct _SHA_CTX {
    sha1_quadbyte   state[5];
    sha1_quadbyte   count[2];
    sha1_byte	buffer[SHA1_BLOCK_LENGTH];
} SHA_CTX;


/* Downloaded from http://www.aarongifford.com/computers/hmac_sha1.tar.gz
 * by Bron Gondwana <brong@fastmail.fm> on 2011-09-20
 */

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

/* blk0() and blk() perform the initial expand. */
/* I got the idea of expanding during the round function from SSLeay */

#ifdef LITTLE_ENDIAN
#define blk0(i) (block->l[i] = (rol(block->l[i],24)&(sha1_quadbyte)0xFF00FF00) \
    |(rol(block->l[i],8)&(sha1_quadbyte)0x00FF00FF))
#else
#define blk0(i) block->l[i]
#endif

#define blk(i) (block->l[i&15] = rol(block->l[(i+13)&15]^block->l[(i+8)&15] \
    ^block->l[(i+2)&15]^block->l[i&15],1))

/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
#define R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk0(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R2(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30);
#define R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+blk(i)+0x8F1BBCDC+rol(v,5);w=rol(w,30);
#define R4(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);

typedef union _BYTE64QUAD16 {
    sha1_byte c[64];
    sha1_quadbyte l[16];
} BYTE64QUAD16;

/* Hash a single 512-bit block. This is the core of the algorithm. */
static void SHA1_Transform(sha1_quadbyte state[5], const sha1_byte buffer[64]) {
    sha1_quadbyte   a, b, c, d, e;
    BYTE64QUAD16    *block;
    BYTE64QUAD16    copy;

    /* take a copy of the data */
    memcpy(&copy, buffer, 64);
    block = &copy;

    /* Copy context->state[] to working vars */
    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];
    /* 4 rounds of 20 operations each. Loop unrolled. */
    R0(a,b,c,d,e, 0); R0(e,a,b,c,d, 1); R0(d,e,a,b,c, 2); R0(c,d,e,a,b, 3);
    R0(b,c,d,e,a, 4); R0(a,b,c,d,e, 5); R0(e,a,b,c,d, 6); R0(d,e,a,b,c, 7);
    R0(c,d,e,a,b, 8); R0(b,c,d,e,a, 9); R0(a,b,c,d,e,10); R0(e,a,b,c,d,11);
    R0(d,e,a,b,c,12); R0(c,d,e,a,b,13); R0(b,c,d,e,a,14); R0(a,b,c,d,e,15);
    R1(e,a,b,c,d,16); R1(d,e,a,b,c,17); R1(c,d,e,a,b,18); R1(b,c,d,e,a,19);
    R2(a,b,c,d,e,20); R2(e,a,b,c,d,21); R2(d,e,a,b,c,22); R2(c,d,e,a,b,23);
    R2(b,c,d,e,a,24); R2(a,b,c,d,e,25); R2(e,a,b,c,d,26); R2(d,e,a,b,c,27);
    R2(c,d,e,a,b,28); R2(b,c,d,e,a,29); R2(a,b,c,d,e,30); R2(e,a,b,c,d,31);
    R2(d,e,a,b,c,32); R2(c,d,e,a,b,33); R2(b,c,d,e,a,34); R2(a,b,c,d,e,35);
    R2(e,a,b,c,d,36); R2(d,e,a,b,c,37); R2(c,d,e,a,b,38); R2(b,c,d,e,a,39);
    R3(a,b,c,d,e,40); R3(e,a,b,c,d,41); R3(d,e,a,b,c,42); R3(c,d,e,a,b,43);
    R3(b,c,d,e,a,44); R3(a,b,c,d,e,45); R3(e,a,b,c,d,46); R3(d,e,a,b,c,47);
    R3(c,d,e,a,b,48); R3(b,c,d,e,a,49); R3(a,b,c,d,e,50); R3(e,a,b,c,d,51);
    R3(d,e,a,b,c,52); R3(c,d,e,a,b,53); R3(b,c,d,e,a,54); R3(a,b,c,d,e,55);
    R3(e,a,b,c,d,56); R3(d,e,a,b,c,57); R3(c,d,e,a,b,58); R3(b,c,d,e,a,59);
    R4(a,b,c,d,e,60); R4(e,a,b,c,d,61); R4(d,e,a,b,c,62); R4(c,d,e,a,b,63);
    R4(b,c,d,e,a,64); R4(a,b,c,d,e,65); R4(e,a,b,c,d,66); R4(d,e,a,b,c,67);
    R4(c,d,e,a,b,68); R4(b,c,d,e,a,69); R4(a,b,c,d,e,70); R4(e,a,b,c,d,71);
    R4(d,e,a,b,c,72); R4(c,d,e,a,b,73); R4(b,c,d,e,a,74); R4(a,b,c,d,e,75);
    R4(e,a,b,c,d,76); R4(d,e,a,b,c,77); R4(c,d,e,a,b,78); R4(b,c,d,e,a,79);
    /* Add the working vars back into context.state[] */
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    /* Wipe variables */
    a = b = c = d = e = 0;
}


/* SHA1_Init - Initialize new context */
static void SHA1_Init(SHA_CTX* context) {
    /* SHA1 initialization constants */
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
    context->state[4] = 0xC3D2E1F0;
    context->count[0] = context->count[1] = 0;
}

/* Run your data through this. */
static void SHA1_Update(SHA_CTX *context, const sha1_byte *data, unsigned int len) {
    unsigned int    i, j;

    j = (context->count[0] >> 3) & 63;
    if ((context->count[0] += len << 3) < (len << 3)) context->count[1]++;
    context->count[1] += (len >> 29);
    if ((j + len) > 63) {
        memcpy(&context->buffer[j], data, (i = 64-j));
        SHA1_Transform(context->state, context->buffer);
        for ( ; i + 63 < len; i += 64) {
            SHA1_Transform(context->state, &data[i]);
        }
        j = 0;
    }
    else i = 0;
    memcpy(&context->buffer[j], &data[i], len - i);
}


/* Add padding and return the message digest. */
static void SHA1_Final(sha1_byte digest[SHA1_DIGEST_LENGTH], SHA_CTX *context) {
    sha1_quadbyte   i, j;
    sha1_byte	finalcount[8];

    for (i = 0; i < 8; i++) {
        finalcount[i] = (sha1_byte)((context->count[(i >= 4 ? 0 : 1)]
         >> ((3-(i & 3)) * 8) ) & 255);  /* Endian independent */
    }
    SHA1_Update(context, (sha1_byte *)"\200", 1);
    while ((context->count[0] & 504) != 448) {
        SHA1_Update(context, (sha1_byte *)"\0", 1);
    }
    /* Should cause a SHA1_Transform() */
    SHA1_Update(context, finalcount, 8);
    for (i = 0; i < SHA1_DIGEST_LENGTH; i++) {
        digest[i] = (sha1_byte)
         ((context->state[i>>2] >> ((3-(i & 3)) * 8) ) & 255);
    }
    /* Wipe variables */
    i = j = 0;
    memset(context->buffer, 0, SHA1_BLOCK_LENGTH);
    memset(context->state, 0, SHA1_DIGEST_LENGTH);
    memset(context->count, 0, 8);
    memset(&finalcount, 0, 8);
}

static void our_sha1(const unsigned char *buf, unsigned long len,
		     sha1_byte dest[SHA1_DIGEST_LENGTH])
{
    SHA_CTX ctx;

    memset(&ctx, 0, sizeof(SHA_CTX));

    SHA1_Init(&ctx);
    SHA1_Update(&ctx, buf, len);
    SHA1_Final(dest, &ctx);
}

#endif

/* Four possible forms of Message GUID:
 *
 * Private:
 *   Used for internal manipulation.  Not visible to clients.
 *
 * Public:
 *   Opaque handle to GUID that Cyrus can pass around.
 *
 *   OR
 *
 *   Byte sequence of known length (MESSAGE_GUID_SIZE) which can
 *   be stored on disk.
 *
 * Textual:
 *   Textual represenatation for Message GUID for passing over the wire
 *   Currently BASE64 string + '\0'.
 *
 */

/* ====================================================================== */


/* Public interface */

/* message_guid_generate() ***********************************************
 *
 * Generate GUID from message
 *
 ************************************************************************/

void message_guid_generate(struct message_guid *guid,
			   const char *msg_base, unsigned long msg_len)
{
    guid->status = GUID_NULL;
    memset(guid->value, 0, MESSAGE_GUID_SIZE);

    guid->status = GUID_NONNULL;
    our_sha1((const unsigned char *) msg_base, msg_len, guid->value);
}

/* message_guid_copy() ***************************************************
 *
 * Copy GUID
 *
 ************************************************************************/

void message_guid_copy(struct message_guid *dst, struct message_guid *src)
{
    memcpy(dst, src, sizeof(struct message_guid));
}

/* message_guid_equal() **************************************************
 *
 * Compare a pair of GUIDs: Returns 1 => match.
 *
 ************************************************************************/

int message_guid_equal(struct message_guid *g1,
		       struct message_guid *g2)
{
    return (memcmp(g1->value, g2->value, MESSAGE_GUID_SIZE) == 0);
}

int message_guid_cmp(struct message_guid *g1,
		       struct message_guid *g2)
{
    return memcmp(g1->value, g2->value, MESSAGE_GUID_SIZE);
}

/* message_guid_hash() ***************************************************
 *
 * Convert GUID into hash value for hash table lookup
 * Returns: positive int in range [0, hash_size-1]
 *
 ************************************************************************/

unsigned long message_guid_hash(struct message_guid *guid, int hash_size)
{
    int i;
    unsigned long result = 0;
    unsigned char *s = &guid->value[0];

    assert(hash_size > 1);

    if (hash_size > 1024) {
        /* Pair up chars to get 16 bit values */
        for (i = 0; i < MESSAGE_GUID_SIZE; i += 2) 
	    result += (s[i] << 8) + s[i+1];
    } 
    else 
	for (i = 0; i < MESSAGE_GUID_SIZE; i++)
	    result += s[i];

    return (result % hash_size);
}

/* message_guid_set_null() ***********************************************
 *
 * Create NULL GUID
 *
 ************************************************************************/

void message_guid_set_null(struct message_guid *guid)
{
    guid->status = GUID_NULL;
    memset(guid->value, 0, MESSAGE_GUID_SIZE);
}

/* message_guid_isnull() ************************************************
 *
 * Returns: 1 if GUID is NULL value
 *
 ************************************************************************/

int message_guid_isnull(struct message_guid *guid)
{
    if (guid->status == GUID_UNKNOWN) {
	unsigned char *p = guid->value;
	int i;

	for (i = 0; (i < MESSAGE_GUID_SIZE) && !*p++; i++);
	guid->status = (i == MESSAGE_GUID_SIZE) ? GUID_NULL : GUID_NONNULL;
    }

    return(guid->status == GUID_NULL);
}

/* message_guid_export() *************************************************
 *
 * Export Message GUID as byte sequence (MESSAGE_GUID_SIZE)
 * (Wrapper for memcpy() with current implementation)
 *
 ************************************************************************/

void message_guid_export(const struct message_guid *guid,
			 unsigned char *buf)
{
    memcpy(buf, guid->value, MESSAGE_GUID_SIZE);
}

/* message_guid_import() *************************************************
 *
 * Import Message GUID from byte sequence (MESSAGE_GUID_SIZE)
 * (Wrapper for memcpy() with current implementation)
 *
 ************************************************************************/

struct message_guid *message_guid_import(struct message_guid *guid,
					 const unsigned char *buf)
{
    static struct message_guid tmp;

    if (!guid) guid = &tmp;

    guid->status = GUID_UNKNOWN;
    memcpy(guid->value, buf, MESSAGE_GUID_SIZE);

    return(guid);
}


/* Routines for manipulating text value (ASCII hex encoding) */

/* message_guid_encode() *************************************************
 *
 * Returns ptr to '\0' terminated static char * which can be strdup()ed
 * NULL => error. Should be impossible as entire range covered
 *
 ************************************************************************/

static char XDIGIT[] = "0123456789abcdef";

char *message_guid_encode(const struct message_guid *guid)
{
    static char text[2*MESSAGE_GUID_SIZE+1];
    const unsigned char *v = guid->value;
    char *p = text;
    int i;

    for (i = 0; i < MESSAGE_GUID_SIZE; i++, v++) {
        *p++ = XDIGIT[(*v >> 4) & 0xf];
        *p++ = XDIGIT[*v & 0xf];
    }
    *p = '\0';

    return(text);
}

/* message_guid_decode() *************************************************
 *
 * Sets Message GUID from text form. Returns 1 if valid
 * Returns: boolean success
 * 
 ************************************************************************/

int message_guid_decode(struct message_guid *guid, const char *text)
{
    unsigned char *v = guid->value, msn, lsn;
    const char *p = text;
    int i;

    guid->status = GUID_NULL;

    for (i = 0; i < MESSAGE_GUID_SIZE; i++, v++) {
	if (!Uisxdigit(*p)) return(0);
	msn = (*p > '9') ? tolower((int) *p) - 'a' + 10 : *p - '0';
	p++;

	if (!Uisxdigit(*p)) return(0);
	lsn = (*p > '9') ? tolower((int) *p) - 'a' + 10 : *p - '0';
	p++;
	
	*v = (unsigned char) (msn << 4) | lsn;
	if (*v) guid->status = GUID_NONNULL;
    }

    return(*p == '\0');
}
