/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

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
#line 213 "sieve/sieve.y" /* yacc.c:1909  */

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

#line 266 "sieve/sieve.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int sieveparse (sieve_script_t *parse_script);

#endif /* !YY_SIEVE_SIEVE_SIEVE_H_INCLUDED  */
