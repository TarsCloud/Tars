/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TARS_VOID = 258,
     TARS_STRUCT = 259,
     TARS_BOOL = 260,
     TARS_BYTE = 261,
     TARS_SHORT = 262,
     TARS_INT = 263,
     TARS_DOUBLE = 264,
     TARS_FLOAT = 265,
     TARS_LONG = 266,
     TARS_STRING = 267,
     TARS_VECTOR = 268,
     TARS_MAP = 269,
     TARS_NAMESPACE = 270,
     TARS_INTERFACE = 271,
     TARS_IDENTIFIER = 272,
     TARS_OUT = 273,
     TARS_OP = 274,
     TARS_KEY = 275,
     TARS_ROUTE_KEY = 276,
     TARS_REQUIRE = 277,
     TARS_OPTIONAL = 278,
     TARS_CONST_INTEGER = 279,
     TARS_CONST_FLOAT = 280,
     TARS_FALSE = 281,
     TARS_TRUE = 282,
     TARS_STRING_LITERAL = 283,
     TARS_SCOPE_DELIMITER = 284,
     TARS_CONST = 285,
     TARS_ENUM = 286,
     TARS_UNSIGNED = 287,
     BAD_CHAR = 288
   };
#endif
/* Tokens.  */
#define TARS_VOID 258
#define TARS_STRUCT 259
#define TARS_BOOL 260
#define TARS_BYTE 261
#define TARS_SHORT 262
#define TARS_INT 263
#define TARS_DOUBLE 264
#define TARS_FLOAT 265
#define TARS_LONG 266
#define TARS_STRING 267
#define TARS_VECTOR 268
#define TARS_MAP 269
#define TARS_NAMESPACE 270
#define TARS_INTERFACE 271
#define TARS_IDENTIFIER 272
#define TARS_OUT 273
#define TARS_OP 274
#define TARS_KEY 275
#define TARS_ROUTE_KEY 276
#define TARS_REQUIRE 277
#define TARS_OPTIONAL 278
#define TARS_CONST_INTEGER 279
#define TARS_CONST_FLOAT 280
#define TARS_FALSE 281
#define TARS_TRUE 282
#define TARS_STRING_LITERAL 283
#define TARS_SCOPE_DELIMITER 284
#define TARS_CONST 285
#define TARS_ENUM 286
#define TARS_UNSIGNED 287
#define BAD_CHAR 288




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

