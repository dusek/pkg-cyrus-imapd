/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         sieveparse
#define yylex           sievelex
#define yyerror         sieveerror
#define yydebug         sievedebug
#define yynerrs         sievenerrs


/* Copy the first part of user declarations.  */
#line 1 "sieve/sieve.y" /* yacc.c:339  */

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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "xmalloc.h"
#include "sieve/comparator.h"
#include "sieve/interp.h"
#include "sieve/script.h"
#include "sieve/tree.h"
#include "sieve/flags.h"

#include "imapurl.h"
#include "lib/gmtoff.h"
#include "util.h"
#include "imparse.h"
#include "libconfig.h"
#include "times.h"

#define ERR_BUF_SIZE 1024

/* definitions */
extern int addrparse(sieve_script_t*);
typedef struct yy_buffer_state *YY_BUFFER_STATE;
extern YY_BUFFER_STATE addr_scan_string(const char*);
extern void addr_delete_buffer(YY_BUFFER_STATE);

extern int sievelineno;

struct vtags {
    int seconds;
    strarray_t *addresses;
    char *subject;
    char *from;
    char *handle;
    int mime;
};

struct htags {
    int index;
    char *comparator;
    int comptag;
    int relation;
};

struct aetags {
    int index;
    int addrtag;
    char *comparator;
    int comptag;
    int relation;
};

struct btags {
    int transform;
    int offset;
    strarray_t *content_types;
    char *comparator;
    int comptag;
    int relation;
};

struct ntags {
    char *method;
    char *id;
    strarray_t *options;
    int priority;
    char *message;
};

struct dtags {
    int comptag;
    int relation;
    void *pattern;
    int priority;
};

struct itags {
  int location;
  int once;
  int optional;
};

struct dttags {
    int index;
    int zonetag;
    char *zone;
    int comptag;
    int relation;
    char *comparator;
    int date_part;
};

static char *check_reqs(sieve_script_t *script, strarray_t *sl);
struct ftags {
    int copy;
    strarray_t *flags;
};

static test_t *build_address(int t, struct aetags *ae,
			     strarray_t *sl, strarray_t *pl);
static test_t *build_header(int t, struct htags *h,
			    strarray_t *sl, strarray_t *pl);
static test_t *build_body(int t, struct btags *b, strarray_t *pl);
static test_t *build_date(int t, struct dttags *dt, char *hn, strarray_t *kl);
static test_t *build_hasflag(int t, struct htags *h,
			    strarray_t *pl);
static commandlist_t *build_vacation(int t, struct vtags *h, char *s);
static commandlist_t *build_notify(int t, struct ntags *n);
static commandlist_t *build_denotify(int t, struct dtags *n);
static commandlist_t *build_keep(int t, struct ftags *f);
static commandlist_t *build_fileinto(int t, struct ftags *f, char *folder);
static commandlist_t *build_redirect(int t, int c, char *a);
static commandlist_t *build_include(int, struct itags *, char*);
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
static struct vtags *canon_vtags(sieve_script_t *script, struct vtags *v);
static void free_vtags(struct vtags *v);
static struct ntags *new_ntags(void);
static struct ntags *canon_ntags(struct ntags *n);
static void free_ntags(struct ntags *n);
static struct dtags *new_dtags(void);
static struct dtags *canon_dtags(struct dtags *d);
static void free_dtags(struct dtags *d);
static struct itags *new_itags(void);
static struct dttags *new_dttags(void);
static struct dttags *canon_dttags(struct dttags *dt);
static void free_dttags(struct dttags *b);
static struct ftags *new_ftags(void);
static struct ftags *canon_ftags(struct ftags *f);
static void free_ftags(struct ftags *f);

static int verify_stringlist(sieve_script_t*, strarray_t *sl, int (*verify)(sieve_script_t*, char *));
static int verify_mailbox(sieve_script_t*, char *s);
static int verify_address(sieve_script_t*, char *s);
static int verify_header(sieve_script_t*, char *s);
static int verify_addrheader(sieve_script_t*, char *s);
static int verify_envelope(sieve_script_t*, char *s);
static int verify_relat(sieve_script_t*, char *s);
static int verify_zone(sieve_script_t*, char *s);
static int verify_date_part(sieve_script_t *parse_script, char *dp);
#ifdef ENABLE_REGEX
static int verify_regex(sieve_script_t*, char *s, int cflags);
static int verify_regexs(sieve_script_t*,const strarray_t *sl, char *comp);
#endif
static int verify_utf8(sieve_script_t*, char *s);

void yyerror(sieve_script_t*, const char *msg);
extern int yylex(void*, sieve_script_t*);
extern void sieverestart(FILE *f);

#define YYERROR_VERBOSE /* i want better error messages! */

/* byacc default is 500, bison default is 10000 - go with the
   larger to support big sieve scripts (see Bug #3461) */
#define YYSTACKSIZE 10000

#line 284 "sieve/sieve.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef YY_SIEVE_SIEVE_SIEVE_H_INCLUDED
# define YY_SIEVE_SIEVE_SIEVE_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int sievedebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    NUMBER = 258,
    STRING = 259,
    IF = 260,
    ELSIF = 261,
    ELSE = 262,
    REJCT = 263,
    FILEINTO = 264,
    REDIRECT = 265,
    KEEP = 266,
    STOP = 267,
    DISCARD = 268,
    VACATION = 269,
    REQUIRE = 270,
    SETFLAG = 271,
    ADDFLAG = 272,
    REMOVEFLAG = 273,
    MARK = 274,
    UNMARK = 275,
    HASFLAG = 276,
    FLAGS = 277,
    NOTIFY = 278,
    DENOTIFY = 279,
    ANYOF = 280,
    ALLOF = 281,
    EXISTS = 282,
    SFALSE = 283,
    STRUE = 284,
    HEADER = 285,
    NOT = 286,
    SIZE = 287,
    ADDRESS = 288,
    ENVELOPE = 289,
    BODY = 290,
    COMPARATOR = 291,
    IS = 292,
    CONTAINS = 293,
    MATCHES = 294,
    REGEX = 295,
    COUNT = 296,
    VALUE = 297,
    OVER = 298,
    UNDER = 299,
    GT = 300,
    GE = 301,
    LT = 302,
    LE = 303,
    EQ = 304,
    NE = 305,
    ALL = 306,
    LOCALPART = 307,
    DOMAIN = 308,
    USER = 309,
    DETAIL = 310,
    RAW = 311,
    TEXT = 312,
    CONTENT = 313,
    DAYS = 314,
    ADDRESSES = 315,
    SUBJECT = 316,
    FROM = 317,
    HANDLE = 318,
    MIME = 319,
    SECONDS = 320,
    METHOD = 321,
    ID = 322,
    OPTIONS = 323,
    LOW = 324,
    NORMAL = 325,
    HIGH = 326,
    ANY = 327,
    MESSAGE = 328,
    INCLUDE = 329,
    PERSONAL = 330,
    GLOBAL = 331,
    RETURN = 332,
    OPTIONAL = 333,
    ONCE = 334,
    COPY = 335,
    DATE = 336,
    CURRENTDATE = 337,
    INDEX = 338,
    LAST = 339,
    ZONE = 340,
    ORIGINALZONE = 341,
    YEAR = 342,
    MONTH = 343,
    DAY = 344,
    JULIAN = 345,
    HOUR = 346,
    MINUTE = 347,
    SECOND = 348,
    TIME = 349,
    ISO8601 = 350,
    STD11 = 351,
    WEEKDAY = 352
  };
#endif
/* Tokens.  */
#define NUMBER 258
#define STRING 259
#define IF 260
#define ELSIF 261
#define ELSE 262
#define REJCT 263
#define FILEINTO 264
#define REDIRECT 265
#define KEEP 266
#define STOP 267
#define DISCARD 268
#define VACATION 269
#define REQUIRE 270
#define SETFLAG 271
#define ADDFLAG 272
#define REMOVEFLAG 273
#define MARK 274
#define UNMARK 275
#define HASFLAG 276
#define FLAGS 277
#define NOTIFY 278
#define DENOTIFY 279
#define ANYOF 280
#define ALLOF 281
#define EXISTS 282
#define SFALSE 283
#define STRUE 284
#define HEADER 285
#define NOT 286
#define SIZE 287
#define ADDRESS 288
#define ENVELOPE 289
#define BODY 290
#define COMPARATOR 291
#define IS 292
#define CONTAINS 293
#define MATCHES 294
#define REGEX 295
#define COUNT 296
#define VALUE 297
#define OVER 298
#define UNDER 299
#define GT 300
#define GE 301
#define LT 302
#define LE 303
#define EQ 304
#define NE 305
#define ALL 306
#define LOCALPART 307
#define DOMAIN 308
#define USER 309
#define DETAIL 310
#define RAW 311
#define TEXT 312
#define CONTENT 313
#define DAYS 314
#define ADDRESSES 315
#define SUBJECT 316
#define FROM 317
#define HANDLE 318
#define MIME 319
#define SECONDS 320
#define METHOD 321
#define ID 322
#define OPTIONS 323
#define LOW 324
#define NORMAL 325
#define HIGH 326
#define ANY 327
#define MESSAGE 328
#define INCLUDE 329
#define PERSONAL 330
#define GLOBAL 331
#define RETURN 332
#define OPTIONAL 333
#define ONCE 334
#define COPY 335
#define DATE 336
#define CURRENTDATE 337
#define INDEX 338
#define LAST 339
#define ZONE 340
#define ORIGINALZONE 341
#define YEAR 342
#define MONTH 343
#define DAY 344
#define JULIAN 345
#define HOUR 346
#define MINUTE 347
#define SECOND 348
#define TIME 349
#define ISO8601 350
#define STD11 351
#define WEEKDAY 352

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 213 "sieve/sieve.y" /* yacc.c:355  */

    int nval;
    char *sval;
    strarray_t *sl;
    test_t *test;
    testlist_t *testl;
    commandlist_t *cl;
    struct vtags *vtag;
    struct aetags *aetag;
    struct htags *htag;
    struct btags *btag;
    struct ntags *ntag;
    struct dtags *dtag;
    struct itags *itag;
    struct dttags *dttag;
    struct ftags *ftag;

#line 536 "sieve/sieve.c" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int sieveparse (sieve_script_t *parse_script);

#endif /* !YY_SIEVE_SIEVE_SIEVE_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 552 "sieve/sieve.c" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  8
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   333

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  106
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  31
/* YYNRULES -- Number of rules.  */
#define YYNRULES  128
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  202

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   352

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     104,   105,     2,     2,   101,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    98,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    99,     2,   100,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   102,     2,   103,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   274,   274,   275,   278,   279,   282,   290,   291,   294,
     295,   296,   299,   300,   301,   304,   313,   321,   325,   326,
     327,   328,   337,   348,   359,   370,   375,   381,   389,   399,
     412,   419,   420,   423,   426,   429,   434,   435,   438,   441,
     444,   447,   452,   453,   456,   471,   481,   482,   483,   486,
     487,   490,   497,   505,   511,   517,   523,   529,   530,   536,
     540,   546,   547,   550,   551,   552,   553,   554,   555,   580,
     602,   624,   650,   651,   653,   669,   693,   696,   697,   704,
     705,   710,   714,   722,   730,   739,   750,   751,   755,   763,
     771,   780,   791,   792,   797,   802,   810,   814,   822,   832,
     833,   838,   847,   855,   865,   875,   884,   890,   891,   892,
     893,   898,   904,   905,   906,   907,   914,   919,   927,   928,
     931,   938,   939,   943,   959,   960,   966,   969,   970
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NUMBER", "STRING", "IF", "ELSIF",
  "ELSE", "REJCT", "FILEINTO", "REDIRECT", "KEEP", "STOP", "DISCARD",
  "VACATION", "REQUIRE", "SETFLAG", "ADDFLAG", "REMOVEFLAG", "MARK",
  "UNMARK", "HASFLAG", "FLAGS", "NOTIFY", "DENOTIFY", "ANYOF", "ALLOF",
  "EXISTS", "SFALSE", "STRUE", "HEADER", "NOT", "SIZE", "ADDRESS",
  "ENVELOPE", "BODY", "COMPARATOR", "IS", "CONTAINS", "MATCHES", "REGEX",
  "COUNT", "VALUE", "OVER", "UNDER", "GT", "GE", "LT", "LE", "EQ", "NE",
  "ALL", "LOCALPART", "DOMAIN", "USER", "DETAIL", "RAW", "TEXT", "CONTENT",
  "DAYS", "ADDRESSES", "SUBJECT", "FROM", "HANDLE", "MIME", "SECONDS",
  "METHOD", "ID", "OPTIONS", "LOW", "NORMAL", "HIGH", "ANY", "MESSAGE",
  "INCLUDE", "PERSONAL", "GLOBAL", "RETURN", "OPTIONAL", "ONCE", "COPY",
  "DATE", "CURRENTDATE", "INDEX", "LAST", "ZONE", "ORIGINALZONE", "YEAR",
  "MONTH", "DAY", "JULIAN", "HOUR", "MINUTE", "SECOND", "TIME", "ISO8601",
  "STD11", "WEEKDAY", "';'", "'['", "']'", "','", "'{'", "'}'", "'('",
  "')'", "$accept", "start", "reqs", "require", "commands", "command",
  "elsif", "action", "itags", "ntags", "dtags", "priority", "vtags",
  "stringlist", "strings", "block", "test", "addrorenv", "aetags", "htags",
  "btags", "dttags", "addrparttag", "comptag", "relcomp", "sizetag",
  "copy", "ftags", "rtags", "testlist", "tests", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,    59,    91,
      93,    44,   123,   125,    40,    41
};
# endif

#define YYPACT_NINF -138

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-138)))

#define YYTABLE_NINF -8

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      -5,    -1,    13,   256,    -5,  -138,    32,   -60,  -138,   -59,
     181,    36,  -138,  -138,  -138,  -138,  -138,  -138,    -1,    -1,
      -1,  -138,  -138,  -138,  -138,  -138,  -138,  -138,     6,   -57,
    -138,  -138,   -73,  -138,  -138,  -138,  -138,   -56,   -56,    -1,
    -138,  -138,  -138,   181,   -12,  -138,  -138,  -138,  -138,  -138,
     -52,  -138,  -138,    31,     4,   -17,   109,  -138,  -138,  -138,
     215,     5,    68,  -138,  -138,  -138,    47,    95,   181,  -138,
    -138,  -138,    95,  -138,  -138,  -138,    49,   119,   148,   159,
      77,    27,    66,  -138,    -1,  -138,  -138,  -138,  -138,  -138,
      53,    -1,    54,    56,    58,  -138,    62,    63,    64,    -1,
    -138,  -138,  -138,    65,  -138,  -138,  -138,  -138,  -138,  -138,
    -138,  -138,    67,    73,  -138,  -138,  -138,  -138,  -138,  -138,
      75,    89,  -138,  -138,  -138,   108,   -44,     9,    -1,  -138,
     112,  -138,  -138,    -1,  -138,  -138,   118,   120,   121,   123,
    -138,   124,  -138,  -138,   125,    -1,  -138,    35,   181,   -52,
    -138,   126,  -138,  -138,  -138,  -138,  -138,   136,  -138,    -1,
    -138,  -138,   141,  -138,  -138,  -138,  -138,  -138,  -138,  -138,
    -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,   181,
    -138,  -138,  -138,  -138,  -138,    -1,  -138,  -138,  -138,  -138,
    -138,  -138,   -52,  -138,  -138,  -138,  -138,  -138,  -138,  -138,
      27,  -138
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,     0,     0,     0,     4,    58,     0,     0,     1,     0,
       0,     0,   121,   124,   121,    19,    20,    49,     0,     0,
       0,    25,    26,    36,    42,    31,    30,     3,     0,     0,
       5,    59,     0,     6,    11,    76,    86,     0,     0,     0,
      66,    67,    86,     0,     0,    77,    78,    92,    99,    99,
       0,    79,    15,     0,     0,    18,     0,    22,    23,    24,
      27,    28,     0,     8,     9,    57,     0,     0,     0,    63,
      64,    65,     0,    72,   118,   119,     0,     0,     0,     0,
       0,    12,     0,    16,     0,   120,   122,    17,   125,    21,
       0,     0,     0,     0,     0,    56,     0,     0,     0,     0,
      46,    47,    48,     0,    40,   112,   113,   114,   115,   116,
     117,    43,     0,     0,    29,    32,    33,    35,    34,    60,
       0,     0,    91,    69,    87,     0,   127,     0,     0,    73,
       0,    93,    94,     0,    71,    96,     0,     0,     0,     0,
     104,     0,   106,   100,     0,     0,    62,     0,     0,     0,
      10,     0,   107,   108,   109,   110,   111,     0,    85,     0,
      80,    81,     0,   123,    50,    52,    53,    54,    55,    51,
      38,    37,    39,    41,    44,    45,    89,    90,    88,     0,
     126,    68,    98,    95,    97,     0,   102,   103,   105,   101,
      75,    61,     0,    14,    83,    84,    70,    82,   128,    74,
      12,    13
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -138,  -138,   144,  -138,   -19,  -138,   -47,  -138,  -138,  -138,
    -138,    79,  -138,   -18,  -138,  -137,    -6,  -138,  -138,   122,
    -138,   113,  -138,   142,   169,  -138,   127,   152,  -138,   145,
      12
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,     4,    27,    28,   150,    29,    62,    60,
      61,   104,    56,     7,    32,    81,   126,    51,    82,    67,
      77,    78,   160,   124,   125,    76,    86,    53,    54,    69,
     127
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      57,    58,    59,     5,    50,    84,    -7,     9,    87,    63,
       1,    10,   193,     8,    11,    12,    13,    14,    15,    16,
      17,    71,    18,    19,    20,    21,    22,    65,    66,    23,
      24,    74,    75,   148,   149,    83,    31,    73,    33,    34,
      52,    64,   105,   106,   107,   108,   109,   110,    68,   123,
      80,   119,   129,    84,   128,   200,   164,   179,   166,   134,
     167,   147,   168,    85,   159,   169,   163,   170,   171,   173,
       5,   174,   114,   165,   100,   101,   102,   175,     9,   176,
      25,   172,    10,    26,    85,    11,    12,    13,    14,    15,
      16,    17,   177,    18,    19,    20,    21,    22,     6,     5,
      23,    24,   151,   105,   106,   107,   108,   109,   110,    -7,
     181,    85,   178,    89,   180,   183,   182,   152,   153,   154,
     155,   156,   184,     5,   185,   186,   187,   190,   188,   189,
     194,   120,   105,   106,   107,   108,   109,   110,   191,   195,
     111,   196,   192,   115,   116,   197,   117,   118,    30,   157,
     158,    25,   137,   201,    26,   130,   105,   106,   107,   108,
     109,   110,    79,   145,    72,     6,    55,   199,    90,    91,
      92,    93,    94,    95,    96,   131,   132,   133,   121,   122,
     146,    88,    35,    70,   138,   105,   106,   107,   108,   109,
     110,   198,     0,     0,     6,   138,   105,   106,   107,   108,
     109,   110,    36,   112,     0,     0,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,     0,     6,   135,
     143,   143,     0,     0,   161,     0,     0,     0,     0,     0,
     113,   139,   140,   141,   142,     0,     0,     0,     0,     0,
       0,     0,   139,   140,   141,   142,   136,   144,   144,     0,
       0,   162,     0,     0,     0,     0,    -2,     9,     0,     0,
       0,    10,    48,    49,    11,    12,    13,    14,    15,    16,
      17,     0,    18,    19,    20,    21,    22,     0,     0,    23,
      24,    97,    98,    99,   100,   101,   102,     0,   103,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      25,     0,     0,    26
};

static const yytype_int16 yycheck[] =
{
      18,    19,    20,     4,    10,    22,     0,     1,     4,    28,
      15,     5,   149,     0,     8,     9,    10,    11,    12,    13,
      14,    39,    16,    17,    18,    19,    20,   100,   101,    23,
      24,    43,    44,     6,     7,     4,     4,    43,    98,    98,
       4,    98,    37,    38,    39,    40,    41,    42,   104,    67,
     102,     4,     3,    22,    72,   192,     3,   101,     4,    77,
       4,    80,     4,    80,    82,     3,    84,     4,     4,     4,
       4,     4,     4,    91,    69,    70,    71,     4,     1,     4,
      74,    99,     5,    77,    80,     8,     9,    10,    11,    12,
      13,    14,     3,    16,    17,    18,    19,    20,    99,     4,
      23,    24,    36,    37,    38,    39,    40,    41,    42,   103,
     128,    80,     4,     4,   105,   133,     4,    51,    52,    53,
      54,    55,     4,     4,     4,     4,     3,   145,     4,     4,
       4,    36,    37,    38,    39,    40,    41,    42,   103,     3,
      61,   159,   148,    75,    76,     4,    78,    79,     4,    83,
      84,    74,     4,   200,    77,    36,    37,    38,    39,    40,
      41,    42,    49,     4,    42,    99,    14,   185,    59,    60,
      61,    62,    63,    64,    65,    56,    57,    58,    83,    84,
     103,    54,     1,    38,    36,    37,    38,    39,    40,    41,
      42,   179,    -1,    -1,    99,    36,    37,    38,    39,    40,
      41,    42,    21,    61,    -1,    -1,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    -1,    99,    77,
      78,    79,    -1,    -1,    82,    -1,    -1,    -1,    -1,    -1,
      61,    83,    84,    85,    86,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    83,    84,    85,    86,    77,    78,    79,    -1,
      -1,    82,    -1,    -1,    -1,    -1,     0,     1,    -1,    -1,
      -1,     5,    81,    82,     8,     9,    10,    11,    12,    13,
      14,    -1,    16,    17,    18,    19,    20,    -1,    -1,    23,
      24,    66,    67,    68,    69,    70,    71,    -1,    73,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      74,    -1,    -1,    77
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    15,   107,   108,   109,     4,    99,   119,     0,     1,
       5,     8,     9,    10,    11,    12,    13,    14,    16,    17,
      18,    19,    20,    23,    24,    74,    77,   110,   111,   113,
     108,     4,   120,    98,    98,     1,    21,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    81,    82,
     122,   123,     4,   133,   134,   133,   118,   119,   119,   119,
     115,   116,   114,   110,    98,   100,   101,   125,   104,   135,
     135,   119,   125,   122,    43,    44,   131,   126,   127,   127,
     102,   121,   124,     4,    22,    80,   132,     4,   132,     4,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    73,   117,    37,    38,    39,    40,    41,
      42,   117,   129,   130,     4,    75,    76,    78,    79,     4,
      36,    83,    84,   119,   129,   130,   122,   136,   119,     3,
      36,    56,    57,    58,   119,   129,   130,     4,    36,    83,
      84,    85,    86,   129,   130,     4,   103,   110,     6,     7,
     112,    36,    51,    52,    53,    54,    55,    83,    84,   119,
     128,   129,   130,   119,     3,   119,     4,     4,     4,     3,
       4,     4,   119,     4,     4,     4,     4,     3,     4,   101,
     105,   119,     4,   119,     4,     4,     4,     3,     4,     4,
     119,   103,   122,   121,     4,     3,   119,     4,   136,   119,
     121,   112
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   106,   107,   107,   108,   108,   109,   110,   110,   111,
     111,   111,   112,   112,   112,   113,   113,   113,   113,   113,
     113,   113,   113,   113,   113,   113,   113,   113,   113,   113,
     113,   114,   114,   114,   114,   114,   115,   115,   115,   115,
     115,   115,   116,   116,   116,   116,   117,   117,   117,   118,
     118,   118,   118,   118,   118,   118,   118,   119,   119,   120,
     120,   121,   121,   122,   122,   122,   122,   122,   122,   122,
     122,   122,   122,   122,   122,   122,   122,   123,   123,   124,
     124,   124,   124,   124,   124,   124,   125,   125,   125,   125,
     125,   125,   126,   126,   126,   126,   126,   126,   126,   127,
     127,   127,   127,   127,   127,   127,   127,   128,   128,   128,
     128,   128,   129,   129,   129,   129,   130,   130,   131,   131,
     132,   133,   133,   133,   134,   134,   135,   136,   136
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     0,     2,     3,     1,     2,     2,
       4,     2,     0,     4,     2,     2,     3,     3,     2,     1,
       1,     3,     2,     2,     2,     1,     1,     2,     2,     3,
       1,     0,     2,     2,     2,     2,     0,     3,     3,     3,
       2,     3,     0,     2,     3,     3,     1,     1,     1,     0,
       3,     3,     3,     3,     3,     3,     2,     3,     1,     1,
       3,     3,     2,     2,     2,     2,     1,     1,     4,     3,
       4,     3,     2,     3,     5,     4,     1,     1,     1,     0,
       2,     2,     3,     3,     3,     2,     0,     2,     3,     3,
       3,     2,     0,     2,     2,     3,     2,     3,     3,     0,
       2,     3,     3,     3,     2,     3,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     2,     3,     0,     2,     3,     1,     3
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (parse_script, YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, parse_script); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, sieve_script_t *parse_script)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (parse_script);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, sieve_script_t *parse_script)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, parse_script);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule, sieve_script_t *parse_script)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              , parse_script);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, parse_script); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, sieve_script_t *parse_script)
{
  YYUSE (yyvaluep);
  YYUSE (parse_script);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yytype)
    {
          case 110: /* commands  */
#line 267 "sieve/sieve.y" /* yacc.c:1257  */
      { free_tree(((*yyvaluep).cl)); }
#line 1571 "sieve/sieve.c" /* yacc.c:1257  */
        break;

    case 111: /* command  */
#line 267 "sieve/sieve.y" /* yacc.c:1257  */
      { free_tree(((*yyvaluep).cl)); }
#line 1577 "sieve/sieve.c" /* yacc.c:1257  */
        break;

    case 112: /* elsif  */
#line 267 "sieve/sieve.y" /* yacc.c:1257  */
      { free_tree(((*yyvaluep).cl)); }
#line 1583 "sieve/sieve.c" /* yacc.c:1257  */
        break;

    case 113: /* action  */
#line 267 "sieve/sieve.y" /* yacc.c:1257  */
      { free_tree(((*yyvaluep).cl)); }
#line 1589 "sieve/sieve.c" /* yacc.c:1257  */
        break;

    case 121: /* block  */
#line 267 "sieve/sieve.y" /* yacc.c:1257  */
      { free_tree(((*yyvaluep).cl)); }
#line 1595 "sieve/sieve.c" /* yacc.c:1257  */
        break;


      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (sieve_script_t *parse_script)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, parse_script);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 274 "sieve/sieve.y" /* yacc.c:1646  */
    { parse_script->cmds = NULL; }
#line 1863 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 3:
#line 275 "sieve/sieve.y" /* yacc.c:1646  */
    { parse_script->cmds = (yyvsp[0].cl); }
#line 1869 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 6:
#line 282 "sieve/sieve.y" /* yacc.c:1646  */
    { char *err = check_reqs(parse_script, (yyvsp[-1].sl));
                                  if (err) {
				    yyerror(parse_script, err);
				    free(err);
				    YYERROR; 
                                  } }
#line 1880 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 7:
#line 290 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.cl) = (yyvsp[0].cl); }
#line 1886 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 8:
#line 291 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyvsp[-1].cl)->next = (yyvsp[0].cl); (yyval.cl) = (yyvsp[-1].cl); }
#line 1892 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 9:
#line 294 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.cl) = (yyvsp[-1].cl); }
#line 1898 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 10:
#line 295 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.cl) = new_if((yyvsp[-2].test), (yyvsp[-1].cl), (yyvsp[0].cl)); }
#line 1904 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 11:
#line 296 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.cl) = new_command(STOP); }
#line 1910 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 12:
#line 299 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.cl) = NULL; }
#line 1916 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 13:
#line 300 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.cl) = new_if((yyvsp[-2].test), (yyvsp[-1].cl), (yyvsp[0].cl)); }
#line 1922 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 14:
#line 301 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.cl) = (yyvsp[0].cl); }
#line 1928 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 15:
#line 304 "sieve/sieve.y" /* yacc.c:1646  */
    { if (!parse_script->support.reject) {
				     yyerror(parse_script, "reject MUST be enabled with \"require\"");
				     YYERROR;
				   }
				   if (!verify_utf8(parse_script, (yyvsp[0].sval))) {
				     YYERROR; /* vu should call yyerror() */
				   }
				   (yyval.cl) = new_command(REJCT);
				   (yyval.cl)->u.str = (yyvsp[0].sval); }
#line 1942 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 16:
#line 313 "sieve/sieve.y" /* yacc.c:1646  */
    { if (!parse_script->support.fileinto) {
				     yyerror(parse_script, "fileinto MUST be enabled with \"require\"");
	                             YYERROR;
                                   }
				   if (!verify_mailbox(parse_script, (yyvsp[0].sval))) {
				     YYERROR; /* vm should call yyerror() */
				   }
	                           (yyval.cl) = build_fileinto(FILEINTO, canon_ftags((yyvsp[-1].ftag)), (yyvsp[0].sval)); }
#line 1955 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 17:
#line 321 "sieve/sieve.y" /* yacc.c:1646  */
    { if (!verify_address(parse_script, (yyvsp[0].sval))) {
				     YYERROR; /* va should call yyerror() */
				   }
	                           (yyval.cl) = build_redirect(REDIRECT, (yyvsp[-1].nval), (yyvsp[0].sval)); }
#line 1964 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 18:
#line 325 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.cl) = build_keep(KEEP,canon_ftags((yyvsp[0].ftag))); }
#line 1970 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 19:
#line 326 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.cl) = new_command(STOP); }
#line 1976 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 20:
#line 327 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.cl) = new_command(DISCARD); }
#line 1982 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 21:
#line 328 "sieve/sieve.y" /* yacc.c:1646  */
    { if (!parse_script->support.vacation) {
				     yyerror(parse_script, "vacation MUST be enabled with \"require\"");
				     YYERROR;
				   }
				   if (((yyvsp[-1].vtag)->mime == -1) && !verify_utf8(parse_script, (yyvsp[0].sval))) {
				     YYERROR; /* vu should call yyerror() */
				   }
  				   (yyval.cl) = build_vacation(VACATION,
					    canon_vtags(parse_script, (yyvsp[-1].vtag)), (yyvsp[0].sval)); }
#line 1996 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 22:
#line 337 "sieve/sieve.y" /* yacc.c:1646  */
    { if (!(parse_script->support.imapflags ||
					parse_script->support.imap4flags)) {
                                    yyerror(parse_script, "imap4flags MUST be enabled with \"require\"");
                                    YYERROR;
                                   }
                                  verify_flaglist((yyvsp[0].sl));
                                  if(!(yyvsp[0].sl)->count) {
                                      strarray_add((yyvsp[0].sl), "");
                                  }
                                  (yyval.cl) = new_command(SETFLAG);
                                  (yyval.cl)->u.sl = (yyvsp[0].sl); }
#line 2012 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 23:
#line 348 "sieve/sieve.y" /* yacc.c:1646  */
    { if (!(parse_script->support.imapflags ||
					parse_script->support.imap4flags)) {
                                    yyerror(parse_script, "imap4flags MUST be enabled with \"require\"");
                                    YYERROR;
                                    }
                                  verify_flaglist((yyvsp[0].sl));
                                  if(!(yyvsp[0].sl)->count) {
                                      strarray_add((yyvsp[0].sl), "");
                                  }
                                  (yyval.cl) = new_command(ADDFLAG);
                                  (yyval.cl)->u.sl = (yyvsp[0].sl); }
#line 2028 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 24:
#line 359 "sieve/sieve.y" /* yacc.c:1646  */
    { if (!(parse_script->support.imapflags ||
					parse_script->support.imap4flags)) {
                                    yyerror(parse_script, "imap4flags MUST be enabled with \"require\"");
                                    YYERROR;
                                    }
                                  verify_flaglist((yyvsp[0].sl));
                                  if(!(yyvsp[0].sl)->count) {
                                      strarray_add((yyvsp[0].sl), "");
                                  }
                                  (yyval.cl) = new_command(REMOVEFLAG);
                                  (yyval.cl)->u.sl = (yyvsp[0].sl); }
#line 2044 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 25:
#line 370 "sieve/sieve.y" /* yacc.c:1646  */
    { if (!parse_script->support.imapflags) {
                                    yyerror(parse_script, "imapflags MUST be enabled with \"require\"");
                                    YYERROR;
                                    }
                                  (yyval.cl) = new_command(MARK); }
#line 2054 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 26:
#line 375 "sieve/sieve.y" /* yacc.c:1646  */
    { if (!parse_script->support.imapflags) {
                                    yyerror(parse_script, "imapflags MUST be enabled with \"require\"");
                                    YYERROR;
                                    }
                                  (yyval.cl) = new_command(UNMARK); }
#line 2064 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 27:
#line 381 "sieve/sieve.y" /* yacc.c:1646  */
    { if (!parse_script->support.notify) {
				       yyerror(parse_script, "notify MUST be enabled with \"require\"");
				       (yyval.cl) = new_command(NOTIFY); 
				       YYERROR;
	 			    } else {
				      (yyval.cl) = build_notify(NOTIFY,
				             canon_ntags((yyvsp[0].ntag)));
				    } }
#line 2077 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 28:
#line 389 "sieve/sieve.y" /* yacc.c:1646  */
    { if (!parse_script->support.notify) {
                                       yyerror(parse_script, "notify MUST be enabled with \"require\"");
				       (yyval.cl) = new_command(DENOTIFY);
				       YYERROR;
				    } else {
					(yyval.cl) = build_denotify(DENOTIFY, canon_dtags((yyvsp[0].dtag)));
					if ((yyval.cl) == NULL) { 
			yyerror(parse_script, "unable to find a compatible comparator");
			YYERROR; } } }
#line 2091 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 29:
#line 399 "sieve/sieve.y" /* yacc.c:1646  */
    { if (!parse_script->support.include) {
				     yyerror(parse_script, "include MUST be enabled with \"require\"");
	                             YYERROR;
                                   }
				   int i;
				   for (i = 0; (yyvsp[0].sval)[i] != '\0'; i++) {
				     if ((yyvsp[0].sval)[i] == '/') {
				       yyerror(parse_script, "included script name must not contain slash"); YYERROR;
				       break;
				     }
				   }
				   (yyval.cl) = build_include(INCLUDE, (yyvsp[-1].itag), (yyvsp[0].sval));
				 }
#line 2109 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 30:
#line 412 "sieve/sieve.y" /* yacc.c:1646  */
    { if (!parse_script->support.include) {
                                    yyerror(parse_script, "include MUST be enabled with \"require\"");
                                    YYERROR;
                                  }
                                   (yyval.cl) = new_command(RETURN); }
#line 2119 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 31:
#line 419 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.itag) = new_itags(); }
#line 2125 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 32:
#line 420 "sieve/sieve.y" /* yacc.c:1646  */
    { if ((yyval.itag)->location != -1) {
				     yyerror(parse_script, "duplicate location (:personal or :global)"); YYERROR; }
				   else { (yyval.itag)->location = PERSONAL; }}
#line 2133 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 33:
#line 423 "sieve/sieve.y" /* yacc.c:1646  */
    { if ((yyval.itag)->location != -1) {
				     yyerror(parse_script, "duplicate location (:personal or :global)"); YYERROR; }
				   else { (yyval.itag)->location = GLOBAL; }}
#line 2141 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 34:
#line 426 "sieve/sieve.y" /* yacc.c:1646  */
    { if ((yyval.itag)->once != -1) {
				     yyerror(parse_script, "duplicate :once"); YYERROR; }
				   else { (yyval.itag)->once = 1; }}
#line 2149 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 35:
#line 429 "sieve/sieve.y" /* yacc.c:1646  */
    { if ((yyval.itag)->optional != -1) {
				     yyerror(parse_script, "duplicate :optional"); YYERROR; }
				   else { (yyval.itag)->optional = 1; }}
#line 2157 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 36:
#line 434 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.ntag) = new_ntags(); }
#line 2163 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 37:
#line 435 "sieve/sieve.y" /* yacc.c:1646  */
    { if ((yyval.ntag)->id != NULL) { 
					yyerror(parse_script, "duplicate :method"); YYERROR; }
				   else { (yyval.ntag)->id = (yyvsp[0].sval); } }
#line 2171 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 38:
#line 438 "sieve/sieve.y" /* yacc.c:1646  */
    { if ((yyval.ntag)->method != NULL) { 
					yyerror(parse_script, "duplicate :method"); YYERROR; }
				   else { (yyval.ntag)->method = (yyvsp[0].sval); } }
#line 2179 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 39:
#line 441 "sieve/sieve.y" /* yacc.c:1646  */
    { if ((yyval.ntag)->options != NULL) { 
					yyerror(parse_script, "duplicate :options"); YYERROR; }
				     else { (yyval.ntag)->options = (yyvsp[0].sl); } }
#line 2187 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 40:
#line 444 "sieve/sieve.y" /* yacc.c:1646  */
    { if ((yyval.ntag)->priority != -1) { 
                                 yyerror(parse_script, "duplicate :priority"); YYERROR; }
                                   else { (yyval.ntag)->priority = (yyvsp[0].nval); } }
#line 2195 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 41:
#line 447 "sieve/sieve.y" /* yacc.c:1646  */
    { if ((yyval.ntag)->message != NULL) { 
					yyerror(parse_script, "duplicate :message"); YYERROR; }
				   else { (yyval.ntag)->message = (yyvsp[0].sval); } }
#line 2203 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 42:
#line 452 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.dtag) = new_dtags(); }
#line 2209 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 43:
#line 453 "sieve/sieve.y" /* yacc.c:1646  */
    { if ((yyval.dtag)->priority != -1) { 
				yyerror(parse_script, "duplicate priority level"); YYERROR; }
				   else { (yyval.dtag)->priority = (yyvsp[0].nval); } }
#line 2217 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 44:
#line 456 "sieve/sieve.y" /* yacc.c:1646  */
    { if ((yyval.dtag)->comptag != -1)
	                             { 
					 yyerror(parse_script, "duplicate comparator type tag"); YYERROR;
				     }
	                           (yyval.dtag)->comptag = (yyvsp[-1].nval);
#ifdef ENABLE_REGEX
				   if ((yyval.dtag)->comptag == REGEX)
				   {
				       int cflags = REG_EXTENDED |
					   REG_NOSUB | REG_ICASE;
				       if (!verify_regex(parse_script, (yyvsp[0].sval), cflags)) { YYERROR; }
				   }
#endif
				   (yyval.dtag)->pattern = (yyvsp[0].sval);
	                          }
#line 2237 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 45:
#line 471 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.dtag) = (yyvsp[-2].dtag);
				   if ((yyval.dtag)->comptag != -1) { 
			yyerror(parse_script, "duplicate comparator type tag"); YYERROR; }
				   else { (yyval.dtag)->comptag = (yyvsp[-1].nval);
				   (yyval.dtag)->relation = verify_relat(parse_script, (yyvsp[0].sval));
				   if ((yyval.dtag)->relation==-1) 
				     {YYERROR; /*vr called yyerror()*/ }
				   } }
#line 2250 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 46:
#line 481 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.nval) = LOW; }
#line 2256 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 47:
#line 482 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.nval) = NORMAL; }
#line 2262 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 48:
#line 483 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.nval) = HIGH; }
#line 2268 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 49:
#line 486 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.vtag) = new_vtags(); }
#line 2274 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 50:
#line 487 "sieve/sieve.y" /* yacc.c:1646  */
    { if ((yyval.vtag)->seconds != -1) {
					yyerror(parse_script, "duplicate :days or :seconds"); YYERROR; }
				   else { (yyval.vtag)->seconds = (yyvsp[0].nval) * DAY2SEC; } }
#line 2282 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 51:
#line 490 "sieve/sieve.y" /* yacc.c:1646  */
    { if (!parse_script->support.vacation_seconds) {
				     yyerror(parse_script, "vacation-seconds not required");
				     YYERROR;
				   }
				   if ((yyval.vtag)->seconds != -1) {
					yyerror(parse_script, "duplicate :days or :seconds"); YYERROR; }
				   else { (yyval.vtag)->seconds = (yyvsp[0].nval); } }
#line 2294 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 52:
#line 497 "sieve/sieve.y" /* yacc.c:1646  */
    { if ((yyval.vtag)->addresses != NULL) {
					yyerror(parse_script, "duplicate :addresses");
					YYERROR;
				       } else if (!verify_stringlist(parse_script, (yyvsp[0].sl),
							verify_address)) {
					  YYERROR;
				       } else {
					 (yyval.vtag)->addresses = (yyvsp[0].sl); } }
#line 2307 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 53:
#line 505 "sieve/sieve.y" /* yacc.c:1646  */
    { if ((yyval.vtag)->subject != NULL) { 
					yyerror(parse_script, "duplicate :subject");
					YYERROR;
				   } else if (!verify_utf8(parse_script, (yyvsp[0].sval))) {
				        YYERROR; /* vu should call yyerror() */
				   } else { (yyval.vtag)->subject = (yyvsp[0].sval); } }
#line 2318 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 54:
#line 511 "sieve/sieve.y" /* yacc.c:1646  */
    { if ((yyval.vtag)->from != NULL) { 
					yyerror(parse_script, "duplicate :from");
					YYERROR;
				   } else if (!verify_address(parse_script, (yyvsp[0].sval))) {
				        YYERROR; /* va should call yyerror() */
				   } else { (yyval.vtag)->from = (yyvsp[0].sval); } }
#line 2329 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 55:
#line 517 "sieve/sieve.y" /* yacc.c:1646  */
    { if ((yyval.vtag)->handle != NULL) { 
					yyerror(parse_script, "duplicate :handle");
					YYERROR;
				   } else if (!verify_utf8(parse_script, (yyvsp[0].sval))) {
				        YYERROR; /* vu should call yyerror() */
				   } else { (yyval.vtag)->handle = (yyvsp[0].sval); } }
#line 2340 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 56:
#line 523 "sieve/sieve.y" /* yacc.c:1646  */
    { if ((yyval.vtag)->mime != -1) { 
					yyerror(parse_script, "duplicate :mime");
					YYERROR; }
				   else { (yyval.vtag)->mime = MIME; } }
#line 2349 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 57:
#line 529 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.sl) = (yyvsp[-1].sl); }
#line 2355 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 58:
#line 530 "sieve/sieve.y" /* yacc.c:1646  */
    {
				    (yyval.sl) = strarray_new();
				    strarray_appendm((yyval.sl), (yyvsp[0].sval));
				 }
#line 2364 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 59:
#line 536 "sieve/sieve.y" /* yacc.c:1646  */
    {
				    (yyval.sl) = strarray_new();
				    strarray_appendm((yyval.sl), (yyvsp[0].sval));
				 }
#line 2373 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 60:
#line 540 "sieve/sieve.y" /* yacc.c:1646  */
    {
				    (yyval.sl) = (yyvsp[-2].sl);
				    strarray_appendm((yyval.sl), (yyvsp[0].sval));
				 }
#line 2382 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 61:
#line 546 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.cl) = (yyvsp[-1].cl); }
#line 2388 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 62:
#line 547 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.cl) = NULL; }
#line 2394 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 63:
#line 550 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.test) = new_test(ANYOF); (yyval.test)->u.tl = (yyvsp[0].testl); }
#line 2400 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 64:
#line 551 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.test) = new_test(ALLOF); (yyval.test)->u.tl = (yyvsp[0].testl); }
#line 2406 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 65:
#line 552 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.test) = new_test(EXISTS); (yyval.test)->u.sl = (yyvsp[0].sl); }
#line 2412 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 66:
#line 553 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.test) = new_test(SFALSE); }
#line 2418 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 67:
#line 554 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.test) = new_test(STRUE); }
#line 2424 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 68:
#line 556 "sieve/sieve.y" /* yacc.c:1646  */
    {
				     if (!verify_stringlist(parse_script, (yyvsp[-1].sl), verify_header)) {
					 YYERROR; /* vh should call yyerror() */
				     }
				     if (!verify_stringlist(parse_script, (yyvsp[0].sl), verify_utf8)) {
					 YYERROR; /* vu should call yyerror() */
				     }
				     
				     (yyvsp[-2].htag) = canon_htags((yyvsp[-2].htag));
#ifdef ENABLE_REGEX
				     if ((yyvsp[-2].htag)->comptag == REGEX)
				     {
					 if (!(verify_regexs(parse_script, (yyvsp[0].sl), (yyvsp[-2].htag)->comparator)))
					 { YYERROR; }
				     }
#endif
				     (yyval.test) = build_header(HEADER, (yyvsp[-2].htag), (yyvsp[-1].sl), (yyvsp[0].sl));
				     if ((yyval.test) == NULL) { 
					 yyerror(parse_script, "unable to find a compatible comparator");
					 YYERROR; } 
				 }
#line 2450 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 69:
#line 581 "sieve/sieve.y" /* yacc.c:1646  */
    {
				     if (!parse_script->support.imap4flags) {
                                       yyerror(parse_script, "imap4flags MUST be enabled with \"require\"");
				       YYERROR;
				     }

				     (yyvsp[-1].htag) = canon_htags((yyvsp[-1].htag));
#ifdef ENABLE_REGEX
				     if ((yyvsp[-1].htag)->comptag == REGEX)
				     {
					 if (!(verify_regexs(parse_script, (yyvsp[0].sl), (yyvsp[-1].htag)->comparator)))
					 { YYERROR; }
				     }
#endif
				     (yyval.test) = build_hasflag(HASFLAG, (yyvsp[-1].htag), (yyvsp[0].sl));
				     if ((yyval.test) == NULL) {
					 yyerror(parse_script, "unable to find a compatible comparator");
					 YYERROR; }
				 }
#line 2474 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 70:
#line 603 "sieve/sieve.y" /* yacc.c:1646  */
    { 
				     if (((yyvsp[-3].nval) == ADDRESS) &&
					 !verify_stringlist(parse_script, (yyvsp[-1].sl), verify_addrheader))
					 { YYERROR; }
				     else if (((yyvsp[-3].nval) == ENVELOPE) &&
					      !verify_stringlist(parse_script, (yyvsp[-1].sl), verify_envelope))
					 { YYERROR; }
				     (yyvsp[-2].aetag) = canon_aetags((yyvsp[-2].aetag));
#ifdef ENABLE_REGEX
				     if ((yyvsp[-2].aetag)->comptag == REGEX)
				     {
					 if (!( verify_regexs(parse_script, (yyvsp[0].sl), (yyvsp[-2].aetag)->comparator)))
					 { YYERROR; }
				     }
#endif
				     (yyval.test) = build_address((yyvsp[-3].nval), (yyvsp[-2].aetag), (yyvsp[-1].sl), (yyvsp[0].sl));
				     if ((yyval.test) == NULL) { 
					 yyerror(parse_script, "unable to find a compatible comparator");
					 YYERROR; } 
				 }
#line 2499 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 71:
#line 625 "sieve/sieve.y" /* yacc.c:1646  */
    {
				     if (!parse_script->support.body) {
                                       yyerror(parse_script, "body MUST be enabled with \"require\"");
				       YYERROR;
				     }
					
				     if (!verify_stringlist(parse_script, (yyvsp[0].sl), verify_utf8)) {
					 YYERROR; /* vu should call yyerror() */
				     }
				     
				     (yyvsp[-1].btag) = canon_btags((yyvsp[-1].btag));
#ifdef ENABLE_REGEX
				     if ((yyvsp[-1].btag)->comptag == REGEX)
				     {
					 if (!(verify_regexs(parse_script, (yyvsp[0].sl), (yyvsp[-1].btag)->comparator)))
					 { YYERROR; }
				     }
#endif
				     (yyval.test) = build_body(BODY, (yyvsp[-1].btag), (yyvsp[0].sl));
				     if ((yyval.test) == NULL) { 
					 yyerror(parse_script, "unable to find a compatible comparator");
					 YYERROR; } 
				 }
#line 2527 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 72:
#line 650 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.test) = new_test(NOT); (yyval.test)->u.t = (yyvsp[0].test); }
#line 2533 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 73:
#line 651 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.test) = new_test(SIZE); (yyval.test)->u.sz.t = (yyvsp[-1].nval);
		                   (yyval.test)->u.sz.n = (yyvsp[0].nval); }
#line 2540 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 74:
#line 654 "sieve/sieve.y" /* yacc.c:1646  */
    {if (!parse_script->support.date)
                                     { yyerror(parse_script, "date MUST be enabled with \"require\"");
                                       YYERROR; }

                                   (yyvsp[-3].dttag)->date_part = verify_date_part(parse_script, (yyvsp[-1].sval));
                                   if ((yyvsp[-3].dttag)->date_part == -1)
                                     { YYERROR; /*vr called yyerror()*/ }

                                   (yyvsp[-3].dttag) = canon_dttags((yyvsp[-3].dttag));

                                   (yyval.test) = build_date(DATE, (yyvsp[-3].dttag), (yyvsp[-2].sval), (yyvsp[0].sl));
                                   if ((yyval.test) == NULL) {
                                     yyerror(parse_script, "unable to find a compatible comparator");
                                     YYERROR; }
                                 }
#line 2560 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 75:
#line 670 "sieve/sieve.y" /* yacc.c:1646  */
    {if (!parse_script->support.date)
                                     { yyerror(parse_script, "date MUST be enabled with \"require\"");
                                       YYERROR; }

                                   if ((yyvsp[-2].dttag)->index != 0) {
                                     yyerror(parse_script, "index argument is not allowed in currentdate");
                                     YYERROR; }

                                   if ((yyvsp[-2].dttag)->zonetag == ORIGINALZONE) {
                                     yyerror(parse_script, "originalzone argument is not allowed in currentdate");
                                     YYERROR; }

                                   (yyvsp[-2].dttag)->date_part = verify_date_part(parse_script, (yyvsp[-1].sval));
                                   if ((yyvsp[-2].dttag)->date_part == -1)
                                     { YYERROR; /*vr called yyerror()*/ }

                                   (yyvsp[-2].dttag) = canon_dttags((yyvsp[-2].dttag));

                                   (yyval.test) = build_date(CURRENTDATE, (yyvsp[-2].dttag), NULL, (yyvsp[0].sl));
                                   if ((yyval.test) == NULL) {
                                     yyerror(parse_script, "unable to find a compatible comparator");
                                     YYERROR; }
                                 }
#line 2588 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 76:
#line 693 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.test) = NULL; }
#line 2594 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 77:
#line 696 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.nval) = ADDRESS; }
#line 2600 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 78:
#line 697 "sieve/sieve.y" /* yacc.c:1646  */
    {if (!parse_script->support.envelope)
	                              {yyerror(parse_script, "envelope MUST be enabled with \"require\""); YYERROR;}
	                          else{(yyval.nval) = ENVELOPE; }
	                         }
#line 2609 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 79:
#line 704 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.aetag) = new_aetags(); }
#line 2615 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 80:
#line 705 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.aetag) = (yyvsp[-1].aetag);
				   if ((yyval.aetag)->addrtag != -1) {
			yyerror(parse_script, "duplicate or conflicting address part tag");
			YYERROR; }
				   else { (yyval.aetag)->addrtag = (yyvsp[0].nval); } }
#line 2625 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 81:
#line 710 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.aetag) = (yyvsp[-1].aetag);
				   if ((yyval.aetag)->comptag != -1) {
			yyerror(parse_script, "duplicate comparator type tag"); YYERROR; }
				   else { (yyval.aetag)->comptag = (yyvsp[0].nval); } }
#line 2634 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 82:
#line 714 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.aetag) = (yyvsp[-2].aetag);
				   if ((yyval.aetag)->comptag != -1) {
			yyerror(parse_script, "duplicate comparator type tag"); YYERROR; }
				   else { (yyval.aetag)->comptag = (yyvsp[-1].nval);
				   (yyval.aetag)->relation = verify_relat(parse_script, (yyvsp[0].sval));
				   if ((yyval.aetag)->relation==-1)
				     {YYERROR; /*vr called yyerror()*/ }
				   } }
#line 2647 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 83:
#line 722 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.aetag) = (yyvsp[-2].aetag);
				   if ((yyval.aetag)->comparator != NULL) {
				     yyerror(parse_script, "duplicate comparator tag"); YYERROR; }
				   else if (!strcmp((yyvsp[0].sval), "i;ascii-numeric") &&
					    !parse_script->support.i_ascii_numeric) {
			yyerror(parse_script, "comparator-i;ascii-numeric MUST be enabled with \"require\"");
			YYERROR; }
				   else { (yyval.aetag)->comparator = (yyvsp[0].sval); } }
#line 2660 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 84:
#line 730 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.aetag) = (yyvsp[-2].aetag);
				   if (!parse_script->support.index)
				      { yyerror(parse_script, "index MUST be enabled with \"require\"");
				        YYERROR; }
				   if ((yyval.aetag)->index != 0) {
				     yyerror(parse_script, "duplicate index argument"); YYERROR; }
				   if ((yyvsp[0].nval) <= 0) {
				     yyerror(parse_script, "invalid index value"); YYERROR; }
				   else { (yyval.aetag)->index = (yyvsp[0].nval); } }
#line 2674 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 85:
#line 739 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.aetag) = (yyvsp[-1].aetag);
				   if (!parse_script->support.index)
				      { yyerror(parse_script, "index MUST be enabled with \"require\"");
				        YYERROR; }
				   if ((yyval.aetag)->index == 0) {
				     yyerror(parse_script, "index argument is required"); YYERROR; }
				   else if ((yyval.aetag)->index < 0) {
				     yyerror(parse_script, "duplicate last argument"); YYERROR; }
				   else { (yyval.aetag)->index *= -1; } }
#line 2688 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 86:
#line 750 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.htag) = new_htags(); }
#line 2694 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 87:
#line 751 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.htag) = (yyvsp[-1].htag);
				   if ((yyval.htag)->comptag != -1) { 
			yyerror(parse_script, "duplicate comparator type tag"); YYERROR; }
				   else { (yyval.htag)->comptag = (yyvsp[0].nval); } }
#line 2703 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 88:
#line 755 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.htag) = (yyvsp[-2].htag);
				   if ((yyval.htag)->comptag != -1) { 
			yyerror(parse_script, "duplicate comparator type tag"); YYERROR; }
				   else { (yyval.htag)->comptag = (yyvsp[-1].nval);
				   (yyval.htag)->relation = verify_relat(parse_script, (yyvsp[0].sval));
				   if ((yyval.htag)->relation==-1) 
				     {YYERROR; /*vr called yyerror()*/ }
				   } }
#line 2716 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 89:
#line 763 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.htag) = (yyvsp[-2].htag);
				   if ((yyval.htag)->comparator != NULL) { 
			 yyerror(parse_script, "duplicate comparator tag"); YYERROR; }
				   else if (!strcmp((yyvsp[0].sval), "i;ascii-numeric") &&
					    !parse_script->support.i_ascii_numeric) { 
			 yyerror(parse_script, "comparator-i;ascii-numeric MUST be enabled with \"require\"");  YYERROR; }
				   else { 
				     (yyval.htag)->comparator = (yyvsp[0].sval); } }
#line 2729 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 90:
#line 771 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.htag) = (yyvsp[-2].htag);
				   if (!parse_script->support.index)
				      { yyerror(parse_script, "index MUST be enabled with \"require\"");
				        YYERROR; }
				   if ((yyval.htag)->index != 0) {
				     yyerror(parse_script, "duplicate index argument"); YYERROR; }
				   if ((yyvsp[0].nval) <= 0) {
				     yyerror(parse_script, "invalid index value"); YYERROR; }
				   else { (yyval.htag)->index = (yyvsp[0].nval); } }
#line 2743 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 91:
#line 780 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.htag) = (yyvsp[-1].htag);
				   if (!parse_script->support.index)
				      { yyerror(parse_script, "index MUST be enabled with \"require\"");
				        YYERROR; }
				   if ((yyval.htag)->index == 0) {
				     yyerror(parse_script, "index argument is required"); YYERROR; }
				   else if ((yyval.htag)->index < 0) {
				     yyerror(parse_script, "duplicate last argument"); YYERROR; }
				   else { (yyval.htag)->index *= -1; } }
#line 2757 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 92:
#line 791 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.btag) = new_btags(); }
#line 2763 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 93:
#line 792 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.btag) = (yyvsp[-1].btag);
				   if ((yyval.btag)->transform != -1) {
			yyerror(parse_script, "duplicate or conflicting transform tag");
			YYERROR; }
				   else { (yyval.btag)->transform = RAW; } }
#line 2773 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 94:
#line 797 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.btag) = (yyvsp[-1].btag);
				   if ((yyval.btag)->transform != -1) {
			yyerror(parse_script, "duplicate or conflicting transform tag");
			YYERROR; }
				   else { (yyval.btag)->transform = TEXT; } }
#line 2783 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 95:
#line 802 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.btag) = (yyvsp[-2].btag);
				   if ((yyval.btag)->transform != -1) {
			yyerror(parse_script, "duplicate or conflicting transform tag");
			YYERROR; }
				   else {
				       (yyval.btag)->transform = CONTENT;
				       (yyval.btag)->content_types = (yyvsp[0].sl);
				   } }
#line 2796 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 96:
#line 810 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.btag) = (yyvsp[-1].btag);
				   if ((yyval.btag)->comptag != -1) { 
			yyerror(parse_script, "duplicate comparator type tag"); YYERROR; }
				   else { (yyval.btag)->comptag = (yyvsp[0].nval); } }
#line 2805 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 97:
#line 814 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.btag) = (yyvsp[-2].btag);
				   if ((yyval.btag)->comptag != -1) { 
			yyerror(parse_script, "duplicate comparator type tag"); YYERROR; }
				   else { (yyval.btag)->comptag = (yyvsp[-1].nval);
				   (yyval.btag)->relation = verify_relat(parse_script, (yyvsp[0].sval));
				   if ((yyval.btag)->relation==-1) 
				     {YYERROR; /*vr called yyerror()*/ }
				   } }
#line 2818 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 98:
#line 822 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.btag) = (yyvsp[-2].btag);
				   if ((yyval.btag)->comparator != NULL) { 
			 yyerror(parse_script, "duplicate comparator tag"); YYERROR; }
				   else if (!strcmp((yyvsp[0].sval), "i;ascii-numeric") &&
					    !parse_script->support.i_ascii_numeric) { 
			 yyerror(parse_script, "comparator-i;ascii-numeric MUST be enabled with \"require\"");  YYERROR; }
				   else { 
				     (yyval.btag)->comparator = (yyvsp[0].sval); } }
#line 2831 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 99:
#line 832 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.dttag) = new_dttags(); }
#line 2837 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 100:
#line 833 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.dttag) = (yyvsp[-1].dttag);
                                   if ((yyval.dttag)->comptag != -1) {
                                     yyerror(parse_script, "duplicate comparator type tag"); YYERROR; }
                                   else { (yyval.dttag)->comptag = (yyvsp[0].nval); } }
#line 2846 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 101:
#line 838 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.dttag) = (yyvsp[-2].dttag);
                                   if ((yyval.dttag)->comptag != -1) {
                                     yyerror(parse_script, "duplicate comparator type tag"); YYERROR; }
                                   else {
                                     (yyval.dttag)->comptag = (yyvsp[-1].nval);
                                     (yyval.dttag)->relation = verify_relat(parse_script, (yyvsp[0].sval));
                                     if ((yyval.dttag)->relation == -1) {
                                       YYERROR; /*vr called yyerror()*/ } } }
#line 2859 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 102:
#line 847 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.dttag) = (yyvsp[-2].dttag);
                                    if ((yyval.dttag)->comparator != NULL) {
                                      yyerror(parse_script, "duplicate comparator tag"); YYERROR; }
                                    else if (!strcmp((yyvsp[0].sval), "i;ascii-numeric") &&
                                      !parse_script->support.i_ascii_numeric) {
                                      yyerror(parse_script, "comparator-i;ascii-numeric MUST be enabled with \"require\"");  YYERROR; }
                                    else { (yyval.dttag)->comparator = (yyvsp[0].sval); } }
#line 2871 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 103:
#line 855 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.dttag) = (yyvsp[-2].dttag);
                                   if (!parse_script->support.index)
                                      { yyerror(parse_script, "index MUST be enabled with \"require\"");
                                        YYERROR; }
                                   if ((yyval.dttag)->index != 0) {
                                     yyerror(parse_script, "duplicate index argument"); YYERROR; }
                                   if ((yyvsp[0].nval) <= 0) {
                                     yyerror(parse_script, "invalid index value"); YYERROR; }
                                   else { (yyval.dttag)->index = (yyvsp[0].nval); } }
#line 2885 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 104:
#line 865 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.dttag) = (yyvsp[-1].dttag);
                                   if (!parse_script->support.index)
                                      { yyerror(parse_script, "index MUST be enabled with \"require\"");
                                        YYERROR; }
                                   if ((yyval.dttag)->index == 0) {
                                     yyerror(parse_script, "index argument is required"); YYERROR; }
                                   else if ((yyval.dttag)->index < 0) {
                                     yyerror(parse_script, "duplicate last argument"); YYERROR; }
                                   else { (yyval.dttag)->index *= -1; } }
#line 2899 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 105:
#line 875 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.dttag) = (yyvsp[-2].dttag);
                                   if ((yyval.dttag)->zonetag != -1) {
                                     yyerror(parse_script, "duplicate zone tag"); YYERROR; }
                                   else {
                                     if (verify_zone(parse_script, (yyvsp[0].sval)) == -1) {
                                       YYERROR; /*vr called yyerror()*/ }
                                     else { (yyval.dttag)->zone = (yyvsp[0].sval);
                                            (yyval.dttag)->zonetag = ZONE; } } }
#line 2912 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 106:
#line 884 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.dttag) = (yyvsp[-1].dttag);
                                   if ((yyval.dttag)->zonetag != -1) {
                                     yyerror(parse_script, "duplicate zone tag"); YYERROR; }
                                   else { (yyval.dttag)->zonetag = ORIGINALZONE; } }
#line 2921 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 107:
#line 890 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.nval) = ALL; }
#line 2927 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 108:
#line 891 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.nval) = LOCALPART; }
#line 2933 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 109:
#line 892 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.nval) = DOMAIN; }
#line 2939 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 110:
#line 893 "sieve/sieve.y" /* yacc.c:1646  */
    { if (!parse_script->support.subaddress) {
				     yyerror(parse_script, "subaddress MUST be enabled with \"require\"");
				     YYERROR;
				   }
				   (yyval.nval) = USER; }
#line 2949 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 111:
#line 898 "sieve/sieve.y" /* yacc.c:1646  */
    { if (!parse_script->support.subaddress) {
				     yyerror(parse_script, "subaddress MUST be enabled with \"require\"");
				     YYERROR;
				   }
				   (yyval.nval) = DETAIL; }
#line 2959 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 112:
#line 904 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.nval) = IS; }
#line 2965 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 113:
#line 905 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.nval) = CONTAINS; }
#line 2971 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 114:
#line 906 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.nval) = MATCHES; }
#line 2977 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 115:
#line 907 "sieve/sieve.y" /* yacc.c:1646  */
    { if (!parse_script->support.regex) {
				     yyerror(parse_script, "regex MUST be enabled with \"require\"");
				     YYERROR;
				   }
				   (yyval.nval) = REGEX; }
#line 2987 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 116:
#line 914 "sieve/sieve.y" /* yacc.c:1646  */
    { if (!parse_script->support.relational) {
				     yyerror(parse_script, "relational MUST be enabled with \"require\"");
				     YYERROR;
				   }
				   (yyval.nval) = COUNT; }
#line 2997 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 117:
#line 919 "sieve/sieve.y" /* yacc.c:1646  */
    { if (!parse_script->support.relational) {
				     yyerror(parse_script, "relational MUST be enabled with \"require\"");
				     YYERROR;
				   }
				   (yyval.nval) = VALUE; }
#line 3007 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 118:
#line 927 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.nval) = OVER; }
#line 3013 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 119:
#line 928 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.nval) = UNDER; }
#line 3019 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 120:
#line 931 "sieve/sieve.y" /* yacc.c:1646  */
    { if (!parse_script->support.copy) {
				     yyerror(parse_script, "copy MUST be enabled with \"require\"");
	                             YYERROR;
                                   }
				   (yyval.nval) = 1; }
#line 3029 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 121:
#line 938 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.ftag) = new_ftags(); }
#line 3035 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 122:
#line 939 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.ftag) = (yyvsp[-1].ftag);
				   if ((yyval.ftag)->copy) {
			yyerror(parse_script, "duplicate copy tag"); YYERROR; }
				   else { (yyval.ftag)->copy = (yyvsp[0].nval); } }
#line 3044 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 123:
#line 943 "sieve/sieve.y" /* yacc.c:1646  */
    { if (!parse_script->support.imap4flags) {
				     yyerror(parse_script, "imap4flags MUST be enabled with \"require\"");
	                             YYERROR;
                                   }
				   (yyval.ftag) = (yyvsp[-2].ftag);
				   if ((yyval.ftag)->flags != NULL) {
			yyerror(parse_script, "duplicate flags tag"); YYERROR; }
				   else {
				    verify_flaglist((yyvsp[0].sl));
				    if(!(yyvsp[0].sl)->count) {
				        strarray_add((yyvsp[0].sl), "");
				    }
				   (yyval.ftag)->flags = (yyvsp[0].sl); }
				 }
#line 3063 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 124:
#line 959 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.nval) = 0; }
#line 3069 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 125:
#line 960 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.nval) = (yyvsp[-1].nval);
				   if ((yyval.nval)) {
			yyerror(parse_script, "duplicate copy tag"); YYERROR; }
				   else { (yyval.nval) = (yyvsp[0].nval); } }
#line 3078 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 126:
#line 966 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.testl) = (yyvsp[-1].testl); }
#line 3084 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 127:
#line 969 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.testl) = new_testlist((yyvsp[0].test), NULL); }
#line 3090 "sieve/sieve.c" /* yacc.c:1646  */
    break;

  case 128:
#line 970 "sieve/sieve.y" /* yacc.c:1646  */
    { (yyval.testl) = new_testlist((yyvsp[-2].test), (yyvsp[0].testl)); }
#line 3096 "sieve/sieve.c" /* yacc.c:1646  */
    break;


#line 3100 "sieve/sieve.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (parse_script, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (parse_script, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, parse_script);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, parse_script);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (parse_script, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, parse_script);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, parse_script);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 973 "sieve/sieve.y" /* yacc.c:1906  */

void yyerror(sieve_script_t *parse_script, const char *msg)
{
    parse_script->err++;
    if (parse_script->interp.err) {
	parse_script->interp.err(sievelineno, msg, 
				 parse_script->interp.interp_context,
				 parse_script->script_context);
    }
}

static char *check_reqs(sieve_script_t *parse_script, strarray_t *sa)
{
    char *s;
    struct buf errs = BUF_INITIALIZER;
    char *res;

    while ((s = strarray_shift(sa))) {
	if (!script_require(parse_script, s)) {
	    if (!errs.len)
		buf_printf(&errs, "Unsupported feature(s) in \"require\": \"%s\"", s);
	    else
		buf_printf(&errs, ", \"%s\"", s);
	}
	free(s);
    }
    strarray_free(sa);

    res = buf_release(&errs);
    if (!res[0]) {
	free(res);
	return NULL;
    }

    return res;
}

static test_t *build_address(int t, struct aetags *ae,
			     strarray_t *sl, strarray_t *pl)
{
    test_t *ret = new_test(t);	/* can be either ADDRESS or ENVELOPE */

    assert((t == ADDRESS) || (t == ENVELOPE));

    if (ret) {
	ret->u.ae.index = ae->index;
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
			    strarray_t *sl, strarray_t *pl)
{
    test_t *ret = new_test(t);	/* can be HEADER or HASFLAG */

    assert((t == HEADER) || (t == HASFLAG));

    if (ret) {
	ret->u.h.index = h->index;
	ret->u.h.comptag = h->comptag;
	ret->u.h.relation=h->relation;
	ret->u.h.comparator=xstrdup(h->comparator);
	ret->u.h.sl = sl;
	ret->u.h.pl = pl;
	free_htags(h);
    }
    return ret;
}

static test_t *build_hasflag(int t, struct htags *h,
			    strarray_t *pl)
{
    return build_header(t,h,NULL,pl);
}

static test_t *build_body(int t, struct btags *b, strarray_t *pl)
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
	ret->u.v.seconds = v->seconds;
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

static commandlist_t *build_keep(int t, struct ftags *f)
{
    commandlist_t *ret = new_command(t);

    assert(t == KEEP);

    if (ret) {
	ret->u.k.copy = f->copy;
	ret->u.k.flags = f->flags; f->flags = NULL;
	free_ftags(f);
    }
    return ret;
}

static commandlist_t *build_fileinto(int t, struct ftags *f, char *folder)
{
    commandlist_t *ret = new_command(t);

    assert(t == FILEINTO);

    if (ret) {
	ret->u.f.copy = f->copy;
	ret->u.f.flags = f->flags; f->flags = NULL;
	if (config_getswitch(IMAPOPT_SIEVE_UTF8FILEINTO)) {
	    ret->u.f.folder = xmalloc(5 * strlen(folder) + 1);
	    UTF8_to_mUTF7(ret->u.f.folder, folder);
	    free(folder);
	}
	else {
	    ret->u.f.folder = folder;
	}
	free_ftags(f);
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

static commandlist_t *build_include(int t, struct itags *i, char* script)
{
    commandlist_t *ret = new_command(t);

    assert(t == INCLUDE);

    if (i->location == -1) i->location = PERSONAL;
    if (i->once == -1) i->once = 0;
    if (i->optional == -1) i->optional = 0;

    if (ret) {
	ret->u.inc.location = i->location;
	ret->u.inc.once = i->once;
	ret->u.inc.optional = i->optional;
	ret->u.inc.script = script;
	free(i);
    }
    return ret;
}

static test_t *build_date(int t, struct dttags *dt,
    char *hn, strarray_t *kl)
{
    test_t *ret = new_test(t);
    assert(t == DATE || t == CURRENTDATE);

    if (ret) {
        ret->u.dt.index = dt->index;
        ret->u.dt.zone = (dt->zone ? xstrdup(dt->zone) : NULL);
        ret->u.dt.comparator = xstrdup(dt->comparator);
        ret->u.dt.zonetag = dt->zonetag;
        ret->u.dt.comptag = dt->comptag;
        ret->u.dt.relation = dt->relation;
        ret->u.dt.date_part = dt->date_part;
        ret->u.dt.header_name = (hn ? xstrdup(hn) : NULL);
        ret->u.dt.kl = kl;
        free_dttags(dt);
    }
    return ret;
}


static struct aetags *new_aetags(void)
{
    struct aetags *r = (struct aetags *) xmalloc(sizeof(struct aetags));

    r->index = 0;
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

    r->index = 0;
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
	b->content_types = strarray_new();
	if (b->transform == RAW) {
	    strarray_append(b->content_types, "");
	} else {
	    strarray_append(b->content_types, "text");
	}
    }
    if (b->offset == -1) { b->offset = 0; }
    if (b->comparator == NULL) { b->comparator = xstrdup("i;ascii-casemap"); }
    if (b->comptag == -1) { b->comptag = IS; }
    return b;
}

static void free_btags(struct btags *b)
{
    if (b->content_types) { strarray_free(b->content_types); }
    free(b->comparator);
    free(b);
}

static struct vtags *new_vtags(void)
{
    struct vtags *r = (struct vtags *) xmalloc(sizeof(struct vtags));

    r->seconds = -1;
    r->addresses = NULL;
    r->subject = NULL;
    r->from = NULL;
    r->handle = NULL;
    r->mime = -1;

    return r;
}

static struct vtags *canon_vtags(sieve_script_t *parse_script, struct vtags *v)
{
    assert(parse_script->interp.vacation != NULL);

    if (v->seconds == -1) { v->seconds = 7 * DAY2SEC; }
    if (v->seconds < parse_script->interp.vacation->min_response)
       { v->seconds = parse_script->interp.vacation->min_response; }
    if (v->seconds > parse_script->interp.vacation->max_response)
       { v->seconds = parse_script->interp.vacation->max_response; }
    if (v->mime == -1) { v->mime = 0; }

    return v;
}

static void free_vtags(struct vtags *v)
{
    if (v->addresses) { strarray_free(v->addresses); }
    if (v->subject) { free(v->subject); }
    if (v->from) { free(v->from); }
    if (v->handle) { free(v->handle); }
    free(v);
}

static struct itags *new_itags() {
    struct itags *r = (struct itags *) xmalloc(sizeof(struct itags));

    r->once = -1;
    r->location = -1;
    r->optional = -1;

    return r;
}

static struct dttags *new_dttags(void)
{
    struct dttags *dt = (struct dttags *) xmalloc(sizeof(struct dttags));
    dt->comptag = -1;
    dt->index = 0;
    dt->zonetag = -1;
    dt->relation = -1;
    dt->comparator = NULL;
    dt->zone = NULL;
    dt->date_part = -1;
    return dt;
}

static struct dttags *canon_dttags(struct dttags *dt)
{
    char zone[6];
    int gmoffset;
    int hours;
    int minutes;
    struct tm tm;
    time_t t;

    if (dt->comparator == NULL) {
        dt->comparator = xstrdup("i;ascii-casemap");
    }
    if (dt->index == 0) {
        dt->index = 1;
    }
    if (dt->zonetag == -1) {
        t = time(NULL);
        localtime_r(&t, &tm);
        gmoffset = gmtoff_of(&tm, t) / 60;
        hours = abs(gmoffset) / 60;
        minutes = abs(gmoffset) % 60;
        snprintf(zone, 6, "%c%02d%02d", (gmoffset >= 0 ? '+' : '-'), hours, minutes);
        dt->zone = xstrdup(zone);
        dt->zonetag = ZONE;
    }
    if (dt->comptag == -1) {
        dt->comptag = IS;
    }
    return dt;
}

static void free_dttags(struct dttags *dt)
{
    free(dt->comparator);
    free(dt->zone);
    free(dt);
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
    if (n->options) { strarray_free(n->options); }
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

static struct ftags *new_ftags(void)
{
    struct ftags *f = (struct ftags *) xmalloc(sizeof(struct ftags));

    f->copy = 0;
    f->flags  = NULL;

    return f;
}

static struct ftags *canon_ftags(struct ftags *f)
{
    return f;
}

static void free_ftags(struct ftags *f)
{
    if (f->flags) { strarray_free(f->flags); }
    free(f);
}

static int verify_stringlist(sieve_script_t *parse_script, strarray_t *sa, int (*verify)(sieve_script_t*, char *))
{
    int i;

    for (i = 0 ; i < sa->count ; i++)
	if (!verify(parse_script, sa->data[i]))
	    return 0;
    return 1;
}

static int verify_address(sieve_script_t *parse_script, char *s)
{
    parse_script->addrerr[0] = '\0';	/* paranoia */
    YY_BUFFER_STATE buffer = addr_scan_string(s);
    if (addrparse(parse_script)) {
	snprintf(parse_script->sieveerr, ERR_BUF_SIZE,
		 "address '%s': %s", s, parse_script->addrerr);
	yyerror(parse_script, parse_script->sieveerr);
	addr_delete_buffer(buffer);
	return 0;
    }
    addr_delete_buffer(buffer);
    return 1;
}

static int verify_mailbox(sieve_script_t *parse_script, char *s)
{
    if (!verify_utf8(parse_script, s)) return 0;

    /* xxx if not a mailbox, call yyerror */
    return 1;
}

static int verify_header(sieve_script_t *parse_script, char *hdr)
{
    char *h = hdr;

    while (*h) {
	/* field-name      =       1*ftext
	   ftext           =       %d33-57 / %d59-126         
	   ; Any character except
	   ;  controls, SP, and
	   ;  ":". */
	if (!((*h >= 33 && *h <= 57) || (*h >= 59 && *h <= 126))) {
	    snprintf(parse_script->sieveerr, ERR_BUF_SIZE,
		     "header '%s': not a valid header", hdr);
	    yyerror(parse_script, parse_script->sieveerr);
	    return 0;
	}
	h++;
    }
    return 1;
}
 
static int verify_addrheader(sieve_script_t *parse_script, char *hdr)
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
        return verify_header(parse_script, hdr);

    for (lcase(hdr), h = hdrs; *h; h++) {
	if (!strcmp(*h, hdr)) return 1;
    }

    snprintf(parse_script->sieveerr, ERR_BUF_SIZE,
	     "header '%s': not a valid header for an address test", hdr);
    yyerror(parse_script, parse_script->sieveerr);
    return 0;
}
 
static int verify_envelope(sieve_script_t *parse_script, char *env)
{
    lcase(env);
    if (!config_getswitch(IMAPOPT_RFC3028_STRICT) ||
	!strcmp(env, "from") || !strcmp(env, "to") || !strcmp(env, "auth")) {
	return 1;
    }

    snprintf(parse_script->sieveerr, ERR_BUF_SIZE,
	     "env-part '%s': not a valid part for an envelope test", env);
    yyerror(parse_script, parse_script->sieveerr);
    return 0;
}
 
static int verify_relat(sieve_script_t *parse_script, char *r)
{/* this really should have been a token to begin with.*/
	lcase(r);
	if (!strcmp(r, "gt")) {return GT;}
	else if (!strcmp(r, "ge")) {return GE;}
	else if (!strcmp(r, "lt")) {return LT;}
	else if (!strcmp(r, "le")) {return LE;}
	else if (!strcmp(r, "ne")) {return NE;}
	else if (!strcmp(r, "eq")) {return EQ;}
	else{
	  snprintf(parse_script->sieveerr, ERR_BUF_SIZE,
		   "flag '%s': not a valid relational operation", r);
	  yyerror(parse_script, parse_script->sieveerr);
	  return -1;
	}
	
}

static int verify_zone(sieve_script_t *parse_script, char *tz)
{
    int valid = 0;
    unsigned hours;
    unsigned minutes;
    char sign;

    if (sscanf(tz, "%c%02u%02u", &sign, &hours, &minutes) != 3) {
        valid |= -1;
    }

    // test sign
    switch (sign) {
    case '+':
    case '-':
        break;

    default:
        valid |= -1;
        break;
    }

    // test minutes
    if (minutes > 59) {
            valid |= -1;
    }

    if (valid != 0) {
        snprintf(parse_script->sieveerr, ERR_BUF_SIZE,
                 "flag '%s': not a valid timezone offset", tz);
        yyerror(parse_script, parse_script->sieveerr);
    }

    return valid;
}

static int verify_date_part(sieve_script_t *parse_script, char *dp)
{
    lcase(dp);
    if (!strcmp(dp, "year")) { return YEAR; }
    else if (!strcmp(dp, "month")) { return MONTH; }
    else if (!strcmp(dp, "day")) { return DAY; }
    else if (!strcmp(dp, "date")) { return DATE; }
    else if (!strcmp(dp, "julian")) { return JULIAN; }
    else if (!strcmp(dp, "hour")) { return HOUR; }
    else if (!strcmp(dp, "minute")) { return MINUTE; }
    else if (!strcmp(dp, "second")) { return SECOND; }
    else if (!strcmp(dp, "time")) { return TIME; }
    else if (!strcmp(dp, "iso8601")) { return ISO8601; }
    else if (!strcmp(dp, "std11")) { return STD11; }
    else if (!strcmp(dp, "zone")) { return ZONE; }
    else if (!strcmp(dp, "weekday")) { return WEEKDAY; }
    else {
        snprintf(parse_script->sieveerr, ERR_BUF_SIZE,
                 "flag '%s': not a valid relational operation", dp);
        yyerror(parse_script, parse_script->sieveerr);
    }

    return -1;
}

#ifdef ENABLE_REGEX
static int verify_regex(sieve_script_t *parse_script, char *s, int cflags)
{
    int ret;
    regex_t *reg = (regex_t *) xmalloc(sizeof(regex_t));

#ifdef HAVE_PCREPOSIX_H
    /* support UTF8 comparisons */
    cflags |= REG_UTF8;
#endif

    if ((ret = regcomp(reg, s, cflags)) != 0) {
	(void) regerror(ret, reg, parse_script->sieveerr, ERR_BUF_SIZE);
	yyerror(parse_script, parse_script->sieveerr);
	free(reg);
	return 0;
    }
    free(reg);
    return 1;
}

static int verify_regexs(sieve_script_t *parse_script, const strarray_t *sa, char *comp)
{
    int i;
    int cflags = REG_EXTENDED | REG_NOSUB;

    if (!strcmp(comp, "i;ascii-casemap")) {
	cflags |= REG_ICASE;
    }

    for (i = 0 ; i < sa->count ; i++) {
	if ((verify_regex(parse_script, sa->data[i], cflags)) == 0)
	    return 0;
    }
    return 1;
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
static int verify_utf8(sieve_script_t *parse_script, char *s)
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
	snprintf(parse_script->sieveerr, ERR_BUF_SIZE,
		 "string '%s': not valid utf8", s);
	yyerror(parse_script, parse_script->sieveerr);
	return 0;
    }

    return 1;
}
