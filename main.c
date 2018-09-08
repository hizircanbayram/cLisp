#include	<stdio.h>
#include	<stdlib.h>
#include	"/home/hizircan/Desktop/cLisp/mpc/mpc.h"
#include	"/home/hizircan/cLisp/evaluation/evaluation.h"
#include	"/home/hizircan/cLisp/expressions/expressions.h"
#include	"/home/hizircan/cLisp/expressions/builtins/builtins.h"

#ifdef _WIN32 

#include 	<string.h>


char *readline(const char *a) {
	fputs(a, stdout);
	fgets(buffer, 2048, stdin);
	size_t len = strlen(buffer);
	char *input = malloc(sizeof(char) * (len + 1)); 
	strcpy(input, buffer)[len - 1] = '\0';
	return input;
}


void add_history(char *input) {

}


#else /*__linux__*/


#include <editline/readline.h>
#include <editline/history.h>

#endif


mpc_parser_t *Number;
mpc_parser_t *Symbol;
mpc_parser_t *Sexpr;
mpc_parser_t *Qexpr;
mpc_parser_t *Expr;
mpc_parser_t *Lispy;
mpc_parser_t *String;
mpc_parser_t *Comment;

int main(int argc, char **argv) {
	
	/* Create Some Parsers */
	Number = mpc_new("number");
	Symbol = mpc_new("symbol");
	Sexpr = mpc_new("sexpr");
	Qexpr = mpc_new("qexpr");
	Expr = mpc_new("expr");
	Lispy = mpc_new("lispy");
	String = mpc_new("string");
	Comment = mpc_new("comment");
	/* Define them with the following Language */
	mpca_lang(MPCA_LANG_DEFAULT,
	  "                                              \
	    number  : /-?[0-9]+/ ;                       \
	    symbol  : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&%]+/ ; \
	    string  : /\"(\\\\.|[^\"])*\"/ ;             \
	    comment : /;[^\\r\\n]*/ ;                    \
	    sexpr   : '(' <expr>* ')' ;                  \
	    qexpr   : '{' <expr>* '}' ;                  \
	    expr    : <number>  | <symbol> | <string>    \
	            | <comment> | <sexpr>  | <qexpr>;    \
	    lispy   : /^/ <expr>* /$/ ;                  \
	  ",
	  Number, Symbol, String, Comment, Sexpr, Qexpr, Expr, Lispy);

	lenv *e = lenv_new();
	lenv_add_builtins(e);

	puts("\ncLisp v1.0.0.0");
	puts("ctrl + c to exit\n");

	if (argc == 1) {
	
		while (1) {
			char *input = readline("cLisp > ");
			add_history(input);
			mpc_result_t r;
	
			if (mpc_parse("<stdin>", input, Lispy, &r)) {
				mpc_ast_t *a = r.output;
				//mpc_ast_print(r.output);
				lval* x = lval_read(r.output);
				x = lval_eval(e, x);
				lval_println(x);
				lval_del(x);
				mpc_ast_delete(r.output);	
				a = NULL; //dangling pointer fark ettirme idiomu
			}
			else {
				mpc_err_print(r.error);
				mpc_err_delete(r.error);		
			}
			free(input);
		}
	}
	else if (argc >= 2) {
		for (int i = 1; i < argc; ++i) {
			lval *args = lval_add(lval_sexpr(), lval_str(argv[i]));
			lval *x = builtin_load(e, args);
			if (x->type == LVAL_ERR)
				lval_println(x);
			lval_del(x);
		}
	}

	mpc_cleanup(8, Number, Symbol, Sexpr, Qexpr, Expr, Lispy, String, Comment);
	return 0;
}

