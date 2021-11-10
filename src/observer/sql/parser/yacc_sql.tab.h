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
    NULL_VALUE = 263,              /* NULL_VALUE  */
    NULLABLE = 264,                /* NULLABLE  */
    INDEX = 265,                   /* INDEX  */
    UNIQUE = 266,                  /* UNIQUE  */
    SELECT = 267,                  /* SELECT  */
    DESC = 268,                    /* DESC  */
    SHOW = 269,                    /* SHOW  */
    SYNC = 270,                    /* SYNC  */
    INSERT = 271,                  /* INSERT  */
    DELETE = 272,                  /* DELETE  */
    UPDATE = 273,                  /* UPDATE  */
    LBRACE = 274,                  /* LBRACE  */
    RBRACE = 275,                  /* RBRACE  */
    COMMA = 276,                   /* COMMA  */
    TRX_BEGIN = 277,               /* TRX_BEGIN  */
    TRX_COMMIT = 278,              /* TRX_COMMIT  */
    TRX_ROLLBACK = 279,            /* TRX_ROLLBACK  */
    INT_T = 280,                   /* INT_T  */
    STRING_T = 281,                /* STRING_T  */
    DATE_T = 282,                  /* DATE_T  */
    FLOAT_T = 283,                 /* FLOAT_T  */
    HELP = 284,                    /* HELP  */
    EXIT = 285,                    /* EXIT  */
    DOT = 286,                     /* DOT  */
    INTO = 287,                    /* INTO  */
    VALUES = 288,                  /* VALUES  */
    FROM = 289,                    /* FROM  */
    INNER = 290,                   /* INNER  */
    JOIN = 291,                    /* JOIN  */
    WHERE = 292,                   /* WHERE  */
    IS = 293,                      /* IS  */
    AND = 294,                     /* AND  */
    NOT = 295,                     /* NOT  */
    ORDER = 296,                   /* ORDER  */
    BY = 297,                      /* BY  */
    ASC = 298,                     /* ASC  */
    GROUP = 299,                   /* GROUP  */
    SET = 300,                     /* SET  */
    ON = 301,                      /* ON  */
    LOAD = 302,                    /* LOAD  */
    DATA = 303,                    /* DATA  */
    INFILE = 304,                  /* INFILE  */
    EQ = 305,                      /* EQ  */
    LT = 306,                      /* LT  */
    GT = 307,                      /* GT  */
    LE = 308,                      /* LE  */
    GE = 309,                      /* GE  */
    NE = 310,                      /* NE  */
    IN = 311,                      /* IN  */
    NUMBER = 312,                  /* NUMBER  */
    FLOAT = 313,                   /* FLOAT  */
    ID = 314,                      /* ID  */
    PATH = 315,                    /* PATH  */
    SSS = 316,                     /* SSS  */
    STAR = 317,                    /* STAR  */
    STRING_V = 318,                /* STRING_V  */
    ADD = 319,                     /* ADD  */
    MINUS = 320,                   /* MINUS  */
    DIV = 321                      /* DIV  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 113 "yacc_sql.y"

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

#line 154 "yacc_sql.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif




int yyparse (void *scanner);


#endif /* !YY_YY_YACC_SQL_TAB_H_INCLUDED  */
