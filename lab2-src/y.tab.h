#ifndef _yy_defines_h_
#define _yy_defines_h_

#define WORD 257
#define NOTOKEN 258
#define GREAT 259
#define SMALL 260
#define NEWLINE 261
#define DOUBLEGREAT 262
#define PIPE 263
#define BACKGROUND 264
#define GREATGREATBACK 265
#define EXIT 266
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union YYSTYPE	{
		char   *string_val;
	} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
extern YYSTYPE yylval;

#endif /* _yy_defines_h_ */
