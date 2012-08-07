/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is include which follows the "include" declaration
** in the input file. */
#include <stdio.h>
#line 35 "parser.lemon"


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_phalcon.h"

#include "parser.h"
#include "scanner.h"
#include "lang.h"

#include "kernel/main.h";
#include "kernel/memory.h";

static zval *phql_ret_literal_zval(int type, phql_parser_token *T)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);
	add_assoc_long(ret, "type", type);
	if (T) {
		add_assoc_stringl(ret, "value", T->token, T->token_len, 1);
		efree(T->token);
		efree(T);
	}

	return ret;
}

static zval *phql_ret_placeholder_zval(int type, phql_parser_token *T)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);
	add_assoc_long(ret, "type", type);
	add_assoc_stringl(ret, "value", T->token, T->token_len, 1);
	efree(T->token);
	efree(T);

	return ret;
}

static zval *phql_ret_qualified_name(phql_parser_token *A, phql_parser_token *B)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);
	add_assoc_stringl(ret, "type", "qualified", strlen("qualified"), 1);
	if (B != NULL) {
		add_assoc_stringl(ret, "domain", A->token, A->token_len, 1);
		add_assoc_stringl(ret, "name", B->token, B->token_len, 1);
		efree(B->token);
		efree(B);
	} else {
		add_assoc_stringl(ret, "name", A->token, A->token_len, 1);
	}
	efree(A->token);
	efree(A);

	return ret;
}

static zval *phql_ret_select_statement(zval *S, zval *W, zval *O, zval *G, zval *H, zval *L)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);

	add_assoc_long(ret, "type", PHQL_T_SELECT);
	add_assoc_zval(ret, "select", S);

	if (W != NULL) {
		add_assoc_zval(ret, "where", W);
	}
	if (O != NULL) {
		add_assoc_zval(ret, "orderBy", O);
	}
	if (G != NULL) {
		add_assoc_zval(ret, "groupBy", G);
	}
	if (H != NULL) {
		add_assoc_zval(ret, "having", H);
	}
	if (L != NULL) {
		add_assoc_zval(ret, "limit", L);
	}

	return ret;
}

static zval *phql_ret_select_clause(zval *columns, zval *tables, zval *join_list)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);
	add_assoc_zval(ret, "columns", columns);
	add_assoc_zval(ret, "tables", tables);
	if (join_list) {
		add_assoc_zval(ret, "joins", join_list);
	}

	return ret;
}

static zval *phql_ret_insert_statement(zval *Q, zval *F, zval *V)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);

	add_assoc_long(ret, "type", PHQL_T_INSERT);
	add_assoc_zval(ret, "qualifiedName", Q);
	if (F != NULL) {
		add_assoc_zval(ret, "fields", F);
	}
	add_assoc_zval(ret, "values", V);

	return ret;
}

static zval *phql_ret_update_statement(zval *U, zval *W, zval *L)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);

	add_assoc_long(ret, "type", PHQL_T_UPDATE);
	add_assoc_zval(ret, "update", U);
	if (W != NULL) {
		add_assoc_zval(ret, "where", W);
	}
	if (L != NULL) {
		add_assoc_zval(ret, "limit", L);
	}

	return ret;
}

static zval *phql_ret_update_clause(zval *tables, zval *values)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);
	add_assoc_zval(ret, "tables", tables);
	add_assoc_zval(ret, "values", values);

	return ret;
}

static zval *phql_ret_update_item(zval *column, zval *expr)
{

	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);
	add_assoc_zval(ret, "column", column);
	add_assoc_zval(ret, "expr", expr);

	return ret;
}

static zval *phql_ret_delete_statement(zval *D, zval *W, zval *L)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);

	add_assoc_long(ret, "type", PHQL_T_DELETE);
	add_assoc_zval(ret, "delete", D);
	if (W != NULL) {
		add_assoc_zval(ret, "where", W);
	}
	if (L != NULL) {
		add_assoc_zval(ret, "limit", L);
	}

	return ret;
}

static zval *phql_ret_delete_clause(zval *tables)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);
	add_assoc_zval(ret, "tables", tables);

	return ret;
}

static zval *phql_ret_zval_list(zval *list_left, zval *right_list)
{

	zval *ret;
	HashPosition pos;
	HashTable *list;

	MAKE_STD_ZVAL(ret);
	array_init(ret);

	list = Z_ARRVAL_P(list_left);
	if (zend_hash_index_exists(list, 0)) {
		zend_hash_internal_pointer_reset_ex(list, &pos);
		for (;; zend_hash_move_forward_ex(list, &pos)) {

			zval ** item;

			if (zend_hash_get_current_data_ex(list, (void**)&item, &pos) == FAILURE) {
				break;
			}

			Z_ADDREF_PP(item);
			add_next_index_zval(ret, *item);

		}
		zval_ptr_dtor(&list_left);
	} else {
		add_next_index_zval(ret, list_left);
	}

	add_next_index_zval(ret, right_list);

	return ret;
}

static zval *phql_ret_column_item(char *type, int type_len, zval *column, phql_parser_token *identifier_column, phql_parser_token *alias)
{

	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);
	add_assoc_stringl(ret, "type", type, type_len, 1);
	if (column) {
		add_assoc_zval(ret, "column", column);
	}
	if (identifier_column) {
		add_assoc_stringl(ret, "column", identifier_column->token, identifier_column->token_len, 1);
		efree(identifier_column->token);
		efree(identifier_column);
	}
	if (alias) {
		add_assoc_stringl(ret, "alias", alias->token, alias->token_len, 1);
		efree(alias->token);
		efree(alias);
	}

	return ret;
}

static zval *phql_ret_assoc_name(zval *qualified_name, phql_parser_token *alias)
{

	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);
	add_assoc_zval(ret, "qualifiedName", qualified_name);
	if (alias) {
		add_assoc_stringl(ret, "alias", alias->token, alias->token_len, 1);
		efree(alias->token);
		efree(alias);
	}

	return ret;

}

static zval *phql_ret_join_item(zval *join, phql_parser_token *alias)
{

	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);
	add_assoc_zval(ret, "join", join);
	if (alias) {
		add_assoc_stringl(ret, "alias", alias->token, alias->token_len, 1);
		efree(alias->token);
		efree(alias);
	}

	return ret;

}

static zval *phql_ret_join_clause(int type, zval *qualified_name, zval *expr)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);
	add_assoc_long(ret, "type", type);
	add_assoc_zval(ret, "qualifiedName", qualified_name);
	if (expr) {
		add_assoc_zval(ret, "expr", expr);
	}

	return ret;
}

static zval *phql_ret_expr(int type, zval *left, zval *right)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);
	add_assoc_long(ret, "type", type);
	if (left) {
		add_assoc_zval(ret, "left", left);
	}
	if (right) {
		add_assoc_zval(ret, "right", right);
	}

	return ret;
}

static zval *phql_ret_func_call(phql_parser_token *name, zval *arguments)
{

	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);
	add_assoc_long(ret, "type", PHQL_T_FCALL);
	add_assoc_stringl(ret, "name", name->token, name->token_len, 1);
	efree(name->token);
	efree(name);

	add_assoc_zval(ret, "arguments", arguments);

	return ret;
}


#line 354 "parser.c"
/* Next is all token values, in a form suitable for use by makeheaders.
** This section will be null unless lemon is run with the -m switch.
*/
/*
** These constants (all generated automatically by the parser generator)
** specify the various kinds of tokens (terminals) that the parser
** understands.
**
** Each symbol here is a terminal symbol in the grammar.
*/
/* Make sure the INTERFACE macro is defined.
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/* The next thing included is series of defines which control
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 terminals
**                       and nonterminals.  "int" is used otherwise.
**    YYNOCODE           is a number of type YYCODETYPE which corresponds
**                       to no legal terminal or nonterminal number.  This
**                       number is used to fill in empty slots of the hash
**                       table.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       have fall-back values which should be used if the
**                       original value of the token will not parse.
**    YYACTIONTYPE       is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 rules and
**                       states combined.  "int" is used otherwise.
**    phql_TOKENTYPE     is the data type used for minor tokens given
**                       directly to the parser from the tokenizer.
**    YYMINORTYPE        is the data type used for all minor tokens.
**                       This is typically a union of many types, one of
**                       which is phql_TOKENTYPE.  The entry in the union
**                       for base tokens is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.
**    phql_ARG_SDECL     A static variable declaration for the %extra_argument
**    phql_ARG_PDECL     A parameter declaration for the %extra_argument
**    phql_ARG_STORE     Code to store %extra_argument into yypParser
**    phql_ARG_FETCH     Code to extract %extra_argument from yypParser
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
*/
#define YYCODETYPE unsigned char
#define YYNOCODE 86
#define YYACTIONTYPE unsigned short int
#define phql_TOKENTYPE phql_parser_token*
typedef union {
  phql_TOKENTYPE yy0;
  zval* yy58;
  int yy171;
} YYMINORTYPE;
#define YYSTACKDEPTH 100
#define phql_ARG_SDECL phql_parser_status *status;
#define phql_ARG_PDECL ,phql_parser_status *status
#define phql_ARG_FETCH phql_parser_status *status = yypParser->status
#define phql_ARG_STORE yypParser->status = status
#define YYNSTATE 201
#define YYNRULE 122
#define YYERRORSYMBOL 46
#define YYERRSYMDT yy171
#define YY_NO_ACTION      (YYNSTATE+YYNRULE+2)
#define YY_ACCEPT_ACTION  (YYNSTATE+YYNRULE+1)
#define YY_ERROR_ACTION   (YYNSTATE+YYNRULE)

/* Next are that tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N < YYNSTATE                  Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   YYNSTATE <= N < YYNSTATE+YYNRULE   Reduce by rule N-YYNSTATE.
**
**   N == YYNSTATE+YYNRULE              A syntax error has occurred.
**
**   N == YYNSTATE+YYNRULE+1            The parser accepts its input.
**
**   N == YYNSTATE+YYNRULE+2            No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as
**
**      yy_action[ yy_shift_ofst[S] + X ]
**
** If the index value yy_shift_ofst[S]+X is out of range or if the value
** yy_lookahead[yy_shift_ofst[S]+X] is not equal to X or if yy_shift_ofst[S]
** is equal to YY_SHIFT_USE_DFLT, it means that the action is not in the table
** and that yy_default[S] should be used instead.
**
** The formula above is for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array and YY_REDUCE_USE_DFLT is used in place of
** YY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
*/
static YYACTIONTYPE yy_action[] = {
 /*     0 */    54,   57,   60,   34,   32,   38,   36,   52,   40,   42,
 /*    10 */    44,   46,   48,   50,    7,   97,   99,  155,  106,  153,
 /*    20 */   101,   54,   57,   60,   34,   32,   38,   36,   52,   40,
 /*    30 */    42,   44,   46,   48,   50,   54,   57,   60,   34,   32,
 /*    40 */    38,   36,   52,   40,   42,   44,   46,   48,   50,   75,
 /*    50 */    79,   44,   46,   48,   50,   74,   38,   36,   52,   40,
 /*    60 */    42,   44,   46,   48,   50,   27,   68,   13,   29,  324,
 /*    70 */     1,    2,    3,    4,    5,    6,  166,   77,   72,  112,
 /*    80 */    75,   80,  162,  145,  167,   81,   74,  140,   23,   84,
 /*    90 */    82,   83,   85,   86,  200,  179,   67,   68,  140,  194,
 /*   100 */    52,   40,   42,   44,   46,   48,   50,   22,   77,   40,
 /*   110 */    42,   44,   46,   48,   50,  119,   81,  123,  128,  133,
 /*   120 */    84,   82,   83,   85,   86,   75,  110,  192,  166,  156,
 /*   130 */   152,  148,   80,   80,  173,   80,  167,  195,   59,   94,
 /*   140 */   191,  197,  149,   80,  160,  168,  103,   67,   67,  104,
 /*   150 */    67,   91,   96,   77,    9,   75,  138,   70,   67,   73,
 /*   160 */   119,   81,  123,  128,  133,   84,   82,   83,   85,   86,
 /*   170 */   206,  109,   68,   59,  223,   11,    8,   12,   80,   88,
 /*   180 */   166,   51,  157,   77,  209,   80,   80,  183,  165,  198,
 /*   190 */   207,   81,   64,   67,   73,   84,   82,   83,   85,   86,
 /*   200 */    67,   67,  111,  146,  107,   16,  147,  152,   89,   16,
 /*   210 */    10,   59,   80,   30,   10,  100,   80,  102,  105,   16,
 /*   220 */   143,  127,  141,   30,   10,   16,   80,   67,   89,  113,
 /*   230 */    10,   67,   87,  144,  115,  116,  139,  169,   37,  178,
 /*   240 */   140,   67,   53,   80,   35,   45,   78,   80,   76,   80,
 /*   250 */    80,   80,   56,   80,   41,  122,  137,   80,   67,   80,
 /*   260 */    80,   80,   67,  225,   67,   67,   67,  260,   67,   23,
 /*   270 */   164,  201,   67,   47,   67,   67,   67,  269,   80,  114,
 /*   280 */   115,  116,   39,  163,   21,  108,  211,   80,   95,  151,
 /*   290 */    80,  202,   49,   67,   18,   26,  208,   80,   16,  132,
 /*   300 */    25,  107,   67,   10,   80,   67,  261,   10,   31,   43,
 /*   310 */    33,  107,   67,   80,   80,   80,  189,   10,  213,   67,
 /*   320 */    66,  150,  186,  193,   24,  175,   10,  164,   67,   67,
 /*   330 */    67,   69,   21,   65,   69,  180,   10,  184,  170,  182,
 /*   340 */   174,  140,  270,   20,  210,  217,   10,  189,   94,   62,
 /*   350 */   218,   66,  203,  204,  188,  205,  215,  289,   10,   14,
 /*   360 */   222,   93,   17,   19,   71,   15,   24,   25,  212,   28,
 /*   370 */   216,   55,   58,   63,  214,   90,   92,   98,  220,   16,
 /*   380 */   221,  219,   10,  226,  224,   10,  227,   23,  120,   61,
 /*   390 */    10,  117,  125,  118,  121,  126,  130,  124,  129,  135,
 /*   400 */   252,  131,  253,  134,  159,  142,  154,  136,  263,  177,
 /*   410 */   176,  262,  181,  187,  190,  272,  158,  271,  209,  161,
 /*   420 */   171,  199,  172,  196,  209,  209,  185,
};
static YYCODETYPE yy_lookahead[] = {
 /*     0 */     1,    2,    3,    4,    5,    6,    7,    8,    9,   10,
 /*    10 */    11,   12,   13,   14,   54,   55,   56,   18,   58,   20,
 /*    20 */    58,    1,    2,    3,    4,    5,    6,    7,    8,    9,
 /*    30 */    10,   11,   12,   13,   14,    1,    2,    3,    4,    5,
 /*    40 */     6,    7,    8,    9,   10,   11,   12,   13,   14,    1,
 /*    50 */    30,   11,   12,   13,   14,    7,    6,    7,    8,    9,
 /*    60 */    10,   11,   12,   13,   14,   55,   18,   57,   58,   47,
 /*    70 */    48,   49,   50,   51,   52,   53,   63,   29,   30,   60,
 /*    80 */     1,   68,   69,   64,   71,   37,    7,   68,   18,   41,
 /*    90 */    42,   43,   44,   45,   64,   73,   83,   18,   68,   77,
 /*   100 */     8,    9,   10,   11,   12,   13,   14,   37,   29,    9,
 /*   110 */    10,   11,   12,   13,   14,   21,   37,   23,   24,   25,
 /*   120 */    41,   42,   43,   44,   45,    1,   59,   63,   63,   62,
 /*   130 */    63,    7,   68,   68,   69,   68,   71,   54,   63,   68,
 /*   140 */    76,   58,   18,   68,   28,   29,   55,   83,   83,   58,
 /*   150 */    83,   80,   81,   29,   58,    1,   17,   82,   83,   84,
 /*   160 */    21,   37,   23,   24,   25,   41,   42,   43,   44,   45,
 /*   170 */     0,   15,   18,   63,    0,   37,   55,   56,   68,   58,
 /*   180 */    63,   63,   26,   29,    0,   68,   68,   31,   71,   33,
 /*   190 */     0,   37,   82,   83,   84,   41,   42,   43,   44,   45,
 /*   200 */    83,   83,   16,   17,   34,   35,   62,   63,   38,   35,
 /*   210 */    40,   63,   68,   39,   40,   55,   68,   57,   58,   35,
 /*   220 */    18,   63,   20,   39,   40,   35,   68,   83,   38,   61,
 /*   230 */    40,   83,   84,   65,   66,   67,   64,   70,   63,   72,
 /*   240 */    68,   83,   63,   68,   63,   63,   63,   68,   63,   68,
 /*   250 */    68,   68,   63,   68,   63,   63,   63,   68,   83,   68,
 /*   260 */    68,   68,   83,    0,   83,   83,   83,    0,   83,   18,
 /*   270 */    17,    0,   83,   63,   83,   83,   83,    0,   68,   65,
 /*   280 */    66,   67,   63,   30,   68,   63,    0,   68,   37,    7,
 /*   290 */    68,    0,   63,   83,   78,   79,    0,   68,   35,   63,
 /*   300 */    18,   34,   83,   40,   68,   83,    0,   40,   63,   63,
 /*   310 */    63,   34,   83,   68,   68,   68,   68,   40,    0,   83,
 /*   320 */    17,   19,   74,   75,   19,   17,   40,   17,   83,   83,
 /*   330 */    83,   29,   68,   30,   29,   54,   40,   64,   30,   58,
 /*   340 */    30,   68,    0,   79,    0,    0,   40,   68,   68,    1,
 /*   350 */     0,   17,    0,    0,   75,    0,    0,    0,   40,   55,
 /*   360 */     0,   81,   36,   17,   30,   58,   19,   18,    0,   58,
 /*   370 */     0,    8,   29,   41,    0,   36,   17,   58,    0,   35,
 /*   380 */     0,    0,   40,    0,    0,   40,    0,   18,   68,   41,
 /*   390 */    40,   20,   68,   18,   22,   22,   68,   21,   21,   68,
 /*   400 */     0,   22,    0,   21,   68,   18,   18,   22,    0,   18,
 /*   410 */    72,    0,   58,   17,   11,    0,   27,    0,   85,   29,
 /*   420 */    28,   16,   29,   58,   85,   85,   32,
};
#define YY_SHIFT_USE_DFLT (-2)
static short yy_shift_ofst[] = {
 /*     0 */   156,  271,  291,  352,  353,  355,  170,  190,  296,  356,
 /*    10 */   138,  357,  184,  344,  318,  360,  326,   70,  346,   70,
 /*    20 */    -2,   -2,   -2,  347,  349,   -2,   -2,  286,  368,  370,
 /*    30 */   154,   34,  154,   50,  154,   50,  154,   92,  154,   92,
 /*    40 */   154,   40,  154,   40,  154,   -2,  154,   -2,  154,   -2,
 /*    50 */   154,   -2,  154,  100,  363,  154,   34,  343,   79,   34,
 /*    60 */   348,   -2,  332,   -2,  303,   -2,   79,   -2,  305,   48,
 /*    70 */   334,   -2,   -2,   -2,   -2,  154,   34,  154,   20,   -2,
 /*    80 */    -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2,  374,  339,
 /*    90 */   251,  359,  251,   -2,   -2,   -2,   -2,  345,  378,  174,
 /*   100 */   350,  380,  263,  381,  383,  384,  386,  154,   34,  124,
 /*   110 */   186,  369,  139,   94,   -2,   -2,  371,  375,   -2,  369,
 /*   120 */   372,  154,   34,  376,  369,  373,  154,   34,  377,  369,
 /*   130 */   379,  154,   34,  382,  369,  385,  154,   34,  369,   -2,
 /*   140 */   202,  387,   -2,   -2,   -2,   -2,  124,   -2,   -2,  302,
 /*   150 */   282,   -2,   -1,  388,   -2,   -2,   -2,  389,  369,  116,
 /*   160 */   390,  154,  253,  400,  154,   -2,   34,   -2,  391,  308,
 /*   170 */   392,  393,  154,  310,  402,  391,   -2,   -2,   -2,  267,
 /*   180 */   306,  408,  411,  369,  394,  369,  396,  369,   -2,  403,
 /*   190 */   154,   -2,   34,   -2,  277,  342,  415,  417,  405,  369,
 /*   200 */    -2,
};
#define YY_REDUCE_USE_DFLT (-41)
static short yy_reduce_ofst[] = {
 /*     0 */    22,  -41,  -41,  -41,  -41,  -41,  -40,  121,   96,  -41,
 /*    10 */   -41,  -41,   10,  304,  307,  -41,  -41,  216,  -41,  264,
 /*    20 */   -41,  -41,  -41,  -41,  -41,  -41,  -41,  311,  -41,  -41,
 /*    30 */   245,  -41,  247,  -41,  181,  -41,  175,  -41,  219,  -41,
 /*    40 */   191,  -41,  246,  -41,  182,  -41,  210,  -41,  229,  -41,
 /*    50 */   118,  -41,  179,  -41,  -41,  189,  -41,  -41,  110,  -41,
 /*    60 */   -41,  -41,  -41,  -41,  -41,  -41,  148,  -41,  -41,   75,
 /*    70 */   -41,  -41,  -41,  -41,  -41,  185,  -41,  183,  -41,  -41,
 /*    80 */   -41,  -41,  -41,  -41,  -41,  -41,  -41,  -41,  -41,  -41,
 /*    90 */    71,  -41,  280,  -41,  -41,  -41,  -41,  319,  -41,  160,
 /*   100 */   -38,  -41,   91,  -41,  -41,  -41,  -41,  222,  -41,   67,
 /*   110 */   -41,   19,  168,  214,  -41,  -41,  -41,  -41,  -41,  320,
 /*   120 */   -41,  192,  -41,  -41,  324,  -41,  158,  -41,  -41,  328,
 /*   130 */   -41,  236,  -41,  -41,  331,  -41,  193,  -41,  172,  -41,
 /*   140 */   -41,  -41,  -41,  -41,  -41,  -41,  144,  -41,  -41,  -41,
 /*   150 */   -41,  -41,  -41,  -41,  -41,  -41,  -41,  -41,  336,  -41,
 /*   160 */   -41,   13,  -41,  -41,  117,  -41,  -41,  -41,  167,  -41,
 /*   170 */   -41,  -41,   65,  -41,  -41,  338,  -41,  -41,  -41,  281,
 /*   180 */   354,  -41,  -41,  273,  -41,  248,  -41,  279,  -41,  -41,
 /*   190 */    64,  -41,  -41,  -41,   83,  365,  -41,  -41,  -41,   30,
 /*   200 */   -41,
};
static YYACTIONTYPE yy_default[] = {
 /*     0 */   323,  323,  323,  323,  323,  323,  323,  323,  323,  323,
 /*    10 */   323,  323,  323,  323,  323,  323,  323,  323,  278,  323,
 /*    20 */   279,  281,  282,  322,  323,  321,  280,  323,  323,  323,
 /*    30 */   323,  288,  323,  290,  323,  291,  323,  292,  323,  293,
 /*    40 */   323,  294,  323,  295,  323,  296,  323,  297,  323,  298,
 /*    50 */   323,  299,  323,  300,  323,  323,  301,  323,  323,  309,
 /*    60 */   323,  310,  323,  311,  323,  302,  323,  303,  322,  323,
 /*    70 */   323,  304,  305,  307,  308,  323,  312,  323,  323,  313,
 /*    80 */   314,  315,  316,  317,  318,  319,  320,  306,  323,  323,
 /*    90 */   323,  283,  323,  284,  286,  287,  285,  323,  323,  323,
 /*   100 */   323,  323,  323,  323,  323,  323,  323,  323,  277,  323,
 /*   110 */   323,  323,  228,  229,  239,  241,  242,  323,  243,  323,
 /*   120 */   244,  323,  245,  323,  323,  246,  323,  247,  323,  323,
 /*   130 */   248,  323,  249,  323,  323,  250,  323,  251,  323,  237,
 /*   140 */   276,  323,  274,  275,  240,  238,  323,  230,  232,  322,
 /*   150 */   323,  233,  236,  323,  234,  235,  231,  323,  323,  323,
 /*   160 */   323,  323,  323,  323,  323,  254,  256,  255,  323,  323,
 /*   170 */   323,  323,  323,  323,  323,  323,  257,  259,  258,  323,
 /*   180 */   323,  323,  323,  323,  323,  323,  264,  323,  265,  323,
 /*   190 */   323,  267,  268,  266,  323,  323,  323,  323,  323,  323,
 /*   200 */   273,
};
#define YY_SZ_ACTTAB (sizeof(yy_action)/sizeof(yy_action[0]))

/* The next table maps tokens into fallback tokens.  If a construct
** like the following:
**
**      %fallback ID X Y Z.
**
** appears in the grammer, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
*/
struct yyStackEntry {
  int stateno;       /* The state-number */
  int major;         /* The major token value.  This is the code
                     ** number for the token at this stack level */
  YYMINORTYPE minor; /* The user-supplied minor token value.  This
                     ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  int yyidx;                    /* Index of top element in stack */
  int yyerrcnt;                 /* Shifts left before out of the error */
  phql_ARG_SDECL                /* A place to hold %extra_argument */
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/*
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void phql_Trace(FILE *TraceFILE, char *zTracePrompt){
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if( yyTraceFILE==0 ) yyTracePrompt = 0;
  else if( yyTracePrompt==0 ) yyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *yyTokenName[] = {
  "$",             "NOT",           "IN",            "IS",
  "PLUS",          "MINUS",         "DIVIDE",        "TIMES",
  "LIKE",          "AND",           "OR",            "EQUALS",
  "NOTEQUALS",     "LESS",          "GREATER",       "SELECT",
  "FROM",          "COMMA",         "IDENTIFIER",    "DOT",
  "AS",            "JOIN",          "ON",            "INNER",
  "LEFT",          "RIGHT",         "INSERT",        "INTO",
  "VALUES",        "BRACKEPHQL_T_OPEN",  "BRACKEPHQL_T_CLOSE",  "UPDATE",
  "SET",           "DELETE",        "WHERE",         "ORDER",
  "BY",            "INTEGER",       "GROUP",         "HAVING",
  "LIMIT",         "NULL",          "STRING",        "DOUBLE",
  "NPLACEHOLDER",  "SPLACEHOLDER",  "error",         "program",
  "query_language",  "select_statement",  "insert_statement",  "update_statement",
  "delete_statement",  "select_clause",  "where_clause",  "order_clause",
  "group_clause",  "having_clause",  "limit_clause",  "column_list",
  "associated_name_list",  "join_list",     "column_item",   "expr",
  "associated_name",  "join_item",     "join_associated_name",  "join_clause",
  "qualified_name",  "values_list",   "field_list",    "value_item",
  "field_item",    "update_clause",  "update_item_list",  "update_item",
  "new_value",     "delete_clause",  "order_list",    "order_item",
  "group_list",    "group_item",    "argument_list",  "function_call",
  "argument_item",
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *yyRuleName[] = {
 /*   0 */ "program ::= query_language",
 /*   1 */ "query_language ::= select_statement",
 /*   2 */ "query_language ::= insert_statement",
 /*   3 */ "query_language ::= update_statement",
 /*   4 */ "query_language ::= delete_statement",
 /*   5 */ "select_statement ::= select_clause",
 /*   6 */ "select_statement ::= select_clause where_clause",
 /*   7 */ "select_statement ::= select_clause where_clause order_clause",
 /*   8 */ "select_statement ::= select_clause where_clause group_clause",
 /*   9 */ "select_statement ::= select_clause where_clause group_clause having_clause",
 /*  10 */ "select_statement ::= select_clause where_clause group_clause order_clause",
 /*  11 */ "select_statement ::= select_clause where_clause group_clause order_clause limit_clause",
 /*  12 */ "select_statement ::= select_clause where_clause group_clause having_clause order_clause",
 /*  13 */ "select_statement ::= select_clause where_clause limit_clause",
 /*  14 */ "select_statement ::= select_clause where_clause order_clause limit_clause",
 /*  15 */ "select_statement ::= select_clause where_clause group_clause limit_clause",
 /*  16 */ "select_statement ::= select_clause order_clause",
 /*  17 */ "select_statement ::= select_clause group_clause order_clause",
 /*  18 */ "select_statement ::= select_clause group_clause having_clause order_clause",
 /*  19 */ "select_statement ::= select_clause order_clause limit_clause",
 /*  20 */ "select_statement ::= select_clause group_clause order_clause limit_clause",
 /*  21 */ "select_statement ::= select_clause where_clause group_clause having_clause order_clause limit_clause",
 /*  22 */ "select_statement ::= select_clause group_clause",
 /*  23 */ "select_statement ::= select_clause group_clause limit_clause",
 /*  24 */ "select_statement ::= select_clause group_clause having_clause",
 /*  25 */ "select_statement ::= select_clause group_clause having_clause limit_clause",
 /*  26 */ "select_statement ::= select_clause limit_clause",
 /*  27 */ "select_clause ::= SELECT column_list FROM associated_name_list",
 /*  28 */ "select_clause ::= SELECT column_list FROM associated_name_list join_list",
 /*  29 */ "column_list ::= column_list COMMA column_item",
 /*  30 */ "column_list ::= column_item",
 /*  31 */ "column_item ::= TIMES",
 /*  32 */ "column_item ::= IDENTIFIER DOT TIMES",
 /*  33 */ "column_item ::= expr AS IDENTIFIER",
 /*  34 */ "column_item ::= expr IDENTIFIER",
 /*  35 */ "column_item ::= expr",
 /*  36 */ "associated_name_list ::= associated_name_list COMMA associated_name",
 /*  37 */ "associated_name_list ::= associated_name",
 /*  38 */ "join_list ::= join_list join_item",
 /*  39 */ "join_list ::= join_item",
 /*  40 */ "join_item ::= join_associated_name",
 /*  41 */ "join_associated_name ::= join_clause",
 /*  42 */ "join_associated_name ::= join_clause AS IDENTIFIER",
 /*  43 */ "join_clause ::= JOIN qualified_name",
 /*  44 */ "join_clause ::= JOIN qualified_name ON expr",
 /*  45 */ "join_clause ::= INNER JOIN qualified_name",
 /*  46 */ "join_clause ::= INNER JOIN qualified_name ON expr",
 /*  47 */ "join_clause ::= LEFT JOIN qualified_name",
 /*  48 */ "join_clause ::= LEFT JOIN qualified_name ON expr",
 /*  49 */ "join_clause ::= RIGHT JOIN qualified_name",
 /*  50 */ "join_clause ::= RIGHT JOIN qualified_name ON expr",
 /*  51 */ "insert_statement ::= INSERT INTO qualified_name VALUES BRACKEPHQL_T_OPEN values_list BRACKEPHQL_T_CLOSE",
 /*  52 */ "insert_statement ::= INSERT INTO qualified_name BRACKEPHQL_T_OPEN field_list BRACKEPHQL_T_CLOSE VALUES BRACKEPHQL_T_OPEN values_list BRACKEPHQL_T_CLOSE",
 /*  53 */ "values_list ::= values_list COMMA value_item",
 /*  54 */ "values_list ::= value_item",
 /*  55 */ "value_item ::= expr",
 /*  56 */ "field_list ::= field_list COMMA field_item",
 /*  57 */ "field_list ::= field_item",
 /*  58 */ "field_item ::= IDENTIFIER",
 /*  59 */ "update_statement ::= update_clause",
 /*  60 */ "update_statement ::= update_clause where_clause",
 /*  61 */ "update_statement ::= update_clause limit_clause",
 /*  62 */ "update_statement ::= update_clause where_clause limit_clause",
 /*  63 */ "update_clause ::= UPDATE associated_name SET update_item_list",
 /*  64 */ "update_item_list ::= update_item_list COMMA update_item",
 /*  65 */ "update_item_list ::= update_item",
 /*  66 */ "update_item ::= qualified_name EQUALS new_value",
 /*  67 */ "new_value ::= expr",
 /*  68 */ "delete_statement ::= delete_clause",
 /*  69 */ "delete_statement ::= delete_clause where_clause",
 /*  70 */ "delete_statement ::= delete_clause limit_clause",
 /*  71 */ "delete_statement ::= delete_clause where_clause limit_clause",
 /*  72 */ "delete_clause ::= DELETE FROM associated_name",
 /*  73 */ "associated_name ::= qualified_name AS IDENTIFIER",
 /*  74 */ "associated_name ::= qualified_name IDENTIFIER",
 /*  75 */ "associated_name ::= qualified_name",
 /*  76 */ "where_clause ::= WHERE expr",
 /*  77 */ "order_clause ::= ORDER BY order_list",
 /*  78 */ "order_list ::= order_list COMMA order_item",
 /*  79 */ "order_list ::= order_item",
 /*  80 */ "order_item ::= qualified_name",
 /*  81 */ "order_item ::= INTEGER",
 /*  82 */ "group_clause ::= GROUP BY group_list",
 /*  83 */ "group_list ::= group_list COMMA group_item",
 /*  84 */ "group_list ::= group_item",
 /*  85 */ "group_item ::= qualified_name",
 /*  86 */ "group_item ::= INTEGER",
 /*  87 */ "having_clause ::= HAVING expr",
 /*  88 */ "limit_clause ::= LIMIT INTEGER",
 /*  89 */ "expr ::= expr MINUS expr",
 /*  90 */ "expr ::= expr PLUS expr",
 /*  91 */ "expr ::= expr TIMES expr",
 /*  92 */ "expr ::= expr DIVIDE expr",
 /*  93 */ "expr ::= expr AND expr",
 /*  94 */ "expr ::= expr OR expr",
 /*  95 */ "expr ::= expr EQUALS expr",
 /*  96 */ "expr ::= expr NOTEQUALS expr",
 /*  97 */ "expr ::= expr LESS expr",
 /*  98 */ "expr ::= expr GREATER expr",
 /*  99 */ "expr ::= expr LIKE expr",
 /* 100 */ "expr ::= expr NOT LIKE expr",
 /* 101 */ "expr ::= expr IN BRACKEPHQL_T_OPEN argument_list BRACKEPHQL_T_CLOSE",
 /* 102 */ "expr ::= function_call",
 /* 103 */ "function_call ::= IDENTIFIER BRACKEPHQL_T_OPEN argument_list BRACKEPHQL_T_CLOSE",
 /* 104 */ "function_call ::= IDENTIFIER BRACKEPHQL_T_OPEN BRACKEPHQL_T_CLOSE",
 /* 105 */ "argument_list ::= argument_list COMMA argument_item",
 /* 106 */ "argument_list ::= argument_item",
 /* 107 */ "argument_item ::= TIMES",
 /* 108 */ "argument_item ::= expr",
 /* 109 */ "expr ::= expr IS NULL",
 /* 110 */ "expr ::= expr IS NOT NULL",
 /* 111 */ "expr ::= NOT expr",
 /* 112 */ "expr ::= BRACKEPHQL_T_OPEN expr BRACKEPHQL_T_CLOSE",
 /* 113 */ "expr ::= qualified_name",
 /* 114 */ "expr ::= INTEGER",
 /* 115 */ "expr ::= STRING",
 /* 116 */ "expr ::= DOUBLE",
 /* 117 */ "expr ::= NULL",
 /* 118 */ "expr ::= NPLACEHOLDER",
 /* 119 */ "expr ::= SPLACEHOLDER",
 /* 120 */ "qualified_name ::= IDENTIFIER DOT IDENTIFIER",
 /* 121 */ "qualified_name ::= IDENTIFIER",
};
#endif /* NDEBUG */

/*
** This function returns the symbolic name associated with a token
** value.
*/
const char *phql_TokenName(int tokenType){
#ifndef NDEBUG
  if( tokenType>0 && tokenType<(sizeof(yyTokenName)/sizeof(yyTokenName[0])) ){
    return yyTokenName[tokenType];
  }else{
    return "Unknown";
  }
#else
  return "";
#endif
}

/*
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to phql_ and phql_Free.
*/
void *phql_Alloc(void *(*mallocProc)(size_t)){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (size_t)sizeof(yyParser) );
  if( pParser ){
    pParser->yyidx = -1;
  }
  return pParser;
}

/* The following function deletes the value associated with a
** symbol.  The symbol can be either a terminal or nonterminal.
** "yymajor" is the symbol code, and "yypminor" is a pointer to
** the value.
*/
static void yy_destructor(YYCODETYPE yymajor, YYMINORTYPE *yypminor){
  switch( yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are not used
    ** inside the C code.
    */
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
**
** Return the major token number for the symbol popped.
*/
static int yy_pop_parser_stack(yyParser *pParser){
  YYCODETYPE yymajor;
  yyStackEntry *yytos = &pParser->yystack[pParser->yyidx];

  if( pParser->yyidx<0 ) return 0;
#ifndef NDEBUG
  if( yyTraceFILE && pParser->yyidx>=0 ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yymajor = yytos->major;
  yy_destructor( yymajor, &yytos->minor);
  pParser->yyidx--;
  return yymajor;
}

/*
** Deallocate and destroy a parser.  Destructors are all called for
** all stack elements before shutting the parser down.
**
** Inputs:
** <ul>
** <li>  A pointer to the parser.  This should be a pointer
**       obtained from phql_Alloc.
** <li>  A pointer to a function used to reclaim memory obtained
**       from malloc.
** </ul>
*/
void phql_Free(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
  yyParser *pParser = (yyParser*)p;
  if( pParser==0 ) return;
  while( pParser->yyidx>=0 ) yy_pop_parser_stack(pParser);
  (*freeProc)((void*)pParser);
}

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_shift_action(
  yyParser *pParser,        /* The parser */
  int iLookAhead            /* The look-ahead token */
){
  int i;
  int stateno = pParser->yystack[pParser->yyidx].stateno;

  /* if( pParser->yyidx<0 ) return YY_NO_ACTION;  */
  i = yy_shift_ofst[stateno];
  if( i==YY_SHIFT_USE_DFLT ){
    return yy_default[stateno];
  }
  if( iLookAhead==YYNOCODE ){
    return YY_NO_ACTION;
  }
  i += iLookAhead;
  if( i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
#ifdef YYFALLBACK
    int iFallback;            /* Fallback token */
    if( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
           && (iFallback = yyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
           yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
      }
#endif
      return yy_find_shift_action(pParser, iFallback);
    }
#endif
    return yy_default[stateno];
  }else{
    return yy_action[i];
  }
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_reduce_action(
  yyParser *pParser,        /* The parser */
  int iLookAhead            /* The look-ahead token */
){
  int i;
  int stateno = pParser->yystack[pParser->yyidx].stateno;

  i = yy_reduce_ofst[stateno];
  if( i==YY_REDUCE_USE_DFLT ){
    return yy_default[stateno];
  }
  if( iLookAhead==YYNOCODE ){
    return YY_NO_ACTION;
  }
  i += iLookAhead;
  if( i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }else{
    return yy_action[i];
  }
}

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  YYMINORTYPE *yypMinor         /* Pointer ot the minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yyidx++;
  if( yypParser->yyidx>=YYSTACKDEPTH ){
     phql_ARG_FETCH;
     yypParser->yyidx--;
#ifndef NDEBUG
     if( yyTraceFILE ){
       fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
     }
#endif
     while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
     /* Here code is inserted which will execute if the parser
     ** stack every overflows */
     phql_ARG_STORE; /* Suppress warning about unused %extra_argument var */
     return;
  }
  yytos = &yypParser->yystack[yypParser->yyidx];
  yytos->stateno = yyNewState;
  yytos->major = yyMajor;
  yytos->minor = *yypMinor;
#ifndef NDEBUG
  if( yyTraceFILE && yypParser->yyidx>0 ){
    int i;
    fprintf(yyTraceFILE,"%sShift %d\n",yyTracePrompt,yyNewState);
    fprintf(yyTraceFILE,"%sStack:",yyTracePrompt);
    for(i=1; i<=yypParser->yyidx; i++)
      fprintf(yyTraceFILE," %s",yyTokenName[yypParser->yystack[i].major]);
    fprintf(yyTraceFILE,"\n");
  }
#endif
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static struct {
  YYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
  unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} yyRuleInfo[] = {
  { 47, 1 },
  { 48, 1 },
  { 48, 1 },
  { 48, 1 },
  { 48, 1 },
  { 49, 1 },
  { 49, 2 },
  { 49, 3 },
  { 49, 3 },
  { 49, 4 },
  { 49, 4 },
  { 49, 5 },
  { 49, 5 },
  { 49, 3 },
  { 49, 4 },
  { 49, 4 },
  { 49, 2 },
  { 49, 3 },
  { 49, 4 },
  { 49, 3 },
  { 49, 4 },
  { 49, 6 },
  { 49, 2 },
  { 49, 3 },
  { 49, 3 },
  { 49, 4 },
  { 49, 2 },
  { 53, 4 },
  { 53, 5 },
  { 59, 3 },
  { 59, 1 },
  { 62, 1 },
  { 62, 3 },
  { 62, 3 },
  { 62, 2 },
  { 62, 1 },
  { 60, 3 },
  { 60, 1 },
  { 61, 2 },
  { 61, 1 },
  { 65, 1 },
  { 66, 1 },
  { 66, 3 },
  { 67, 2 },
  { 67, 4 },
  { 67, 3 },
  { 67, 5 },
  { 67, 3 },
  { 67, 5 },
  { 67, 3 },
  { 67, 5 },
  { 50, 7 },
  { 50, 10 },
  { 69, 3 },
  { 69, 1 },
  { 71, 1 },
  { 70, 3 },
  { 70, 1 },
  { 72, 1 },
  { 51, 1 },
  { 51, 2 },
  { 51, 2 },
  { 51, 3 },
  { 73, 4 },
  { 74, 3 },
  { 74, 1 },
  { 75, 3 },
  { 76, 1 },
  { 52, 1 },
  { 52, 2 },
  { 52, 2 },
  { 52, 3 },
  { 77, 3 },
  { 64, 3 },
  { 64, 2 },
  { 64, 1 },
  { 54, 2 },
  { 55, 3 },
  { 78, 3 },
  { 78, 1 },
  { 79, 1 },
  { 79, 1 },
  { 56, 3 },
  { 80, 3 },
  { 80, 1 },
  { 81, 1 },
  { 81, 1 },
  { 57, 2 },
  { 58, 2 },
  { 63, 3 },
  { 63, 3 },
  { 63, 3 },
  { 63, 3 },
  { 63, 3 },
  { 63, 3 },
  { 63, 3 },
  { 63, 3 },
  { 63, 3 },
  { 63, 3 },
  { 63, 3 },
  { 63, 4 },
  { 63, 5 },
  { 63, 1 },
  { 83, 4 },
  { 83, 3 },
  { 82, 3 },
  { 82, 1 },
  { 84, 1 },
  { 84, 1 },
  { 63, 3 },
  { 63, 4 },
  { 63, 2 },
  { 63, 3 },
  { 63, 1 },
  { 63, 1 },
  { 63, 1 },
  { 63, 1 },
  { 63, 1 },
  { 63, 1 },
  { 63, 1 },
  { 68, 3 },
  { 68, 1 },
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
  yyParser *yypParser,         /* The parser */
  int yyruleno                 /* Number of the rule by which to reduce */
){
  int yygoto;                     /* The next state */
  int yyact;                      /* The next action */
  YYMINORTYPE yygotominor;        /* The LHS of the rule reduced */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  phql_ARG_FETCH;
  yymsp = &yypParser->yystack[yypParser->yyidx];
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno>=0
        && yyruleno<sizeof(yyRuleName)/sizeof(yyRuleName[0]) ){
    fprintf(yyTraceFILE, "%sReduce [%s].\n", yyTracePrompt,
      yyRuleName[yyruleno]);
  }
#endif /* NDEBUG */

  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
      case 0:
#line 386 "parser.lemon"
{
	status->ret = yymsp[0].minor.yy58;
}
#line 1262 "parser.c"
        break;
      case 1:
      case 2:
      case 3:
      case 4:
      case 30:
      case 37:
      case 39:
      case 40:
      case 54:
      case 55:
      case 57:
      case 65:
      case 67:
      case 76:
      case 77:
      case 79:
      case 80:
      case 82:
      case 84:
      case 85:
      case 87:
      case 102:
      case 106:
      case 108:
      case 113:
#line 390 "parser.lemon"
{
	yygotominor.yy58 = yymsp[0].minor.yy58;
}
#line 1293 "parser.c"
        break;
      case 5:
#line 406 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_select_statement(yymsp[0].minor.yy58, NULL, NULL, NULL, NULL, NULL);
}
#line 1300 "parser.c"
        break;
      case 6:
#line 410 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_select_statement(yymsp[-1].minor.yy58, yymsp[0].minor.yy58, NULL, NULL, NULL, NULL);
}
#line 1307 "parser.c"
        break;
      case 7:
#line 414 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_select_statement(yymsp[-2].minor.yy58, yymsp[-1].minor.yy58, yymsp[0].minor.yy58, NULL, NULL, NULL);
}
#line 1314 "parser.c"
        break;
      case 8:
#line 418 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_select_statement(yymsp[-2].minor.yy58, yymsp[-1].minor.yy58, NULL, yymsp[0].minor.yy58, NULL, NULL);
}
#line 1321 "parser.c"
        break;
      case 9:
#line 422 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_select_statement(yymsp[-3].minor.yy58, yymsp[-2].minor.yy58, NULL, yymsp[-1].minor.yy58, yymsp[0].minor.yy58, NULL);
}
#line 1328 "parser.c"
        break;
      case 10:
#line 426 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_select_statement(yymsp[-3].minor.yy58, yymsp[-2].minor.yy58, yymsp[0].minor.yy58, yymsp[-1].minor.yy58, NULL, NULL);
}
#line 1335 "parser.c"
        break;
      case 11:
#line 430 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_select_statement(yymsp[-4].minor.yy58, yymsp[-3].minor.yy58, yymsp[-1].minor.yy58, yymsp[-2].minor.yy58, NULL, yymsp[0].minor.yy58);
}
#line 1342 "parser.c"
        break;
      case 12:
#line 434 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_select_statement(yymsp[-4].minor.yy58, yymsp[-3].minor.yy58, yymsp[0].minor.yy58, yymsp[-2].minor.yy58, yymsp[-1].minor.yy58, NULL);
}
#line 1349 "parser.c"
        break;
      case 13:
#line 438 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_select_statement(yymsp[-2].minor.yy58, yymsp[-1].minor.yy58, NULL, NULL, NULL, yymsp[0].minor.yy58);
}
#line 1356 "parser.c"
        break;
      case 14:
#line 442 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_select_statement(yymsp[-3].minor.yy58, yymsp[-2].minor.yy58, yymsp[-1].minor.yy58, NULL, NULL, yymsp[0].minor.yy58);
}
#line 1363 "parser.c"
        break;
      case 15:
#line 446 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_select_statement(yymsp[-3].minor.yy58, yymsp[-2].minor.yy58, NULL, yymsp[-1].minor.yy58, NULL, yymsp[0].minor.yy58);
}
#line 1370 "parser.c"
        break;
      case 16:
#line 450 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_select_statement(yymsp[-1].minor.yy58, NULL, yymsp[0].minor.yy58, NULL, NULL, NULL);
}
#line 1377 "parser.c"
        break;
      case 17:
#line 454 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_select_statement(yymsp[-2].minor.yy58, NULL, yymsp[0].minor.yy58, yymsp[-1].minor.yy58, NULL, NULL);
}
#line 1384 "parser.c"
        break;
      case 18:
#line 458 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_select_statement(yymsp[-3].minor.yy58, NULL, yymsp[0].minor.yy58, yymsp[-2].minor.yy58, yymsp[-1].minor.yy58, NULL);
}
#line 1391 "parser.c"
        break;
      case 19:
#line 462 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_select_statement(yymsp[-2].minor.yy58, NULL, yymsp[-1].minor.yy58, NULL, NULL, yymsp[0].minor.yy58);
}
#line 1398 "parser.c"
        break;
      case 20:
#line 466 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_select_statement(yymsp[-3].minor.yy58, NULL, yymsp[-1].minor.yy58, yymsp[-2].minor.yy58, NULL, yymsp[0].minor.yy58);
}
#line 1405 "parser.c"
        break;
      case 21:
#line 470 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_select_statement(yymsp[-5].minor.yy58, yymsp[-4].minor.yy58, yymsp[-1].minor.yy58, yymsp[-3].minor.yy58, yymsp[-2].minor.yy58, yymsp[0].minor.yy58);
}
#line 1412 "parser.c"
        break;
      case 22:
#line 474 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_select_statement(yymsp[-1].minor.yy58, NULL, NULL, yymsp[0].minor.yy58, NULL, NULL);
}
#line 1419 "parser.c"
        break;
      case 23:
#line 478 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_select_statement(yymsp[-2].minor.yy58, NULL, NULL, yymsp[-1].minor.yy58, NULL, yymsp[0].minor.yy58);
}
#line 1426 "parser.c"
        break;
      case 24:
#line 482 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_select_statement(yymsp[-2].minor.yy58, NULL, NULL, yymsp[-1].minor.yy58, yymsp[0].minor.yy58, NULL);
}
#line 1433 "parser.c"
        break;
      case 25:
#line 486 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_select_statement(yymsp[-3].minor.yy58, NULL, NULL, yymsp[-2].minor.yy58, yymsp[-1].minor.yy58, yymsp[0].minor.yy58);
}
#line 1440 "parser.c"
        break;
      case 26:
#line 490 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_select_statement(yymsp[-1].minor.yy58, NULL, NULL, NULL, NULL, yymsp[0].minor.yy58);
}
#line 1447 "parser.c"
        break;
      case 27:
#line 494 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_select_clause(yymsp[-2].minor.yy58, yymsp[0].minor.yy58, NULL);
}
#line 1454 "parser.c"
        break;
      case 28:
#line 498 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_select_clause(yymsp[-3].minor.yy58, yymsp[-1].minor.yy58, yymsp[0].minor.yy58);
}
#line 1461 "parser.c"
        break;
      case 29:
      case 36:
      case 53:
      case 56:
      case 64:
      case 78:
      case 83:
      case 105:
#line 502 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_zval_list(yymsp[-2].minor.yy58, yymsp[0].minor.yy58);
}
#line 1475 "parser.c"
        break;
      case 31:
      case 107:
#line 510 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_column_item("all", strlen("all"), NULL, NULL, NULL);
}
#line 1483 "parser.c"
        break;
      case 32:
#line 514 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_column_item("domain-all", strlen("domain-all"), NULL, yymsp[-2].minor.yy0, NULL);
}
#line 1490 "parser.c"
        break;
      case 33:
#line 518 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_column_item("expr", strlen("expr"), yymsp[-2].minor.yy58, NULL, yymsp[0].minor.yy0);
}
#line 1497 "parser.c"
        break;
      case 34:
#line 522 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_column_item("expr", strlen("expr"), yymsp[-1].minor.yy58, NULL, yymsp[0].minor.yy0);
}
#line 1504 "parser.c"
        break;
      case 35:
#line 526 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_column_item("expr", strlen("expr"), yymsp[0].minor.yy58, NULL, NULL);
}
#line 1511 "parser.c"
        break;
      case 38:
#line 538 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_zval_list(yymsp[-1].minor.yy58, yymsp[0].minor.yy58);
}
#line 1518 "parser.c"
        break;
      case 41:
#line 550 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_join_item(yymsp[0].minor.yy58, NULL);
}
#line 1525 "parser.c"
        break;
      case 42:
#line 554 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_join_item(yymsp[-2].minor.yy58, yymsp[0].minor.yy0);
}
#line 1532 "parser.c"
        break;
      case 43:
      case 45:
#line 558 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_join_clause(PHQL_T_INNERJOIN, yymsp[0].minor.yy58, NULL);
}
#line 1540 "parser.c"
        break;
      case 44:
      case 46:
#line 562 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_join_clause(PHQL_T_INNERJOIN, yymsp[-2].minor.yy58, yymsp[0].minor.yy58);
}
#line 1548 "parser.c"
        break;
      case 47:
#line 574 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_join_clause(PHQL_T_LEFTJOIN, yymsp[0].minor.yy58, NULL);
}
#line 1555 "parser.c"
        break;
      case 48:
#line 578 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_join_clause(PHQL_T_LEFTJOIN, yymsp[-2].minor.yy58, yymsp[0].minor.yy58);
}
#line 1562 "parser.c"
        break;
      case 49:
#line 582 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_join_clause(PHQL_T_RIGHTJOIN, yymsp[0].minor.yy58, NULL);
}
#line 1569 "parser.c"
        break;
      case 50:
#line 586 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_join_clause(PHQL_T_RIGHTJOIN, yymsp[-2].minor.yy58, yymsp[0].minor.yy58);
}
#line 1576 "parser.c"
        break;
      case 51:
#line 591 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_insert_statement(yymsp[-4].minor.yy58, NULL, yymsp[-1].minor.yy58);
}
#line 1583 "parser.c"
        break;
      case 52:
#line 595 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_insert_statement(yymsp[-7].minor.yy58, yymsp[-5].minor.yy58, yymsp[-1].minor.yy58);
}
#line 1590 "parser.c"
        break;
      case 58:
      case 121:
#line 619 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_qualified_name(yymsp[0].minor.yy0, NULL);
}
#line 1598 "parser.c"
        break;
      case 59:
#line 624 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_update_statement(yymsp[0].minor.yy58, NULL, NULL);
}
#line 1605 "parser.c"
        break;
      case 60:
#line 628 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_update_statement(yymsp[-1].minor.yy58, yymsp[0].minor.yy58, NULL);
}
#line 1612 "parser.c"
        break;
      case 61:
#line 632 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_update_statement(yymsp[-1].minor.yy58, NULL, yymsp[0].minor.yy58);
}
#line 1619 "parser.c"
        break;
      case 62:
#line 636 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_update_statement(yymsp[-2].minor.yy58, yymsp[-1].minor.yy58, yymsp[0].minor.yy58);
}
#line 1626 "parser.c"
        break;
      case 63:
#line 640 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_update_clause(yymsp[-2].minor.yy58, yymsp[0].minor.yy58);
}
#line 1633 "parser.c"
        break;
      case 66:
#line 652 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_update_item(yymsp[-2].minor.yy58, yymsp[0].minor.yy58);
}
#line 1640 "parser.c"
        break;
      case 68:
#line 661 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_delete_statement(yymsp[0].minor.yy58, NULL, NULL);
}
#line 1647 "parser.c"
        break;
      case 69:
#line 665 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_delete_statement(yymsp[-1].minor.yy58, yymsp[0].minor.yy58, NULL);
}
#line 1654 "parser.c"
        break;
      case 70:
#line 669 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_delete_statement(yymsp[-1].minor.yy58, NULL, yymsp[0].minor.yy58);
}
#line 1661 "parser.c"
        break;
      case 71:
#line 673 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_delete_statement(yymsp[-2].minor.yy58, yymsp[-1].minor.yy58, yymsp[0].minor.yy58);
}
#line 1668 "parser.c"
        break;
      case 72:
#line 677 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_delete_clause(yymsp[0].minor.yy58);
}
#line 1675 "parser.c"
        break;
      case 73:
#line 681 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_assoc_name(yymsp[-2].minor.yy58, yymsp[0].minor.yy0);
}
#line 1682 "parser.c"
        break;
      case 74:
#line 685 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_assoc_name(yymsp[-1].minor.yy58, yymsp[0].minor.yy0);
}
#line 1689 "parser.c"
        break;
      case 75:
#line 689 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_assoc_name(yymsp[0].minor.yy58, NULL);
}
#line 1696 "parser.c"
        break;
      case 81:
      case 86:
      case 88:
      case 114:
#line 713 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_literal_zval(PHQL_T_INTEGER, yymsp[0].minor.yy0);
}
#line 1706 "parser.c"
        break;
      case 89:
#line 745 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_expr(PHQL_T_SUB, yymsp[-2].minor.yy58, yymsp[0].minor.yy58);
}
#line 1713 "parser.c"
        break;
      case 90:
#line 749 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_expr(PHQL_T_ADD, yymsp[-2].minor.yy58, yymsp[0].minor.yy58);
}
#line 1720 "parser.c"
        break;
      case 91:
#line 753 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_expr(PHQL_T_MUL, yymsp[-2].minor.yy58, yymsp[0].minor.yy58);
}
#line 1727 "parser.c"
        break;
      case 92:
#line 757 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_expr(PHQL_T_DIV, yymsp[-2].minor.yy58, yymsp[0].minor.yy58);
}
#line 1734 "parser.c"
        break;
      case 93:
#line 761 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_expr(PHQL_T_AND, yymsp[-2].minor.yy58, yymsp[0].minor.yy58);
}
#line 1741 "parser.c"
        break;
      case 94:
#line 765 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_expr(PHQL_T_OR, yymsp[-2].minor.yy58, yymsp[0].minor.yy58);
}
#line 1748 "parser.c"
        break;
      case 95:
#line 769 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_expr(PHQL_T_EQUALS, yymsp[-2].minor.yy58, yymsp[0].minor.yy58);
}
#line 1755 "parser.c"
        break;
      case 96:
#line 773 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_expr(PHQL_T_NOTEQUALS, yymsp[-2].minor.yy58, yymsp[0].minor.yy58);
}
#line 1762 "parser.c"
        break;
      case 97:
#line 777 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_expr(PHQL_T_LESS, yymsp[-2].minor.yy58, yymsp[0].minor.yy58);
}
#line 1769 "parser.c"
        break;
      case 98:
#line 781 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_expr(PHQL_T_GREATER, yymsp[-2].minor.yy58, yymsp[0].minor.yy58);
}
#line 1776 "parser.c"
        break;
      case 99:
#line 785 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_expr(PHQL_T_LIKE, yymsp[-2].minor.yy58, yymsp[0].minor.yy58);
}
#line 1783 "parser.c"
        break;
      case 100:
#line 789 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_expr(PHQL_T_NLIKE, yymsp[-3].minor.yy58, yymsp[0].minor.yy58);
}
#line 1790 "parser.c"
        break;
      case 101:
#line 793 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_expr(PHQL_T_IN, yymsp[-4].minor.yy58, yymsp[-1].minor.yy58);
}
#line 1797 "parser.c"
        break;
      case 103:
#line 801 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_func_call(yymsp[-3].minor.yy0, yymsp[-1].minor.yy58);
}
#line 1804 "parser.c"
        break;
      case 104:
#line 805 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_func_call(yymsp[-2].minor.yy0, NULL);
}
#line 1811 "parser.c"
        break;
      case 109:
#line 825 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_expr(PHQL_T_ISNULL, NULL, yymsp[-2].minor.yy58);
}
#line 1818 "parser.c"
        break;
      case 110:
#line 829 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_expr(PHQL_T_ISNOTNULL, NULL, yymsp[-3].minor.yy58);
}
#line 1825 "parser.c"
        break;
      case 111:
#line 833 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_expr(PHQL_T_NOT, NULL, yymsp[0].minor.yy58);
}
#line 1832 "parser.c"
        break;
      case 112:
#line 837 "parser.lemon"
{
	yygotominor.yy58 = yymsp[-1].minor.yy58;
}
#line 1839 "parser.c"
        break;
      case 115:
#line 849 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_literal_zval(PHQL_T_STRING, yymsp[0].minor.yy0);
}
#line 1846 "parser.c"
        break;
      case 116:
#line 853 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_literal_zval(PHQL_T_DOUBLE, yymsp[0].minor.yy0);
}
#line 1853 "parser.c"
        break;
      case 117:
#line 857 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_literal_zval(PHQL_T_NULL, NULL);
}
#line 1860 "parser.c"
        break;
      case 118:
#line 861 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_placeholder_zval(PHQL_T_NPLACEHOLDER, yymsp[0].minor.yy0);
}
#line 1867 "parser.c"
        break;
      case 119:
#line 865 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_placeholder_zval(PHQL_T_SPLACEHOLDER, yymsp[0].minor.yy0);
}
#line 1874 "parser.c"
        break;
      case 120:
#line 869 "parser.lemon"
{
	yygotominor.yy58 = phql_ret_qualified_name(yymsp[-2].minor.yy0, yymsp[0].minor.yy0);
}
#line 1881 "parser.c"
        break;
  };
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yypParser->yyidx -= yysize;
  yyact = yy_find_reduce_action(yypParser,yygoto);
  if( yyact < YYNSTATE ){
    yy_shift(yypParser,yyact,yygoto,&yygotominor);
  }else if( yyact == YYNSTATE + YYNRULE + 1 ){
    yy_accept(yypParser);
  }
}

/*
** The following code executes when the parse fails
*/
static void yy_parse_failed(
  yyParser *yypParser           /* The parser */
){
  phql_ARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
  phql_ARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  YYMINORTYPE yyminor            /* The minor type of the error token */
){
  phql_ARG_FETCH;
#define TOKEN (yyminor.yy0)
#line 381 "parser.lemon"

	fprintf(stderr, "Syntax error near to %s!\n", status->scanner_state->start);
	status->status = PHQL_PARSING_FAILED;

#line 1928 "parser.c"
  phql_ARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  phql_ARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
  phql_ARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "phql_Alloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
void phql_(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  phql_TOKENTYPE yyminor       /* The value for the token */
  phql_ARG_PDECL               /* Optional %extra_argument parameter */
){
  YYMINORTYPE yyminorunion;
  int yyact;            /* The parser action. */
  int yyendofinput;     /* True if we are at the end of input */
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
  yyParser *yypParser;  /* The parser */

  /* (re)initialize the parser, if necessary */
  yypParser = (yyParser*)yyp;
  if( yypParser->yyidx<0 ){
    if( yymajor==0 ) return;
    yypParser->yyidx = 0;
    yypParser->yyerrcnt = -1;
    yypParser->yystack[0].stateno = 0;
    yypParser->yystack[0].major = 0;
  }
  yyminorunion.yy0 = yyminor;
  yyendofinput = (yymajor==0);
  phql_ARG_STORE;

#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sInput %s\n",yyTracePrompt,yyTokenName[yymajor]);
  }
#endif

  do{
    yyact = yy_find_shift_action(yypParser,yymajor);
    if( yyact<YYNSTATE ){
      yy_shift(yypParser,yyact,yymajor,&yyminorunion);
      yypParser->yyerrcnt--;
      if( yyendofinput && yypParser->yyidx>=0 ){
        yymajor = 0;
      }else{
        yymajor = YYNOCODE;
      }
    }else if( yyact < YYNSTATE + YYNRULE ){
      yy_reduce(yypParser,yyact-YYNSTATE);
    }else if( yyact == YY_ERROR_ACTION ){
      int yymx;
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
      }
#endif
#ifdef YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( yypParser->yyerrcnt<0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yymx = yypParser->yystack[yypParser->yyidx].major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yymajor,&yyminorunion);
        yymajor = YYNOCODE;
      }else{
         while(
          yypParser->yyidx >= 0 &&
          yymx != YYERRORSYMBOL &&
          (yyact = yy_find_shift_action(yypParser,YYERRORSYMBOL)) >= YYNSTATE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yyidx < 0 || yymajor==0 ){
          yy_destructor(yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          YYMINORTYPE u2;
          u2.YYERRSYMDT = 0;
          yy_shift(yypParser,yyact,YYERRORSYMBOL,&u2);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
#else  /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( yypParser->yyerrcnt<=0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
      }
      yymajor = YYNOCODE;
#endif
    }else{
      yy_accept(yypParser);
      yymajor = YYNOCODE;
    }
  }while( yymajor!=YYNOCODE && yypParser->yyidx>=0 );
  return;
}

/*
  +------------------------------------------------------------------------+
  | Phalcon Framework                                                      |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2012 Phalcon Team (http://www.phalconphp.com)       |
  +------------------------------------------------------------------------+
  | This source file is subject to the New BSD License that is bundled     |
  | with this package in the file docs/LICENSE.txt.                        |
  |                                                                        |
  | If you did not receive a copy of the license and are unable to         |
  | obtain it through the world-wide-web, please send an email             |
  | to license@phalconphp.com so we can send you a copy immediately.       |
  +------------------------------------------------------------------------+
  | Authors: Andres Gutierrez <andres@phalconphp.com>                      |
  |          Eduar Carvajal <eduar@phalconphp.com>                         |
  +------------------------------------------------------------------------+
*/

static void *phql_wrapper_alloc(size_t bytes){
	return emalloc(bytes);
}

static void phql_wrapper_free(void *pointer){
	efree(pointer);
}

static void phql_parse_with_token(void* phql_parser, int opcode, int parsercode, phql_scanner_token *token, phql_parser_status *parser_status){
	phql_parser_token *pToken;
	pToken = emalloc(sizeof(phql_parser_token));
	pToken->opcode = opcode;
	pToken->token = estrndup(token->value, token->len);
	pToken->token_len = token->len;
	phql_(phql_parser, parsercode, pToken, parser_status);
	efree(token->value);
}

int phql_parse_sql(zval *result, zval *sql){
	zval *error_msg;
	phql_internal_parse_sql(&result, Z_STRVAL_P(sql), &error_msg);
}

int phql_internal_parse_sql(zval **result, char *sql, zval **error_msg) {

	char *error;
	phql_scanner_state *state;
	phql_scanner_token *token;
	int scanner_status, status = SUCCESS;
	phql_parser_status *parser_status = NULL;
	TSRMLS_FETCH();

	void* phql_parser = phql_Alloc(phql_wrapper_alloc);

	parser_status = emalloc(sizeof(phql_parser_status));
	state = emalloc(sizeof(phql_scanner_state));
	token = emalloc(sizeof(phql_scanner_token));

	parser_status->status = PHQL_PARSING_OK;
	parser_status->scanner_state = state;
	state->start = sql;

	state->end = state->start;

	while(0 <= (scanner_status = phql_get_token(state, token))) {

		switch(token->opcode){

			case PHQL_T_IGNORE:
				break;

			case PHQL_T_ADD:
				phql_(phql_parser, PHQL_PLUS, NULL, parser_status);
				break;
			case PHQL_T_SUB:
				phql_(phql_parser, PHQL_MINUS, NULL, parser_status);
				break;
			case PHQL_T_MUL:
				phql_(phql_parser, PHQL_TIMES, NULL, parser_status);
				break;
			case PHQL_T_DIV:
				phql_(phql_parser, PHQL_DIVIDE, NULL, parser_status);
				break;
			case PHQL_T_AND:
				phql_(phql_parser, PHQL_AND, NULL, parser_status);
				break;
			case PHQL_T_OR:
				phql_(phql_parser, PHQL_OR, NULL, parser_status);
				break;
			case PHQL_T_EQUALS:
				phql_(phql_parser, PHQL_EQUALS, NULL, parser_status);
				break;
			case PHQL_T_NOTEQUALS:
				phql_(phql_parser, PHQL_NOTEQUALS, NULL, parser_status);
				break;
			case PHQL_T_LESS:
				phql_(phql_parser, PHQL_LESS, NULL, parser_status);
				break;
			case PHQL_T_GREATER:
				phql_(phql_parser, PHQL_GREATER, NULL, parser_status);
				break;
			case PHQL_T_LIKE:
				phql_(phql_parser, PHQL_LIKE, NULL, parser_status);
				break;
			case PHQL_T_NOT:
				phql_(phql_parser, PHQL_NOT, NULL, parser_status);
				break;
			case PHQL_T_DOT:
				phql_(phql_parser, PHQL_DOT, NULL, parser_status);
				break;
			case PHQL_T_COMMA:
				phql_(phql_parser, PHQL_COMMA, NULL, parser_status);
				break;

			case PHQL_T_BRACKEPHQL_T_OPEN:
				phql_(phql_parser, PHQL_BRACKEPHQL_T_OPEN, NULL, parser_status);
				break;
			case PHQL_T_BRACKEPHQL_T_CLOSE:
				phql_(phql_parser, PHQL_BRACKEPHQL_T_CLOSE, NULL, parser_status);
				break;

			case PHQL_T_INTEGER:
				phql_parse_with_token(phql_parser, PHQL_T_INTEGER, PHQL_INTEGER, token, parser_status);
				break;
			case PHQL_T_DOUBLE:
				phql_parse_with_token(phql_parser, PHQL_T_DOUBLE, PHQL_DOUBLE, token, parser_status);
				break;
			case PHQL_T_STRING:
				phql_parse_with_token(phql_parser, PHQL_T_STRING, PHQL_STRING, token, parser_status);
				break;
			case PHQL_T_IDENTIFIER:
	      		phql_parse_with_token(phql_parser, PHQL_T_IDENTIFIER, PHQL_IDENTIFIER, token, parser_status);
				break;
			case PHQL_T_NPLACEHOLDER:
				phql_parse_with_token(phql_parser, PHQL_T_NPLACEHOLDER, PHQL_NPLACEHOLDER, token, parser_status);
				break;
			case PHQL_T_SPLACEHOLDER:
				phql_parse_with_token(phql_parser, PHQL_T_SPLACEHOLDER, PHQL_SPLACEHOLDER, token, parser_status);
				break;

			case PHQL_T_FROM:
				phql_(phql_parser, PHQL_FROM, NULL, parser_status);
				break;
			case PHQL_T_UPDATE:
				phql_(phql_parser, PHQL_UPDATE, NULL, parser_status);
				break;
			case PHQL_T_SET:
				phql_(phql_parser, PHQL_SET, NULL, parser_status);
				break;
			case PHQL_T_WHERE:
				phql_(phql_parser, PHQL_WHERE, NULL, parser_status);
				break;
			case PHQL_T_DELETE:
				phql_(phql_parser, PHQL_DELETE, NULL, parser_status);
				break;
			case PHQL_T_INSERT:
				phql_(phql_parser, PHQL_INSERT, NULL, parser_status);
				break;
			case PHQL_T_INTO:
				phql_(phql_parser, PHQL_INTO, NULL, parser_status);
				break;
			case PHQL_T_VALUES:
				phql_(phql_parser, PHQL_VALUES, NULL, parser_status);
				break;
			case PHQL_T_SELECT:
				phql_(phql_parser, PHQL_SELECT, NULL, parser_status);
				break;
			case PHQL_T_AS:
				phql_(phql_parser, PHQL_AS, NULL, parser_status);
				break;
			case PHQL_T_ORDER:
				phql_(phql_parser, PHQL_ORDER, NULL, parser_status);
				break;
			case PHQL_T_BY:
				phql_(phql_parser, PHQL_BY, NULL, parser_status);
				break;
			case PHQL_T_LIMIT:
				phql_(phql_parser, PHQL_LIMIT, NULL, parser_status);
				break;
			case PHQL_T_GROUP:
				phql_(phql_parser, PHQL_GROUP, NULL, parser_status);
				break;
			case PHQL_T_HAVING:
				phql_(phql_parser, PHQL_HAVING, NULL, parser_status);
				break;
			case PHQL_T_IN:
				phql_(phql_parser, PHQL_IN, NULL, parser_status);
				break;
			case PHQL_T_ON:
				phql_(phql_parser, PHQL_ON, NULL, parser_status);
				break;
			case PHQL_T_INNER:
				phql_(phql_parser, PHQL_INNER, NULL, parser_status);
				break;
			case PHQL_T_JOIN:
				phql_(phql_parser, PHQL_JOIN, NULL, parser_status);
				break;
			case PHQL_T_LEFT:
				phql_(phql_parser, PHQL_LEFT, NULL, parser_status);
				break;
			case PHQL_T_RIGHT:
				phql_(phql_parser, PHQL_RIGHT, NULL, parser_status);
				break;
			case PHQL_T_IS:
				phql_(phql_parser, PHQL_IS, NULL, parser_status);
				break;
			case PHQL_T_NULL:
				phql_(phql_parser, PHQL_NULL, NULL, parser_status);
				break;
			default:
				status = FAILURE;
				error = emalloc(sizeof(char)*32);
				sprintf(error, "scanner: unknown opcode %c", token->opcode);
				PHALCON_ALLOC_ZVAL_MM(*error_msg);
				ZVAL_STRING(*error_msg, error, 0);
				break;
		}

		if (parser_status->status != PHQL_PARSING_OK) {
			status = FAILURE;
			break;
		}

		state->end = state->start;
	}

	if (status != FAILURE) {
		switch (scanner_status) {
			case PHQL_SCANNER_RETCODE_ERR:
			case PHQL_SCANNER_RETCODE_IMPOSSIBLE:
				PHALCON_ALLOC_ZVAL_MM(*error_msg);
				if (state->start) {
					error = emalloc(sizeof(char)*(48+strlen(state->start)));
					sprintf(error, "Parsing error near to %s (%d)", state->start, status);
					ZVAL_STRING(*error_msg, error, 0);
				} else {
					ZVAL_STRING(*error_msg, "Parsing error near to EOF", 1);
				}
				status = FAILURE;
				break;
			default:
				phql_(phql_parser, 0, NULL, parser_status);
		}
	}

	phql_Free(phql_parser, phql_wrapper_free);

	if (parser_status->status == PHQL_PARSING_OK) {
		ZVAL_ZVAL(*result, parser_status->ret, 1, 0);
	}

	efree(parser_status);
	efree(state);
	efree(token);

	return status;
}