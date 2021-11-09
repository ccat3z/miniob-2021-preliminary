/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 6 "yacc_sql.y"


#include "sql/parser/parse_defs.h"
#include "sql/parser/yacc_sql.tab.h"
#include "sql/parser/lex.yy.h"
// #include "common/log/log.h" // 包含C++中的头文件

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct ParserContext {
  Query * ssql;
	char id[MAX_NUM];
} ParserContext;

//获取子串
char *substr(const char *s,int n1,int n2)/*从s中提取下标为n1~n2的字符组成一个新字符串，然后返回这个新串的首地址*/
{
  char *sp = malloc(sizeof(char) * (n2 - n1 + 2));
  int i, j = 0;
  for (i = n1; i <= n2; i++) {
    sp[j++] = s[i];
  }
  sp[j] = 0;
  return sp;
}

void yyerror(yyscan_t scanner, const char *str)
{
  ParserContext *context = (ParserContext *)(yyget_extra(scanner));
  query_reset(context->ssql);
  context->ssql->flag = SCF_ERROR;
  context->ssql->sstr.insertion.value_num = 0;
  context->ssql->sstr.errors = strdup(str);
  printf("parse sql failed. error=%s", str);
}

ParserContext *get_context(yyscan_t scanner)
{
  return (ParserContext *)yyget_extra(scanner);
}

#define CONTEXT get_context(scanner)


#line 118 "yacc_sql.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "yacc_sql.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_SEMICOLON = 3,                  /* SEMICOLON  */
  YYSYMBOL_CREATE = 4,                     /* CREATE  */
  YYSYMBOL_DROP = 5,                       /* DROP  */
  YYSYMBOL_TABLE = 6,                      /* TABLE  */
  YYSYMBOL_TABLES = 7,                     /* TABLES  */
  YYSYMBOL_NULL_VALUE = 8,                 /* NULL_VALUE  */
  YYSYMBOL_NULLABLE = 9,                   /* NULLABLE  */
  YYSYMBOL_INDEX = 10,                     /* INDEX  */
  YYSYMBOL_SELECT = 11,                    /* SELECT  */
  YYSYMBOL_DESC = 12,                      /* DESC  */
  YYSYMBOL_SHOW = 13,                      /* SHOW  */
  YYSYMBOL_SYNC = 14,                      /* SYNC  */
  YYSYMBOL_INSERT = 15,                    /* INSERT  */
  YYSYMBOL_DELETE = 16,                    /* DELETE  */
  YYSYMBOL_UPDATE = 17,                    /* UPDATE  */
  YYSYMBOL_LBRACE = 18,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 19,                    /* RBRACE  */
  YYSYMBOL_COMMA = 20,                     /* COMMA  */
  YYSYMBOL_TRX_BEGIN = 21,                 /* TRX_BEGIN  */
  YYSYMBOL_TRX_COMMIT = 22,                /* TRX_COMMIT  */
  YYSYMBOL_TRX_ROLLBACK = 23,              /* TRX_ROLLBACK  */
  YYSYMBOL_INT_T = 24,                     /* INT_T  */
  YYSYMBOL_STRING_T = 25,                  /* STRING_T  */
  YYSYMBOL_DATE_T = 26,                    /* DATE_T  */
  YYSYMBOL_FLOAT_T = 27,                   /* FLOAT_T  */
  YYSYMBOL_HELP = 28,                      /* HELP  */
  YYSYMBOL_EXIT = 29,                      /* EXIT  */
  YYSYMBOL_DOT = 30,                       /* DOT  */
  YYSYMBOL_INTO = 31,                      /* INTO  */
  YYSYMBOL_VALUES = 32,                    /* VALUES  */
  YYSYMBOL_FROM = 33,                      /* FROM  */
  YYSYMBOL_INNER = 34,                     /* INNER  */
  YYSYMBOL_JOIN = 35,                      /* JOIN  */
  YYSYMBOL_WHERE = 36,                     /* WHERE  */
  YYSYMBOL_IS = 37,                        /* IS  */
  YYSYMBOL_AND = 38,                       /* AND  */
  YYSYMBOL_NOT = 39,                       /* NOT  */
  YYSYMBOL_ORDER = 40,                     /* ORDER  */
  YYSYMBOL_BY = 41,                        /* BY  */
  YYSYMBOL_ASC = 42,                       /* ASC  */
  YYSYMBOL_GROUP = 43,                     /* GROUP  */
  YYSYMBOL_SET = 44,                       /* SET  */
  YYSYMBOL_ON = 45,                        /* ON  */
  YYSYMBOL_LOAD = 46,                      /* LOAD  */
  YYSYMBOL_DATA = 47,                      /* DATA  */
  YYSYMBOL_INFILE = 48,                    /* INFILE  */
  YYSYMBOL_EQ = 49,                        /* EQ  */
  YYSYMBOL_LT = 50,                        /* LT  */
  YYSYMBOL_GT = 51,                        /* GT  */
  YYSYMBOL_LE = 52,                        /* LE  */
  YYSYMBOL_GE = 53,                        /* GE  */
  YYSYMBOL_NE = 54,                        /* NE  */
  YYSYMBOL_IN = 55,                        /* IN  */
  YYSYMBOL_NUMBER = 56,                    /* NUMBER  */
  YYSYMBOL_FLOAT = 57,                     /* FLOAT  */
  YYSYMBOL_ID = 58,                        /* ID  */
  YYSYMBOL_PATH = 59,                      /* PATH  */
  YYSYMBOL_SSS = 60,                       /* SSS  */
  YYSYMBOL_STAR = 61,                      /* STAR  */
  YYSYMBOL_STRING_V = 62,                  /* STRING_V  */
  YYSYMBOL_ADD = 63,                       /* ADD  */
  YYSYMBOL_MINUS = 64,                     /* MINUS  */
  YYSYMBOL_DIV = 65,                       /* DIV  */
  YYSYMBOL_YYACCEPT = 66,                  /* $accept  */
  YYSYMBOL_commands = 67,                  /* commands  */
  YYSYMBOL_command = 68,                   /* command  */
  YYSYMBOL_exit = 69,                      /* exit  */
  YYSYMBOL_help = 70,                      /* help  */
  YYSYMBOL_sync = 71,                      /* sync  */
  YYSYMBOL_begin = 72,                     /* begin  */
  YYSYMBOL_commit = 73,                    /* commit  */
  YYSYMBOL_rollback = 74,                  /* rollback  */
  YYSYMBOL_drop_table = 75,                /* drop_table  */
  YYSYMBOL_show_tables = 76,               /* show_tables  */
  YYSYMBOL_desc_table = 77,                /* desc_table  */
  YYSYMBOL_create_index = 78,              /* create_index  */
  YYSYMBOL_drop_index = 79,                /* drop_index  */
  YYSYMBOL_create_table = 80,              /* create_table  */
  YYSYMBOL_attr_def_list = 81,             /* attr_def_list  */
  YYSYMBOL_attr_def = 82,                  /* attr_def  */
  YYSYMBOL_number = 83,                    /* number  */
  YYSYMBOL_type = 84,                      /* type  */
  YYSYMBOL_ID_get = 85,                    /* ID_get  */
  YYSYMBOL_attr_def_nullable = 86,         /* attr_def_nullable  */
  YYSYMBOL_insert = 87,                    /* insert  */
  YYSYMBOL_value_lists = 88,               /* value_lists  */
  YYSYMBOL_value_list = 89,                /* value_list  */
  YYSYMBOL_value = 90,                     /* value  */
  YYSYMBOL_pos_value = 91,                 /* pos_value  */
  YYSYMBOL_delete = 92,                    /* delete  */
  YYSYMBOL_update = 93,                    /* update  */
  YYSYMBOL_select = 94,                    /* select  */
  YYSYMBOL_select_statement = 95,          /* select_statement  */
  YYSYMBOL_select_expr_list = 96,          /* select_expr_list  */
  YYSYMBOL_select_expr = 97,               /* select_expr  */
  YYSYMBOL_select_calc_expr = 98,          /* select_calc_expr  */
  YYSYMBOL_select_attr = 99,               /* select_attr  */
  YYSYMBOL_rel_list = 100,                 /* rel_list  */
  YYSYMBOL_join_list = 101,                /* join_list  */
  YYSYMBOL_where = 102,                    /* where  */
  YYSYMBOL_condition_list = 103,           /* condition_list  */
  YYSYMBOL_condition = 104,                /* condition  */
  YYSYMBOL_condition_expr = 105,           /* condition_expr  */
  YYSYMBOL_condition_calc_expr = 106,      /* condition_calc_expr  */
  YYSYMBOL_comp_op = 107,                  /* comp_op  */
  YYSYMBOL_order_by = 108,                 /* order_by  */
  YYSYMBOL_order_by_attr_list = 109,       /* order_by_attr_list  */
  YYSYMBOL_order_attr = 110,               /* order_attr  */
  YYSYMBOL_order_dir = 111,                /* order_dir  */
  YYSYMBOL_group_by = 112,                 /* group_by  */
  YYSYMBOL_group_by_attr_list = 113,       /* group_by_attr_list  */
  YYSYMBOL_group_attr = 114,               /* group_attr  */
  YYSYMBOL_load_data = 115                 /* load_data  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

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


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
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

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

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
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
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
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   250

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  66
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  50
/* YYNRULES -- Number of rules.  */
#define YYNRULES  124
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  238

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   320


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
      65
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   167,   167,   169,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
     189,   193,   198,   203,   209,   215,   221,   227,   233,   239,
     246,   254,   261,   267,   269,   273,   279,   287,   290,   291,
     292,   293,   296,   303,   304,   305,   310,   318,   322,   330,
     334,   340,   343,   346,   351,   354,   357,   360,   367,   377,
     386,   394,   430,   434,   441,   445,   450,   453,   460,   470,
     474,   478,   482,   487,   491,   498,   503,   508,   513,   521,
     522,   528,   532,   540,   541,   546,   551,   558,   562,   572,
     584,   588,   592,   596,   600,   605,   613,   617,   621,   625,
     629,   636,   637,   638,   639,   640,   641,   642,   643,   647,
     648,   654,   659,   667,   672,   681,   682,   683,   687,   688,
     694,   698,   705,   710,   719
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "SEMICOLON", "CREATE",
  "DROP", "TABLE", "TABLES", "NULL_VALUE", "NULLABLE", "INDEX", "SELECT",
  "DESC", "SHOW", "SYNC", "INSERT", "DELETE", "UPDATE", "LBRACE", "RBRACE",
  "COMMA", "TRX_BEGIN", "TRX_COMMIT", "TRX_ROLLBACK", "INT_T", "STRING_T",
  "DATE_T", "FLOAT_T", "HELP", "EXIT", "DOT", "INTO", "VALUES", "FROM",
  "INNER", "JOIN", "WHERE", "IS", "AND", "NOT", "ORDER", "BY", "ASC",
  "GROUP", "SET", "ON", "LOAD", "DATA", "INFILE", "EQ", "LT", "GT", "LE",
  "GE", "NE", "IN", "NUMBER", "FLOAT", "ID", "PATH", "SSS", "STAR",
  "STRING_V", "ADD", "MINUS", "DIV", "$accept", "commands", "command",
  "exit", "help", "sync", "begin", "commit", "rollback", "drop_table",
  "show_tables", "desc_table", "create_index", "drop_index",
  "create_table", "attr_def_list", "attr_def", "number", "type", "ID_get",
  "attr_def_nullable", "insert", "value_lists", "value_list", "value",
  "pos_value", "delete", "update", "select", "select_statement",
  "select_expr_list", "select_expr", "select_calc_expr", "select_attr",
  "rel_list", "join_list", "where", "condition_list", "condition",
  "condition_expr", "condition_calc_expr", "comp_op", "order_by",
  "order_by_attr_list", "order_attr", "order_dir", "group_by",
  "group_by_attr_list", "group_attr", "load_data", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-152)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -152,    97,  -152,    13,    18,    -2,   -53,    14,    33,    38,
      50,    41,   101,   112,   114,   118,   120,    81,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,   138,  -152,    84,    86,    87,
      89,  -152,    -2,  -152,  -152,    -8,  -152,  -152,    -2,  -152,
     119,   -12,  -152,  -152,   148,   150,  -152,    96,    98,   111,
    -152,  -152,  -152,  -152,  -152,   109,  -152,   140,   115,   156,
     158,   -31,   143,    21,    37,     5,   105,    -2,    -2,    -2,
      -2,    -2,  -152,  -152,   132,   129,   108,   107,   110,   113,
    -152,  -152,  -152,   139,   -11,   151,  -152,   153,  -152,  -152,
     154,  -152,  -152,     5,     5,  -152,   155,    30,   172,   128,
     147,  -152,   159,   -13,   162,  -152,  -152,  -152,  -152,   123,
     149,    36,   179,     7,   157,    30,  -152,  -152,   146,    85,
    -152,  -152,    36,   180,   110,   166,  -152,  -152,  -152,  -152,
       8,   130,   154,   160,   129,   170,   171,  -152,   173,    42,
     174,   136,    19,    30,    -4,   135,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,    30,    30,    30,    30,    30,   129,   141,
     159,   193,  -152,   142,   189,  -152,   181,  -152,   144,   161,
     183,    36,  -152,  -152,  -152,  -152,  -152,   197,  -152,  -152,
      19,    19,  -152,    42,   198,   203,  -152,  -152,  -152,   188,
    -152,   205,   164,   169,   175,   155,  -152,  -152,  -152,  -152,
      11,  -152,    30,   163,   176,  -152,  -152,  -152,   149,   182,
    -152,   191,   165,  -152,   167,   163,   184,  -152,    15,  -152,
    -152,   168,  -152,  -152,   196,  -152,   165,  -152
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     3,    20,
      19,    14,    15,    16,    17,     9,    10,    11,    12,    13,
       8,     5,     7,     6,     4,     0,    18,     0,     0,     0,
       0,    54,     0,    55,    56,    76,    57,    75,     0,    65,
       0,    62,    66,    64,     0,     0,    23,     0,     0,     0,
      24,    25,    26,    22,    21,     0,    60,     0,     0,     0,
       0,     0,    66,     0,     0,    69,     0,     0,     0,     0,
       0,     0,    29,    28,     0,    83,     0,     0,     0,     0,
      27,    31,    74,    76,     0,     0,    51,     0,    77,    78,
      79,    63,    72,    70,    71,    73,     0,     0,     0,     0,
       0,    42,    33,     0,     0,    52,    53,    68,    67,     0,
      81,     0,     0,     0,    90,     0,    92,    84,    85,     0,
      94,    58,     0,     0,     0,     0,    38,    39,    40,    41,
      43,     0,    79,     0,    83,     0,    49,    46,     0,     0,
      94,     0,    95,     0,     0,     0,   101,   102,   103,   104,
     105,   106,   107,     0,     0,     0,     0,     0,    83,     0,
      33,     0,    45,     0,     0,    36,     0,    80,     0,   118,
      47,     0,    93,   100,    91,    86,    88,     0,   108,    98,
      96,    97,    99,    87,     0,     0,    34,    32,    37,     0,
      44,     0,     0,     0,   109,     0,    50,    89,    59,   124,
      43,    30,     0,     0,     0,    61,    48,    35,    81,   122,
     119,   120,     0,    82,     0,     0,   113,   110,   115,   123,
     121,     0,   117,   116,   111,   114,     0,   112
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,    43,    88,  -152,  -152,  -152,
       9,  -152,    22,    39,   -72,    -5,  -152,  -152,  -152,    95,
     152,   -39,   186,   177,    82,    12,  -137,  -151,  -152,   -34,
     116,  -152,  -152,    -3,  -152,  -152,  -152,     6,  -152,  -152
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     1,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,   135,   112,   199,   140,   113,
     175,    31,   122,   145,   146,   126,    32,    33,    34,    35,
      50,    51,    52,    53,   120,   144,   108,   127,   128,   129,
     130,   167,   215,   227,   228,   234,   204,   220,   221,    36
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      49,    95,   185,    71,   186,    54,    41,   179,    77,    75,
      73,   136,   137,   138,   139,    41,    42,   172,     5,    37,
     172,    55,    74,    38,    39,   123,   173,   232,    40,    41,
      78,   194,    79,    80,    81,   187,    56,    49,    41,   102,
     103,   104,   105,    49,    41,   115,   116,   174,   123,    78,
     174,    79,    80,    81,    43,    44,    45,   233,    46,    47,
     168,   218,    48,    43,    44,   124,    78,    46,    96,    57,
      81,   125,    49,    49,    49,    49,    49,    43,    44,    93,
     163,    46,    47,    58,   166,    94,    43,    44,   124,   149,
      46,   152,    43,    44,   125,    98,    46,     2,    99,    59,
      94,     3,     4,   163,    60,   164,   165,   166,     5,     6,
       7,     8,     9,    10,    11,    61,    96,    62,    12,    13,
      14,    63,   154,    64,   155,    15,    16,    96,    65,   189,
     190,   191,   192,   193,   156,   157,   158,   159,   160,   161,
     162,    66,    67,    17,    68,    69,   163,    70,   164,   165,
     166,    82,    76,    83,    84,    86,    85,    87,    88,    90,
      89,    91,    92,   100,   106,   107,   109,   110,   111,    74,
     117,   114,   118,   121,   119,   131,    96,   132,   133,   134,
     141,   142,   147,   143,   153,   171,   169,   151,   176,   180,
     188,   181,   182,   183,   184,   178,   197,   200,   198,   195,
     201,   208,   202,   205,   203,   207,   209,   210,   211,   212,
     213,   225,   224,   196,   231,   214,   236,   222,   148,   217,
     206,   219,   170,   226,   177,   229,   235,   216,    72,   101,
     223,   230,     0,   237,     0,     0,     0,     0,     0,   150,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      97
};

static const yytype_int16 yycheck[] =
{
       5,    73,   153,    42,     8,    58,     8,   144,    20,    48,
      18,    24,    25,    26,    27,     8,    18,     9,    11,     6,
       9,     7,    30,    10,     6,    18,    18,    12,    10,     8,
      61,   168,    63,    64,    65,    39,     3,    42,     8,    78,
      79,    80,    81,    48,     8,    56,    57,    39,    18,    61,
      39,    63,    64,    65,    56,    57,    58,    42,    60,    61,
     132,   212,    64,    56,    57,    58,    61,    60,    73,    31,
      65,    64,    77,    78,    79,    80,    81,    56,    57,    58,
      61,    60,    61,    33,    65,    64,    56,    57,    58,   123,
      60,   125,    56,    57,    64,    58,    60,     0,    61,    58,
      64,     4,     5,    61,     3,    63,    64,    65,    11,    12,
      13,    14,    15,    16,    17,     3,   121,     3,    21,    22,
      23,     3,    37,     3,    39,    28,    29,   132,    47,   163,
     164,   165,   166,   167,    49,    50,    51,    52,    53,    54,
      55,     3,    58,    46,    58,    58,    61,    58,    63,    64,
      65,     3,    33,     3,    58,    44,    58,    48,    18,     3,
      45,     3,    19,    58,    32,    36,    58,    60,    58,    30,
      19,    58,    19,    18,    20,     3,   181,    49,    31,    20,
      18,    58,     3,    34,    38,    19,     6,    30,    58,    19,
      55,    20,    19,    19,    58,    35,     3,     8,    56,    58,
      19,     3,    58,    20,    43,     8,     3,    19,     3,    45,
      41,    20,    30,   170,    30,    40,    20,    41,   123,   210,
     181,    58,   134,    58,   142,    58,    58,   205,    42,    77,
     218,   225,    -1,   236,    -1,    -1,    -1,    -1,    -1,   123,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      73
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    67,     0,     4,     5,    11,    12,    13,    14,    15,
      16,    17,    21,    22,    23,    28,    29,    46,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    87,    92,    93,    94,    95,   115,     6,    10,     6,
      10,     8,    18,    56,    57,    58,    60,    61,    64,    91,
      96,    97,    98,    99,    58,     7,     3,    31,    33,    58,
       3,     3,     3,     3,     3,    47,     3,    58,    58,    58,
      58,    97,    98,    18,    30,    97,    33,    20,    61,    63,
      64,    65,     3,     3,    58,    58,    44,    48,    18,    45,
       3,     3,    19,    58,    64,    90,    91,    99,    58,    61,
      58,    96,    97,    97,    97,    97,    32,    36,   102,    58,
      60,    58,    82,    85,    58,    56,    57,    19,    19,    20,
     100,    18,    88,    18,    58,    64,    91,   103,   104,   105,
     106,     3,    49,    31,    20,    81,    24,    25,    26,    27,
      84,    18,    58,    34,   101,    89,    90,     3,    95,   105,
     106,    30,   105,    38,    37,    39,    49,    50,    51,    52,
      53,    54,    55,    61,    63,    64,    65,   107,    90,     6,
      82,    19,     9,    18,    39,    86,    58,   100,    35,   102,
      19,    20,    19,    19,    58,   103,     8,    39,    55,   105,
     105,   105,   105,   105,   102,    58,    81,     3,    56,    83,
       8,    19,    58,    43,   112,    20,    89,     8,     3,     3,
      19,     3,    45,    41,    40,   108,    88,    86,   103,    58,
     113,   114,    41,   101,    30,    20,    58,   109,   110,    58,
     113,    30,    12,    42,   111,    58,    20,   109
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    66,    67,    67,    68,    68,    68,    68,    68,    68,
      68,    68,    68,    68,    68,    68,    68,    68,    68,    68,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    81,    82,    82,    83,    84,    84,
      84,    84,    85,    86,    86,    86,    87,    88,    88,    89,
      89,    90,    90,    90,    91,    91,    91,    91,    92,    93,
      94,    95,    96,    96,    97,    97,    97,    97,    97,    98,
      98,    98,    98,    98,    98,    99,    99,    99,    99,   100,
     100,   101,   101,   102,   102,   103,   103,   104,   104,   104,
     105,   105,   105,   105,   105,   106,   106,   106,   106,   106,
     106,   107,   107,   107,   107,   107,   107,   107,   107,   108,
     108,   109,   109,   110,   110,   111,   111,   111,   112,   112,
     113,   113,   114,   114,   115
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     2,     2,     2,     2,     2,     4,     3,     3,
       9,     4,     8,     0,     3,     6,     3,     1,     1,     1,
       1,     1,     1,     0,     2,     1,     6,     3,     5,     1,
       3,     1,     2,     2,     1,     1,     1,     1,     5,     8,
       2,     9,     1,     3,     1,     1,     1,     4,     4,     2,
       3,     3,     3,     3,     3,     1,     1,     3,     3,     0,
       3,     0,     6,     0,     2,     1,     3,     3,     3,     4,
       1,     3,     1,     3,     1,     2,     3,     3,     3,     3,
       3,     1,     1,     1,     1,     1,     1,     1,     2,     0,
       3,     2,     4,     1,     3,     0,     1,     1,     0,     3,
       1,     3,     1,     3,     8
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
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
        yyerror (scanner, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


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




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, void *scanner)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (scanner);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, void *scanner)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep, scanner);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule, void *scanner)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)], scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, scanner); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
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






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, void *scanner)
{
  YY_USE (yyvaluep);
  YY_USE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (void *scanner)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


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

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
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
  case 21: /* exit: EXIT SEMICOLON  */
#line 193 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    }
#line 1403 "yacc_sql.tab.c"
    break;

  case 22: /* help: HELP SEMICOLON  */
#line 198 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    }
#line 1411 "yacc_sql.tab.c"
    break;

  case 23: /* sync: SYNC SEMICOLON  */
#line 203 "yacc_sql.y"
                   {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
#line 1419 "yacc_sql.tab.c"
    break;

  case 24: /* begin: TRX_BEGIN SEMICOLON  */
#line 209 "yacc_sql.y"
                        {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
#line 1427 "yacc_sql.tab.c"
    break;

  case 25: /* commit: TRX_COMMIT SEMICOLON  */
#line 215 "yacc_sql.y"
                         {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
#line 1435 "yacc_sql.tab.c"
    break;

  case 26: /* rollback: TRX_ROLLBACK SEMICOLON  */
#line 221 "yacc_sql.y"
                           {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
#line 1443 "yacc_sql.tab.c"
    break;

  case 27: /* drop_table: DROP TABLE ID SEMICOLON  */
#line 227 "yacc_sql.y"
                            {
        CONTEXT->ssql->flag = SCF_DROP_TABLE;//"drop_table";
        drop_table_init(&CONTEXT->ssql->sstr.drop_table, (yyvsp[-1].string));
    }
#line 1452 "yacc_sql.tab.c"
    break;

  case 28: /* show_tables: SHOW TABLES SEMICOLON  */
#line 233 "yacc_sql.y"
                          {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
#line 1460 "yacc_sql.tab.c"
    break;

  case 29: /* desc_table: DESC ID SEMICOLON  */
#line 239 "yacc_sql.y"
                      {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
      desc_table_init(&CONTEXT->ssql->sstr.desc_table, (yyvsp[-1].string));
    }
#line 1469 "yacc_sql.tab.c"
    break;

  case 30: /* create_index: CREATE INDEX ID ON ID LBRACE ID RBRACE SEMICOLON  */
#line 247 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-6].string), (yyvsp[-4].string), (yyvsp[-2].string));
		}
#line 1478 "yacc_sql.tab.c"
    break;

  case 31: /* drop_index: DROP INDEX ID SEMICOLON  */
#line 255 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			drop_index_init(&CONTEXT->ssql->sstr.drop_index, (yyvsp[-1].string));
		}
#line 1487 "yacc_sql.tab.c"
    break;

  case 32: /* create_table: CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE SEMICOLON  */
#line 262 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, (yyvsp[-5].string));
		}
#line 1496 "yacc_sql.tab.c"
    break;

  case 34: /* attr_def_list: COMMA attr_def attr_def_list  */
#line 269 "yacc_sql.y"
                                   {    }
#line 1502 "yacc_sql.tab.c"
    break;

  case 35: /* attr_def: ID_get type LBRACE number RBRACE attr_def_nullable  */
#line 274 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-4].number), (yyvsp[-2].number), (yyvsp[0].boolean));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
		}
#line 1512 "yacc_sql.tab.c"
    break;

  case 36: /* attr_def: ID_get type attr_def_nullable  */
#line 280 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-1].number), 4, (yyvsp[0].boolean));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
		}
#line 1522 "yacc_sql.tab.c"
    break;

  case 37: /* number: NUMBER  */
#line 287 "yacc_sql.y"
                       {(yyval.number) = (yyvsp[0].number);}
#line 1528 "yacc_sql.tab.c"
    break;

  case 38: /* type: INT_T  */
#line 290 "yacc_sql.y"
              { (yyval.number)=INTS; }
#line 1534 "yacc_sql.tab.c"
    break;

  case 39: /* type: STRING_T  */
#line 291 "yacc_sql.y"
                  { (yyval.number)=CHARS; }
#line 1540 "yacc_sql.tab.c"
    break;

  case 40: /* type: DATE_T  */
#line 292 "yacc_sql.y"
                    { (yyval.number)=DATE; }
#line 1546 "yacc_sql.tab.c"
    break;

  case 41: /* type: FLOAT_T  */
#line 293 "yacc_sql.y"
                 { (yyval.number)=FLOATS; }
#line 1552 "yacc_sql.tab.c"
    break;

  case 42: /* ID_get: ID  */
#line 297 "yacc_sql.y"
        {
		char *temp=(yyvsp[0].string); 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
#line 1561 "yacc_sql.tab.c"
    break;

  case 43: /* attr_def_nullable: %empty  */
#line 303 "yacc_sql.y"
        { (yyval.boolean) = false; }
#line 1567 "yacc_sql.tab.c"
    break;

  case 44: /* attr_def_nullable: NOT NULL_VALUE  */
#line 304 "yacc_sql.y"
                         { (yyval.boolean) = false; }
#line 1573 "yacc_sql.tab.c"
    break;

  case 45: /* attr_def_nullable: NULLABLE  */
#line 305 "yacc_sql.y"
                   { (yyval.boolean) = true; }
#line 1579 "yacc_sql.tab.c"
    break;

  case 46: /* insert: INSERT INTO ID VALUES value_lists SEMICOLON  */
#line 311 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_INSERT;//"insert";
			inserts_init(&CONTEXT->ssql->sstr.insertion, (yyvsp[-3].string), (Value *)(yyvsp[-1].list)->values, (yyvsp[-1].list)->len);
			list_free((yyvsp[-1].list));
    }
#line 1589 "yacc_sql.tab.c"
    break;

  case 47: /* value_lists: LBRACE value_list RBRACE  */
#line 318 "yacc_sql.y"
                                 {
		CONTEXT->ssql->sstr.insertion.tuple_num++;
		(yyval.list) = (yyvsp[-1].list);
	}
#line 1598 "yacc_sql.tab.c"
    break;

  case 48: /* value_lists: LBRACE value_list RBRACE COMMA value_lists  */
#line 322 "yacc_sql.y"
                                                     {
		CONTEXT->ssql->sstr.insertion.tuple_num++;
		(yyval.list) = (yyvsp[0].list);
		list_append_list((yyval.list), (yyvsp[-3].list));
		list_free((yyvsp[-3].list));
	}
#line 1609 "yacc_sql.tab.c"
    break;

  case 49: /* value_list: value  */
#line 330 "yacc_sql.y"
              {
		(yyval.list) = list_create(sizeof(Value), MAX_NUM);
		list_append((yyval.list), &(yyvsp[0].value));
	}
#line 1618 "yacc_sql.tab.c"
    break;

  case 50: /* value_list: value COMMA value_list  */
#line 334 "yacc_sql.y"
                              { 
		(yyval.list) = (yyvsp[0].list);
		list_append((yyval.list), &(yyvsp[-2].value));
	  }
#line 1627 "yacc_sql.tab.c"
    break;

  case 51: /* value: pos_value  */
#line 340 "yacc_sql.y"
                  {
		(yyval.value) = (yyvsp[0].value);
	}
#line 1635 "yacc_sql.tab.c"
    break;

  case 52: /* value: MINUS NUMBER  */
#line 343 "yacc_sql.y"
                 {	
  		value_init_integer(&(yyval.value), -(yyvsp[0].number));
		}
#line 1643 "yacc_sql.tab.c"
    break;

  case 53: /* value: MINUS FLOAT  */
#line 346 "yacc_sql.y"
                {
  		value_init_float(&(yyval.value), -(yyvsp[0].floats));
		}
#line 1651 "yacc_sql.tab.c"
    break;

  case 54: /* pos_value: NULL_VALUE  */
#line 351 "yacc_sql.y"
                   {
		value_init_null(&(yyval.value));
	}
#line 1659 "yacc_sql.tab.c"
    break;

  case 55: /* pos_value: NUMBER  */
#line 354 "yacc_sql.y"
           {	
  		value_init_integer(&(yyval.value), (yyvsp[0].number));
		}
#line 1667 "yacc_sql.tab.c"
    break;

  case 56: /* pos_value: FLOAT  */
#line 357 "yacc_sql.y"
          {
  		value_init_float(&(yyval.value), (yyvsp[0].floats));
		}
#line 1675 "yacc_sql.tab.c"
    break;

  case 57: /* pos_value: SSS  */
#line 360 "yacc_sql.y"
         {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  		value_init_string(&(yyval.value), (yyvsp[0].string));
		}
#line 1684 "yacc_sql.tab.c"
    break;

  case 58: /* delete: DELETE FROM ID where SEMICOLON  */
#line 368 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			deletes_init_relation(&CONTEXT->ssql->sstr.deletion, (yyvsp[-2].string));
			deletes_set_conditions(&CONTEXT->ssql->sstr.deletion, 
					(Condition *) (yyvsp[-1].list)->values, (yyvsp[-1].list)->len);
			list_free((yyvsp[-1].list));
    }
#line 1696 "yacc_sql.tab.c"
    break;

  case 59: /* update: UPDATE ID SET ID EQ value where SEMICOLON  */
#line 378 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			updates_init(&CONTEXT->ssql->sstr.update, (yyvsp[-6].string), (yyvsp[-4].string), &(yyvsp[-2].value), 
					(Condition *) (yyvsp[-1].list)->values, (yyvsp[-1].list)->len);
			list_free((yyvsp[-1].list));
		}
#line 1707 "yacc_sql.tab.c"
    break;

  case 60: /* select: select_statement SEMICOLON  */
#line 386 "yacc_sql.y"
                                   {
		CONTEXT->ssql->flag=SCF_SELECT;//"select";
		CONTEXT->ssql->sstr.selection = *(yyvsp[-1].select_statement);
		free((yyvsp[-1].select_statement));
	}
#line 1717 "yacc_sql.tab.c"
    break;

  case 61: /* select_statement: SELECT select_expr_list FROM ID rel_list join_list where group_by order_by  */
#line 395 "yacc_sql.y"
                {
			Selects *selects = (Selects *) malloc(sizeof(Selects));
			memset(selects, 0, sizeof(*selects));

			selects_append_exprs(selects, (SelectExpr *) (yyvsp[-7].list)->values, (yyvsp[-7].list)->len);
			list_free((yyvsp[-7].list));

			selects_append_relations(selects, (const char **) (yyvsp[-3].join_list).rels->values, (yyvsp[-3].join_list).rels->len);
			list_free((yyvsp[-3].join_list).rels);
			selects_append_relations(selects, (const char **) (yyvsp[-4].list)->values, (yyvsp[-4].list)->len);
			list_free((yyvsp[-4].list));
			selects_append_relation(selects, (yyvsp[-5].string));

			selects_append_conditions(selects, (Condition *) (yyvsp[-3].join_list).conds->values, (yyvsp[-3].join_list).conds->len);
			list_free((yyvsp[-3].join_list).conds);
			selects_append_conditions(selects, (Condition *) (yyvsp[-2].list)->values, (yyvsp[-2].list)->len);
			list_free((yyvsp[-2].list));

			if ((yyvsp[-1].list) != NULL) {
				selects_append_group_attrs(selects, (RelAttr *) (yyvsp[-1].list)->values, (yyvsp[-1].list)->len);
				list_free((yyvsp[-1].list));
			}

			if ((yyvsp[0].list) != NULL) {
				selects_append_order_attrs(selects, (OrderBy *) (yyvsp[0].list)->values, (yyvsp[0].list)->len);
				list_free((yyvsp[0].list));
			}

			CONTEXT->ssql->flag=SCF_SELECT;//"select";

			(yyval.select_statement) = selects;
	}
#line 1754 "yacc_sql.tab.c"
    break;

  case 62: /* select_expr_list: select_expr  */
#line 430 "yacc_sql.y"
                    {
		(yyval.list) = list_create(sizeof(SelectExpr), MAX_NUM);
		list_append((yyval.list), &(yyvsp[0].select_expr));
	}
#line 1763 "yacc_sql.tab.c"
    break;

  case 63: /* select_expr_list: select_expr COMMA select_expr_list  */
#line 434 "yacc_sql.y"
                                             {
		(yyval.list) = (yyvsp[0].list);
		list_append((yyval.list), &(yyvsp[-2].select_expr));
	}
#line 1772 "yacc_sql.tab.c"
    break;

  case 64: /* select_expr: select_attr  */
#line 441 "yacc_sql.y"
                    {
		memset(&(yyval.select_expr), 0, sizeof (yyval.select_expr));
		(yyval.select_expr).attribute = (yyvsp[0].rel_attr);
	}
#line 1781 "yacc_sql.tab.c"
    break;

  case 65: /* select_expr: pos_value  */
#line 445 "yacc_sql.y"
                    {
		memset(&(yyval.select_expr), 0, sizeof (yyval.select_expr));
		(yyval.select_expr).value = malloc(sizeof(Value));
		*(yyval.select_expr).value = (yyvsp[0].value);
	}
#line 1791 "yacc_sql.tab.c"
    break;

  case 66: /* select_expr: select_calc_expr  */
#line 450 "yacc_sql.y"
                           {
		(yyval.select_expr) = (yyvsp[0].select_expr);
	}
#line 1799 "yacc_sql.tab.c"
    break;

  case 67: /* select_expr: ID LBRACE select_attr RBRACE  */
#line 453 "yacc_sql.y"
                                       {
		AggExpr *expr = (AggExpr *) malloc(sizeof(AggExpr));
		agg_expr_init_attr(expr, (yyvsp[-3].string), (yyvsp[-1].rel_attr));

		memset(&(yyval.select_expr), 0, sizeof (yyval.select_expr));
		(yyval.select_expr).agg = expr;
	}
#line 1811 "yacc_sql.tab.c"
    break;

  case 68: /* select_expr: ID LBRACE value RBRACE  */
#line 460 "yacc_sql.y"
                                 {
		AggExpr *expr = (AggExpr *) malloc(sizeof(AggExpr));
		agg_expr_init_value(expr, (yyvsp[-3].string), &(yyvsp[-1].value));

		memset(&(yyval.select_expr), 0, sizeof (yyval.select_expr));
		(yyval.select_expr).agg = expr;
	}
#line 1823 "yacc_sql.tab.c"
    break;

  case 69: /* select_calc_expr: MINUS select_expr  */
#line 470 "yacc_sql.y"
                          {
		memset(&(yyval.select_expr), 0, sizeof (yyval.select_expr));
		(yyval.select_expr).calc = select_calc_expr_create(NULL, CALC_MINUS, &(yyvsp[0].select_expr));
	}
#line 1832 "yacc_sql.tab.c"
    break;

  case 70: /* select_calc_expr: select_expr ADD select_expr  */
#line 474 "yacc_sql.y"
                                      {
		memset(&(yyval.select_expr), 0, sizeof (yyval.select_expr));
		(yyval.select_expr).calc = select_calc_expr_create(&(yyvsp[-2].select_expr), CALC_ADD, &(yyvsp[0].select_expr));
	}
#line 1841 "yacc_sql.tab.c"
    break;

  case 71: /* select_calc_expr: select_expr MINUS select_expr  */
#line 478 "yacc_sql.y"
                                        {
		memset(&(yyval.select_expr), 0, sizeof (yyval.select_expr));
		(yyval.select_expr).calc = select_calc_expr_create(&(yyvsp[-2].select_expr), CALC_MINUS, &(yyvsp[0].select_expr));
	}
#line 1850 "yacc_sql.tab.c"
    break;

  case 72: /* select_calc_expr: select_expr STAR select_expr  */
#line 482 "yacc_sql.y"
                                       {
		// 9 * 9
		memset(&(yyval.select_expr), 0, sizeof (yyval.select_expr));
		(yyval.select_expr).calc = select_calc_expr_create(&(yyvsp[-2].select_expr), CALC_MULTI, &(yyvsp[0].select_expr));
	}
#line 1860 "yacc_sql.tab.c"
    break;

  case 73: /* select_calc_expr: select_expr DIV select_expr  */
#line 487 "yacc_sql.y"
                                      {
		memset(&(yyval.select_expr), 0, sizeof (yyval.select_expr));
		(yyval.select_expr).calc = select_calc_expr_create(&(yyvsp[-2].select_expr), CALC_DIV, &(yyvsp[0].select_expr));
	}
#line 1869 "yacc_sql.tab.c"
    break;

  case 74: /* select_calc_expr: LBRACE select_calc_expr RBRACE  */
#line 491 "yacc_sql.y"
                                         {
		(yyval.select_expr) = (yyvsp[-1].select_expr);
	    select_calc_expr_add_brace((yyval.select_expr).calc);
	}
#line 1878 "yacc_sql.tab.c"
    break;

  case 75: /* select_attr: STAR  */
#line 498 "yacc_sql.y"
         {
			RelAttr *attr = malloc(sizeof(RelAttr));
			relation_attr_init(attr, NULL, "*");
			(yyval.rel_attr) = attr;
      }
#line 1888 "yacc_sql.tab.c"
    break;

  case 76: /* select_attr: ID  */
#line 503 "yacc_sql.y"
         {
			RelAttr *attr = malloc(sizeof(RelAttr));
			relation_attr_init(attr, NULL, (yyvsp[0].string));
			(yyval.rel_attr) = attr;
      }
#line 1898 "yacc_sql.tab.c"
    break;

  case 77: /* select_attr: ID DOT ID  */
#line 508 "yacc_sql.y"
                {
			RelAttr *attr = malloc(sizeof(RelAttr));
			relation_attr_init(attr, (yyvsp[-2].string), (yyvsp[0].string));
			(yyval.rel_attr) = attr;
  	  }
#line 1908 "yacc_sql.tab.c"
    break;

  case 78: /* select_attr: ID DOT STAR  */
#line 513 "yacc_sql.y"
                  {
			RelAttr *attr = malloc(sizeof(RelAttr));
			relation_attr_init(attr, (yyvsp[-2].string), "*");
			(yyval.rel_attr) = attr;
  	  }
#line 1918 "yacc_sql.tab.c"
    break;

  case 79: /* rel_list: %empty  */
#line 521 "yacc_sql.y"
                { (yyval.list) = list_create(sizeof(char *), MAX_NUM); }
#line 1924 "yacc_sql.tab.c"
    break;

  case 80: /* rel_list: COMMA ID rel_list  */
#line 522 "yacc_sql.y"
                        {	
		(yyval.list) = (yyvsp[0].list);
		list_append((yyvsp[0].list), &(yyvsp[-1].string));
	}
#line 1933 "yacc_sql.tab.c"
    break;

  case 81: /* join_list: %empty  */
#line 528 "yacc_sql.y"
                    {
		(yyval.join_list).rels = list_create(sizeof(char *), MAX_NUM);
		(yyval.join_list).conds = list_create(sizeof(Condition), MAX_NUM);
	}
#line 1942 "yacc_sql.tab.c"
    break;

  case 82: /* join_list: INNER JOIN ID ON condition_list join_list  */
#line 532 "yacc_sql.y"
                                                    {
		(yyval.join_list) = (yyvsp[0].join_list);
		list_append((yyval.join_list).rels, &(yyvsp[-3].string));
		list_append_list((yyval.join_list).conds, (yyvsp[-1].list));
		list_free((yyvsp[-1].list));
	}
#line 1953 "yacc_sql.tab.c"
    break;

  case 83: /* where: %empty  */
#line 540 "yacc_sql.y"
                { (yyval.list) = list_create(sizeof(Condition), MAX_NUM); }
#line 1959 "yacc_sql.tab.c"
    break;

  case 84: /* where: WHERE condition_list  */
#line 541 "yacc_sql.y"
                           {	
		(yyval.list) = (yyvsp[0].list);
	}
#line 1967 "yacc_sql.tab.c"
    break;

  case 85: /* condition_list: condition  */
#line 546 "yacc_sql.y"
                  {
		(yyval.list) = list_create(sizeof(Condition), MAX_NUM);
		list_append((yyval.list), (yyvsp[0].condition));
		free((yyvsp[0].condition));
	}
#line 1977 "yacc_sql.tab.c"
    break;

  case 86: /* condition_list: condition AND condition_list  */
#line 551 "yacc_sql.y"
                                   {
		(yyval.list) = (yyvsp[0].list);
		list_append((yyval.list), (yyvsp[-2].condition));
		free((yyvsp[-2].condition));
	}
#line 1987 "yacc_sql.tab.c"
    break;

  case 87: /* condition: condition_expr comp_op condition_expr  */
#line 558 "yacc_sql.y"
                                              {
		(yyval.condition) = (Condition *) malloc(sizeof(Condition));
		condition_init((yyval.condition), (yyvsp[-1].comp_op), &(yyvsp[-2].condition_expr), &(yyvsp[0].condition_expr));
	}
#line 1996 "yacc_sql.tab.c"
    break;

  case 88: /* condition: condition_expr IS NULL_VALUE  */
#line 562 "yacc_sql.y"
                                       {
		(yyval.condition) = (Condition *) malloc(sizeof(Condition));
		Value null;
		value_init_null(&null);
		ConditionExpr null_expr;
		null_expr.type = COND_EXPR_VALUE;
		null_expr.value.value = null;

		condition_init((yyval.condition), IS_NULL, &(yyvsp[-2].condition_expr), &null_expr);
	}
#line 2011 "yacc_sql.tab.c"
    break;

  case 89: /* condition: condition_expr IS NOT NULL_VALUE  */
#line 572 "yacc_sql.y"
                                           {
		(yyval.condition) = (Condition *) malloc(sizeof(Condition));
		Value null;
		value_init_null(&null);
		ConditionExpr null_expr;
		null_expr.type = COND_EXPR_VALUE;
		null_expr.value.value = null;

		condition_init((yyval.condition), IS_NOT_NULL, &(yyvsp[-3].condition_expr), &null_expr);
	}
#line 2026 "yacc_sql.tab.c"
    break;

  case 90: /* condition_expr: ID  */
#line 584 "yacc_sql.y"
           {
		(yyval.condition_expr).type = COND_EXPR_ATTR;
		relation_attr_init(&(yyval.condition_expr).value.attr, NULL, (yyvsp[0].string));
	}
#line 2035 "yacc_sql.tab.c"
    break;

  case 91: /* condition_expr: ID DOT ID  */
#line 588 "yacc_sql.y"
                    {
		(yyval.condition_expr).type = COND_EXPR_ATTR;
		relation_attr_init(&(yyval.condition_expr).value.attr, (yyvsp[-2].string), (yyvsp[0].string));
	}
#line 2044 "yacc_sql.tab.c"
    break;

  case 92: /* condition_expr: pos_value  */
#line 592 "yacc_sql.y"
                    {
		(yyval.condition_expr).type = COND_EXPR_VALUE;
		(yyval.condition_expr).value.value = (yyvsp[0].value);
	}
#line 2053 "yacc_sql.tab.c"
    break;

  case 93: /* condition_expr: LBRACE select_statement RBRACE  */
#line 596 "yacc_sql.y"
                                         {
		(yyval.condition_expr).type = COND_EXPR_SELECT;
		(yyval.condition_expr).value.selects = (yyvsp[-1].select_statement);
	}
#line 2062 "yacc_sql.tab.c"
    break;

  case 94: /* condition_expr: condition_calc_expr  */
#line 600 "yacc_sql.y"
                              {
		(yyval.condition_expr) = (yyvsp[0].condition_expr);
	}
#line 2070 "yacc_sql.tab.c"
    break;

  case 95: /* condition_calc_expr: MINUS condition_expr  */
#line 605 "yacc_sql.y"
                             {
		ConditionExpr expr;
		expr.type = COND_EXPR_VALUE;
		value_init_integer(&expr.value.value, 0);

		(yyval.condition_expr).type = COND_EXPR_CALC;
		(yyval.condition_expr).value.calc = condition_calc_create(&expr, CALC_MINUS, &(yyvsp[0].condition_expr));
	}
#line 2083 "yacc_sql.tab.c"
    break;

  case 96: /* condition_calc_expr: condition_expr ADD condition_expr  */
#line 613 "yacc_sql.y"
                                            {
		(yyval.condition_expr).type = COND_EXPR_CALC;
		(yyval.condition_expr).value.calc = condition_calc_create(&(yyvsp[-2].condition_expr), CALC_ADD, &(yyvsp[0].condition_expr));
	}
#line 2092 "yacc_sql.tab.c"
    break;

  case 97: /* condition_calc_expr: condition_expr MINUS condition_expr  */
#line 617 "yacc_sql.y"
                                              {
		(yyval.condition_expr).type = COND_EXPR_CALC;
		(yyval.condition_expr).value.calc = condition_calc_create(&(yyvsp[-2].condition_expr), CALC_MINUS, &(yyvsp[0].condition_expr));
	}
#line 2101 "yacc_sql.tab.c"
    break;

  case 98: /* condition_calc_expr: condition_expr STAR condition_expr  */
#line 621 "yacc_sql.y"
                                             {
		(yyval.condition_expr).type = COND_EXPR_CALC;
		(yyval.condition_expr).value.calc = condition_calc_create(&(yyvsp[-2].condition_expr), CALC_MULTI, &(yyvsp[0].condition_expr));
	}
#line 2110 "yacc_sql.tab.c"
    break;

  case 99: /* condition_calc_expr: condition_expr DIV condition_expr  */
#line 625 "yacc_sql.y"
                                            {
		(yyval.condition_expr).type = COND_EXPR_CALC;
		(yyval.condition_expr).value.calc = condition_calc_create(&(yyvsp[-2].condition_expr), CALC_DIV, &(yyvsp[0].condition_expr));
	}
#line 2119 "yacc_sql.tab.c"
    break;

  case 100: /* condition_calc_expr: LBRACE condition_calc_expr RBRACE  */
#line 629 "yacc_sql.y"
                                            {
		(yyval.condition_expr) = (yyvsp[-1].condition_expr);
	}
#line 2127 "yacc_sql.tab.c"
    break;

  case 101: /* comp_op: EQ  */
#line 636 "yacc_sql.y"
             { (yyval.comp_op) = EQUAL_TO; }
#line 2133 "yacc_sql.tab.c"
    break;

  case 102: /* comp_op: LT  */
#line 637 "yacc_sql.y"
         { (yyval.comp_op) = LESS_THAN; }
#line 2139 "yacc_sql.tab.c"
    break;

  case 103: /* comp_op: GT  */
#line 638 "yacc_sql.y"
         { (yyval.comp_op) = GREAT_THAN; }
#line 2145 "yacc_sql.tab.c"
    break;

  case 104: /* comp_op: LE  */
#line 639 "yacc_sql.y"
         { (yyval.comp_op) = LESS_EQUAL; }
#line 2151 "yacc_sql.tab.c"
    break;

  case 105: /* comp_op: GE  */
#line 640 "yacc_sql.y"
         { (yyval.comp_op) = GREAT_EQUAL; }
#line 2157 "yacc_sql.tab.c"
    break;

  case 106: /* comp_op: NE  */
#line 641 "yacc_sql.y"
         { (yyval.comp_op) = NOT_EQUAL; }
#line 2163 "yacc_sql.tab.c"
    break;

  case 107: /* comp_op: IN  */
#line 642 "yacc_sql.y"
             { (yyval.comp_op) = IN_SET; }
#line 2169 "yacc_sql.tab.c"
    break;

  case 108: /* comp_op: NOT IN  */
#line 643 "yacc_sql.y"
                 { (yyval.comp_op) = NOT_IN_SET; }
#line 2175 "yacc_sql.tab.c"
    break;

  case 109: /* order_by: %empty  */
#line 647 "yacc_sql.y"
                    { (yyval.list) = NULL; }
#line 2181 "yacc_sql.tab.c"
    break;

  case 110: /* order_by: ORDER BY order_by_attr_list  */
#line 648 "yacc_sql.y"
                                      {
		(yyval.list) = (yyvsp[0].list);
	}
#line 2189 "yacc_sql.tab.c"
    break;

  case 111: /* order_by_attr_list: order_attr order_dir  */
#line 654 "yacc_sql.y"
                         {
		(yyval.list) = list_create(sizeof(OrderBy), MAX_NUM);
		OrderBy ob = {(yyvsp[0].orderdir), (yyvsp[-1].rel_attr)};
		list_append((yyval.list), &ob);
    }
#line 2199 "yacc_sql.tab.c"
    break;

  case 112: /* order_by_attr_list: order_attr order_dir COMMA order_by_attr_list  */
#line 659 "yacc_sql.y"
                                                    {
		(yyval.list) = (yyvsp[0].list);
		OrderBy ob = {(yyvsp[-2].orderdir), (yyvsp[-3].rel_attr)};
		list_append((yyval.list), &ob);
    }
#line 2209 "yacc_sql.tab.c"
    break;

  case 113: /* order_attr: ID  */
#line 667 "yacc_sql.y"
       {
			RelAttr *attr = malloc(sizeof(RelAttr));
			relation_attr_init(attr, NULL, (yyvsp[0].string));
			(yyval.rel_attr) = attr;
      }
#line 2219 "yacc_sql.tab.c"
    break;

  case 114: /* order_attr: ID DOT ID  */
#line 672 "yacc_sql.y"
                {
			RelAttr *attr = malloc(sizeof(RelAttr));
			relation_attr_init(attr, (yyvsp[-2].string), (yyvsp[0].string));
			(yyval.rel_attr) = attr;
  	   }
#line 2229 "yacc_sql.tab.c"
    break;

  case 115: /* order_dir: %empty  */
#line 681 "yacc_sql.y"
        { (yyval.orderdir) = DIR_ASC; }
#line 2235 "yacc_sql.tab.c"
    break;

  case 116: /* order_dir: ASC  */
#line 682 "yacc_sql.y"
              { (yyval.orderdir) = DIR_ASC; }
#line 2241 "yacc_sql.tab.c"
    break;

  case 117: /* order_dir: DESC  */
#line 683 "yacc_sql.y"
               { (yyval.orderdir) = DIR_DESC; }
#line 2247 "yacc_sql.tab.c"
    break;

  case 118: /* group_by: %empty  */
#line 687 "yacc_sql.y"
                    { (yyval.list) = NULL; }
#line 2253 "yacc_sql.tab.c"
    break;

  case 119: /* group_by: GROUP BY group_by_attr_list  */
#line 688 "yacc_sql.y"
                                      {
		(yyval.list) = (yyvsp[0].list);
	}
#line 2261 "yacc_sql.tab.c"
    break;

  case 120: /* group_by_attr_list: group_attr  */
#line 694 "yacc_sql.y"
               {
		(yyval.list) = list_create(sizeof(RelAttr), MAX_NUM);
		list_append((yyval.list), (yyvsp[0].rel_attr));
    }
#line 2270 "yacc_sql.tab.c"
    break;

  case 121: /* group_by_attr_list: group_attr COMMA group_by_attr_list  */
#line 698 "yacc_sql.y"
                                          {
		(yyval.list) = (yyvsp[0].list);
		list_append((yyval.list), (yyvsp[-2].rel_attr));
    }
#line 2279 "yacc_sql.tab.c"
    break;

  case 122: /* group_attr: ID  */
#line 705 "yacc_sql.y"
       {
		RelAttr *attr = malloc(sizeof(RelAttr));
		relation_attr_init(attr, NULL, (yyvsp[0].string));
		(yyval.rel_attr) = attr;
	}
#line 2289 "yacc_sql.tab.c"
    break;

  case 123: /* group_attr: ID DOT ID  */
#line 710 "yacc_sql.y"
                {
		RelAttr *attr = malloc(sizeof(RelAttr));
		relation_attr_init(attr, (yyvsp[-2].string), (yyvsp[0].string));
		(yyval.rel_attr) = attr;
	}
#line 2299 "yacc_sql.tab.c"
    break;

  case 124: /* load_data: LOAD DATA INFILE SSS INTO TABLE ID SEMICOLON  */
#line 720 "yacc_sql.y"
                {
		  CONTEXT->ssql->flag = SCF_LOAD_DATA;
			load_data_init(&CONTEXT->ssql->sstr.load_data, (yyvsp[-1].string), (yyvsp[-4].string));
		}
#line 2308 "yacc_sql.tab.c"
    break;


#line 2312 "yacc_sql.tab.c"

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
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (scanner, YY_("syntax error"));
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
                      yytoken, &yylval, scanner);
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

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

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (scanner, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, scanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 725 "yacc_sql.y"

//_____________________________________________________________________
extern void scan_string(const char *str, yyscan_t scanner);

int sql_parse(const char *s, Query *sqls){
	ParserContext context;
	memset(&context, 0, sizeof(context));

	yyscan_t scanner;
	yylex_init_extra(&context, &scanner);
	context.ssql = sqls;
	scan_string(s, scanner);
	int result = yyparse(scanner);
	yylex_destroy(scanner);
	return result;
}
