#ifndef EVALUATION_H
#define EVALUATION_H

#include	<stdlib.h>
#include	<string.h>
#include	<math.h>
#include	"/home/hizircan/Desktop/cLisp/mpc/mpc.h"
#include	"/home/hizircan/cLisp/expressions/expressions.h"

#define PRIVATE static


lval *lval_eval(lenv *e, lval* v); 
lval *builtin_op(const lenv *e, lval *a, const char *op); //TAMAMLANACAK
lval *lval_pop(lval *v, int i);
lval *lval_take(lval *v, int i);

PRIVATE lval* lval_eval_sexpr(lenv *e,  lval* v);


#define LASSERT(cond, args, fmt, ...) \
	if (cond) { \
		lval *err = lval_err(fmt, ##__VA_ARGS__); \
		lval_del(args);	\
		return err; \
	}


#define LASSERT_TYPE(func, args, index, expect) \
  LASSERT(args->cell[index]->type != expect, args, \
    "Function '%s' passed incorrect type for argument %i. " \
    "Got %s, Expected %s.", \
    func, index, ltype_name(args->cell[index]->type), ltype_name(expect))
  

#define LASSERT_NUM(func, args, num) \
  LASSERT(args->count != num, args, \
    "Function '%s' passed incorrect number of arguments. " \
    "Got %i, Expected %i.", \
    func, args->count, num)


#endif
