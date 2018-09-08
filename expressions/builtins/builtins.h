#ifndef BUILTINS_H
#define BUILTINS_H 

#include	"/home/hizircan/cLisp/expressions/expressions.h"
#include	"/home/hizircan/cLisp/evaluation/evaluation.h"
#include	<string.h>


//lval *builtin(lval *a, const char *func);

//source ve burada const luk durumlarını kontrol et
lval* builtin_add(lenv *e, lval *a);
lval* builtin_sub(lenv *e, lval *a);
lval* builtin_mul(lenv *e, lval *a);
lval* builtin_div(lenv *e, lval *a);
lval* builtin_mod(lenv *e, lval *a);
lval* builtin_gt(lenv* e, lval* a);
lval* builtin_lt(lenv* e, lval* a);
lval* builtin_ge(lenv* e, lval* a);
lval* builtin_le(lenv* e, lval* a);
lval* builtin_eq(lenv* e, lval* a);
lval* builtin_ne(lenv* e, lval* a);
lval* builtin_load(lenv *e, lval *a);


extern mpc_parser_t *Lispy;

void  lenv_add_builtin(lenv *e, const char *name, lbuiltin func); 
void lenv_add_builtins(lenv* e);

lval *builtin_lambda(lenv *e, lval *a); //lenv kullanılmamıs gozukuyor. belkı ılerıde...
lval *builtin_eval(lenv *e, lval *a);

PRIVATE lval *builtin_ord(lenv *e, lval *a, const char *op);
PRIVATE lval *builtin_if(lenv *e, lval *a);
PRIVATE lval *builtin_cmp(lenv *e, lval *a, const char *op);
PRIVATE lval *builtin_var(lenv *e, lval *a, const char *func);
PRIVATE lval *builtin_head(lenv *e, lval *a);
PRIVATE lval *builtin_tail(lenv *e, lval *a);
lval *builtin_list(lenv *e, lval *a);
lval *builtin_print(lenv *e, lval *a);
lval *builtin_error(lenv *e, lval *a);
PRIVATE lval *builtin_join(lenv *e, lval *a);
PRIVATE lval *builtin_put(lenv *e, lval *a);
PRIVATE lval *lval_join(lenv *e, lval *x, lval *y);
PRIVATE lval *builtin_def(lenv *e, lval *a);
PRIVATE const char *ltype_name(int t);
#endif
