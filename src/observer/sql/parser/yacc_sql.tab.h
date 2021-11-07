/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_YACC_SQL_TAB_H_INCLUDED
# define YY_YY_YACC_SQL_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 2 "yacc_sql.y"

#include "sql/parser/parse_defs.h"

#line 53 "yacc_sql.tab.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    SEMICOLON = 258,               /* SEMICOLON  */
    CREATE = 259,                  /* CREATE  */
    DROP = 260,                    /* DROP  */
    TABLE = 261,                   /* TABLE  */
    TABLES = 262,                  /* TABLES  */
    NULLABLE = 263,                /* NULLABLE  */
    INDEX = 264,                   /* INDEX  */
    SELECT = 265,                  /* SELECT  */
    DESC = 266,                    /* DESC  */
    SHOW = 267,                    /* SHOW  */
    SYNC = 268,                    /* SYNC  */
    INSERT = 269,                  /* INSERT  */
    DELETE = 270,                  /* DELETE  */
    UPDATE = 271,                  /* UPDATE  */
    LBRACE = 272,                  /* LBRACE  */
    RBRACE = 273,                  /* RBRACE  */
    COMMA = 274,                   /* COMMA  */
    TRX_BEGIN = 275,               /* TRX_BEGIN  */
    TRX_COMMIT = 276,              /* TRX_COMMIT  */
    TRX_ROLLBACK = 277,            /* TRX_ROLLBACK  */
    INT_T = 278,                   /* INT_T  */
    STRING_T = 279,                /* STRING_T  */
    DATE_T = 280,                  /* DATE_T  */
    FLOAT_T = 281,                 /* FLOAT_T  */
    HELP = 282,                    /* HELP  */
    EXIT = 283,                    /* EXIT  */
    DOT = 284,                     /* DOT  */
    INTO = 285,                    /* INTO  */
    VALUES = 286,                  /* VALUES  */
    FROM = 287,                    /* FROM  */
    INNER = 288,                   /* INNER  */
    JOIN = 289,                    /* JOIN  */
    WHERE = 290,                   /* WHERE  */
    AND = 291,                     /* AND  */
    NOT = 292,                     /* NOT  */
    ORDER = 293,                   /* ORDER  */
    BY = 294,                      /* BY  */
    ASC = 295,                     /* ASC  */
    SET = 296,                     /* SET  */
    ON = 297,                      /* ON  */
    LOAD = 298,                    /* LOAD  */
    DATA = 299,                    /* DATA  */
    INFILE = 300,                  /* INFILE  */
    EQ = 301,                      /* EQ  */
    LT = 302,                      /* LT  */
    GT = 303,                      /* GT  */
    LE = 304,                      /* LE  */
    GE = 305,                      /* GE  */
    NE = 306,                      /* NE  */
    IN = 307,                      /* IN  */
    NUMBER = 308,                  /* NUMBER  */
    FLOAT = 309,                   /* FLOAT  */
    ID = 310,                      /* ID  */
    PATH = 311,                    /* PATH  */
    SSS = 312,                     /* SSS  */
    STAR = 313,                    /* STAR  */
    STRING_V = 314                 /* STRING_V  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 109 "yacc_sql.y"

  Condition *condition;
  Value value;
  char *string;
  int number;
  float floats;
  char *position;
  bool boolean;
  OrderDir orderdir;
  RelAttr *rel_attr;
  List *list;
  struct{List *rels; List *conds;} join_list;
  CompOp comp_op;
  SelectExpr select_expr;
  Selects *select_statement;
  ConditionExpr condition_expr;

#line 147 "yacc_sql.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif




int yyparse (void *scanner);


#endif /* !YY_YY_YACC_SQL_TAB_H_INCLUDED  */
