#include	"evaluation.h"
#include	"/home/hizircan/cLisp/expressions/builtins/builtins.h"



lval *lval_eval(lenv *e, lval* v) {
	if (v->type == LVAL_SYM) {
		//neden direkt e ile islem yapmiyoruz? cunku environment'te bulunmayabilir. bu durumda "unbount symbol" geri doner. environmentten get ile baktigimiz icin bize bir copy geri donuyor. bu halde artik v ye ihtiyac yok : free
		lval *x = lenv_get(e, v);
		lval_del(v);
		return x;
	}
	if (v->type == LVAL_SEXPR) 
		return lval_eval_sexpr(e, v); 
	return v;
}


PRIVATE lval* lval_eval_sexpr(lenv *e, lval *v) {
	for (int i = 0; i < v->count; i++) 
		v->cell[i] = lval_eval(e, v->cell[i]);	

	for (int i = 0; i < v->count; i++) 
		if (v->cell[i]->type == LVAL_ERR) { return lval_take(v, i); }
	
	if (v->count == 0) { return v; }
	if (v->count == 1) { return lval_take(v, 0); }

	lval *f = lval_pop(v, 0);
	if (f->type != LVAL_FUN) {
		lval_del(v);
		lval_del(f); 
		return lval_err("first element is not a function!");
	}

	lval *result = lval_call(e, f, v);
	lval_del(f);
	return result;
}


lval *lval_pop(lval *v, int i) {
	lval *x = v->cell[i];
	memmove(&v->cell[i], &v->cell[i + 1], (v->count - i - 1) * sizeof(lval *));
	v->count--;
	v->cell = (lval **) realloc(v->cell, v->count * sizeof(lval *));
	return x;
}


lval *lval_take(lval *v, int i) {
	lval *x = lval_pop(v, i);
	lval_del(v);
	return x;
}


lval *builtin_op(const lenv *e, lval *a, const char *op) {
	for (int i = 0; i < a->count; ++i) {
		if (a->cell[i]->type != LVAL_NUM) {
			lval_del(a);
			return lval_err("can't operate number!");
		}
	}

	lval *x = lval_pop(a, 0);
	if (!strcmp(op, "-") && a->count == 0)
		x->info.number *= -1;

	while (a->count > 0) {
		lval *y = lval_pop(a, 0);

		if (!strcmp(op, "+")) { x->info.number += y->info.number; }
    	if (!strcmp(op, "-")) { x->info.number -= y->info.number; }
    	if (!strcmp(op, "*")) { x->info.number *= y->info.number; }
		if (!strcmp(op, "%")) {
      		if (y->info.number == 0) {
        		lval_del(x); 
				lval_del(y);
        		x = lval_err("Modulus By Zero!"); 
				break;
      		}
			if ((y->info.number > (int) y->info.number) ||
				(x->info.number > (int) x->info.number)) {
				lval_del(x);
				lval_del(y);
				x = lval_err("Floating numbers with modulus operator!");
				break;
			}
			int opr1 = x->info.number;
			int opr2 = y->info.number;
			x->info.number = opr1 % opr2;
		}
    	if (!strcmp(op, "/")) {
      		if (y->info.number == 0) {
        		lval_del(x); 
				lval_del(y);
        		x = lval_err("Division By Zero!"); 
				break;
      		}
      		x->info.number /= y->info.number;
    	}
		if (!strcmp(op, "%")) {
			if (y->info.number == 0) {
				lval_del(x);
				lval_del(y);
				x = lval_err("Modulus By Zero!");
			}	
		}
		lval_del(y);
	}
	
	lval_del(a);
	return x;
}

