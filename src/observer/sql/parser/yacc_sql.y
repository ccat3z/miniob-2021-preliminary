
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
  size_t value_length;
  Value values[MAX_NUM];
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
  context->value_length = 0;
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
        INDEX
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
        HELP
        EXIT
        DOT //QUOTE
        INTO
        VALUES
        FROM
		INNER
		JOIN
        WHERE
        AND
		ORDER
		BY
		ASC
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

%union {
  Condition *condition;
  Value *value;
  char *string;
  int number;
  float floats;
  char *position;
  OrderDir orderdir;
  RelAttr *rel_attr;
  List *list;
  struct{List *rels; List *conds;} join_list;
  CompOp comp_op;
  SelectExpr select_expr;
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
%type <value> value;
%type <number> number;
%type <orderdir> order_dir;
%type <rel_attr> order_attr;
%type <rel_attr> select_attr;
%type <list> where;
%type <join_list> join_list;
%type <list> condition_list;
%type <list> rel_list;
%type <comp_op> comp_op;
%type <select_expr> select_expr;
%type <list> select_expr_list;

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
    CREATE INDEX ID ON ID LBRACE ID RBRACE SEMICOLON 
		{
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, $3, $5, $7);
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
			// CONTEXT->ssql->sstr.create_table.attribute_count = CONTEXT->value_length;
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, $3);
			//临时变量清零	
			CONTEXT->value_length = 0;
		}
    ;
attr_def_list:
    /* empty */
    | COMMA attr_def attr_def_list {    }
    ;
    
attr_def:
    ID_get type LBRACE number RBRACE 
		{
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, $2, $4);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name =(char*)malloc(sizeof(char));
			// strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type = $2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length = $4;
			CONTEXT->value_length++;
		}
    |ID_get type
		{
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, $2, 4);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name=(char*)malloc(sizeof(char));
			// strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type=$2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length=4; // default attribute length
			CONTEXT->value_length++;
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
       ;
ID_get:
	ID 
	{
		char *temp=$1; 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
	;

	
insert:				/*insert   语句的语法解析树*/
    INSERT INTO ID VALUES value_lists SEMICOLON 
		{
			// CONTEXT->values[CONTEXT->value_length++] = *$6;

			CONTEXT->ssql->flag=SCF_INSERT;//"insert";
			// CONTEXT->ssql->sstr.insertion.relation_name = $3;
			// CONTEXT->ssql->sstr.insertion.value_num = CONTEXT->value_length;
			// for(i = 0; i < CONTEXT->value_length; i++){
			// 	CONTEXT->ssql->sstr.insertion.values[i] = CONTEXT->values[i];
      // }
			inserts_init(&CONTEXT->ssql->sstr.insertion, $3, CONTEXT->values, CONTEXT->value_length);

      //临时变量清零
      CONTEXT->value_length=0;
    }

value_lists:
	LBRACE value_list RBRACE {
		CONTEXT->ssql->sstr.insertion.tuple_num++;
	}
	| LBRACE value_list RBRACE COMMA value_lists {
		CONTEXT->ssql->sstr.insertion.tuple_num++;
	}
	;
value_list:
	value
    | value COMMA value_list  { 
  		// CONTEXT->values[CONTEXT->value_length++] = *$2;
	  }
    ;
value:
    NUMBER{	
  		value_init_integer(&CONTEXT->values[CONTEXT->value_length++], $1);
		}
    |FLOAT{
  		value_init_float(&CONTEXT->values[CONTEXT->value_length++], $1);
		}
    |SSS {
			$1 = substr($1,1,strlen($1)-2);
  		value_init_string(&CONTEXT->values[CONTEXT->value_length++], $1);
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
			Value *value = &CONTEXT->values[0];
			updates_init(&CONTEXT->ssql->sstr.update, $2, $4, value, 
					(Condition *) $7->values, $7->len);
			list_free($7);
		}
    ;
select:				/*  select 语句的语法解析树*/
    SELECT select_expr_list FROM ID rel_list join_list where order_by SEMICOLON
		{
			selects_append_exprs(&CONTEXT->ssql->sstr.selection, (SelectExpr *) $2->values, $2->len);
			list_free($2);

			selects_append_relations(&CONTEXT->ssql->sstr.selection, (const char **) $6.rels->values, $6.rels->len);
			selects_append_relations(&CONTEXT->ssql->sstr.selection, (const char **) $5->values, $5->len);
			selects_append_relation(&CONTEXT->ssql->sstr.selection, $4);

			selects_append_conditions(&CONTEXT->ssql->sstr.selection, (Condition *) $6.conds->values, $6.conds->len);
			list_free($6.rels);
			list_free($6.conds);
			selects_append_conditions(&CONTEXT->ssql->sstr.selection, (Condition *) $7->values, $7->len);
			list_free($7);

			CONTEXT->ssql->flag=SCF_SELECT;//"select";

			//临时变量清零
			CONTEXT->value_length = 0;
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
		$$.attribute = $1;
		$$.agg = NULL;
	}
	| ID LBRACE select_attr RBRACE {
		AggExpr *expr = (AggExpr *) malloc(sizeof(AggExpr));
		agg_expr_init_attr(expr, $1, $3);
		$$.agg = expr;
		$$.attribute = NULL;
	}
	| ID LBRACE value RBRACE {
		AggExpr *expr = (AggExpr *) malloc(sizeof(AggExpr));
		agg_expr_init_value(expr, $1, &CONTEXT->values[CONTEXT->value_length - 1]);
		$$.agg = expr;
		$$.attribute = NULL;
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
    ID comp_op value 
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, $1);

			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			$$ = (Condition *) malloc(sizeof(Condition));
			condition_init($$, $2, 1, &left_attr, NULL, 0, NULL, right_value);
		}
		|value comp_op value 
		{
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 2];
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			$$ = (Condition *) malloc(sizeof(Condition));
			condition_init($$, $2, 0, NULL, left_value, 0, NULL, right_value);
		}
		|ID comp_op ID 
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, $1);
			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, $3);

			$$ = (Condition *) malloc(sizeof(Condition));
			condition_init($$, $2, 1, &left_attr, NULL, 1, &right_attr, NULL);
		}
    |value comp_op ID
		{
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];
			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, $3);

			$$ = (Condition *) malloc(sizeof(Condition));
			condition_init($$, $2, 0, NULL, left_value, 1, &right_attr, NULL);
		}
    |ID DOT ID comp_op value
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, $1, $3);
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			$$ = (Condition *) malloc(sizeof(Condition));
			condition_init($$, $4, 1, &left_attr, NULL, 0, NULL, right_value);
    }
    |value comp_op ID DOT ID
		{
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];

			RelAttr right_attr;
			relation_attr_init(&right_attr, $3, $5);

			$$ = (Condition *) malloc(sizeof(Condition));
			condition_init($$, $2, 0, NULL, left_value, 1, &right_attr, NULL);
    }
    |ID DOT ID comp_op ID DOT ID
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, $1, $3);
			RelAttr right_attr;
			relation_attr_init(&right_attr, $5, $7);

			$$ = (Condition *) malloc(sizeof(Condition));
			condition_init($$, $4, 1, &left_attr, NULL, 1, &right_attr, NULL);
    }
    ;

comp_op:
  	  EQ { $$ = EQUAL_TO; }
    | LT { $$ = LESS_THAN; }
    | GT { $$ = GREAT_THAN; }
    | LE { $$ = LESS_EQUAL; }
    | GE { $$ = GREAT_EQUAL; }
    | NE { $$ = NOT_EQUAL; }
    ;

order_by:
	/* empty */
	| ORDER BY order_by_attr_list {}
	;

order_by_attr_list:
    order_attr order_dir {
			selects_append_order_attr(&CONTEXT->ssql->sstr.selection, $1, $2);
      }
    | order_attr order_dir COMMA order_by_attr_list {
			selects_append_order_attr(&CONTEXT->ssql->sstr.selection, $1, $2);
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
