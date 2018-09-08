#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H 

#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#include 	"/home/hizircan/cLisp/mpc/mpc.h"

#define PRIVATE static

enum {	LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR, LVAL_FUN, LVAL_STR };

struct lval;
struct lenv;


typedef struct lval lval;
typedef struct lenv lenv;


typedef lval*(*lbuiltin)(lenv *, lval *); //parametrelerini const'a cevirebilirim


typedef struct func_t {
	lbuiltin builtin;
	lenv *env;
	lval *formals;
	lval *body;
} func_t;


typedef union value {
	double number;
	char *err;
	char *sym;
	char *str;
	//lbuiltin builtin;
	func_t *func; 
} value;


typedef struct lval {
	int type;
	value info;
	int count;
	struct lval **cell;
} lval;


typedef struct lenv {
	lenv *par;
	int count;
	char **syms;
	lval **vals;
} lenv;


lval *lval_read(mpc_ast_t *t);
lval *lval_read_str(mpc_ast_t *t);
lval *lval_num(double x);
lval *lval_err(const char *fmt, ...);
lval *lval_sym(const char *s);
lval *lval_str(const char *s);
lval *lval_sexpr(void);
lval *lval_qexpr(void);
lval *lval_fun(lbuiltin func);
lval *lval_add(lval *v, lval *c);
lval *lval_copy(const lval *v);
void lval_print(const lval* v);
void lval_print_str(const lval *v);
void lval_println(const lval *v);
void lval_del(lval *v); //free etmemiz basina const koymak icin sıkıntı mı bak, sanmam
lval *lval_lambda(const lval *formals, const lval *body);
lval *lval_call(lenv *e, lval *f, lval *a);
int lval_eq(lval *x, lval *y);

lenv *lenv_new(void);
void lenv_del(lenv *v);
lval *lenv_get(const lenv *e, const lval *k);
void lenv_put(lenv *e, const lval *f, const lval *s);
lenv *lenv_copy(const lenv *e);
void lenv_def(lenv *e, const lval *f, const lval *s);

PRIVATE lval *lval_read_num(const char *c);//kitapta mpc_ast_t ile sig. cagirirken dikkat!
PRIVATE void lval_expr_print(const lval* v, char open, char close);

#endif
