#include "builtins.h"
#include <stdio.h>



lval* builtin_add(lenv *e, lval *a) {
	return builtin_op(e, a, "+");
}

lval* builtin_sub(lenv *e, lval *a) {
	return builtin_op(e, a, "-");
}

lval* builtin_mul(lenv *e, lval *a) {
	return builtin_op(e, a, "*");
}

lval* builtin_div(lenv *e, lval *a) {
	return builtin_op(e, a, "/");
}

lval* builtin_mod(lenv *e, lval *a) {
	return builtin_op(e, a, "%");
}


lval* builtin_gt(lenv* e, lval* a) {
  return builtin_ord(e, a, ">");
}

lval* builtin_lt(lenv* e, lval* a) {
  return builtin_ord(e, a, "<");
}

lval* builtin_ge(lenv* e, lval* a) {
  return builtin_ord(e, a, ">=");
}

lval* builtin_le(lenv* e, lval* a) {
  return builtin_ord(e, a, "<=");
}

lval* builtin_eq(lenv* e, lval* a) {
  return builtin_cmp(e, a, "==");
}

lval* builtin_ne(lenv* e, lval* a) {
  return builtin_cmp(e, a, "!=");
}

lval* builtin_load(lenv *e, lval *a) {
	LASSERT_NUM("load", a, 1);
	LASSERT_TYPE("load", a, 0, LVAL_STR);

	mpc_result_t r;
	if (mpc_parse_contents(a->cell[0]->info.str, Lispy, &r)) {
	mpc_ast_print(r.output);
    lval* expr = lval_read(r.output);
    mpc_ast_delete(r.output);

    while (expr->count) {
    	lval* x = lval_eval(e, lval_pop(expr, 0));
		lval_println(x); 
    	lval_del(x);
    }

    lval_del(expr);
    lval_del(a);

    return lval_sexpr();

	} 
	else {
		char* err_msg = mpc_err_string(r.error);
    	mpc_err_delete(r.error);

		lval* err = lval_err("Could not load Library %s", err_msg);
		free(err_msg);
		lval_del(a);

		return err;
    }
}


void lenv_add_builtin(lenv *e, const char *name, lbuiltin func) {
	lval *n = lval_sym(name);
	lval *f = lval_fun(func);
	lenv_put(e, f, n);
	lval_del(n);
	lval_del(f);
}


lval *builtin_lambda(lenv *e, lval *a) { //lenv kullanılmamıs gozukuyor. belkı ılerıde... 
	LASSERT_NUM("\\", a, 2);
	LASSERT_TYPE("\\", a, 0, LVAL_QEXPR);
	LASSERT_TYPE("\\", a, 1, LVAL_QEXPR);

	for (int i = 0; i < a->cell[0]->count; ++i) {
		LASSERT(a->cell[0]->cell[i]->type != LVAL_SYM, a,
			"Cannot define non-symbol. Got %s, Expected %s.", 
			ltype_name(a->cell[0]->cell[i]->type), ltype_name(LVAL_SYM));
	}
		
	lval *formals = lval_pop(a, 0);
	lval *body = lval_pop(a, 0);

	lval_del(a);

	return lval_lambda(formals, body);
}


void lenv_add_builtins(lenv* e) {
  /* List Functions */
  lenv_add_builtin(e, "list", builtin_list);
  lenv_add_builtin(e, "head", builtin_head);
  lenv_add_builtin(e, "tail", builtin_tail);
  lenv_add_builtin(e, "eval", builtin_eval);
  lenv_add_builtin(e, "join", builtin_join);
  lenv_add_builtin(e, "def",  builtin_def);
  lenv_add_builtin(e, "=",    builtin_put);
  lenv_add_builtin(e, "\\",   builtin_lambda);

  /* Mathematical Functions */
  lenv_add_builtin(e, "+", builtin_add);
  lenv_add_builtin(e, "-", builtin_sub);
  lenv_add_builtin(e, "*", builtin_mul);
  lenv_add_builtin(e, "/", builtin_div);
  lenv_add_builtin(e, "%", builtin_mod);

  /* Comparison Functions */
  lenv_add_builtin(e, "if", builtin_if);
  lenv_add_builtin(e, "==", builtin_eq);
  lenv_add_builtin(e, "!=", builtin_ne);
  lenv_add_builtin(e, ">",  builtin_gt);
  lenv_add_builtin(e, "<",  builtin_lt);
  lenv_add_builtin(e, ">=", builtin_ge);
  lenv_add_builtin(e, "<=", builtin_le);

  /* String Functions */
  lenv_add_builtin(e, "load",  builtin_load);
  lenv_add_builtin(e, "error", builtin_error);
  lenv_add_builtin(e, "print", builtin_print);
}


PRIVATE lval *builtin_ord(lenv *e, lval *a, const char *op) {
	LASSERT_NUM(op, a, 2);
	LASSERT_TYPE(op, a, 0, LVAL_NUM);
	LASSERT_TYPE(op, a, 1, LVAL_NUM);

	int r;
	if (!strcmp(op, ">"))
		r = (a->cell[0]->info.number > a->cell[1]->info.number);
	else if (!strcmp(op, "<"))
		r = (a->cell[0]->info.number < a->cell[1]->info.number);
	else if (!strcmp(op, ">="))
		r = (a->cell[0]->info.number >= a->cell[1]->info.number);
	else if (!strcmp(op, "<="))
		r = (a->cell[0]->info.number <= a->cell[1]->info.number);
	lval_del(a);
	return lval_num(r);
}


PRIVATE lval *builtin_if(lenv *e, lval *a) {
	LASSERT_NUM("if", a, 3);
	LASSERT_TYPE("if", a, 0, LVAL_NUM);
	LASSERT_TYPE("if", a, 1, LVAL_QEXPR);
	LASSERT_TYPE("if", a, 2, LVAL_QEXPR);
	
	lval *x;
	a->cell[1]->type = LVAL_SEXPR;
	a->cell[2]->type = LVAL_SEXPR;

	if (a->cell[0]->info.number)
		x = lval_eval(e, lval_pop(a, 1));
	else 
		x = lval_eval(e, lval_pop(a, 2));

	lval_del(a);
	return x;
}


PRIVATE lval *builtin_cmp(lenv *e, lval *a, const char *op) {
	LASSERT_NUM(op, a, 2);
	int r;

	if (!strcmp(op, "=="))
		r = lval_eq(a->cell[0], a->cell[1]);
	else if (!strcmp(op, "!="))
		r = !lval_eq(a->cell[0], a->cell[1]);
	
	lval_del(a);
	return lval_num(r);
}


PRIVATE lval *builtin_head(lenv *e, lval *a) {
	//ast gib düşünme. head { 1 2 3 } ifadesinde qexpr'nin bir cocugu vardir : { 1 2 3 }
	LASSERT(a->count != 1, a, "Function 'head' passed too many arguments! " 
			"Got %d, Expected %d.", a->count, 1);
	LASSERT(a->cell[0]->type != LVAL_QEXPR, a, "Function 'head' passed incorrect types! "
			"Got %s, Expected %s", ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));
	LASSERT(a->cell[0]->count == 0, a, "Function 'head' passed {}!");
	
	
	lval *retVal = lval_take(a, 0); //ilk elemanı çekiyorum : { 1 2 3 } ' ü yani. 1 ' i değil.

	while (retVal->count > 1)
		lval_del(lval_pop(retVal, 1));

	return retVal;
}


PRIVATE lval *builtin_put(lenv *e, lval *a) {
	return builtin_var(e, a, "=");
}


PRIVATE lval *builtin_var(lenv *e, lval *a, const char *func) {
	LASSERT_TYPE(func, a, 0, LVAL_QEXPR);

	lval *syms = a->cell[0];

	for (int i = 0; i < syms->count; ++i) {
		LASSERT(syms->cell[i]->type != LVAL_SYM, a,  
				"Function '%s' cannot define non-symbol. "	"Got %s, Expected %s.", 
				func, ltype_name(syms->cell[i]->type), ltype_name(LVAL_SYM));
	}

	LASSERT(syms->count != a->count - 1, a, 
			"Function '%s' passed too many arguments for symbol. " "Got : %d, Expected : %d", 
			func, syms->count, a->count - 1);

	for (int i = 0; i < syms->count; ++i) {
		if (!strcmp(func, "def"))
			lenv_def(e, a->cell[i + 1], syms->cell[i]);
		if (!strcmp(func, "=")) //else if ?? koymamis...
			lenv_put(e, a->cell[i + 1], syms->cell[i]);	
	}

	lval_del(a);
	return lval_sexpr();
}

  
PRIVATE lval *builtin_tail(lenv *e, lval *a) {
	//ast gib düşünme. head { 1 2 3 } ifadesinde qexpr'nin bir cocugu vardir : { 1 2 3 }

	LASSERT(a->count != 1, a, "Function 'tail' passed too many arguments! "
			"Got %d, Expected %d.", a->count, 1);
	LASSERT(a->cell[0]->type != LVAL_QEXPR, a, "Function 'tail' passed incorrect types! " 
			"Got %s, Expected %s", ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));
	LASSERT(a->cell[0]->count == 0, a, "Function 'tail' passed {}!");
	
	lval *retVal = lval_take(a, 0); //ilk elemanı çekiyorum : { 1 2 3 } ' ü yani. 1 ' i değil.
	lval_del(lval_pop(retVal, 0));

	return retVal;
}


PRIVATE lval *builtin_join(lenv *e, lval *a) {
	for (int i = 0; i < a->count; ++i) 
		LASSERT(a->cell[i]->type != LVAL_QEXPR, a, "Function 'join' passed incorrect type." 
				"Got %s, Expected %s", ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));

	lval *x = lval_pop(a, 0); //pop yerine take, a ' daki tüm list'leri silerdi.

	while (a->count)
		x = lval_join(e, x, lval_pop(a, 0));
	
	lval_del(a);
	return x;
}


PRIVATE lval *lval_join(lenv *e, lval *x, lval *y) {
	while (y->count) 
		x = lval_add(x, lval_pop(y, 0));

	lval_del(y);
	return x;
}


lval *builtin_list(lenv *e, lval *a) {
	a->type = LVAL_QEXPR;
	return a;
}


lval *builtin_print(lenv *e, lval *a) {
	for (int i = 0; i < a->count; ++i) {
		lval_print(a->cell[i]);
		putchar(' ');
	}

	putchar('\n');
	lval_del(a);

	return lval_sexpr();
}


lval *builtin_error(lenv *e, lval *a) {
	LASSERT_NUM("error", a, 1);
	LASSERT_TYPE("error", a, 0, LVAL_STR);

	lval *err = lval_err(a->cell[0]->info.str);	

	lval_del(a);
	return err;
}


lval *builtin_eval(lenv *e, lval *a) {
	LASSERT(a->count != 1, a, "Function 'eval' passed too many arguments!" 
			"Got %d, Expected %d.", a->count, 1);
	LASSERT(a->cell[0]->type != LVAL_QEXPR, a, "Function 'eval' passed incorrect type! "
			"Got %s, Expected %s", ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));

	lval *x = lval_take(a, 0);
	x->type = LVAL_SEXPR;
	return lval_eval(e, x);
}


PRIVATE lval *builtin_def(lenv *e, lval *a) {
	return builtin_var(e, a, "def");
}


PRIVATE const char *ltype_name(int t) {
	switch (t) {
		case LVAL_FUN: 
			return "Function";
	    case LVAL_NUM: 
			return "Number";
	    case LVAL_ERR: 
			return "Error";
	    case LVAL_SYM: 
			return "Symbol";
	    case LVAL_SEXPR: 
			return "S-Expression";
	    case LVAL_QEXPR: 
			return "Q-Expression";
		case LVAL_STR :
			return "String";
	    default: 
			return "Unknown";
	}
}
