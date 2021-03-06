
%code requires {
#include "sql/parser/parse_defs.h"
}

%{

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

%}

%define api.pure full
%lex-param { yyscan_t scanner }
%parse-param { void *scanner }

//标识tokens
%token  SEMICOLON
        CREATE
        DROP
        TABLE
        TABLES
		NULL_VALUE
		NULLABLE
        INDEX
		UNIQUE
        SELECT
        DESC
        SHOW
        SYNC
        INSERT
        DELETE
        UPDATE
        LBRACE
        RBRACE
        COMMA
        TRX_BEGIN
        TRX_COMMIT
        TRX_ROLLBACK
        INT_T
        STRING_T
		DATE_T
        FLOAT_T
		TEXT_T
        HELP
        EXIT
        DOT //QUOTE
        INTO
        VALUES
        FROM
		INNER
		JOIN
        WHERE
		IS
        AND
		NOT
		ORDER
		BY
		ASC
		GROUP
        SET
        ON
        LOAD
        DATA
        INFILE
        EQ
        LT
        GT
        LE
        GE
        NE
		IN

%union {
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
}

%token <number> NUMBER
%token <floats> FLOAT 
%token <string> ID
%token <string> PATH
%token <string> SSS
%token <string> STAR
%token <string> STRING_V
//非终结符

%type <number> type;
%type <condition> condition;
%type <value> value pos_value;
%type <list> value_list;
%type <list> value_lists;
%type <number> number;
%type <orderdir> order_dir;
%type <rel_attr> order_attr;
%type <rel_attr> select_attr;
%type <list> where;
%type <join_list> join_list;
%type <list> condition_list;
%type <list> rel_list;
%type <comp_op> comp_op;
%type <select_expr> select_expr select_calc_expr;
%type <list> select_expr_list;
%type <list> order_by_attr_list;
%type <list> order_by;
%type <list> group_by group_by_attr_list;
%type <rel_attr> group_attr;
%type <select_statement> select_statement;
%type <condition_expr> condition_expr condition_calc_expr;
%type <boolean> attr_def_nullable create_index_unique;
%type <list> create_index_attr_list;

%left ADD MINUS
%left STAR DIV
%%

commands:		//commands or sqls. parser starts here.
    /* empty */
    | commands command
    ;

command:
	  select  
	| insert
	| update
	| delete
	| create_table
	| drop_table
	| show_tables
	| desc_table
	| create_index	
	| drop_index
	| sync
	| begin
	| commit
	| rollback
	| load_data
	| help
	| exit
    ;

exit:			
    EXIT SEMICOLON {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    };

help:
    HELP SEMICOLON {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    };

sync:
    SYNC SEMICOLON {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
    ;

begin:
    TRX_BEGIN SEMICOLON {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
    ;

commit:
    TRX_COMMIT SEMICOLON {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
    ;

rollback:
    TRX_ROLLBACK SEMICOLON {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
    ;

drop_table:		/*drop table 语句的语法解析树*/
    DROP TABLE ID SEMICOLON {
        CONTEXT->ssql->flag = SCF_DROP_TABLE;//"drop_table";
        drop_table_init(&CONTEXT->ssql->sstr.drop_table, $3);
    };

show_tables:
    SHOW TABLES SEMICOLON {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
    ;

desc_table:
    DESC ID SEMICOLON {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
      desc_table_init(&CONTEXT->ssql->sstr.desc_table, $2);
    }
    ;

create_index:		/*create index 语句的语法解析树*/
    CREATE create_index_unique INDEX ID ON ID LBRACE create_index_attr_list RBRACE SEMICOLON 
		{
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, $4, $6, (const char **)$8->values, $8->len, $2);
			list_free($8);
		}
    ;
create_index_unique:
	{ $$ = false; }
	| UNIQUE { $$ = true; }
	;
create_index_attr_list:
	ID {
		$$ = list_create(sizeof(char *), MAX_NUM);
		list_append($$, &$1);
	}
	| ID COMMA create_index_attr_list {
		$$ = $3;
		list_append($$, &$1);
	}
	;

drop_index:			/*drop index 语句的语法解析树*/
    DROP INDEX ID  SEMICOLON 
		{
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			drop_index_init(&CONTEXT->ssql->sstr.drop_index, $3);
		}
    ;
create_table:		/*create table 语句的语法解析树*/
    CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE SEMICOLON 
		{
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, $3);
		}
    ;
attr_def_list:
    /* empty */
    | COMMA attr_def attr_def_list {    }
    ;
    
attr_def:
    ID_get type LBRACE number RBRACE attr_def_nullable
		{
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, $2, $4, $6);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
		}
    |ID_get type attr_def_nullable
		{
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, $2, 4, $3);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
		}
    ;
number:
		NUMBER {$$ = $1;}
		;
type:
	INT_T { $$=INTS; }
       | STRING_T { $$=CHARS; }
	   | DATE_T { $$=DATE; }
       | FLOAT_T { $$=FLOATS; }
	   | TEXT_T { $$=TEXT; }
       ;
ID_get:
	ID 
	{
		char *temp=$1; 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
	;
attr_def_nullable:
	{ $$ = false; }
	| NOT NULL_VALUE { $$ = false; }
	| NULLABLE { $$ = true; }
	;

	
insert:				/*insert   语句的语法解析树*/
    INSERT INTO ID VALUES value_lists SEMICOLON 
		{
			CONTEXT->ssql->flag=SCF_INSERT;//"insert";
			inserts_init(&CONTEXT->ssql->sstr.insertion, $3, (Value *)$5->values, $5->len);
			list_free($5);
    }

value_lists:
	LBRACE value_list RBRACE {
		CONTEXT->ssql->sstr.insertion.tuple_num++;
		$$ = $2;
	}
	| LBRACE value_list RBRACE COMMA value_lists {
		CONTEXT->ssql->sstr.insertion.tuple_num++;
		$$ = $5;
		list_append_list($$, $2);
		list_free($2);
	}
	;
value_list:
	value {
		$$ = list_create(sizeof(Value), MAX_NUM);
		list_append($$, &$1);
	}
    | value COMMA value_list  { 
		$$ = $3;
		list_append($$, &$1);
	  }
    ;
value:
	pos_value {
		$$ = $1;
	}
    |MINUS NUMBER{	
  		value_init_integer(&$$, -$2);
		}
    |MINUS FLOAT{
  		value_init_float(&$$, -$2);
		}
    ;
pos_value:
	NULL_VALUE {
		value_init_null(&$$);
	}
    |NUMBER{	
  		value_init_integer(&$$, $1);
		}
    |FLOAT{
  		value_init_float(&$$, $1);
		}
    |SSS {
			$1 = substr($1,1,strlen($1)-2);
  		value_init_string(&$$, $1);
		}
    ;
    
delete:		/*  delete 语句的语法解析树*/
    DELETE FROM ID where SEMICOLON 
		{
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			deletes_init_relation(&CONTEXT->ssql->sstr.deletion, $3);
			deletes_set_conditions(&CONTEXT->ssql->sstr.deletion, 
					(Condition *) $4->values, $4->len);
			list_free($4);
    }
    ;
update:			/*  update 语句的语法解析树*/
    UPDATE ID SET ID EQ value where SEMICOLON
		{
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			updates_init(&CONTEXT->ssql->sstr.update, $2, $4, &$6, 
					(Condition *) $7->values, $7->len);
			list_free($7);
		}
    ;
select:
	select_statement SEMICOLON {
		CONTEXT->ssql->flag=SCF_SELECT;//"select";
		CONTEXT->ssql->sstr.selection = *$1;
		free($1);
	}
	;

select_statement:				/*  select 语句的语法解析树*/
    SELECT select_expr_list FROM ID rel_list join_list where group_by order_by
		{
			Selects *selects = (Selects *) malloc(sizeof(Selects));
			memset(selects, 0, sizeof(*selects));

			selects_append_exprs(selects, (SelectExpr *) $2->values, $2->len);
			list_free($2);

			selects_append_relations(selects, (const char **) $6.rels->values, $6.rels->len);
			list_free($6.rels);
			selects_append_relations(selects, (const char **) $5->values, $5->len);
			list_free($5);
			selects_append_relation(selects, $4);

			selects_append_conditions(selects, (Condition *) $6.conds->values, $6.conds->len);
			list_free($6.conds);
			selects_append_conditions(selects, (Condition *) $7->values, $7->len);
			list_free($7);

			if ($8 != NULL) {
				selects_append_group_attrs(selects, (RelAttr *) $8->values, $8->len);
				list_free($8);
			}

			if ($9 != NULL) {
				selects_append_order_attrs(selects, (OrderBy *) $9->values, $9->len);
				list_free($9);
			}

			CONTEXT->ssql->flag=SCF_SELECT;//"select";

			$$ = selects;
	}
	;

select_expr_list:
	select_expr {
		$$ = list_create(sizeof(SelectExpr), MAX_NUM);
		list_append($$, &$1);
	}
	| select_expr COMMA select_expr_list {
		$$ = $3;
		list_append($$, &$1);
	}
    ;

select_expr:
	select_attr {
		memset(&$$, 0, sizeof $$);
		$$.attribute = $1;
	}
	| pos_value {
		memset(&$$, 0, sizeof $$);
		$$.value = malloc(sizeof(Value));
		*$$.value = $1;
	}
	| select_calc_expr {
		$$ = $1;
	}
	| ID LBRACE select_expr RBRACE {
		AggExpr *expr = (AggExpr *) malloc(sizeof(AggExpr));
		agg_expr_init(expr, $1, &$3);

		memset(&$$, 0, sizeof $$);
		$$.agg = expr;
	}
	;

select_calc_expr:
	MINUS select_expr {
		memset(&$$, 0, sizeof $$);
		$$.calc = select_calc_expr_create(NULL, CALC_MINUS, &$2);
	}
	| select_expr ADD select_expr {
		memset(&$$, 0, sizeof $$);
		$$.calc = select_calc_expr_create(&$1, CALC_ADD, &$3);
	}
	| select_expr MINUS select_expr {
		memset(&$$, 0, sizeof $$);
		$$.calc = select_calc_expr_create(&$1, CALC_MINUS, &$3);
	}
	| select_expr STAR select_expr {
		// 9 * 9
		memset(&$$, 0, sizeof $$);
		$$.calc = select_calc_expr_create(&$1, CALC_MULTI, &$3);
	}
	| select_expr DIV select_expr {
		memset(&$$, 0, sizeof $$);
		$$.calc = select_calc_expr_create(&$1, CALC_DIV, &$3);
	}
	| LBRACE select_calc_expr RBRACE {
		$$ = $2;
	    select_calc_expr_add_brace($$.calc);
	}
	;

select_attr:
    STAR {
			RelAttr *attr = malloc(sizeof(RelAttr));
			relation_attr_init(attr, NULL, "*");
			$$ = attr;
      }
    | ID {
			RelAttr *attr = malloc(sizeof(RelAttr));
			relation_attr_init(attr, NULL, $1);
			$$ = attr;
      }
    | ID DOT ID {
			RelAttr *attr = malloc(sizeof(RelAttr));
			relation_attr_init(attr, $1, $3);
			$$ = attr;
  	  }
    | ID DOT STAR {
			RelAttr *attr = malloc(sizeof(RelAttr));
			relation_attr_init(attr, $1, "*");
			$$ = attr;
  	  }
  	;

rel_list:
    /* empty */ { $$ = list_create(sizeof(char *), MAX_NUM); }
    | COMMA ID rel_list {	
		$$ = $3;
		list_append($3, &$2);
	}
    ;
join_list:
	/* empty */ {
		$$.rels = list_create(sizeof(char *), MAX_NUM);
		$$.conds = list_create(sizeof(Condition), MAX_NUM);
	}
	| INNER JOIN ID ON condition_list join_list {
		$$ = $6;
		list_append($$.rels, &$3);
		list_append_list($$.conds, $5);
		list_free($5);
	}
	;
where:
    /* empty */ { $$ = list_create(sizeof(Condition), MAX_NUM); } 
    | WHERE condition_list {	
		$$ = $2;
	}
    ;
condition_list:
	condition {
		$$ = list_create(sizeof(Condition), MAX_NUM);
		list_append($$, $1);
		free($1);
	}
    | condition AND condition_list {
		$$ = $3;
		list_append($$, $1);
		free($1);
	}
    ;
condition:
	condition_expr comp_op condition_expr {
		$$ = (Condition *) malloc(sizeof(Condition));
		condition_init($$, $2, &$1, &$3);
	}
	| condition_expr IS NULL_VALUE {
		$$ = (Condition *) malloc(sizeof(Condition));
		Value null;
		value_init_null(&null);
		ConditionExpr null_expr;
		null_expr.type = COND_EXPR_VALUE;
		null_expr.value.value = null;

		condition_init($$, IS_NULL, &$1, &null_expr);
	}
	| condition_expr IS NOT NULL_VALUE {
		$$ = (Condition *) malloc(sizeof(Condition));
		Value null;
		value_init_null(&null);
		ConditionExpr null_expr;
		null_expr.type = COND_EXPR_VALUE;
		null_expr.value.value = null;

		condition_init($$, IS_NOT_NULL, &$1, &null_expr);
	}
	;
condition_expr:
	ID {
		$$.type = COND_EXPR_ATTR;
		relation_attr_init(&$$.value.attr, NULL, $1);
	}
	| ID DOT ID {
		$$.type = COND_EXPR_ATTR;
		relation_attr_init(&$$.value.attr, $1, $3);
	}
	| pos_value {
		$$.type = COND_EXPR_VALUE;
		$$.value.value = $1;
	}
	| LBRACE select_statement RBRACE {
		$$.type = COND_EXPR_SELECT;
		$$.value.selects = $2;
	}
	| condition_calc_expr {
		$$ = $1;
	}
	;
condition_calc_expr:
	MINUS condition_expr {
		ConditionExpr expr;
		expr.type = COND_EXPR_VALUE;
		value_init_integer(&expr.value.value, 0);

		$$.type = COND_EXPR_CALC;
		$$.value.calc = condition_calc_create(&expr, CALC_MINUS, &$2);
	}
	| condition_expr ADD condition_expr {
		$$.type = COND_EXPR_CALC;
		$$.value.calc = condition_calc_create(&$1, CALC_ADD, &$3);
	}
	| condition_expr MINUS condition_expr {
		$$.type = COND_EXPR_CALC;
		$$.value.calc = condition_calc_create(&$1, CALC_MINUS, &$3);
	}
	| condition_expr STAR condition_expr {
		$$.type = COND_EXPR_CALC;
		$$.value.calc = condition_calc_create(&$1, CALC_MULTI, &$3);
	}
	| condition_expr DIV condition_expr {
		$$.type = COND_EXPR_CALC;
		$$.value.calc = condition_calc_create(&$1, CALC_DIV, &$3);
	}
	| LBRACE condition_calc_expr RBRACE {
		$$ = $2;
	}
	;


comp_op:
  	  EQ { $$ = EQUAL_TO; }
    | LT { $$ = LESS_THAN; }
    | GT { $$ = GREAT_THAN; }
    | LE { $$ = LESS_EQUAL; }
    | GE { $$ = GREAT_EQUAL; }
    | NE { $$ = NOT_EQUAL; }
	| IN { $$ = IN_SET; }
	| NOT IN { $$ = NOT_IN_SET; }
    ;

order_by:
	/* empty */ { $$ = NULL; }
	| ORDER BY order_by_attr_list {
		$$ = $3;
	}
	;

order_by_attr_list:
    order_attr order_dir {
		$$ = list_create(sizeof(OrderBy), MAX_NUM);
		OrderBy ob = {$2, $1};
		list_append($$, &ob);
    }
    | order_attr order_dir COMMA order_by_attr_list {
		$$ = $4;
		OrderBy ob = {$2, $1};
		list_append($$, &ob);
    }
	;

order_attr:
    ID {
			RelAttr *attr = malloc(sizeof(RelAttr));
			relation_attr_init(attr, NULL, $1);
			$$ = attr;
      }
    | ID DOT ID {
			RelAttr *attr = malloc(sizeof(RelAttr));
			relation_attr_init(attr, $1, $3);
			$$ = attr;
  	   }
	;

order_dir:
	/* empty */
	{ $$ = DIR_ASC; }
	| ASC { $$ = DIR_ASC; }
	| DESC { $$ = DIR_DESC; }
	;

group_by:
	/* empty */ { $$ = NULL; }
	| GROUP BY group_by_attr_list {
		$$ = $3;
	}
	;

group_by_attr_list:
    group_attr {
		$$ = list_create(sizeof(RelAttr), MAX_NUM);
		list_append($$, $1);
    }
    | group_attr COMMA group_by_attr_list {
		$$ = $3;
		list_append($$, $1);
    }
	;

group_attr:
    ID {
		RelAttr *attr = malloc(sizeof(RelAttr));
		relation_attr_init(attr, NULL, $1);
		$$ = attr;
	}
    | ID DOT ID {
		RelAttr *attr = malloc(sizeof(RelAttr));
		relation_attr_init(attr, $1, $3);
		$$ = attr;
	}
	;


load_data:
		LOAD DATA INFILE SSS INTO TABLE ID SEMICOLON
		{
		  CONTEXT->ssql->flag = SCF_LOAD_DATA;
			load_data_init(&CONTEXT->ssql->sstr.load_data, $7, $4);
		}
		;
%%
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
