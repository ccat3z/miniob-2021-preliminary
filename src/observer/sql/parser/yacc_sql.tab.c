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
  YYSYMBOL_UNIQUE = 11,                    /* UNIQUE  */
  YYSYMBOL_SELECT = 12,                    /* SELECT  */
  YYSYMBOL_DESC = 13,                      /* DESC  */
  YYSYMBOL_SHOW = 14,                      /* SHOW  */
  YYSYMBOL_SYNC = 15,                      /* SYNC  */
  YYSYMBOL_INSERT = 16,                    /* INSERT  */
  YYSYMBOL_DELETE = 17,                    /* DELETE  */
  YYSYMBOL_UPDATE = 18,                    /* UPDATE  */
  YYSYMBOL_LBRACE = 19,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 20,                    /* RBRACE  */
  YYSYMBOL_COMMA = 21,                     /* COMMA  */
  YYSYMBOL_TRX_BEGIN = 22,                 /* TRX_BEGIN  */
  YYSYMBOL_TRX_COMMIT = 23,                /* TRX_COMMIT  */
  YYSYMBOL_TRX_ROLLBACK = 24,              /* TRX_ROLLBACK  */
  YYSYMBOL_INT_T = 25,                     /* INT_T  */
  YYSYMBOL_STRING_T = 26,                  /* STRING_T  */
  YYSYMBOL_DATE_T = 27,                    /* DATE_T  */
  YYSYMBOL_FLOAT_T = 28,                   /* FLOAT_T  */
  YYSYMBOL_TEXT_T = 29,                    /* TEXT_T  */
  YYSYMBOL_HELP = 30,                      /* HELP  */
  YYSYMBOL_EXIT = 31,                      /* EXIT  */
  YYSYMBOL_DOT = 32,                       /* DOT  */
  YYSYMBOL_INTO = 33,                      /* INTO  */
  YYSYMBOL_VALUES = 34,                    /* VALUES  */
  YYSYMBOL_FROM = 35,                      /* FROM  */
  YYSYMBOL_INNER = 36,                     /* INNER  */
  YYSYMBOL_JOIN = 37,                      /* JOIN  */
  YYSYMBOL_WHERE = 38,                     /* WHERE  */
  YYSYMBOL_IS = 39,                        /* IS  */
  YYSYMBOL_AND = 40,                       /* AND  */
  YYSYMBOL_NOT = 41,                       /* NOT  */
  YYSYMBOL_ORDER = 42,                     /* ORDER  */
  YYSYMBOL_BY = 43,                        /* BY  */
  YYSYMBOL_ASC = 44,                       /* ASC  */
  YYSYMBOL_GROUP = 45,                     /* GROUP  */
  YYSYMBOL_SET = 46,                       /* SET  */
  YYSYMBOL_ON = 47,                        /* ON  */
  YYSYMBOL_LOAD = 48,                      /* LOAD  */
  YYSYMBOL_DATA = 49,                      /* DATA  */
  YYSYMBOL_INFILE = 50,                    /* INFILE  */
  YYSYMBOL_EQ = 51,                        /* EQ  */
  YYSYMBOL_LT = 52,                        /* LT  */
  YYSYMBOL_GT = 53,                        /* GT  */
  YYSYMBOL_LE = 54,                        /* LE  */
  YYSYMBOL_GE = 55,                        /* GE  */
  YYSYMBOL_NE = 56,                        /* NE  */
  YYSYMBOL_IN = 57,                        /* IN  */
  YYSYMBOL_NUMBER = 58,                    /* NUMBER  */
  YYSYMBOL_FLOAT = 59,                     /* FLOAT  */
  YYSYMBOL_ID = 60,                        /* ID  */
  YYSYMBOL_PATH = 61,                      /* PATH  */
  YYSYMBOL_SSS = 62,                       /* SSS  */
  YYSYMBOL_STAR = 63,                      /* STAR  */
  YYSYMBOL_STRING_V = 64,                  /* STRING_V  */
  YYSYMBOL_ADD = 65,                       /* ADD  */
  YYSYMBOL_MINUS = 66,                     /* MINUS  */
  YYSYMBOL_DIV = 67,                       /* DIV  */
  YYSYMBOL_YYACCEPT = 68,                  /* $accept  */
  YYSYMBOL_commands = 69,                  /* commands  */
  YYSYMBOL_command = 70,                   /* command  */
  YYSYMBOL_exit = 71,                      /* exit  */
  YYSYMBOL_help = 72,                      /* help  */
  YYSYMBOL_sync = 73,                      /* sync  */
  YYSYMBOL_begin = 74,                     /* begin  */
  YYSYMBOL_commit = 75,                    /* commit  */
  YYSYMBOL_rollback = 76,                  /* rollback  */
  YYSYMBOL_drop_table = 77,                /* drop_table  */
  YYSYMBOL_show_tables = 78,               /* show_tables  */
  YYSYMBOL_desc_table = 79,                /* desc_table  */
  YYSYMBOL_create_index = 80,              /* create_index  */
  YYSYMBOL_create_index_unique = 81,       /* create_index_unique  */
  YYSYMBOL_create_index_attr_list = 82,    /* create_index_attr_list  */
  YYSYMBOL_drop_index = 83,                /* drop_index  */
  YYSYMBOL_create_table = 84,              /* create_table  */
  YYSYMBOL_attr_def_list = 85,             /* attr_def_list  */
  YYSYMBOL_attr_def = 86,                  /* attr_def  */
  YYSYMBOL_number = 87,                    /* number  */
  YYSYMBOL_type = 88,                      /* type  */
  YYSYMBOL_ID_get = 89,                    /* ID_get  */
  YYSYMBOL_attr_def_nullable = 90,         /* attr_def_nullable  */
  YYSYMBOL_insert = 91,                    /* insert  */
  YYSYMBOL_value_lists = 92,               /* value_lists  */
  YYSYMBOL_value_list = 93,                /* value_list  */
  YYSYMBOL_value = 94,                     /* value  */
  YYSYMBOL_pos_value = 95,                 /* pos_value  */
  YYSYMBOL_delete = 96,                    /* delete  */
  YYSYMBOL_update = 97,                    /* update  */
  YYSYMBOL_select = 98,                    /* select  */
  YYSYMBOL_select_statement = 99,          /* select_statement  */
  YYSYMBOL_select_expr_list = 100,         /* select_expr_list  */
  YYSYMBOL_select_expr = 101,              /* select_expr  */
  YYSYMBOL_select_calc_expr = 102,         /* select_calc_expr  */
  YYSYMBOL_select_attr = 103,              /* select_attr  */
  YYSYMBOL_rel_list = 104,                 /* rel_list  */
  YYSYMBOL_join_list = 105,                /* join_list  */
  YYSYMBOL_where = 106,                    /* where  */
  YYSYMBOL_condition_list = 107,           /* condition_list  */
  YYSYMBOL_condition = 108,                /* condition  */
  YYSYMBOL_condition_expr = 109,           /* condition_expr  */
  YYSYMBOL_condition_calc_expr = 110,      /* condition_calc_expr  */
  YYSYMBOL_comp_op = 111,                  /* comp_op  */
  YYSYMBOL_order_by = 112,                 /* order_by  */
  YYSYMBOL_order_by_attr_list = 113,       /* order_by_attr_list  */
  YYSYMBOL_order_attr = 114,               /* order_attr  */
  YYSYMBOL_order_dir = 115,                /* order_dir  */
  YYSYMBOL_group_by = 116,                 /* group_by  */
  YYSYMBOL_group_by_attr_list = 117,       /* group_by_attr_list  */
  YYSYMBOL_group_attr = 118,               /* group_attr  */
  YYSYMBOL_load_data = 119                 /* load_data  */
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
#define YYLAST   232

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  68
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  52
/* YYNRULES -- Number of rules.  */
#define YYNRULES  128
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  241

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   322


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
      65,    66,    67
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   170,   170,   172,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   187,   188,   189,   190,   191,
     192,   196,   201,   206,   212,   218,   224,   230,   236,   242,
     249,   257,   258,   261,   265,   272,   279,   285,   287,   291,
     297,   305,   308,   309,   310,   311,   312,   315,   322,   323,
     324,   329,   337,   341,   349,   353,   359,   362,   365,   370,
     373,   376,   379,   386,   396,   405,   413,   449,   453,   460,
     464,   469,   472,   482,   486,   490,   494,   499,   503,   510,
     515,   520,   525,   533,   534,   540,   544,   552,   553,   558,
     563,   570,   574,   584,   596,   600,   604,   608,   612,   617,
     625,   629,   633,   637,   641,   648,   649,   650,   651,   652,
     653,   654,   655,   659,   660,   666,   671,   679,   684,   693,
     694,   695,   699,   700,   706,   710,   717,   722,   731
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
  "DROP", "TABLE", "TABLES", "NULL_VALUE", "NULLABLE", "INDEX", "UNIQUE",
  "SELECT", "DESC", "SHOW", "SYNC", "INSERT", "DELETE", "UPDATE", "LBRACE",
  "RBRACE", "COMMA", "TRX_BEGIN", "TRX_COMMIT", "TRX_ROLLBACK", "INT_T",
  "STRING_T", "DATE_T", "FLOAT_T", "TEXT_T", "HELP", "EXIT", "DOT", "INTO",
  "VALUES", "FROM", "INNER", "JOIN", "WHERE", "IS", "AND", "NOT", "ORDER",
  "BY", "ASC", "GROUP", "SET", "ON", "LOAD", "DATA", "INFILE", "EQ", "LT",
  "GT", "LE", "GE", "NE", "IN", "NUMBER", "FLOAT", "ID", "PATH", "SSS",
  "STAR", "STRING_V", "ADD", "MINUS", "DIV", "$accept", "commands",
  "command", "exit", "help", "sync", "begin", "commit", "rollback",
  "drop_table", "show_tables", "desc_table", "create_index",
  "create_index_unique", "create_index_attr_list", "drop_index",
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

#define YYPACT_NINF (-150)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -150,   100,  -150,     3,     6,    -6,   -43,    23,    24,    13,
      26,     7,    52,    60,    62,    90,    96,    53,  -150,  -150,
    -150,  -150,  -150,  -150,  -150,  -150,  -150,  -150,  -150,  -150,
    -150,  -150,  -150,  -150,  -150,    98,  -150,    43,  -150,    97,
      46,    48,  -150,    -6,  -150,  -150,   -13,  -150,  -150,    -6,
    -150,    74,     5,  -150,  -150,   108,   117,  -150,    66,    67,
      82,  -150,  -150,  -150,  -150,  -150,    79,  -150,   119,    72,
     136,   137,    18,   121,    -6,   -45,   -34,    83,    -6,    -6,
      -6,    -6,    -6,  -150,  -150,   110,   104,    85,    84,    87,
     102,  -150,  -150,  -150,   -16,  -150,  -150,   130,  -150,  -150,
     -34,   -34,  -150,   134,    29,   156,   109,   128,  -150,   141,
      -4,   103,  -150,   111,   138,     0,   169,    20,   145,    29,
    -150,  -150,   133,   113,  -150,  -150,     0,   175,    87,   162,
    -150,  -150,  -150,  -150,  -150,     1,   164,   130,   147,   104,
     -23,   165,   166,  -150,  -150,   168,    31,   170,   126,   -22,
      29,    -1,   132,  -150,  -150,  -150,  -150,  -150,  -150,  -150,
      29,    29,    29,    29,    29,   104,   131,   141,   189,  -150,
     135,   186,  -150,   139,  -150,   140,   150,  -150,  -150,   176,
       0,  -150,  -150,  -150,  -150,  -150,   188,  -150,  -150,   -22,
     -22,  -150,    31,   195,   198,  -150,  -150,  -150,   182,  -150,
     183,   185,   159,   160,   167,   134,  -150,  -150,  -150,  -150,
       2,   139,   204,    29,   148,   171,  -150,  -150,  -150,  -150,
    -150,   138,   178,  -150,   190,   152,  -150,   153,   148,   184,
    -150,   -10,  -150,  -150,   155,  -150,  -150,   196,  -150,   152,
    -150
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,    31,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     3,    20,
      19,    14,    15,    16,    17,     9,    10,    11,    12,    13,
       8,     5,     7,     6,     4,     0,    18,     0,    32,     0,
       0,     0,    59,     0,    60,    61,    80,    62,    79,     0,
      70,     0,    67,    71,    69,     0,     0,    23,     0,     0,
       0,    24,    25,    26,    22,    21,     0,    65,     0,     0,
       0,     0,     0,    71,     0,     0,    73,     0,     0,     0,
       0,     0,     0,    29,    28,     0,    87,     0,     0,     0,
       0,    27,    35,    78,     0,    81,    82,    83,    68,    76,
      74,    75,    77,     0,     0,     0,     0,     0,    47,    37,
       0,     0,    72,     0,    85,     0,     0,     0,    94,     0,
      96,    88,    89,     0,    98,    63,     0,     0,     0,     0,
      42,    43,    44,    45,    46,    48,     0,    83,     0,    87,
       0,     0,    54,    56,    51,     0,     0,    98,     0,    99,
       0,     0,     0,   105,   106,   107,   108,   109,   110,   111,
       0,     0,     0,     0,     0,    87,     0,    37,     0,    50,
       0,     0,    40,     0,    84,     0,   122,    57,    58,    52,
       0,    97,   104,    95,    90,    92,     0,   112,   102,   100,
     101,   103,    91,     0,     0,    38,    36,    41,     0,    49,
      33,     0,     0,     0,   113,     0,    55,    93,    64,   128,
      48,     0,     0,     0,     0,     0,    66,    53,    39,    34,
      30,    85,   126,   123,   124,     0,    86,     0,     0,   117,
     114,   119,   127,   125,     0,   121,   120,   115,   118,     0,
     116
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -150,  -150,  -150,  -150,  -150,  -150,  -150,  -150,  -150,  -150,
    -150,  -150,  -150,  -150,     8,  -150,  -150,    51,    92,  -150,
    -150,  -150,    11,  -150,    17,    44,    99,    -5,  -150,  -150,
    -150,   106,   149,    76,   187,  -150,    89,    10,  -134,  -149,
    -150,   -27,   112,  -150,  -150,   -11,  -150,  -150,  -150,     4,
    -150,  -150
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     1,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    39,   201,    29,    30,   129,   109,   198,
     135,   110,   172,    31,   116,   141,   142,   120,    32,    33,
      34,    35,    51,    52,    53,    54,   114,   139,   105,   121,
     122,   123,   124,   164,   216,   230,   231,   237,   204,   223,
     224,    36
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      50,   184,    42,   235,   112,   176,    74,   185,    42,    37,
     169,   169,    40,    43,    38,    95,    41,    55,    96,    75,
     170,   130,   131,   132,   133,   134,    78,    57,    42,    79,
      56,   193,     5,    82,   236,   177,   178,    42,    50,   117,
     186,   160,   171,   171,    50,   163,    58,    79,   117,    80,
      81,    82,    44,    45,    46,    61,    47,    48,    44,    45,
      49,    59,    47,    62,   221,    63,   140,    60,    79,    50,
      80,    81,    82,    50,    50,    50,    50,    50,    44,    45,
     118,    79,    47,    80,    81,    82,   119,    44,    45,   118,
     146,    47,   149,    64,   160,   119,   161,   162,   163,    65,
       2,    67,    66,    68,     3,     4,    70,    69,    71,    77,
     143,    83,     5,     6,     7,     8,     9,    10,    11,    72,
      84,   143,    12,    13,    14,    76,    85,    86,    87,    88,
      15,    16,    90,   188,   189,   190,   191,   192,    89,    91,
      92,    93,   104,    97,   103,   106,   107,   108,    17,   111,
      94,   113,   151,   115,   152,    99,   100,   101,   102,   125,
     126,   127,   128,   136,   153,   154,   155,   156,   157,   158,
     159,   137,   144,   150,   138,   143,   160,   148,   161,   162,
     163,   166,   168,   173,   175,   179,   183,   180,   181,   187,
     182,   194,   196,   197,   199,   203,   207,   205,   208,   200,
     202,   209,   210,   214,   211,   212,   213,   220,   222,   215,
     227,   228,   229,   232,   225,   238,   234,   239,   195,   219,
     167,   218,   217,   145,   206,   165,   174,    98,   240,   147,
      73,   226,   233
};

static const yytype_uint8 yycheck[] =
{
       5,   150,     8,    13,    20,   139,    19,     8,     8,     6,
       9,     9,     6,    19,    11,    60,    10,    60,    63,    32,
      19,    25,    26,    27,    28,    29,    21,     3,     8,    63,
       7,   165,    12,    67,    44,    58,    59,     8,    43,    19,
      41,    63,    41,    41,    49,    67,    33,    63,    19,    65,
      66,    67,    58,    59,    60,     3,    62,    63,    58,    59,
      66,    35,    62,     3,   213,     3,    66,    60,    63,    74,
      65,    66,    67,    78,    79,    80,    81,    82,    58,    59,
      60,    63,    62,    65,    66,    67,    66,    58,    59,    60,
     117,    62,   119,     3,    63,    66,    65,    66,    67,     3,
       0,     3,    49,    60,     4,     5,    60,    10,    60,    35,
     115,     3,    12,    13,    14,    15,    16,    17,    18,    43,
       3,   126,    22,    23,    24,    49,    60,    60,    46,    50,
      30,    31,    60,   160,   161,   162,   163,   164,    19,     3,
       3,    20,    38,    60,    34,    60,    62,    60,    48,    47,
      74,    21,    39,    19,    41,    79,    80,    81,    82,     3,
      51,    33,    21,    60,    51,    52,    53,    54,    55,    56,
      57,    60,     3,    40,    36,   180,    63,    32,    65,    66,
      67,     6,    20,    19,    37,    20,    60,    21,    20,    57,
      20,    60,     3,    58,     8,    45,     8,    21,     3,    60,
      60,     3,    20,    43,    21,    20,    47,     3,    60,    42,
      32,    21,    60,    60,    43,    60,    32,    21,   167,   211,
     128,   210,   205,   117,   180,   126,   137,    78,   239,   117,
      43,   221,   228
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    69,     0,     4,     5,    12,    13,    14,    15,    16,
      17,    18,    22,    23,    24,    30,    31,    48,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    83,
      84,    91,    96,    97,    98,    99,   119,     6,    11,    81,
       6,    10,     8,    19,    58,    59,    60,    62,    63,    66,
      95,   100,   101,   102,   103,    60,     7,     3,    33,    35,
      60,     3,     3,     3,     3,     3,    49,     3,    60,    10,
      60,    60,   101,   102,    19,    32,   101,    35,    21,    63,
      65,    66,    67,     3,     3,    60,    60,    46,    50,    19,
      60,     3,     3,    20,   101,    60,    63,    60,   100,   101,
     101,   101,   101,    34,    38,   106,    60,    62,    60,    86,
      89,    47,    20,    21,   104,    19,    92,    19,    60,    66,
      95,   107,   108,   109,   110,     3,    51,    33,    21,    85,
      25,    26,    27,    28,    29,    88,    60,    60,    36,   105,
      66,    93,    94,    95,     3,    99,   109,   110,    32,   109,
      40,    39,    41,    51,    52,    53,    54,    55,    56,    57,
      63,    65,    66,    67,   111,    94,     6,    86,    20,     9,
      19,    41,    90,    19,   104,    37,   106,    58,    59,    20,
      21,    20,    20,    60,   107,     8,    41,    57,   109,   109,
     109,   109,   109,   106,    60,    85,     3,    58,    87,     8,
      60,    82,    60,    45,   116,    21,    93,     8,     3,     3,
      20,    21,    20,    47,    43,    42,   112,    92,    90,    82,
       3,   107,    60,   117,   118,    43,   105,    32,    21,    60,
     113,   114,    60,   117,    32,    13,    44,   115,    60,    21,
     113
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    68,    69,    69,    70,    70,    70,    70,    70,    70,
      70,    70,    70,    70,    70,    70,    70,    70,    70,    70,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    81,    82,    82,    83,    84,    85,    85,    86,
      86,    87,    88,    88,    88,    88,    88,    89,    90,    90,
      90,    91,    92,    92,    93,    93,    94,    94,    94,    95,
      95,    95,    95,    96,    97,    98,    99,   100,   100,   101,
     101,   101,   101,   102,   102,   102,   102,   102,   102,   103,
     103,   103,   103,   104,   104,   105,   105,   106,   106,   107,
     107,   108,   108,   108,   109,   109,   109,   109,   109,   110,
     110,   110,   110,   110,   110,   111,   111,   111,   111,   111,
     111,   111,   111,   112,   112,   113,   113,   114,   114,   115,
     115,   115,   116,   116,   117,   117,   118,   118,   119
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     2,     2,     2,     2,     2,     4,     3,     3,
      10,     0,     1,     1,     3,     4,     8,     0,     3,     6,
       3,     1,     1,     1,     1,     1,     1,     1,     0,     2,
       1,     6,     3,     5,     1,     3,     1,     2,     2,     1,
       1,     1,     1,     5,     8,     2,     9,     1,     3,     1,
       1,     1,     4,     2,     3,     3,     3,     3,     3,     1,
       1,     3,     3,     0,     3,     0,     6,     0,     2,     1,
       3,     3,     3,     4,     1,     3,     1,     3,     1,     2,
       3,     3,     3,     3,     3,     1,     1,     1,     1,     1,
       1,     1,     2,     0,     3,     2,     4,     1,     3,     0,
       1,     1,     0,     3,     1,     3,     1,     3,     8
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
#line 196 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    }
#line 1409 "yacc_sql.tab.c"
    break;

  case 22: /* help: HELP SEMICOLON  */
#line 201 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    }
#line 1417 "yacc_sql.tab.c"
    break;

  case 23: /* sync: SYNC SEMICOLON  */
#line 206 "yacc_sql.y"
                   {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
#line 1425 "yacc_sql.tab.c"
    break;

  case 24: /* begin: TRX_BEGIN SEMICOLON  */
#line 212 "yacc_sql.y"
                        {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
#line 1433 "yacc_sql.tab.c"
    break;

  case 25: /* commit: TRX_COMMIT SEMICOLON  */
#line 218 "yacc_sql.y"
                         {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
#line 1441 "yacc_sql.tab.c"
    break;

  case 26: /* rollback: TRX_ROLLBACK SEMICOLON  */
#line 224 "yacc_sql.y"
                           {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
#line 1449 "yacc_sql.tab.c"
    break;

  case 27: /* drop_table: DROP TABLE ID SEMICOLON  */
#line 230 "yacc_sql.y"
                            {
        CONTEXT->ssql->flag = SCF_DROP_TABLE;//"drop_table";
        drop_table_init(&CONTEXT->ssql->sstr.drop_table, (yyvsp[-1].string));
    }
#line 1458 "yacc_sql.tab.c"
    break;

  case 28: /* show_tables: SHOW TABLES SEMICOLON  */
#line 236 "yacc_sql.y"
                          {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
#line 1466 "yacc_sql.tab.c"
    break;

  case 29: /* desc_table: DESC ID SEMICOLON  */
#line 242 "yacc_sql.y"
                      {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
      desc_table_init(&CONTEXT->ssql->sstr.desc_table, (yyvsp[-1].string));
    }
#line 1475 "yacc_sql.tab.c"
    break;

  case 30: /* create_index: CREATE create_index_unique INDEX ID ON ID LBRACE create_index_attr_list RBRACE SEMICOLON  */
#line 250 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-6].string), (yyvsp[-4].string), (const char **)(yyvsp[-2].list)->values, (yyvsp[-2].list)->len, (yyvsp[-8].boolean));
			list_free((yyvsp[-2].list));
		}
#line 1485 "yacc_sql.tab.c"
    break;

  case 31: /* create_index_unique: %empty  */
#line 257 "yacc_sql.y"
        { (yyval.boolean) = false; }
#line 1491 "yacc_sql.tab.c"
    break;

  case 32: /* create_index_unique: UNIQUE  */
#line 258 "yacc_sql.y"
                 { (yyval.boolean) = true; }
#line 1497 "yacc_sql.tab.c"
    break;

  case 33: /* create_index_attr_list: ID  */
#line 261 "yacc_sql.y"
           {
		(yyval.list) = list_create(sizeof(char *), MAX_NUM);
		list_append((yyval.list), &(yyvsp[0].string));
	}
#line 1506 "yacc_sql.tab.c"
    break;

  case 34: /* create_index_attr_list: ID COMMA create_index_attr_list  */
#line 265 "yacc_sql.y"
                                          {
		(yyval.list) = (yyvsp[0].list);
		list_append((yyval.list), &(yyvsp[-2].string));
	}
#line 1515 "yacc_sql.tab.c"
    break;

  case 35: /* drop_index: DROP INDEX ID SEMICOLON  */
#line 273 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			drop_index_init(&CONTEXT->ssql->sstr.drop_index, (yyvsp[-1].string));
		}
#line 1524 "yacc_sql.tab.c"
    break;

  case 36: /* create_table: CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE SEMICOLON  */
#line 280 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, (yyvsp[-5].string));
		}
#line 1533 "yacc_sql.tab.c"
    break;

  case 38: /* attr_def_list: COMMA attr_def attr_def_list  */
#line 287 "yacc_sql.y"
                                   {    }
#line 1539 "yacc_sql.tab.c"
    break;

  case 39: /* attr_def: ID_get type LBRACE number RBRACE attr_def_nullable  */
#line 292 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-4].number), (yyvsp[-2].number), (yyvsp[0].boolean));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
		}
#line 1549 "yacc_sql.tab.c"
    break;

  case 40: /* attr_def: ID_get type attr_def_nullable  */
#line 298 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-1].number), 4, (yyvsp[0].boolean));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
		}
#line 1559 "yacc_sql.tab.c"
    break;

  case 41: /* number: NUMBER  */
#line 305 "yacc_sql.y"
                       {(yyval.number) = (yyvsp[0].number);}
#line 1565 "yacc_sql.tab.c"
    break;

  case 42: /* type: INT_T  */
#line 308 "yacc_sql.y"
              { (yyval.number)=INTS; }
#line 1571 "yacc_sql.tab.c"
    break;

  case 43: /* type: STRING_T  */
#line 309 "yacc_sql.y"
                  { (yyval.number)=CHARS; }
#line 1577 "yacc_sql.tab.c"
    break;

  case 44: /* type: DATE_T  */
#line 310 "yacc_sql.y"
                    { (yyval.number)=DATE; }
#line 1583 "yacc_sql.tab.c"
    break;

  case 45: /* type: FLOAT_T  */
#line 311 "yacc_sql.y"
                 { (yyval.number)=FLOATS; }
#line 1589 "yacc_sql.tab.c"
    break;

  case 46: /* type: TEXT_T  */
#line 312 "yacc_sql.y"
                    { (yyval.number)=TEXT; }
#line 1595 "yacc_sql.tab.c"
    break;

  case 47: /* ID_get: ID  */
#line 316 "yacc_sql.y"
        {
		char *temp=(yyvsp[0].string); 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
#line 1604 "yacc_sql.tab.c"
    break;

  case 48: /* attr_def_nullable: %empty  */
#line 322 "yacc_sql.y"
        { (yyval.boolean) = false; }
#line 1610 "yacc_sql.tab.c"
    break;

  case 49: /* attr_def_nullable: NOT NULL_VALUE  */
#line 323 "yacc_sql.y"
                         { (yyval.boolean) = false; }
#line 1616 "yacc_sql.tab.c"
    break;

  case 50: /* attr_def_nullable: NULLABLE  */
#line 324 "yacc_sql.y"
                   { (yyval.boolean) = true; }
#line 1622 "yacc_sql.tab.c"
    break;

  case 51: /* insert: INSERT INTO ID VALUES value_lists SEMICOLON  */
#line 330 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_INSERT;//"insert";
			inserts_init(&CONTEXT->ssql->sstr.insertion, (yyvsp[-3].string), (Value *)(yyvsp[-1].list)->values, (yyvsp[-1].list)->len);
			list_free((yyvsp[-1].list));
    }
#line 1632 "yacc_sql.tab.c"
    break;

  case 52: /* value_lists: LBRACE value_list RBRACE  */
#line 337 "yacc_sql.y"
                                 {
		CONTEXT->ssql->sstr.insertion.tuple_num++;
		(yyval.list) = (yyvsp[-1].list);
	}
#line 1641 "yacc_sql.tab.c"
    break;

  case 53: /* value_lists: LBRACE value_list RBRACE COMMA value_lists  */
#line 341 "yacc_sql.y"
                                                     {
		CONTEXT->ssql->sstr.insertion.tuple_num++;
		(yyval.list) = (yyvsp[0].list);
		list_append_list((yyval.list), (yyvsp[-3].list));
		list_free((yyvsp[-3].list));
	}
#line 1652 "yacc_sql.tab.c"
    break;

  case 54: /* value_list: value  */
#line 349 "yacc_sql.y"
              {
		(yyval.list) = list_create(sizeof(Value), MAX_NUM);
		list_append((yyval.list), &(yyvsp[0].value));
	}
#line 1661 "yacc_sql.tab.c"
    break;

  case 55: /* value_list: value COMMA value_list  */
#line 353 "yacc_sql.y"
                              { 
		(yyval.list) = (yyvsp[0].list);
		list_append((yyval.list), &(yyvsp[-2].value));
	  }
#line 1670 "yacc_sql.tab.c"
    break;

  case 56: /* value: pos_value  */
#line 359 "yacc_sql.y"
                  {
		(yyval.value) = (yyvsp[0].value);
	}
#line 1678 "yacc_sql.tab.c"
    break;

  case 57: /* value: MINUS NUMBER  */
#line 362 "yacc_sql.y"
                 {	
  		value_init_integer(&(yyval.value), -(yyvsp[0].number));
		}
#line 1686 "yacc_sql.tab.c"
    break;

  case 58: /* value: MINUS FLOAT  */
#line 365 "yacc_sql.y"
                {
  		value_init_float(&(yyval.value), -(yyvsp[0].floats));
		}
#line 1694 "yacc_sql.tab.c"
    break;

  case 59: /* pos_value: NULL_VALUE  */
#line 370 "yacc_sql.y"
                   {
		value_init_null(&(yyval.value));
	}
#line 1702 "yacc_sql.tab.c"
    break;

  case 60: /* pos_value: NUMBER  */
#line 373 "yacc_sql.y"
           {	
  		value_init_integer(&(yyval.value), (yyvsp[0].number));
		}
#line 1710 "yacc_sql.tab.c"
    break;

  case 61: /* pos_value: FLOAT  */
#line 376 "yacc_sql.y"
          {
  		value_init_float(&(yyval.value), (yyvsp[0].floats));
		}
#line 1718 "yacc_sql.tab.c"
    break;

  case 62: /* pos_value: SSS  */
#line 379 "yacc_sql.y"
         {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  		value_init_string(&(yyval.value), (yyvsp[0].string));
		}
#line 1727 "yacc_sql.tab.c"
    break;

  case 63: /* delete: DELETE FROM ID where SEMICOLON  */
#line 387 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			deletes_init_relation(&CONTEXT->ssql->sstr.deletion, (yyvsp[-2].string));
			deletes_set_conditions(&CONTEXT->ssql->sstr.deletion, 
					(Condition *) (yyvsp[-1].list)->values, (yyvsp[-1].list)->len);
			list_free((yyvsp[-1].list));
    }
#line 1739 "yacc_sql.tab.c"
    break;

  case 64: /* update: UPDATE ID SET ID EQ value where SEMICOLON  */
#line 397 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			updates_init(&CONTEXT->ssql->sstr.update, (yyvsp[-6].string), (yyvsp[-4].string), &(yyvsp[-2].value), 
					(Condition *) (yyvsp[-1].list)->values, (yyvsp[-1].list)->len);
			list_free((yyvsp[-1].list));
		}
#line 1750 "yacc_sql.tab.c"
    break;

  case 65: /* select: select_statement SEMICOLON  */
#line 405 "yacc_sql.y"
                                   {
		CONTEXT->ssql->flag=SCF_SELECT;//"select";
		CONTEXT->ssql->sstr.selection = *(yyvsp[-1].select_statement);
		free((yyvsp[-1].select_statement));
	}
#line 1760 "yacc_sql.tab.c"
    break;

  case 66: /* select_statement: SELECT select_expr_list FROM ID rel_list join_list where group_by order_by  */
#line 414 "yacc_sql.y"
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
#line 1797 "yacc_sql.tab.c"
    break;

  case 67: /* select_expr_list: select_expr  */
#line 449 "yacc_sql.y"
                    {
		(yyval.list) = list_create(sizeof(SelectExpr), MAX_NUM);
		list_append((yyval.list), &(yyvsp[0].select_expr));
	}
#line 1806 "yacc_sql.tab.c"
    break;

  case 68: /* select_expr_list: select_expr COMMA select_expr_list  */
#line 453 "yacc_sql.y"
                                             {
		(yyval.list) = (yyvsp[0].list);
		list_append((yyval.list), &(yyvsp[-2].select_expr));
	}
#line 1815 "yacc_sql.tab.c"
    break;

  case 69: /* select_expr: select_attr  */
#line 460 "yacc_sql.y"
                    {
		memset(&(yyval.select_expr), 0, sizeof (yyval.select_expr));
		(yyval.select_expr).attribute = (yyvsp[0].rel_attr);
	}
#line 1824 "yacc_sql.tab.c"
    break;

  case 70: /* select_expr: pos_value  */
#line 464 "yacc_sql.y"
                    {
		memset(&(yyval.select_expr), 0, sizeof (yyval.select_expr));
		(yyval.select_expr).value = malloc(sizeof(Value));
		*(yyval.select_expr).value = (yyvsp[0].value);
	}
#line 1834 "yacc_sql.tab.c"
    break;

  case 71: /* select_expr: select_calc_expr  */
#line 469 "yacc_sql.y"
                           {
		(yyval.select_expr) = (yyvsp[0].select_expr);
	}
#line 1842 "yacc_sql.tab.c"
    break;

  case 72: /* select_expr: ID LBRACE select_expr RBRACE  */
#line 472 "yacc_sql.y"
                                       {
		AggExpr *expr = (AggExpr *) malloc(sizeof(AggExpr));
		agg_expr_init(expr, (yyvsp[-3].string), &(yyvsp[-1].select_expr));

		memset(&(yyval.select_expr), 0, sizeof (yyval.select_expr));
		(yyval.select_expr).agg = expr;
	}
#line 1854 "yacc_sql.tab.c"
    break;

  case 73: /* select_calc_expr: MINUS select_expr  */
#line 482 "yacc_sql.y"
                          {
		memset(&(yyval.select_expr), 0, sizeof (yyval.select_expr));
		(yyval.select_expr).calc = select_calc_expr_create(NULL, CALC_MINUS, &(yyvsp[0].select_expr));
	}
#line 1863 "yacc_sql.tab.c"
    break;

  case 74: /* select_calc_expr: select_expr ADD select_expr  */
#line 486 "yacc_sql.y"
                                      {
		memset(&(yyval.select_expr), 0, sizeof (yyval.select_expr));
		(yyval.select_expr).calc = select_calc_expr_create(&(yyvsp[-2].select_expr), CALC_ADD, &(yyvsp[0].select_expr));
	}
#line 1872 "yacc_sql.tab.c"
    break;

  case 75: /* select_calc_expr: select_expr MINUS select_expr  */
#line 490 "yacc_sql.y"
                                        {
		memset(&(yyval.select_expr), 0, sizeof (yyval.select_expr));
		(yyval.select_expr).calc = select_calc_expr_create(&(yyvsp[-2].select_expr), CALC_MINUS, &(yyvsp[0].select_expr));
	}
#line 1881 "yacc_sql.tab.c"
    break;

  case 76: /* select_calc_expr: select_expr STAR select_expr  */
#line 494 "yacc_sql.y"
                                       {
		// 9 * 9
		memset(&(yyval.select_expr), 0, sizeof (yyval.select_expr));
		(yyval.select_expr).calc = select_calc_expr_create(&(yyvsp[-2].select_expr), CALC_MULTI, &(yyvsp[0].select_expr));
	}
#line 1891 "yacc_sql.tab.c"
    break;

  case 77: /* select_calc_expr: select_expr DIV select_expr  */
#line 499 "yacc_sql.y"
                                      {
		memset(&(yyval.select_expr), 0, sizeof (yyval.select_expr));
		(yyval.select_expr).calc = select_calc_expr_create(&(yyvsp[-2].select_expr), CALC_DIV, &(yyvsp[0].select_expr));
	}
#line 1900 "yacc_sql.tab.c"
    break;

  case 78: /* select_calc_expr: LBRACE select_calc_expr RBRACE  */
#line 503 "yacc_sql.y"
                                         {
		(yyval.select_expr) = (yyvsp[-1].select_expr);
	    select_calc_expr_add_brace((yyval.select_expr).calc);
	}
#line 1909 "yacc_sql.tab.c"
    break;

  case 79: /* select_attr: STAR  */
#line 510 "yacc_sql.y"
         {
			RelAttr *attr = malloc(sizeof(RelAttr));
			relation_attr_init(attr, NULL, "*");
			(yyval.rel_attr) = attr;
      }
#line 1919 "yacc_sql.tab.c"
    break;

  case 80: /* select_attr: ID  */
#line 515 "yacc_sql.y"
         {
			RelAttr *attr = malloc(sizeof(RelAttr));
			relation_attr_init(attr, NULL, (yyvsp[0].string));
			(yyval.rel_attr) = attr;
      }
#line 1929 "yacc_sql.tab.c"
    break;

  case 81: /* select_attr: ID DOT ID  */
#line 520 "yacc_sql.y"
                {
			RelAttr *attr = malloc(sizeof(RelAttr));
			relation_attr_init(attr, (yyvsp[-2].string), (yyvsp[0].string));
			(yyval.rel_attr) = attr;
  	  }
#line 1939 "yacc_sql.tab.c"
    break;

  case 82: /* select_attr: ID DOT STAR  */
#line 525 "yacc_sql.y"
                  {
			RelAttr *attr = malloc(sizeof(RelAttr));
			relation_attr_init(attr, (yyvsp[-2].string), "*");
			(yyval.rel_attr) = attr;
  	  }
#line 1949 "yacc_sql.tab.c"
    break;

  case 83: /* rel_list: %empty  */
#line 533 "yacc_sql.y"
                { (yyval.list) = list_create(sizeof(char *), MAX_NUM); }
#line 1955 "yacc_sql.tab.c"
    break;

  case 84: /* rel_list: COMMA ID rel_list  */
#line 534 "yacc_sql.y"
                        {	
		(yyval.list) = (yyvsp[0].list);
		list_append((yyvsp[0].list), &(yyvsp[-1].string));
	}
#line 1964 "yacc_sql.tab.c"
    break;

  case 85: /* join_list: %empty  */
#line 540 "yacc_sql.y"
                    {
		(yyval.join_list).rels = list_create(sizeof(char *), MAX_NUM);
		(yyval.join_list).conds = list_create(sizeof(Condition), MAX_NUM);
	}
#line 1973 "yacc_sql.tab.c"
    break;

  case 86: /* join_list: INNER JOIN ID ON condition_list join_list  */
#line 544 "yacc_sql.y"
                                                    {
		(yyval.join_list) = (yyvsp[0].join_list);
		list_append((yyval.join_list).rels, &(yyvsp[-3].string));
		list_append_list((yyval.join_list).conds, (yyvsp[-1].list));
		list_free((yyvsp[-1].list));
	}
#line 1984 "yacc_sql.tab.c"
    break;

  case 87: /* where: %empty  */
#line 552 "yacc_sql.y"
                { (yyval.list) = list_create(sizeof(Condition), MAX_NUM); }
#line 1990 "yacc_sql.tab.c"
    break;

  case 88: /* where: WHERE condition_list  */
#line 553 "yacc_sql.y"
                           {	
		(yyval.list) = (yyvsp[0].list);
	}
#line 1998 "yacc_sql.tab.c"
    break;

  case 89: /* condition_list: condition  */
#line 558 "yacc_sql.y"
                  {
		(yyval.list) = list_create(sizeof(Condition), MAX_NUM);
		list_append((yyval.list), (yyvsp[0].condition));
		free((yyvsp[0].condition));
	}
#line 2008 "yacc_sql.tab.c"
    break;

  case 90: /* condition_list: condition AND condition_list  */
#line 563 "yacc_sql.y"
                                   {
		(yyval.list) = (yyvsp[0].list);
		list_append((yyval.list), (yyvsp[-2].condition));
		free((yyvsp[-2].condition));
	}
#line 2018 "yacc_sql.tab.c"
    break;

  case 91: /* condition: condition_expr comp_op condition_expr  */
#line 570 "yacc_sql.y"
                                              {
		(yyval.condition) = (Condition *) malloc(sizeof(Condition));
		condition_init((yyval.condition), (yyvsp[-1].comp_op), &(yyvsp[-2].condition_expr), &(yyvsp[0].condition_expr));
	}
#line 2027 "yacc_sql.tab.c"
    break;

  case 92: /* condition: condition_expr IS NULL_VALUE  */
#line 574 "yacc_sql.y"
                                       {
		(yyval.condition) = (Condition *) malloc(sizeof(Condition));
		Value null;
		value_init_null(&null);
		ConditionExpr null_expr;
		null_expr.type = COND_EXPR_VALUE;
		null_expr.value.value = null;

		condition_init((yyval.condition), IS_NULL, &(yyvsp[-2].condition_expr), &null_expr);
	}
#line 2042 "yacc_sql.tab.c"
    break;

  case 93: /* condition: condition_expr IS NOT NULL_VALUE  */
#line 584 "yacc_sql.y"
                                           {
		(yyval.condition) = (Condition *) malloc(sizeof(Condition));
		Value null;
		value_init_null(&null);
		ConditionExpr null_expr;
		null_expr.type = COND_EXPR_VALUE;
		null_expr.value.value = null;

		condition_init((yyval.condition), IS_NOT_NULL, &(yyvsp[-3].condition_expr), &null_expr);
	}
#line 2057 "yacc_sql.tab.c"
    break;

  case 94: /* condition_expr: ID  */
#line 596 "yacc_sql.y"
           {
		(yyval.condition_expr).type = COND_EXPR_ATTR;
		relation_attr_init(&(yyval.condition_expr).value.attr, NULL, (yyvsp[0].string));
	}
#line 2066 "yacc_sql.tab.c"
    break;

  case 95: /* condition_expr: ID DOT ID  */
#line 600 "yacc_sql.y"
                    {
		(yyval.condition_expr).type = COND_EXPR_ATTR;
		relation_attr_init(&(yyval.condition_expr).value.attr, (yyvsp[-2].string), (yyvsp[0].string));
	}
#line 2075 "yacc_sql.tab.c"
    break;

  case 96: /* condition_expr: pos_value  */
#line 604 "yacc_sql.y"
                    {
		(yyval.condition_expr).type = COND_EXPR_VALUE;
		(yyval.condition_expr).value.value = (yyvsp[0].value);
	}
#line 2084 "yacc_sql.tab.c"
    break;

  case 97: /* condition_expr: LBRACE select_statement RBRACE  */
#line 608 "yacc_sql.y"
                                         {
		(yyval.condition_expr).type = COND_EXPR_SELECT;
		(yyval.condition_expr).value.selects = (yyvsp[-1].select_statement);
	}
#line 2093 "yacc_sql.tab.c"
    break;

  case 98: /* condition_expr: condition_calc_expr  */
#line 612 "yacc_sql.y"
                              {
		(yyval.condition_expr) = (yyvsp[0].condition_expr);
	}
#line 2101 "yacc_sql.tab.c"
    break;

  case 99: /* condition_calc_expr: MINUS condition_expr  */
#line 617 "yacc_sql.y"
                             {
		ConditionExpr expr;
		expr.type = COND_EXPR_VALUE;
		value_init_integer(&expr.value.value, 0);

		(yyval.condition_expr).type = COND_EXPR_CALC;
		(yyval.condition_expr).value.calc = condition_calc_create(&expr, CALC_MINUS, &(yyvsp[0].condition_expr));
	}
#line 2114 "yacc_sql.tab.c"
    break;

  case 100: /* condition_calc_expr: condition_expr ADD condition_expr  */
#line 625 "yacc_sql.y"
                                            {
		(yyval.condition_expr).type = COND_EXPR_CALC;
		(yyval.condition_expr).value.calc = condition_calc_create(&(yyvsp[-2].condition_expr), CALC_ADD, &(yyvsp[0].condition_expr));
	}
#line 2123 "yacc_sql.tab.c"
    break;

  case 101: /* condition_calc_expr: condition_expr MINUS condition_expr  */
#line 629 "yacc_sql.y"
                                              {
		(yyval.condition_expr).type = COND_EXPR_CALC;
		(yyval.condition_expr).value.calc = condition_calc_create(&(yyvsp[-2].condition_expr), CALC_MINUS, &(yyvsp[0].condition_expr));
	}
#line 2132 "yacc_sql.tab.c"
    break;

  case 102: /* condition_calc_expr: condition_expr STAR condition_expr  */
#line 633 "yacc_sql.y"
                                             {
		(yyval.condition_expr).type = COND_EXPR_CALC;
		(yyval.condition_expr).value.calc = condition_calc_create(&(yyvsp[-2].condition_expr), CALC_MULTI, &(yyvsp[0].condition_expr));
	}
#line 2141 "yacc_sql.tab.c"
    break;

  case 103: /* condition_calc_expr: condition_expr DIV condition_expr  */
#line 637 "yacc_sql.y"
                                            {
		(yyval.condition_expr).type = COND_EXPR_CALC;
		(yyval.condition_expr).value.calc = condition_calc_create(&(yyvsp[-2].condition_expr), CALC_DIV, &(yyvsp[0].condition_expr));
	}
#line 2150 "yacc_sql.tab.c"
    break;

  case 104: /* condition_calc_expr: LBRACE condition_calc_expr RBRACE  */
#line 641 "yacc_sql.y"
                                            {
		(yyval.condition_expr) = (yyvsp[-1].condition_expr);
	}
#line 2158 "yacc_sql.tab.c"
    break;

  case 105: /* comp_op: EQ  */
#line 648 "yacc_sql.y"
             { (yyval.comp_op) = EQUAL_TO; }
#line 2164 "yacc_sql.tab.c"
    break;

  case 106: /* comp_op: LT  */
#line 649 "yacc_sql.y"
         { (yyval.comp_op) = LESS_THAN; }
#line 2170 "yacc_sql.tab.c"
    break;

  case 107: /* comp_op: GT  */
#line 650 "yacc_sql.y"
         { (yyval.comp_op) = GREAT_THAN; }
#line 2176 "yacc_sql.tab.c"
    break;

  case 108: /* comp_op: LE  */
#line 651 "yacc_sql.y"
         { (yyval.comp_op) = LESS_EQUAL; }
#line 2182 "yacc_sql.tab.c"
    break;

  case 109: /* comp_op: GE  */
#line 652 "yacc_sql.y"
         { (yyval.comp_op) = GREAT_EQUAL; }
#line 2188 "yacc_sql.tab.c"
    break;

  case 110: /* comp_op: NE  */
#line 653 "yacc_sql.y"
         { (yyval.comp_op) = NOT_EQUAL; }
#line 2194 "yacc_sql.tab.c"
    break;

  case 111: /* comp_op: IN  */
#line 654 "yacc_sql.y"
             { (yyval.comp_op) = IN_SET; }
#line 2200 "yacc_sql.tab.c"
    break;

  case 112: /* comp_op: NOT IN  */
#line 655 "yacc_sql.y"
                 { (yyval.comp_op) = NOT_IN_SET; }
#line 2206 "yacc_sql.tab.c"
    break;

  case 113: /* order_by: %empty  */
#line 659 "yacc_sql.y"
                    { (yyval.list) = NULL; }
#line 2212 "yacc_sql.tab.c"
    break;

  case 114: /* order_by: ORDER BY order_by_attr_list  */
#line 660 "yacc_sql.y"
                                      {
		(yyval.list) = (yyvsp[0].list);
	}
#line 2220 "yacc_sql.tab.c"
    break;

  case 115: /* order_by_attr_list: order_attr order_dir  */
#line 666 "yacc_sql.y"
                         {
		(yyval.list) = list_create(sizeof(OrderBy), MAX_NUM);
		OrderBy ob = {(yyvsp[0].orderdir), (yyvsp[-1].rel_attr)};
		list_append((yyval.list), &ob);
    }
#line 2230 "yacc_sql.tab.c"
    break;

  case 116: /* order_by_attr_list: order_attr order_dir COMMA order_by_attr_list  */
#line 671 "yacc_sql.y"
                                                    {
		(yyval.list) = (yyvsp[0].list);
		OrderBy ob = {(yyvsp[-2].orderdir), (yyvsp[-3].rel_attr)};
		list_append((yyval.list), &ob);
    }
#line 2240 "yacc_sql.tab.c"
    break;

  case 117: /* order_attr: ID  */
#line 679 "yacc_sql.y"
       {
			RelAttr *attr = malloc(sizeof(RelAttr));
			relation_attr_init(attr, NULL, (yyvsp[0].string));
			(yyval.rel_attr) = attr;
      }
#line 2250 "yacc_sql.tab.c"
    break;

  case 118: /* order_attr: ID DOT ID  */
#line 684 "yacc_sql.y"
                {
			RelAttr *attr = malloc(sizeof(RelAttr));
			relation_attr_init(attr, (yyvsp[-2].string), (yyvsp[0].string));
			(yyval.rel_attr) = attr;
  	   }
#line 2260 "yacc_sql.tab.c"
    break;

  case 119: /* order_dir: %empty  */
#line 693 "yacc_sql.y"
        { (yyval.orderdir) = DIR_ASC; }
#line 2266 "yacc_sql.tab.c"
    break;

  case 120: /* order_dir: ASC  */
#line 694 "yacc_sql.y"
              { (yyval.orderdir) = DIR_ASC; }
#line 2272 "yacc_sql.tab.c"
    break;

  case 121: /* order_dir: DESC  */
#line 695 "yacc_sql.y"
               { (yyval.orderdir) = DIR_DESC; }
#line 2278 "yacc_sql.tab.c"
    break;

  case 122: /* group_by: %empty  */
#line 699 "yacc_sql.y"
                    { (yyval.list) = NULL; }
#line 2284 "yacc_sql.tab.c"
    break;

  case 123: /* group_by: GROUP BY group_by_attr_list  */
#line 700 "yacc_sql.y"
                                      {
		(yyval.list) = (yyvsp[0].list);
	}
#line 2292 "yacc_sql.tab.c"
    break;

  case 124: /* group_by_attr_list: group_attr  */
#line 706 "yacc_sql.y"
               {
		(yyval.list) = list_create(sizeof(RelAttr), MAX_NUM);
		list_append((yyval.list), (yyvsp[0].rel_attr));
    }
#line 2301 "yacc_sql.tab.c"
    break;

  case 125: /* group_by_attr_list: group_attr COMMA group_by_attr_list  */
#line 710 "yacc_sql.y"
                                          {
		(yyval.list) = (yyvsp[0].list);
		list_append((yyval.list), (yyvsp[-2].rel_attr));
    }
#line 2310 "yacc_sql.tab.c"
    break;

  case 126: /* group_attr: ID  */
#line 717 "yacc_sql.y"
       {
		RelAttr *attr = malloc(sizeof(RelAttr));
		relation_attr_init(attr, NULL, (yyvsp[0].string));
		(yyval.rel_attr) = attr;
	}
#line 2320 "yacc_sql.tab.c"
    break;

  case 127: /* group_attr: ID DOT ID  */
#line 722 "yacc_sql.y"
                {
		RelAttr *attr = malloc(sizeof(RelAttr));
		relation_attr_init(attr, (yyvsp[-2].string), (yyvsp[0].string));
		(yyval.rel_attr) = attr;
	}
#line 2330 "yacc_sql.tab.c"
    break;

  case 128: /* load_data: LOAD DATA INFILE SSS INTO TABLE ID SEMICOLON  */
#line 732 "yacc_sql.y"
                {
		  CONTEXT->ssql->flag = SCF_LOAD_DATA;
			load_data_init(&CONTEXT->ssql->sstr.load_data, (yyvsp[-1].string), (yyvsp[-4].string));
		}
#line 2339 "yacc_sql.tab.c"
    break;


#line 2343 "yacc_sql.tab.c"

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

#line 737 "yacc_sql.y"

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
