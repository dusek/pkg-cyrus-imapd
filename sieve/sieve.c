#ifndef lint
static const char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif

#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYPATCH 20101229

#define YYEMPTY        (-1)
#define yyclearin      (yychar = YYEMPTY)
#define yyerrok        (yyerrflag = 0)
#define YYRECOVERING() (yyerrflag != 0)

#define YYPREFIX "yy"

#define YYPURE 0

#line 2 "sieve.y"
/* sieve.y -- sieve parser
 * Larry Greenfield
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
 * $Id: sieve.y,v 1.45.2.1 2010/02/12 03:41:11 brong Exp $
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "xmalloc.h"
#include "comparator.h"
#include "interp.h"
#include "script.h"
#include "tree.h"

#include "../lib/imapurl.h"
#include "../lib/util.h"
#include "../lib/imparse.h"
#include "../lib/libconfig.h"

#define ERR_BUF_SIZE 1024

char errbuf[ERR_BUF_SIZE];

    /* definitions */
    extern int addrparse(void);

struct vtags {
    int days;
    stringlist_t *addresses;
    char *subject;
    char *from;
    char *handle;
    int mime;
};

struct htags {
    char *comparator;
    int comptag;
    int relation;
};

struct aetags {
    int addrtag;
    char *comparator;
    int comptag;
    int relation;
};

struct btags {
    int transform;
    int offset;
    stringlist_t *content_types;
    char *comparator;
    int comptag;
    int relation;
};

struct ntags {
    char *method;
    char *id;
    stringlist_t *options;
    int priority;
    char *message;
};

struct dtags {
    int comptag;
    int relation;
    void *pattern;
    int priority;
};

static commandlist_t *ret;
static sieve_script_t *parse_script;
static char *check_reqs(stringlist_t *sl);
static test_t *build_address(int t, struct aetags *ae,
			     stringlist_t *sl, stringlist_t *pl);
static test_t *build_header(int t, struct htags *h,
			    stringlist_t *sl, stringlist_t *pl);
static test_t *build_body(int t, struct btags *b, stringlist_t *pl);
static commandlist_t *build_vacation(int t, struct vtags *h, char *s);
static commandlist_t *build_notify(int t, struct ntags *n);
static commandlist_t *build_denotify(int t, struct dtags *n);
static commandlist_t *build_fileinto(int t, int c, char *f);
static commandlist_t *build_redirect(int t, int c, char *a);
static struct aetags *new_aetags(void);
static struct aetags *canon_aetags(struct aetags *ae);
static void free_aetags(struct aetags *ae);
static struct htags *new_htags(void);
static struct htags *canon_htags(struct htags *h);
static void free_htags(struct htags *h);
static struct btags *new_btags(void);
static struct btags *canon_btags(struct btags *b);
static void free_btags(struct btags *b);
static struct vtags *new_vtags(void);
static struct vtags *canon_vtags(struct vtags *v);
static void free_vtags(struct vtags *v);
static struct ntags *new_ntags(void);
static struct ntags *canon_ntags(struct ntags *n);
static void free_ntags(struct ntags *n);
static struct dtags *new_dtags(void);
static struct dtags *canon_dtags(struct dtags *d);
static void free_dtags(struct dtags *d);

static int verify_stringlist(stringlist_t *sl, int (*verify)(char *));
static int verify_mailbox(char *s);
static int verify_address(char *s);
static int verify_header(char *s);
static int verify_addrheader(char *s);
static int verify_envelope(char *s);
static int verify_flag(char *s);
static int verify_relat(char *s);
#ifdef ENABLE_REGEX
static int verify_regex(char *s, int cflags);
static int verify_regexs(stringlist_t *sl, char *comp);
#endif
static int verify_utf8(char *s);

int yyerror(char *msg);
extern int yylex(void);
extern void yyrestart(FILE *f);

#define YYERROR_VERBOSE /* i want better error messages! */

/* byacc default is 500, bison default is 10000 - go with the
   larger to support big sieve scripts (see Bug #3461) */
#define YYSTACKSIZE 10000
#line 176 "sieve.y"
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union {
    int nval;
    char *sval;
    stringlist_t *sl;
    test_t *test;
    testlist_t *testl;
    commandlist_t *cl;
    struct vtags *vtag;
    struct aetags *aetag;
    struct htags *htag;
    struct btags *btag;
    struct ntags *ntag;
    struct dtags *dtag;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
#line 214 "y.tab.c"
/* compatibility with bison */
#ifdef YYPARSE_PARAM
/* compatibility with FreeBSD */
# ifdef YYPARSE_PARAM_TYPE
#  define YYPARSE_DECL() yyparse(YYPARSE_PARAM_TYPE YYPARSE_PARAM)
# else
#  define YYPARSE_DECL() yyparse(void *YYPARSE_PARAM)
# endif
#else
# define YYPARSE_DECL() yyparse(void)
#endif

/* Parameters sent to lex. */
#ifdef YYLEX_PARAM
# define YYLEX_DECL() yylex(void *YYLEX_PARAM)
# define YYLEX yylex(YYLEX_PARAM)
#else
# define YYLEX_DECL() yylex(void)
# define YYLEX yylex()
#endif

/* Parameters sent to yyerror. */
#define YYERROR_DECL() yyerror(const char *s)
#define YYERROR_CALL(msg) yyerror(msg)

extern int YYPARSE_DECL();

#define NUMBER 257
#define STRING 258
#define IF 259
#define ELSIF 260
#define ELSE 261
#define REJCT 262
#define FILEINTO 263
#define REDIRECT 264
#define KEEP 265
#define STOP 266
#define DISCARD 267
#define VACATION 268
#define REQUIRE 269
#define SETFLAG 270
#define ADDFLAG 271
#define REMOVEFLAG 272
#define MARK 273
#define UNMARK 274
#define NOTIFY 275
#define DENOTIFY 276
#define ANYOF 277
#define ALLOF 278
#define EXISTS 279
#define SFALSE 280
#define STRUE 281
#define HEADER 282
#define NOT 283
#define SIZE 284
#define ADDRESS 285
#define ENVELOPE 286
#define BODY 287
#define COMPARATOR 288
#define IS 289
#define CONTAINS 290
#define MATCHES 291
#define REGEX 292
#define COUNT 293
#define VALUE 294
#define OVER 295
#define UNDER 296
#define GT 297
#define GE 298
#define LT 299
#define LE 300
#define EQ 301
#define NE 302
#define ALL 303
#define LOCALPART 304
#define DOMAIN 305
#define USER 306
#define DETAIL 307
#define RAW 308
#define TEXT 309
#define CONTENT 310
#define DAYS 311
#define ADDRESSES 312
#define SUBJECT 313
#define FROM 314
#define HANDLE 315
#define MIME 316
#define METHOD 317
#define ID 318
#define OPTIONS 319
#define LOW 320
#define NORMAL 321
#define HIGH 322
#define ANY 323
#define MESSAGE 324
#define INCLUDE 325
#define PERSONAL 326
#define GLOBAL 327
#define RETURN 328
#define COPY 329
#define YYERRCODE 256
static const short yylhs[] = {                           -1,
    0,    0,   25,   25,   26,    1,    1,    2,    2,    2,
    4,    4,    4,    3,    3,    3,    3,    3,    3,    3,
    3,    3,    3,    3,    3,    3,    3,    3,    3,   14,
   14,   14,   22,   22,   22,   22,   22,   22,   23,   23,
   23,   23,   24,   24,   24,   21,   21,   21,   21,   21,
   21,   21,    6,    6,    7,    7,    5,    5,    8,    8,
    8,    8,    8,    8,    8,    8,    8,    8,    8,   13,
   13,   19,   19,   19,   19,   19,   18,   18,   18,   18,
   20,   20,   20,   20,   20,   20,   20,   12,   12,   12,
   12,   12,    9,    9,    9,    9,   10,   10,   11,   11,
   15,   15,   16,   17,   17,
};
static const short yylen[] = {                            2,
    1,    2,    0,    2,    3,    1,    2,    2,    4,    2,
    0,    4,    2,    2,    3,    3,    1,    1,    1,    3,
    2,    2,    2,    1,    1,    2,    2,    3,    1,    0,
    1,    1,    0,    3,    3,    3,    2,    3,    0,    2,
    3,    3,    1,    1,    1,    0,    3,    3,    3,    3,
    3,    2,    3,    1,    1,    3,    3,    2,    2,    2,
    2,    1,    1,    4,    4,    3,    2,    3,    1,    1,
    1,    0,    2,    2,    3,    3,    0,    2,    3,    3,
    0,    2,    2,    3,    2,    3,    3,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    0,    1,    3,    1,    3,
};
static const short yydefred[] = {                         0,
    0,    0,    0,    0,   54,    0,    0,    0,    0,    0,
    0,    0,   17,   18,   19,   46,    0,    0,    0,   24,
   25,   33,   39,    0,   29,    2,    0,    0,    4,   55,
    0,    5,   10,   69,    0,    0,    0,   62,   63,   77,
    0,    0,   70,   71,   81,    0,   72,   14,  102,    0,
    0,    0,   21,   22,   23,    0,    0,   31,   32,    0,
    7,    8,   53,    0,    0,   59,   60,   61,    0,   67,
   99,  100,    0,    0,    0,    0,    0,   15,   16,   20,
    0,    0,    0,    0,    0,   52,    0,    0,    0,   43,
   44,   45,    0,   37,   93,   94,   95,   96,   97,   98,
    0,    0,   40,   28,   56,    0,    0,    0,    0,   78,
    0,   68,    0,   82,   83,    0,   66,   85,    0,   58,
    0,    0,    0,    9,    0,   88,   89,   90,   91,   92,
    0,   74,    0,   73,   47,   48,   49,   50,   51,   35,
   34,   36,   38,   41,   42,    0,  103,   80,   64,   79,
   87,   84,   86,   57,    0,   13,   76,   65,   75,  105,
    0,   12,
};
static const short yydgoto[] = {                          2,
   26,   27,   28,  124,   76,    7,   31,  106,  101,  102,
   73,  134,   47,   60,   50,   66,  107,   69,   77,   74,
   52,   56,   57,   94,    3,    4,
};
static const short yysindex[] = {                      -253,
  -89,    0, -230, -253,    0, -235,  -34,  -32, -196, -228,
 -298, -298,    0,    0,    0,    0,  -89,  -89,  -89,    0,
    0,    0,    0, -313,    0,    0, -230,    4,    0,    0,
  -35,    0,    0,    0,   26,   26,  -89,    0,    0,    0,
 -196, -276,    0,    0,    0,  -56,    0,    0,    0, -190,
 -181, -241,    0,    0,    0, -213, -242,    0,    0, -166,
    0,    0,    0, -165, -196,    0,    0,    0,  -63,    0,
    0,    0, -163,  -91, -113, -206,  -83,    0,    0,    0,
 -160,  -89, -159, -158, -157,    0, -156, -148,  -89,    0,
    0,    0, -146,    0,    0,    0,    0,    0,    0,    0,
 -145, -144,    0,    0,    0,   71,   75, -141,  -89,    0,
 -139,    0, -137,    0,    0,  -89,    0,    0, -136,    0,
   -1, -196,  -56,    0, -135,    0,    0,    0,    0,    0,
  -89,    0, -133,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0, -196,    0,    0,    0,    0,
    0,    0,    0,    0,  -56,    0,    0,    0,    0,    0,
 -206,    0,
};
static const short yyrindex[] = {                        22,
    0,    0,  127,   22,    0,    0,    0,    0,    0,    0,
 -129, -129,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0, -235,    0,    0,    3,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   72,   73,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    1,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   89,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    1,    0,
};
static const short yygindex[] = {                         0,
  -16,    0,    0,  -28, -102,  -13,    0,   -2,  -59,  -12,
    0,    0,    0,    0,  122,   99,  -10,    0,    0,    0,
    0,    0,    0,   80,  134,    0,
};
#define YYTABLESIZE 350
static const short yytable[] = {                          6,
   11,    6,    6,   53,   54,   55,   46,    6,   64,  110,
   61,  120,   58,   59,  118,    1,   80,  132,   71,   72,
  156,    3,   30,   68,   32,    8,   33,    6,    9,   48,
   49,   10,   11,   12,   13,   14,   15,   16,   70,   17,
   18,   19,   20,   21,   22,   23,   95,   96,   97,   98,
   99,  100,  161,  122,  123,  109,  111,   63,  121,   34,
  117,  119,   62,  131,  133,   65,   75,   78,  136,   81,
   82,   83,   84,   85,   86,  142,   79,   90,   91,   92,
   35,   36,   37,   38,   39,   40,   41,   42,   43,   44,
   45,  104,  105,  112,   24,  149,  135,   25,  137,  138,
  139,  140,  152,   87,   88,   89,   90,   91,   92,  141,
   93,  143,  144,  145,  146,  147,  148,  158,  150,  155,
  151,  153,  157,  154,  159,   11,    1,    6,  101,  104,
   26,   27,  162,   51,   67,  160,  103,   29,    0,    0,
    0,    0,    8,    0,    0,    9,    0,    0,   10,   11,
   12,   13,   14,   15,   16,    0,   17,   18,   19,   20,
   21,   22,   23,    0,    0,    0,    5,    0,    5,    0,
    0,    0,    0,    0,    5,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    5,    0,  113,   95,   96,   97,
   98,   99,  100,    0,  125,   95,   96,   97,   98,   99,
  100,   24,    0,    0,   25,    0,  114,  115,  116,  126,
  127,  128,  129,  130,  108,   95,   96,   97,   98,   99,
  100,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   11,    0,    0,   11,
    0,    0,   11,   11,   11,   11,   11,   11,   11,    0,
   11,   11,   11,   11,   11,   11,   11,    3,    0,    0,
    3,    0,    0,    3,    3,    3,    3,    3,    3,    3,
    0,    3,    3,    3,    3,    3,    3,    3,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   11,    0,    0,   11,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    3,    0,    0,    3,
};
static const short yycheck[] = {                         91,
    0,   91,    0,   17,   18,   19,    9,   91,   44,   69,
   27,  125,  326,  327,   74,  269,  258,   77,  295,  296,
  123,    0,  258,   37,   59,  256,   59,   91,  259,  258,
  329,  262,  263,  264,  265,  266,  267,  268,   41,  270,
  271,  272,  273,  274,  275,  276,  289,  290,  291,  292,
  293,  294,  155,  260,  261,   69,   69,   93,   75,  256,
   74,   74,   59,   77,   77,   40,  123,  258,   82,  311,
  312,  313,  314,  315,  316,   89,  258,  320,  321,  322,
  277,  278,  279,  280,  281,  282,  283,  284,  285,  286,
  287,  258,  258,  257,  325,  109,  257,  328,  258,  258,
  258,  258,  116,  317,  318,  319,  320,  321,  322,  258,
  324,  258,  258,  258,   44,   41,  258,  131,  258,  122,
  258,  258,  258,  125,  258,  125,    0,  125,  258,   41,
   59,   59,  161,   12,   36,  146,   57,    4,   -1,   -1,
   -1,   -1,  256,   -1,   -1,  259,   -1,   -1,  262,  263,
  264,  265,  266,  267,  268,   -1,  270,  271,  272,  273,
  274,  275,  276,   -1,   -1,   -1,  258,   -1,  258,   -1,
   -1,   -1,   -1,   -1,  258,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  258,   -1,  288,  289,  290,  291,
  292,  293,  294,   -1,  288,  289,  290,  291,  292,  293,
  294,  325,   -1,   -1,  328,   -1,  308,  309,  310,  303,
  304,  305,  306,  307,  288,  289,  290,  291,  292,  293,
  294,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  256,   -1,   -1,  259,
   -1,   -1,  262,  263,  264,  265,  266,  267,  268,   -1,
  270,  271,  272,  273,  274,  275,  276,  256,   -1,   -1,
  259,   -1,   -1,  262,  263,  264,  265,  266,  267,  268,
   -1,  270,  271,  272,  273,  274,  275,  276,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  325,   -1,   -1,  328,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  325,   -1,   -1,  328,
};
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 329
#if YYDEBUG
static const char *yyname[] = {

"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"'('","')'",0,0,"','",0,0,0,0,0,0,0,0,0,0,0,0,0,0,"';'",0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'['",0,"']'",0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'{'",0,"'}'",0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"NUMBER","STRING","IF","ELSIF","ELSE","REJCT","FILEINTO","REDIRECT","KEEP",
"STOP","DISCARD","VACATION","REQUIRE","SETFLAG","ADDFLAG","REMOVEFLAG","MARK",
"UNMARK","NOTIFY","DENOTIFY","ANYOF","ALLOF","EXISTS","SFALSE","STRUE","HEADER",
"NOT","SIZE","ADDRESS","ENVELOPE","BODY","COMPARATOR","IS","CONTAINS","MATCHES",
"REGEX","COUNT","VALUE","OVER","UNDER","GT","GE","LT","LE","EQ","NE","ALL",
"LOCALPART","DOMAIN","USER","DETAIL","RAW","TEXT","CONTENT","DAYS","ADDRESSES",
"SUBJECT","FROM","HANDLE","MIME","METHOD","ID","OPTIONS","LOW","NORMAL","HIGH",
"ANY","MESSAGE","INCLUDE","PERSONAL","GLOBAL","RETURN","COPY",
};
static const char *yyrule[] = {
"$accept : start",
"start : reqs",
"start : reqs commands",
"reqs :",
"reqs : require reqs",
"require : REQUIRE stringlist ';'",
"commands : command",
"commands : command commands",
"command : action ';'",
"command : IF test block elsif",
"command : error ';'",
"elsif :",
"elsif : ELSIF test block elsif",
"elsif : ELSE block",
"action : REJCT STRING",
"action : FILEINTO copy STRING",
"action : REDIRECT copy STRING",
"action : KEEP",
"action : STOP",
"action : DISCARD",
"action : VACATION vtags STRING",
"action : SETFLAG stringlist",
"action : ADDFLAG stringlist",
"action : REMOVEFLAG stringlist",
"action : MARK",
"action : UNMARK",
"action : NOTIFY ntags",
"action : DENOTIFY dtags",
"action : INCLUDE location STRING",
"action : RETURN",
"location :",
"location : PERSONAL",
"location : GLOBAL",
"ntags :",
"ntags : ntags ID STRING",
"ntags : ntags METHOD STRING",
"ntags : ntags OPTIONS stringlist",
"ntags : ntags priority",
"ntags : ntags MESSAGE STRING",
"dtags :",
"dtags : dtags priority",
"dtags : dtags comptag STRING",
"dtags : dtags relcomp STRING",
"priority : LOW",
"priority : NORMAL",
"priority : HIGH",
"vtags :",
"vtags : vtags DAYS NUMBER",
"vtags : vtags ADDRESSES stringlist",
"vtags : vtags SUBJECT STRING",
"vtags : vtags FROM STRING",
"vtags : vtags HANDLE STRING",
"vtags : vtags MIME",
"stringlist : '[' strings ']'",
"stringlist : STRING",
"strings : STRING",
"strings : strings ',' STRING",
"block : '{' commands '}'",
"block : '{' '}'",
"test : ANYOF testlist",
"test : ALLOF testlist",
"test : EXISTS stringlist",
"test : SFALSE",
"test : STRUE",
"test : HEADER htags stringlist stringlist",
"test : addrorenv aetags stringlist stringlist",
"test : BODY btags stringlist",
"test : NOT test",
"test : SIZE sizetag NUMBER",
"test : error",
"addrorenv : ADDRESS",
"addrorenv : ENVELOPE",
"aetags :",
"aetags : aetags addrparttag",
"aetags : aetags comptag",
"aetags : aetags relcomp STRING",
"aetags : aetags COMPARATOR STRING",
"htags :",
"htags : htags comptag",
"htags : htags relcomp STRING",
"htags : htags COMPARATOR STRING",
"btags :",
"btags : btags RAW",
"btags : btags TEXT",
"btags : btags CONTENT stringlist",
"btags : btags comptag",
"btags : btags relcomp STRING",
"btags : btags COMPARATOR STRING",
"addrparttag : ALL",
"addrparttag : LOCALPART",
"addrparttag : DOMAIN",
"addrparttag : USER",
"addrparttag : DETAIL",
"comptag : IS",
"comptag : CONTAINS",
"comptag : MATCHES",
"comptag : REGEX",
"relcomp : COUNT",
"relcomp : VALUE",
"sizetag : OVER",
"sizetag : UNDER",
"copy :",
"copy : COPY",
"testlist : '(' tests ')'",
"tests : test",
"tests : test ',' tests",

};
#endif
/* define the initial stack-sizes */
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH  YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH  500
#endif
#endif

#define YYINITSTACKSIZE 500

int      yydebug;
int      yynerrs;

typedef struct {
    unsigned stacksize;
    short    *s_base;
    short    *s_mark;
    short    *s_last;
    YYSTYPE  *l_base;
    YYSTYPE  *l_mark;
} YYSTACKDATA;
int      yyerrflag;
int      yychar;
YYSTYPE  yyval;
YYSTYPE  yylval;

/* variables for the parser stack */
static YYSTACKDATA yystack;
#line 701 "sieve.y"
commandlist_t *sieve_parse(sieve_script_t *script, FILE *f)
{
    commandlist_t *t;

    parse_script = script;
    yyrestart(f);
    if (yyparse()) {
	t = NULL;
    } else {
	t = ret;
    }
    ret = NULL;
    return t;
}

int yyerror(char *msg)
{
    extern int yylineno;
    int ret;

    parse_script->err++;
    if (parse_script->interp.err) {
	ret = parse_script->interp.err(yylineno, msg, 
				       parse_script->interp.interp_context,
				       parse_script->script_context);
    }

    return 0;
}

static char *check_reqs(stringlist_t *sl)
{
    stringlist_t *s;
    char *err = NULL, *p, sep = ':';
    size_t alloc = 0;
    
    while (sl != NULL) {
	s = sl;
	sl = sl->next;

	if (!script_require(parse_script, s->s)) {
	    if (!err) {
		alloc = 100;
		p = err = xmalloc(alloc);
		p += sprintf(p, "Unsupported feature(s) in \"require\"");
	    }
	    else if ((size_t) (p - err + strlen(s->s) + 5) > alloc) {
		alloc += 100;
		err = xrealloc(err, alloc);
		p = err + strlen(err);
	    }

	    p += sprintf(p, "%c \"%s\"", sep, s->s);
	    sep = ',';
	}

	free(s->s);
	free(s);
    }
    return err;
}

static test_t *build_address(int t, struct aetags *ae,
			     stringlist_t *sl, stringlist_t *pl)
{
    test_t *ret = new_test(t);	/* can be either ADDRESS or ENVELOPE */

    assert((t == ADDRESS) || (t == ENVELOPE));

    if (ret) {
	ret->u.ae.comptag = ae->comptag;
	ret->u.ae.relation=ae->relation;
	ret->u.ae.comparator=xstrdup(ae->comparator);
	ret->u.ae.sl = sl;
	ret->u.ae.pl = pl;
	ret->u.ae.addrpart = ae->addrtag;
	free_aetags(ae);

    }
    return ret;
}

static test_t *build_header(int t, struct htags *h,
			    stringlist_t *sl, stringlist_t *pl)
{
    test_t *ret = new_test(t);	/* can be HEADER */

    assert(t == HEADER);

    if (ret) {
	ret->u.h.comptag = h->comptag;
	ret->u.h.relation=h->relation;
	ret->u.h.comparator=xstrdup(h->comparator);
	ret->u.h.sl = sl;
	ret->u.h.pl = pl;
	free_htags(h);
    }
    return ret;
}

static test_t *build_body(int t, struct btags *b, stringlist_t *pl)
{
    test_t *ret = new_test(t);	/* can be BODY */

    assert(t == BODY);

    if (ret) {
	ret->u.b.comptag = b->comptag;
	ret->u.b.relation = b->relation;
	ret->u.b.comparator = xstrdup(b->comparator);
	ret->u.b.transform = b->transform;
	ret->u.b.offset = b->offset;
	ret->u.b.content_types = b->content_types; b->content_types = NULL;
	ret->u.b.pl = pl;
	free_btags(b);
    }
    return ret;
}

static commandlist_t *build_vacation(int t, struct vtags *v, char *reason)
{
    commandlist_t *ret = new_command(t);

    assert(t == VACATION);

    if (ret) {
	ret->u.v.subject = v->subject; v->subject = NULL;
	ret->u.v.from = v->from; v->from = NULL;
	ret->u.v.handle = v->handle; v->handle = NULL;
	ret->u.v.days = v->days;
	ret->u.v.mime = v->mime;
	ret->u.v.addresses = v->addresses; v->addresses = NULL;
	free_vtags(v);
	ret->u.v.message = reason;
    }
    return ret;
}

static commandlist_t *build_notify(int t, struct ntags *n)
{
    commandlist_t *ret = new_command(t);

    assert(t == NOTIFY);
       if (ret) {
	ret->u.n.method = n->method; n->method = NULL;
	ret->u.n.id = n->id; n->id = NULL;
	ret->u.n.options = n->options; n->options = NULL;
	ret->u.n.priority = n->priority;
	ret->u.n.message = n->message; n->message = NULL;
	free_ntags(n);
    }
    return ret;
}

static commandlist_t *build_denotify(int t, struct dtags *d)
{
    commandlist_t *ret = new_command(t);

    assert(t == DENOTIFY);

    if (ret) {
	ret->u.d.comptag = d->comptag;
	ret->u.d.relation=d->relation;
	ret->u.d.pattern = d->pattern; d->pattern = NULL;
	ret->u.d.priority = d->priority;
	free_dtags(d);
    }
    return ret;
}

static commandlist_t *build_fileinto(int t, int copy, char *folder)
{
    commandlist_t *ret = new_command(t);

    assert(t == FILEINTO);

    if (ret) {
	ret->u.f.copy = copy;
	if (config_getswitch(IMAPOPT_SIEVE_UTF8FILEINTO)) {
	    ret->u.f.folder = xmalloc(5 * strlen(folder) + 1);
	    UTF8_to_mUTF7(ret->u.f.folder, folder);
	    free(folder);
	}
	else {
	    ret->u.f.folder = folder;
	}
    }
    return ret;
}

static commandlist_t *build_redirect(int t, int copy, char *address)
{
    commandlist_t *ret = new_command(t);

    assert(t == REDIRECT);

    if (ret) {
	ret->u.r.copy = copy;
	ret->u.r.address = address;
    }
    return ret;
}

static struct aetags *new_aetags(void)
{
    struct aetags *r = (struct aetags *) xmalloc(sizeof(struct aetags));

    r->addrtag = r->comptag = r->relation=-1;
    r->comparator=NULL;

    return r;
}

static struct aetags *canon_aetags(struct aetags *ae)
{
    if (ae->addrtag == -1) { ae->addrtag = ALL; }
    if (ae->comparator == NULL) {
        ae->comparator = xstrdup("i;ascii-casemap");
    }
    if (ae->comptag == -1) { ae->comptag = IS; }
    return ae;
}

static void free_aetags(struct aetags *ae)
{
    free(ae->comparator);
     free(ae);
}

static struct htags *new_htags(void)
{
    struct htags *r = (struct htags *) xmalloc(sizeof(struct htags));

    r->comptag = r->relation= -1;
    
    r->comparator = NULL;

    return r;
}

static struct htags *canon_htags(struct htags *h)
{
    if (h->comparator == NULL) {
	h->comparator = xstrdup("i;ascii-casemap");
    }
    if (h->comptag == -1) { h->comptag = IS; }
    return h;
}

static void free_htags(struct htags *h)
{
    free(h->comparator);
    free(h);
}

static struct btags *new_btags(void)
{
    struct btags *r = (struct btags *) xmalloc(sizeof(struct btags));

    r->transform = r->offset = r->comptag = r->relation = -1;
    r->content_types = NULL;
    r->comparator = NULL;

    return r;
}

static struct btags *canon_btags(struct btags *b)
{
    if (b->transform == -1) { b->transform = TEXT; }
    if (b->content_types == NULL) {
	if (b->transform == RAW) {
	    b->content_types = new_sl(xstrdup(""), NULL);
	} else {
	    b->content_types = new_sl(xstrdup("text"), NULL);
	}
    }
    if (b->offset == -1) { b->offset = 0; }
    if (b->comparator == NULL) { b->comparator = xstrdup("i;ascii-casemap"); }
    if (b->comptag == -1) { b->comptag = IS; }
    return b;
}

static void free_btags(struct btags *b)
{
    if (b->content_types) { free_sl(b->content_types); }
    free(b->comparator);
    free(b);
}

static struct vtags *new_vtags(void)
{
    struct vtags *r = (struct vtags *) xmalloc(sizeof(struct vtags));

    r->days = -1;
    r->addresses = NULL;
    r->subject = NULL;
    r->from = NULL;
    r->handle = NULL;
    r->mime = -1;

    return r;
}

static struct vtags *canon_vtags(struct vtags *v)
{
    assert(parse_script->interp.vacation != NULL);

    if (v->days == -1) { v->days = 7; }
    if (v->days < parse_script->interp.vacation->min_response) 
       { v->days = parse_script->interp.vacation->min_response; }
    if (v->days > parse_script->interp.vacation->max_response)
       { v->days = parse_script->interp.vacation->max_response; }
    if (v->mime == -1) { v->mime = 0; }

    return v;
}

static void free_vtags(struct vtags *v)
{
    if (v->addresses) { free_sl(v->addresses); }
    if (v->subject) { free(v->subject); }
    if (v->from) { free(v->from); }
    if (v->handle) { free(v->handle); }
    free(v);
}

static struct ntags *new_ntags(void)
{
    struct ntags *r = (struct ntags *) xmalloc(sizeof(struct ntags));

    r->method = NULL;
    r->id = NULL;
    r->options = NULL;
    r->priority = -1;
    r->message = NULL;

    return r;
}

static struct ntags *canon_ntags(struct ntags *n)
{
    if (n->priority == -1) { n->priority = NORMAL; }
    if (n->message == NULL) { n->message = xstrdup("$from$: $subject$"); }
    if (n->method == NULL) { n->method = xstrdup("default"); }
    return n;
}
static struct dtags *canon_dtags(struct dtags *d)
{
    if (d->priority == -1) { d->priority = ANY; }
    if (d->comptag == -1) { d->comptag = ANY; }
       return d;
}

static void free_ntags(struct ntags *n)
{
    if (n->method) { free(n->method); }
    if (n->id) { free(n->id); }
    if (n->options) { free_sl(n->options); }
    if (n->message) { free(n->message); }
    free(n);
}

static struct dtags *new_dtags(void)
{
    struct dtags *r = (struct dtags *) xmalloc(sizeof(struct dtags));

    r->comptag = r->priority= r->relation = -1;
    r->pattern  = NULL;

    return r;
}

static void free_dtags(struct dtags *d)
{
    if (d->pattern) free(d->pattern);
    free(d);
}

static int verify_stringlist(stringlist_t *sl, int (*verify)(char *))
{
    for (; sl != NULL && verify(sl->s); sl = sl->next) ;
    return (sl == NULL);
}

char *addrptr;		/* pointer to address string for address lexer */
char addrerr[500];	/* buffer for address parser error messages */

static int verify_address(char *s)
{
    addrptr = s;
    addrerr[0] = '\0';	/* paranoia */
    if (addrparse()) {
	snprintf(errbuf, ERR_BUF_SIZE, 
		 "address '%s': %s", s, addrerr);
	yyerror(errbuf);
	return 0;
    }
    return 1;
}

static int verify_mailbox(char *s)
{
    if (!verify_utf8(s)) return 0;

    /* xxx if not a mailbox, call yyerror */
    return 1;
}

static int verify_header(char *hdr)
{
    char *h = hdr;

    while (*h) {
	/* field-name      =       1*ftext
	   ftext           =       %d33-57 / %d59-126         
	   ; Any character except
	   ;  controls, SP, and
	   ;  ":". */
	if (!((*h >= 33 && *h <= 57) || (*h >= 59 && *h <= 126))) {
	    snprintf(errbuf, ERR_BUF_SIZE,
		     "header '%s': not a valid header", hdr);
	    yyerror(errbuf);
	    return 0;
	}
	h++;
    }
    return 1;
}
 
static int verify_addrheader(char *hdr)
{
    const char **h, *hdrs[] = {
	"from", "sender", "reply-to",	/* RFC2822 originator fields */
	"to", "cc", "bcc",		/* RFC2822 destination fields */
	"resent-from", "resent-sender",	/* RFC2822 resent fields */
	"resent-to", "resent-cc", "resent-bcc",
	"return-path",			/* RFC2822 trace fields */
	"disposition-notification-to",	/* RFC2298 MDN request fields */
	"delivered-to",			/* non-standard (loop detection) */
	"approved",			/* RFC1036 moderator/control fields */
	NULL
    };

    if (!config_getswitch(IMAPOPT_RFC3028_STRICT))
	return verify_header(hdr);

    for (lcase(hdr), h = hdrs; *h; h++) {
	if (!strcmp(*h, hdr)) return 1;
    }

    snprintf(errbuf, ERR_BUF_SIZE,
	     "header '%s': not a valid header for an address test", hdr);
    yyerror(errbuf);
    return 0;
}
 
static int verify_envelope(char *env)
{
    lcase(env);
    if (!config_getswitch(IMAPOPT_RFC3028_STRICT) ||
	!strcmp(env, "from") || !strcmp(env, "to") || !strcmp(env, "auth")) {
	return 1;
    }

    snprintf(errbuf, ERR_BUF_SIZE,
	     "env-part '%s': not a valid part for an envelope test", env);
    yyerror(errbuf);
    return 0;
}
 
static int verify_relat(char *r)
{/* this really should have been a token to begin with.*/
	lcase(r);
	if (!strcmp(r, "gt")) {return GT;}
	else if (!strcmp(r, "ge")) {return GE;}
	else if (!strcmp(r, "lt")) {return LT;}
	else if (!strcmp(r, "le")) {return LE;}
	else if (!strcmp(r, "ne")) {return NE;}
	else if (!strcmp(r, "eq")) {return EQ;}
	else{
	  snprintf(errbuf, ERR_BUF_SIZE,
		   "flag '%s': not a valid relational operation", r);
	  yyerror(errbuf);
	  return -1;
	}
	
}




static int verify_flag(char *f)
{
    if (f[0] == '\\') {
	lcase(f);
	if (strcmp(f, "\\seen") && strcmp(f, "\\answered") &&
	    strcmp(f, "\\flagged") && strcmp(f, "\\draft") &&
	    strcmp(f, "\\deleted")) {
	    snprintf(errbuf, ERR_BUF_SIZE,
		     "flag '%s': not a system flag", f);
	    yyerror(errbuf);
	    return 0;
	}
	return 1;
    }
    if (!imparse_isatom(f)) {
	snprintf(errbuf, ERR_BUF_SIZE,
		 "flag '%s': not a valid keyword", f);
	yyerror(errbuf);
	return 0;
    }
    return 1;
}
 
#ifdef ENABLE_REGEX
static int verify_regex(char *s, int cflags)
{
    int ret;
    regex_t *reg = (regex_t *) xmalloc(sizeof(regex_t));

#ifdef HAVE_PCREPOSIX_H
    /* support UTF8 comparisons */
    cflags |= REG_UTF8;
#endif

    if ((ret = regcomp(reg, s, cflags)) != 0) {
	(void) regerror(ret, reg, errbuf, ERR_BUF_SIZE);
	yyerror(errbuf);
	free(reg);
	return 0;
    }
    free(reg);
    return 1;
}

static int verify_regexs(stringlist_t *sl, char *comp)
{
    stringlist_t *sl2;
    int cflags = REG_EXTENDED | REG_NOSUB;
 

    if (!strcmp(comp, "i;ascii-casemap")) {
	cflags |= REG_ICASE;
    }

    for (sl2 = sl; sl2 != NULL; sl2 = sl2->next) {
	if ((verify_regex(sl2->s, cflags)) == 0) {
	    break;
	}
    }
    if (sl2 == NULL) {
	return 1;
    }
    return 0;
}
#endif

/*
 * Valid UTF-8 check (from RFC 2640 Annex B.1)
 *
 * The following routine checks if a byte sequence is valid UTF-8. This
 * is done by checking for the proper tagging of the first and following
 * bytes to make sure they conform to the UTF-8 format. It then checks
 * to assure that the data part of the UTF-8 sequence conforms to the
 * proper range allowed by the encoding. Note: This routine will not
 * detect characters that have not been assigned and therefore do not
 * exist.
 */
static int verify_utf8(char *s)
{
    const char *buf = s;
    const char *endbuf = s + strlen(s);
    unsigned char byte2mask = 0x00, c;
    int trailing = 0;  /* trailing (continuation) bytes to follow */

    while (buf != endbuf) {
	c = *buf++;
	if (trailing) {
	    if ((c & 0xC0) == 0x80) {		/* Does trailing byte
						   follow UTF-8 format? */
		if (byte2mask) {		/* Need to check 2nd byte
						   for proper range? */
		    if (c & byte2mask)		/* Are appropriate bits set? */
			byte2mask = 0x00;
		    else
			break;
		}
		trailing--;
	    }
	    else
		break;
	}
	else {
	    if ((c & 0x80) == 0x00)		/* valid 1 byte UTF-8 */
		continue;
	    else if ((c & 0xE0) == 0xC0)	/* valid 2 byte UTF-8 */
		if (c & 0x1E) {			/* Is UTF-8 byte
						   in proper range? */
		    trailing = 1;
		}
		else
		    break;
	    else if ((c & 0xF0) == 0xE0) {	/* valid 3 byte UTF-8 */
		if (!(c & 0x0F)) {		/* Is UTF-8 byte
						   in proper range? */
		    byte2mask = 0x20;		/* If not, set mask
						   to check next byte */
		}
		trailing = 2;
	    }
	    else if ((c & 0xF8) == 0xF0) {	/* valid 4 byte UTF-8 */
		if (!(c & 0x07)) {		/* Is UTF-8 byte
						   in proper range? */
		    byte2mask = 0x30;		/* If not, set mask
						   to check next byte */
		}
		trailing = 3;
	    }
	    else if ((c & 0xFC) == 0xF8) {	/* valid 5 byte UTF-8 */
		if (!(c & 0x03)) {		/* Is UTF-8 byte
						   in proper range? */
		    byte2mask = 0x38;		/* If not, set mask
						   to check next byte */
		}
		trailing = 4;
	    }
	    else if ((c & 0xFE) == 0xFC) {	/* valid 6 byte UTF-8 */
		if (!(c & 0x01)) {		/* Is UTF-8 byte
						   in proper range? */
		    byte2mask = 0x3C;		/* If not, set mask
						   to check next byte */
		}
		trailing = 5;
	    }
	    else
		break;
	}
    }

    if ((buf != endbuf) || trailing) {
	snprintf(errbuf, ERR_BUF_SIZE,
		 "string '%s': not valid utf8", s);
	yyerror(errbuf);
	return 0;
    }

    return 1;
}
#line 1300 "y.tab.c"

#if YYDEBUG
#include <stdio.h>		/* needed for printf */
#endif

#include <stdlib.h>	/* needed for malloc, etc */
#include <string.h>	/* needed for memset */

/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack(YYSTACKDATA *data)
{
    int i;
    unsigned newsize;
    short *newss;
    YYSTYPE *newvs;

    if ((newsize = data->stacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return -1;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;

    i = data->s_mark - data->s_base;
    newss = (short *)realloc(data->s_base, newsize * sizeof(*newss));
    if (newss == 0)
        return -1;

    data->s_base = newss;
    data->s_mark = newss + i;

    newvs = (YYSTYPE *)realloc(data->l_base, newsize * sizeof(*newvs));
    if (newvs == 0)
        return -1;

    data->l_base = newvs;
    data->l_mark = newvs + i;

    data->stacksize = newsize;
    data->s_last = data->s_base + newsize - 1;
    return 0;
}

#if YYPURE || defined(YY_NO_LEAKS)
static void yyfreestack(YYSTACKDATA *data)
{
    free(data->s_base);
    free(data->l_base);
    memset(data, 0, sizeof(*data));
}
#else
#define yyfreestack(data) /* nothing */
#endif

#define YYABORT  goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR  goto yyerrlab

int
YYPARSE_DECL()
{
    int yym, yyn, yystate;
#if YYDEBUG
    const char *yys;

    if ((yys = getenv("YYDEBUG")) != 0)
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = YYEMPTY;
    yystate = 0;

#if YYPURE
    memset(&yystack, 0, sizeof(yystack));
#endif

    if (yystack.s_base == NULL && yygrowstack(&yystack)) goto yyoverflow;
    yystack.s_mark = yystack.s_base;
    yystack.l_mark = yystack.l_base;
    yystate = 0;
    *yystack.s_mark = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = YYLEX) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack))
        {
            goto yyoverflow;
        }
        yystate = yytable[yyn];
        *++yystack.s_mark = yytable[yyn];
        *++yystack.l_mark = yylval;
        yychar = YYEMPTY;
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;

    yyerror("syntax error");

    goto yyerrlab;

yyerrlab:
    ++yynerrs;

yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yystack.s_mark]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yystack.s_mark, yytable[yyn]);
#endif
                if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack))
                {
                    goto yyoverflow;
                }
                yystate = yytable[yyn];
                *++yystack.s_mark = yytable[yyn];
                *++yystack.l_mark = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yystack.s_mark);
#endif
                if (yystack.s_mark <= yystack.s_base) goto yyabort;
                --yystack.s_mark;
                --yystack.l_mark;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = YYEMPTY;
        goto yyloop;
    }

yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    if (yym)
        yyval = yystack.l_mark[1-yym];
    else
        memset(&yyval, 0, sizeof yyval);
    switch (yyn)
    {
case 1:
#line 222 "sieve.y"
	{ ret = NULL; }
break;
case 2:
#line 223 "sieve.y"
	{ ret = yystack.l_mark[0].cl; }
break;
case 5:
#line 230 "sieve.y"
	{ char *err = check_reqs(yystack.l_mark[-1].sl);
                                  if (err) {
				    yyerror(err);
				    free(err);
				    YYERROR; 
                                  } }
break;
case 6:
#line 238 "sieve.y"
	{ yyval.cl = yystack.l_mark[0].cl; }
break;
case 7:
#line 239 "sieve.y"
	{ yystack.l_mark[-1].cl->next = yystack.l_mark[0].cl; yyval.cl = yystack.l_mark[-1].cl; }
break;
case 8:
#line 242 "sieve.y"
	{ yyval.cl = yystack.l_mark[-1].cl; }
break;
case 9:
#line 243 "sieve.y"
	{ yyval.cl = new_if(yystack.l_mark[-2].test, yystack.l_mark[-1].cl, yystack.l_mark[0].cl); }
break;
case 10:
#line 244 "sieve.y"
	{ yyval.cl = new_command(STOP); }
break;
case 11:
#line 247 "sieve.y"
	{ yyval.cl = NULL; }
break;
case 12:
#line 248 "sieve.y"
	{ yyval.cl = new_if(yystack.l_mark[-2].test, yystack.l_mark[-1].cl, yystack.l_mark[0].cl); }
break;
case 13:
#line 249 "sieve.y"
	{ yyval.cl = yystack.l_mark[0].cl; }
break;
case 14:
#line 252 "sieve.y"
	{ if (!parse_script->support.reject) {
				     yyerror("reject MUST be enabled with \"require\"");
				     YYERROR;
				   }
				   if (!verify_utf8(yystack.l_mark[0].sval)) {
				     YYERROR; /* vu should call yyerror() */
				   }
				   yyval.cl = new_command(REJCT);
				   yyval.cl->u.str = yystack.l_mark[0].sval; }
break;
case 15:
#line 261 "sieve.y"
	{ if (!parse_script->support.fileinto) {
				     yyerror("fileinto MUST be enabled with \"require\"");
	                             YYERROR;
                                   }
				   if (!verify_mailbox(yystack.l_mark[0].sval)) {
				     YYERROR; /* vm should call yyerror() */
				   }
	                           yyval.cl = build_fileinto(FILEINTO, yystack.l_mark[-1].nval, yystack.l_mark[0].sval); }
break;
case 16:
#line 269 "sieve.y"
	{ if (!verify_address(yystack.l_mark[0].sval)) {
				     YYERROR; /* va should call yyerror() */
				   }
	                           yyval.cl = build_redirect(REDIRECT, yystack.l_mark[-1].nval, yystack.l_mark[0].sval); }
break;
case 17:
#line 273 "sieve.y"
	{ yyval.cl = new_command(KEEP); }
break;
case 18:
#line 274 "sieve.y"
	{ yyval.cl = new_command(STOP); }
break;
case 19:
#line 275 "sieve.y"
	{ yyval.cl = new_command(DISCARD); }
break;
case 20:
#line 276 "sieve.y"
	{ if (!parse_script->support.vacation) {
				     yyerror("vacation MUST be enabled with \"require\"");
				     YYERROR;
				   }
				   if ((yystack.l_mark[-1].vtag->mime == -1) && !verify_utf8(yystack.l_mark[0].sval)) {
				     YYERROR; /* vu should call yyerror() */
				   }
  				   yyval.cl = build_vacation(VACATION,
					    canon_vtags(yystack.l_mark[-1].vtag), yystack.l_mark[0].sval); }
break;
case 21:
#line 285 "sieve.y"
	{ if (!parse_script->support.imapflags) {
                                    yyerror("imapflags MUST be enabled with \"require\"");
                                    YYERROR;
                                   }
                                  if (!verify_stringlist(yystack.l_mark[0].sl, verify_flag)) {
                                    YYERROR; /* vf should call yyerror() */
                                  }
                                  yyval.cl = new_command(SETFLAG);
                                  yyval.cl->u.sl = yystack.l_mark[0].sl; }
break;
case 22:
#line 294 "sieve.y"
	{ if (!parse_script->support.imapflags) {
                                    yyerror("imapflags MUST be enabled with \"require\"");
                                    YYERROR;
                                    }
                                  if (!verify_stringlist(yystack.l_mark[0].sl, verify_flag)) {
                                    YYERROR; /* vf should call yyerror() */
                                  }
                                  yyval.cl = new_command(ADDFLAG);
                                  yyval.cl->u.sl = yystack.l_mark[0].sl; }
break;
case 23:
#line 303 "sieve.y"
	{ if (!parse_script->support.imapflags) {
                                    yyerror("imapflags MUST be enabled with \"require\"");
                                    YYERROR;
                                    }
                                  if (!verify_stringlist(yystack.l_mark[0].sl, verify_flag)) {
                                    YYERROR; /* vf should call yyerror() */
                                  }
                                  yyval.cl = new_command(REMOVEFLAG);
                                  yyval.cl->u.sl = yystack.l_mark[0].sl; }
break;
case 24:
#line 312 "sieve.y"
	{ if (!parse_script->support.imapflags) {
                                    yyerror("imapflags MUST be enabled with \"require\"");
                                    YYERROR;
                                    }
                                  yyval.cl = new_command(MARK); }
break;
case 25:
#line 317 "sieve.y"
	{ if (!parse_script->support.imapflags) {
                                    yyerror("imapflags MUST be enabled with \"require\"");
                                    YYERROR;
                                    }
                                  yyval.cl = new_command(UNMARK); }
break;
case 26:
#line 323 "sieve.y"
	{ if (!parse_script->support.notify) {
				       yyerror("notify MUST be enabled with \"require\"");
				       yyval.cl = new_command(NOTIFY); 
				       YYERROR;
	 			    } else {
				      yyval.cl = build_notify(NOTIFY,
				             canon_ntags(yystack.l_mark[0].ntag));
				    } }
break;
case 27:
#line 331 "sieve.y"
	{ if (!parse_script->support.notify) {
                                       yyerror("notify MUST be enabled with \"require\"");
				       yyval.cl = new_command(DENOTIFY);
				       YYERROR;
				    } else {
					yyval.cl = build_denotify(DENOTIFY, canon_dtags(yystack.l_mark[0].dtag));
					if (yyval.cl == NULL) { 
			yyerror("unable to find a compatible comparator");
			YYERROR; } } }
break;
case 28:
#line 341 "sieve.y"
	{ if (!parse_script->support.include) {
				     yyerror("include MUST be enabled with \"require\"");
	                             YYERROR;
                                   }
	                           yyval.cl = new_command(INCLUDE);
				   yyval.cl->u.inc.location = yystack.l_mark[-1].nval;
				   yyval.cl->u.inc.script = yystack.l_mark[0].sval; }
break;
case 29:
#line 348 "sieve.y"
	{ if (!parse_script->support.include) {
                                    yyerror("include MUST be enabled with \"require\"");
                                    YYERROR;
                                  }
                                   yyval.cl = new_command(RETURN); }
break;
case 30:
#line 355 "sieve.y"
	{ yyval.nval = PERSONAL; }
break;
case 31:
#line 356 "sieve.y"
	{ yyval.nval = PERSONAL; }
break;
case 32:
#line 357 "sieve.y"
	{ yyval.nval = GLOBAL; }
break;
case 33:
#line 360 "sieve.y"
	{ yyval.ntag = new_ntags(); }
break;
case 34:
#line 361 "sieve.y"
	{ if (yyval.ntag->id != NULL) { 
					yyerror("duplicate :method"); YYERROR; }
				   else { yyval.ntag->id = yystack.l_mark[0].sval; } }
break;
case 35:
#line 364 "sieve.y"
	{ if (yyval.ntag->method != NULL) { 
					yyerror("duplicate :method"); YYERROR; }
				   else { yyval.ntag->method = yystack.l_mark[0].sval; } }
break;
case 36:
#line 367 "sieve.y"
	{ if (yyval.ntag->options != NULL) { 
					yyerror("duplicate :options"); YYERROR; }
				     else { yyval.ntag->options = yystack.l_mark[0].sl; } }
break;
case 37:
#line 370 "sieve.y"
	{ if (yyval.ntag->priority != -1) { 
                                 yyerror("duplicate :priority"); YYERROR; }
                                   else { yyval.ntag->priority = yystack.l_mark[0].nval; } }
break;
case 38:
#line 373 "sieve.y"
	{ if (yyval.ntag->message != NULL) { 
					yyerror("duplicate :message"); YYERROR; }
				   else { yyval.ntag->message = yystack.l_mark[0].sval; } }
break;
case 39:
#line 378 "sieve.y"
	{ yyval.dtag = new_dtags(); }
break;
case 40:
#line 379 "sieve.y"
	{ if (yyval.dtag->priority != -1) { 
				yyerror("duplicate priority level"); YYERROR; }
				   else { yyval.dtag->priority = yystack.l_mark[0].nval; } }
break;
case 41:
#line 382 "sieve.y"
	{ if (yyval.dtag->comptag != -1)
	                             { 
					 yyerror("duplicate comparator type tag"); YYERROR;
				     }
	                           yyval.dtag->comptag = yystack.l_mark[-1].nval;
#ifdef ENABLE_REGEX
				   if (yyval.dtag->comptag == REGEX)
				   {
				       int cflags = REG_EXTENDED |
					   REG_NOSUB | REG_ICASE;
				       if (!verify_regex(yystack.l_mark[0].sval, cflags)) { YYERROR; }
				   }
#endif
				   yyval.dtag->pattern = yystack.l_mark[0].sval;
	                          }
break;
case 42:
#line 397 "sieve.y"
	{ yyval.dtag = yystack.l_mark[-2].dtag;
				   if (yyval.dtag->comptag != -1) { 
			yyerror("duplicate comparator type tag"); YYERROR; }
				   else { yyval.dtag->comptag = yystack.l_mark[-1].nval;
				   yyval.dtag->relation = verify_relat(yystack.l_mark[0].sval);
				   if (yyval.dtag->relation==-1) 
				     {YYERROR; /*vr called yyerror()*/ }
				   } }
break;
case 43:
#line 407 "sieve.y"
	{ yyval.nval = LOW; }
break;
case 44:
#line 408 "sieve.y"
	{ yyval.nval = NORMAL; }
break;
case 45:
#line 409 "sieve.y"
	{ yyval.nval = HIGH; }
break;
case 46:
#line 412 "sieve.y"
	{ yyval.vtag = new_vtags(); }
break;
case 47:
#line 413 "sieve.y"
	{ if (yyval.vtag->days != -1) { 
					yyerror("duplicate :days"); YYERROR; }
				   else { yyval.vtag->days = yystack.l_mark[0].nval; } }
break;
case 48:
#line 416 "sieve.y"
	{ if (yyval.vtag->addresses != NULL) { 
					yyerror("duplicate :addresses"); 
					YYERROR;
				       } else if (!verify_stringlist(yystack.l_mark[0].sl,
							verify_address)) {
					  YYERROR;
				       } else {
					 yyval.vtag->addresses = yystack.l_mark[0].sl; } }
break;
case 49:
#line 424 "sieve.y"
	{ if (yyval.vtag->subject != NULL) { 
					yyerror("duplicate :subject"); 
					YYERROR;
				   } else if (!verify_utf8(yystack.l_mark[0].sval)) {
				        YYERROR; /* vu should call yyerror() */
				   } else { yyval.vtag->subject = yystack.l_mark[0].sval; } }
break;
case 50:
#line 430 "sieve.y"
	{ if (yyval.vtag->from != NULL) { 
					yyerror("duplicate :from"); 
					YYERROR;
				   } else if (!verify_address(yystack.l_mark[0].sval)) {
				        YYERROR; /* vu should call yyerror() */
				   } else { yyval.vtag->from = yystack.l_mark[0].sval; } }
break;
case 51:
#line 436 "sieve.y"
	{ if (yyval.vtag->handle != NULL) { 
					yyerror("duplicate :handle"); 
					YYERROR;
				   } else if (!verify_utf8(yystack.l_mark[0].sval)) {
				        YYERROR; /* vu should call yyerror() */
				   } else { yyval.vtag->handle = yystack.l_mark[0].sval; } }
break;
case 52:
#line 442 "sieve.y"
	{ if (yyval.vtag->mime != -1) { 
					yyerror("duplicate :mime"); 
					YYERROR; }
				   else { yyval.vtag->mime = MIME; } }
break;
case 53:
#line 448 "sieve.y"
	{ yyval.sl = sl_reverse(yystack.l_mark[-1].sl); }
break;
case 54:
#line 449 "sieve.y"
	{ yyval.sl = new_sl(yystack.l_mark[0].sval, NULL); }
break;
case 55:
#line 452 "sieve.y"
	{ yyval.sl = new_sl(yystack.l_mark[0].sval, NULL); }
break;
case 56:
#line 453 "sieve.y"
	{ yyval.sl = new_sl(yystack.l_mark[0].sval, yystack.l_mark[-2].sl); }
break;
case 57:
#line 456 "sieve.y"
	{ yyval.cl = yystack.l_mark[-1].cl; }
break;
case 58:
#line 457 "sieve.y"
	{ yyval.cl = NULL; }
break;
case 59:
#line 460 "sieve.y"
	{ yyval.test = new_test(ANYOF); yyval.test->u.tl = yystack.l_mark[0].testl; }
break;
case 60:
#line 461 "sieve.y"
	{ yyval.test = new_test(ALLOF); yyval.test->u.tl = yystack.l_mark[0].testl; }
break;
case 61:
#line 462 "sieve.y"
	{ yyval.test = new_test(EXISTS); yyval.test->u.sl = yystack.l_mark[0].sl; }
break;
case 62:
#line 463 "sieve.y"
	{ yyval.test = new_test(SFALSE); }
break;
case 63:
#line 464 "sieve.y"
	{ yyval.test = new_test(STRUE); }
break;
case 64:
#line 466 "sieve.y"
	{
				     if (!verify_stringlist(yystack.l_mark[-1].sl, verify_header)) {
					 YYERROR; /* vh should call yyerror() */
				     }
				     if (!verify_stringlist(yystack.l_mark[0].sl, verify_utf8)) {
					 YYERROR; /* vu should call yyerror() */
				     }
				     
				     yystack.l_mark[-2].htag = canon_htags(yystack.l_mark[-2].htag);
#ifdef ENABLE_REGEX
				     if (yystack.l_mark[-2].htag->comptag == REGEX)
				     {
					 if (!(verify_regexs(yystack.l_mark[0].sl, yystack.l_mark[-2].htag->comparator)))
					 { YYERROR; }
				     }
#endif
				     yyval.test = build_header(HEADER, yystack.l_mark[-2].htag, yystack.l_mark[-1].sl, yystack.l_mark[0].sl);
				     if (yyval.test == NULL) { 
					 yyerror("unable to find a compatible comparator");
					 YYERROR; } 
				 }
break;
case 65:
#line 490 "sieve.y"
	{ 
				     if ((yystack.l_mark[-3].nval == ADDRESS) &&
					 !verify_stringlist(yystack.l_mark[-1].sl, verify_addrheader))
					 { YYERROR; }
				     else if ((yystack.l_mark[-3].nval == ENVELOPE) &&
					      !verify_stringlist(yystack.l_mark[-1].sl, verify_envelope))
					 { YYERROR; }
				     yystack.l_mark[-2].aetag = canon_aetags(yystack.l_mark[-2].aetag);
#ifdef ENABLE_REGEX
				     if (yystack.l_mark[-2].aetag->comptag == REGEX)
				     {
					 if (!( verify_regexs(yystack.l_mark[0].sl, yystack.l_mark[-2].aetag->comparator)))
					 { YYERROR; }
				     }
#endif
				     yyval.test = build_address(yystack.l_mark[-3].nval, yystack.l_mark[-2].aetag, yystack.l_mark[-1].sl, yystack.l_mark[0].sl);
				     if (yyval.test == NULL) { 
					 yyerror("unable to find a compatible comparator");
					 YYERROR; } 
				 }
break;
case 66:
#line 512 "sieve.y"
	{
				     if (!parse_script->support.body) {
                                       yyerror("body MUST be enabled with \"require\"");
				       YYERROR;
				     }
					
				     if (!verify_stringlist(yystack.l_mark[0].sl, verify_utf8)) {
					 YYERROR; /* vu should call yyerror() */
				     }
				     
				     yystack.l_mark[-1].btag = canon_btags(yystack.l_mark[-1].btag);
#ifdef ENABLE_REGEX
				     if (yystack.l_mark[-1].btag->comptag == REGEX)
				     {
					 if (!(verify_regexs(yystack.l_mark[0].sl, yystack.l_mark[-1].btag->comparator)))
					 { YYERROR; }
				     }
#endif
				     yyval.test = build_body(BODY, yystack.l_mark[-1].btag, yystack.l_mark[0].sl);
				     if (yyval.test == NULL) { 
					 yyerror("unable to find a compatible comparator");
					 YYERROR; } 
				 }
break;
case 67:
#line 537 "sieve.y"
	{ yyval.test = new_test(NOT); yyval.test->u.t = yystack.l_mark[0].test; }
break;
case 68:
#line 538 "sieve.y"
	{ yyval.test = new_test(SIZE); yyval.test->u.sz.t = yystack.l_mark[-1].nval;
		                   yyval.test->u.sz.n = yystack.l_mark[0].nval; }
break;
case 69:
#line 540 "sieve.y"
	{ yyval.test = NULL; }
break;
case 70:
#line 543 "sieve.y"
	{ yyval.nval = ADDRESS; }
break;
case 71:
#line 544 "sieve.y"
	{if (!parse_script->support.envelope)
	                              {yyerror("envelope MUST be enabled with \"require\""); YYERROR;}
	                          else{yyval.nval = ENVELOPE; }
	                         }
break;
case 72:
#line 551 "sieve.y"
	{ yyval.aetag = new_aetags(); }
break;
case 73:
#line 552 "sieve.y"
	{ yyval.aetag = yystack.l_mark[-1].aetag;
				   if (yyval.aetag->addrtag != -1) { 
			yyerror("duplicate or conflicting address part tag");
			YYERROR; }
				   else { yyval.aetag->addrtag = yystack.l_mark[0].nval; } }
break;
case 74:
#line 557 "sieve.y"
	{ yyval.aetag = yystack.l_mark[-1].aetag;
				   if (yyval.aetag->comptag != -1) { 
			yyerror("duplicate comparator type tag"); YYERROR; }
				   else { yyval.aetag->comptag = yystack.l_mark[0].nval; } }
break;
case 75:
#line 561 "sieve.y"
	{ yyval.aetag = yystack.l_mark[-2].aetag;
				   if (yyval.aetag->comptag != -1) { 
			yyerror("duplicate comparator type tag"); YYERROR; }
				   else { yyval.aetag->comptag = yystack.l_mark[-1].nval;
				   yyval.aetag->relation = verify_relat(yystack.l_mark[0].sval);
				   if (yyval.aetag->relation==-1) 
				     {YYERROR; /*vr called yyerror()*/ }
				   } }
break;
case 76:
#line 569 "sieve.y"
	{ yyval.aetag = yystack.l_mark[-2].aetag;
	if (yyval.aetag->comparator != NULL) { 
			yyerror("duplicate comparator tag"); YYERROR; }
				   else if (!strcmp(yystack.l_mark[0].sval, "i;ascii-numeric") &&
					    !parse_script->support.i_ascii_numeric) {
			yyerror("comparator-i;ascii-numeric MUST be enabled with \"require\"");
			YYERROR; }
				   else { yyval.aetag->comparator = yystack.l_mark[0].sval; } }
break;
case 77:
#line 579 "sieve.y"
	{ yyval.htag = new_htags(); }
break;
case 78:
#line 580 "sieve.y"
	{ yyval.htag = yystack.l_mark[-1].htag;
				   if (yyval.htag->comptag != -1) { 
			yyerror("duplicate comparator type tag"); YYERROR; }
				   else { yyval.htag->comptag = yystack.l_mark[0].nval; } }
break;
case 79:
#line 584 "sieve.y"
	{ yyval.htag = yystack.l_mark[-2].htag;
				   if (yyval.htag->comptag != -1) { 
			yyerror("duplicate comparator type tag"); YYERROR; }
				   else { yyval.htag->comptag = yystack.l_mark[-1].nval;
				   yyval.htag->relation = verify_relat(yystack.l_mark[0].sval);
				   if (yyval.htag->relation==-1) 
				     {YYERROR; /*vr called yyerror()*/ }
				   } }
break;
case 80:
#line 592 "sieve.y"
	{ yyval.htag = yystack.l_mark[-2].htag;
				   if (yyval.htag->comparator != NULL) { 
			 yyerror("duplicate comparator tag"); YYERROR; }
				   else if (!strcmp(yystack.l_mark[0].sval, "i;ascii-numeric") &&
					    !parse_script->support.i_ascii_numeric) { 
			 yyerror("comparator-i;ascii-numeric MUST be enabled with \"require\"");  YYERROR; }
				   else { 
				     yyval.htag->comparator = yystack.l_mark[0].sval; } }
break;
case 81:
#line 602 "sieve.y"
	{ yyval.btag = new_btags(); }
break;
case 82:
#line 603 "sieve.y"
	{ yyval.btag = yystack.l_mark[-1].btag;
				   if (yyval.btag->transform != -1) {
			yyerror("duplicate or conflicting transform tag");
			YYERROR; }
				   else { yyval.btag->transform = RAW; } }
break;
case 83:
#line 608 "sieve.y"
	{ yyval.btag = yystack.l_mark[-1].btag;
				   if (yyval.btag->transform != -1) {
			yyerror("duplicate or conflicting transform tag");
			YYERROR; }
				   else { yyval.btag->transform = TEXT; } }
break;
case 84:
#line 613 "sieve.y"
	{ yyval.btag = yystack.l_mark[-2].btag;
				   if (yyval.btag->transform != -1) {
			yyerror("duplicate or conflicting transform tag");
			YYERROR; }
				   else {
				       yyval.btag->transform = CONTENT;
				       yyval.btag->content_types = yystack.l_mark[0].sl;
				   } }
break;
case 85:
#line 621 "sieve.y"
	{ yyval.btag = yystack.l_mark[-1].btag;
				   if (yyval.btag->comptag != -1) { 
			yyerror("duplicate comparator type tag"); YYERROR; }
				   else { yyval.btag->comptag = yystack.l_mark[0].nval; } }
break;
case 86:
#line 625 "sieve.y"
	{ yyval.btag = yystack.l_mark[-2].btag;
				   if (yyval.btag->comptag != -1) { 
			yyerror("duplicate comparator type tag"); YYERROR; }
				   else { yyval.btag->comptag = yystack.l_mark[-1].nval;
				   yyval.btag->relation = verify_relat(yystack.l_mark[0].sval);
				   if (yyval.btag->relation==-1) 
				     {YYERROR; /*vr called yyerror()*/ }
				   } }
break;
case 87:
#line 633 "sieve.y"
	{ yyval.btag = yystack.l_mark[-2].btag;
				   if (yyval.btag->comparator != NULL) { 
			 yyerror("duplicate comparator tag"); YYERROR; }
				   else if (!strcmp(yystack.l_mark[0].sval, "i;ascii-numeric") &&
					    !parse_script->support.i_ascii_numeric) { 
			 yyerror("comparator-i;ascii-numeric MUST be enabled with \"require\"");  YYERROR; }
				   else { 
				     yyval.btag->comparator = yystack.l_mark[0].sval; } }
break;
case 88:
#line 644 "sieve.y"
	{ yyval.nval = ALL; }
break;
case 89:
#line 645 "sieve.y"
	{ yyval.nval = LOCALPART; }
break;
case 90:
#line 646 "sieve.y"
	{ yyval.nval = DOMAIN; }
break;
case 91:
#line 647 "sieve.y"
	{ if (!parse_script->support.subaddress) {
				     yyerror("subaddress MUST be enabled with \"require\"");
				     YYERROR;
				   }
				   yyval.nval = USER; }
break;
case 92:
#line 652 "sieve.y"
	{ if (!parse_script->support.subaddress) {
				     yyerror("subaddress MUST be enabled with \"require\"");
				     YYERROR;
				   }
				   yyval.nval = DETAIL; }
break;
case 93:
#line 658 "sieve.y"
	{ yyval.nval = IS; }
break;
case 94:
#line 659 "sieve.y"
	{ yyval.nval = CONTAINS; }
break;
case 95:
#line 660 "sieve.y"
	{ yyval.nval = MATCHES; }
break;
case 96:
#line 661 "sieve.y"
	{ if (!parse_script->support.regex) {
				     yyerror("regex MUST be enabled with \"require\"");
				     YYERROR;
				   }
				   yyval.nval = REGEX; }
break;
case 97:
#line 668 "sieve.y"
	{ if (!parse_script->support.relational) {
				     yyerror("relational MUST be enabled with \"require\"");
				     YYERROR;
				   }
				   yyval.nval = COUNT; }
break;
case 98:
#line 673 "sieve.y"
	{ if (!parse_script->support.relational) {
				     yyerror("relational MUST be enabled with \"require\"");
				     YYERROR;
				   }
				   yyval.nval = VALUE; }
break;
case 99:
#line 681 "sieve.y"
	{ yyval.nval = OVER; }
break;
case 100:
#line 682 "sieve.y"
	{ yyval.nval = UNDER; }
break;
case 101:
#line 685 "sieve.y"
	{ yyval.nval = 0; }
break;
case 102:
#line 686 "sieve.y"
	{ if (!parse_script->support.copy) {
				     yyerror("copy MUST be enabled with \"require\"");
	                             YYERROR;
                                   }
				   yyval.nval = COPY; }
break;
case 103:
#line 693 "sieve.y"
	{ yyval.testl = yystack.l_mark[-1].testl; }
break;
case 104:
#line 696 "sieve.y"
	{ yyval.testl = new_testlist(yystack.l_mark[0].test, NULL); }
break;
case 105:
#line 697 "sieve.y"
	{ yyval.testl = new_testlist(yystack.l_mark[-2].test, yystack.l_mark[0].testl); }
break;
#line 2225 "y.tab.c"
    }
    yystack.s_mark -= yym;
    yystate = *yystack.s_mark;
    yystack.l_mark -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yystack.s_mark = YYFINAL;
        *++yystack.l_mark = yyval;
        if (yychar < 0)
        {
            if ((yychar = YYLEX) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yystack.s_mark, yystate);
#endif
    if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack))
    {
        goto yyoverflow;
    }
    *++yystack.s_mark = (short) yystate;
    *++yystack.l_mark = yyval;
    goto yyloop;

yyoverflow:
    yyerror("yacc stack overflow");

yyabort:
    yyfreestack(&yystack);
    return (1);

yyaccept:
    yyfreestack(&yystack);
    return (0);
}
