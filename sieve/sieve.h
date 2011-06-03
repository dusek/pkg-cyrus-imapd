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
extern YYSTYPE yylval;
