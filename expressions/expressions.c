#include	"expressions.h"
#include	"/home/hizircan/cLisp/evaluation/evaluation.h"
#include	"/home/hizircan/cLisp/expressions/builtins/builtins.h"

lval *lval_read(mpc_ast_t *t) {
	if (strstr(t->tag, "number")) {  return lval_read_num(t->contents);  }
	if (strstr(t->tag, "symbol")) {  return lval_sym(t->contents);  }
	
	lval *retVal = NULL;
	if (strstr(t->tag, "sexpr"))   {  retVal =  lval_sexpr();     }
	if (strstr(t->tag, "qexpr"))   {  retVal =  lval_qexpr();     }
	if (!strcmp(t->tag, ">"))      {  retVal =  lval_sexpr();     }
	if (strstr(t->tag, "string"))  {  retVal =  lval_read_str(t); }

	for (int i = 0; i < t->children_num; ++i) {
		if (!strcmp(t->children[i]->contents, "(")) {  continue;  }
		if (!strcmp(t->children[i]->contents, ")")) {  continue;  }
		if (!strcmp(t->children[i]->contents, "{")) {  continue;  }
		if (!strcmp(t->children[i]->contents, "}")) {  continue;  }
		if (!strcmp(t->children[i]->tag, "regex"))  {  continue;  }
		if (strstr(t->children[i]->tag, "comment")) {  continue; }
		retVal = lval_add(retVal, lval_read(t->children[i]));
	}

	return retVal;
}


lval *lval_read_str(mpc_ast_t *t) {
	t->contents[strlen(t->contents) - 1] = '\0';
	char *unescaped = (char *) malloc(strlen(t->contents + 1) + 1);
	strcpy(unescaped, t->contents + 1);
	unescaped = mpcf_unescape(unescaped);
	lval *str = lval_str(unescaped);
	free(unescaped);
	return str;
}


void lval_del(lval *v) /*free etmemiz basina const koymak icin sıkıntı mı bak, sanmam*/ {
	switch (v->type) {
		case LVAL_NUM :
			//intentionally empty
			break;
		case LVAL_ERR :
			free(v->info.err);
			break;
		case LVAL_SYM :
			free(v->info.sym);
			break;
		case LVAL_STR :
			free(v->info.str);
			break;
		case LVAL_FUN :
			if (!v->info.func->builtin) {
				lenv_del(v->info.func->env);
				lval_del(v->info.func->body);
				lval_del(v->info.func->formals);
			}
			free(v->info.func);
			break;
		case LVAL_SEXPR :
		case LVAL_QEXPR :
			for (int i = 0; i < v->count; ++i)
				lval_del(v->cell[i]);
			free(v->cell);
			break;
	}	
	free(v);
}


void lval_print(const lval* v) {
	switch (v->type) {
		case LVAL_NUM:   
			printf("%.2f", v->info.number); 
			break;
    	case LVAL_ERR:   
			printf("Error: %s", v->info.err); 
			break;
    	case LVAL_SYM:   
			printf("%s", v->info.sym); 
			break;
		case LVAL_STR :
			lval_print_str(v);
			break;
    	case LVAL_SEXPR: 
			lval_expr_print(v, '(', ')'); 
			break;
		case LVAL_QEXPR: 
			lval_expr_print(v, '{', '}'); 
			break;
		case LVAL_FUN :
			if (v->info.func->builtin)
				printf("<builtin>");
			else {
				printf("(\\ ");
				lval_print(v->info.func->formals);
				putchar(' ');
				lval_print(v->info.func->body);
				putchar(')');
			}
			break;
	}
}

void lval_print_str(const lval *v) {
	char *escaped = (char *) malloc(strlen(v->info.str) + 1);
	strcpy(escaped, v->info.str);
	escaped = mpcf_escape(escaped);
	printf("\"%s\"", escaped);
	free(escaped);
}




void lval_println(const lval* v) {
	lval_print(v); 
	putchar('\n'); 
}


lval *lval_num(double x) {
	lval *retVal = (lval *) malloc(sizeof(lval));
	retVal->type = LVAL_NUM;
	retVal->count = 0;
	retVal->info.number = x;
	return retVal;
}


lval *lval_err(const char *fmt, ...) {
	lval *retVal = (lval *) malloc(sizeof(lval));
	retVal->type = LVAL_ERR;

	va_list va;
	va_start(va, fmt);

	retVal->info.err = (char *) malloc(512);
	vsnprintf(retVal->info.err, 511, fmt, va);
	retVal->info.err = (char *) realloc(retVal->info.err, strlen(retVal->info.err) + 1);
	va_end(va);

	return retVal;
}


lval *lval_sym(const char *s) {
	lval *retVal = (lval *) malloc(sizeof(lval));
	retVal->type = LVAL_SYM;
	retVal->count = 0;
	retVal->info.sym = (char *) malloc(strlen(s) + 1);
	strcpy(retVal->info.sym, s);
	return retVal;
}


lval *lval_str(const char *s) {
	lval *v = (lval *) malloc(sizeof(lval));
	v->type = LVAL_STR;
	v->info.str = (char *) malloc(strlen(s) + 1);
	strcpy(v->info.str, s);
	return v;
}


lval *lval_sexpr(void) {
	lval *retVal = (lval *) malloc(sizeof(lval));
	retVal->type = LVAL_SEXPR;
	retVal->count = 0;
	retVal->cell = NULL;
	return retVal;
}


lval *lval_qexpr(void) {
	lval *retVal = (lval *) malloc(sizeof(lval));
	retVal->type = LVAL_QEXPR;
	retVal->count = 0;
	retVal->cell = NULL;
	return retVal;
}


lval *lval_fun(lbuiltin func) {
	lval *retVal = (lval *) malloc(sizeof(lval));
	retVal->type = LVAL_FUN;
	retVal->info.func = (func_t *) malloc(sizeof(func_t));
	retVal->info.func->builtin = func;
	retVal->count = 0; //adam koymamis
	retVal->cell = NULL;	   //adam koymamis
	return retVal;
}


lval *lval_lambda(const lval *formals, const lval *body) {
	lval *retVal = (lval *) malloc(sizeof(lval));
	retVal->type = LVAL_FUN;
	retVal->info.func = (func_t *) malloc(sizeof(func_t));
	retVal->info.func->builtin = NULL;
	retVal->info.func->env = lenv_new();
	retVal->info.func->formals = (lval *) formals;
	retVal->info.func->body = (lval *) body;
	retVal->count = 0; //adam koymamis
	retVal->cell = NULL;	   //adam koymamis
	return retVal;
}


lval *lval_call(lenv *e, lval *f, lval *a) {
	if (f->info.func->builtin)
		return f->info.func->builtin(e, a);

	int given = a->count;
	int total = f->info.func->formals->count;

	while (a->count) {
		if (f->info.func->formals->count == 0) {
			lval_del(a); 
			return lval_err(
        	"Function passed too many arguments. " "Got %i, Expected %i.", given, total);
		}
		lval *sym = lval_pop(f->info.func->formals, 0);	
		if (!strcmp(sym->info.sym, "&")) {
			if (f->info.func->formals->count != 1) {
				lval_del(a);
				return lval_err(
						"Function format invalid. Symbol '&' not followed by single symbol.");
			}
			lval *nsym = lval_pop(f->info.func->formals, 0); //xs, arguman gelenler buna liste olarak eklenecek.
			lenv_put(f->info.func->env, builtin_list(e, a), nsym); //PARAMETRE SIRASI HATA OLABILIR
			lval_del(sym);
			lval_del(nsym);
			break;
		}
		lval *val = lval_pop(a, 0);
		lenv_put(f->info.func->env, val, sym);
		
		lval_del(sym);
		lval_del(val);
	}

	lval_del(a);

	if (f->info.func->formals->count > 0 && 
		strcmp(f->info.func->formals->cell[0]->info.sym, "&") == 0) {

        if (f->info.func->formals->count != 2) 
        	return lval_err("Function format invalid.Symbol '&' not followed by single symbol.");

  		lval_del(lval_pop(f->info.func->formals, 0));

  		lval* sym = lval_pop(f->info.func->formals, 0);
  		lval* val = lval_qexpr();

  		lenv_put(f->info.func->env, sym, val);
  		lval_del(sym); 
		lval_del(val);
	}

	if (f->info.func->formals->count == 0) {
		f->info.func->env->par = e;
		return builtin_eval(
				f->info.func->env, lval_add(lval_sexpr(), lval_copy(f->info.func->body)));
	}
	else 
		return lval_copy(f);
}


int lval_eq(lval *x, lval *y) {
	if (x->type != y->type)
		return 0;

	switch (x->type) {
		case LVAL_NUM : 
			return (x->info.number == y->info.number);
		case LVAL_ERR :
			return !strcmp(x->info.err, y->info.err);
		case LVAL_SYM :
			return !strcmp(x->info.sym, y->info.sym);
		case LVAL_STR :
			return !strcmp(x->info.str, y->info.str);
		case LVAL_FUN :
			if (x->info.func->builtin || y->info.func->builtin)
				return x->info.func->builtin == y->info.func->builtin;
			else 
				return lval_eq(x->info.func->formals, y->info.func->formals)
					   && lval_eq(x->info.func->body, y->info.func->body);
		case LVAL_QEXPR :
		case LVAL_SEXPR :
			if (x->count != y->count)
				return 0;
			for (int i = 0; i < x->count; ++i) {
				if (!lval_eq(x->cell[i], y->cell[i]))
					return 0;
			}
			return 1;
			break;
	}

	return 0;
}


lval *lval_add(lval *v, lval *c) {
	v->count++;
	v->cell =  realloc(v->cell, sizeof(lval *) * v->count);
	v->cell[v->count - 1] =  (lval* ) c;
	return v;
}


lval *lval_copy(const lval *v) {
	lval *retVal = (lval *) malloc(sizeof(lval));
	retVal->type = v->type;
	switch (v->type) {
		case LVAL_ERR :
			retVal->info.sym = (char *) malloc(strlen(v->info.err) + 1);
			strcpy(retVal->info.err, v->info.err);
			break;
		case LVAL_FUN :
			retVal->info.func = (func_t *) malloc(sizeof(func_t));
			if (v->info.func->builtin) {
				retVal->info.func->builtin = v->info.func->builtin;
			}
			else {
				retVal->info.func->builtin = NULL;
				retVal->info.func->env = lenv_copy(v->info.func->env);
				retVal->info.func->formals = lval_copy(v->info.func->formals);
				retVal->info.func->body = lval_copy(v->info.func->body);
			}
			break;
		case LVAL_NUM :
			retVal->info.number = v->info.number;
			break;
		case LVAL_SYM :
			retVal->info.sym = (char *) malloc(strlen(v->info.sym) + 1);
			strcpy(retVal->info.sym, v->info.sym);
			break;
		case LVAL_STR : 
			retVal->info.str = (char *) malloc(strlen(v->info.str) + 1);
			strcpy(retVal->info.str, v->info.str);
			break;
		case LVAL_QEXPR : 
		case LVAL_SEXPR :
			retVal->count = v->count;
			retVal->cell = (lval **) malloc(sizeof(lval *) * v->count);
			for (int i = 0; i < retVal->count; ++i)
				retVal->cell[i] = lval_copy(v->cell[i]);
			break;	
	}

	return retVal;
}


lenv *lenv_copy(const lenv *e) {
	lenv *retVal = (lenv *) malloc(sizeof(lenv));
	retVal->par = e->par;
	retVal->count = e->count;
	retVal->syms = (char **) malloc(e->count * sizeof(char *));
	retVal->vals = (lval **) malloc(e->count * sizeof(lval *));
	for (int i = 0; i < e->count; ++i) {
		retVal->syms[i] = (char *) malloc(strlen(e->syms[i]) + 1);
		strcpy(retVal->syms[i], e->syms[i]);
		retVal->vals[i] = lval_copy(e->vals[i]);
	}
	return retVal;
}


void lenv_def(lenv *e, const lval *f, const lval *s) {
	while (e->par)
		e = e->par;
	lenv_put(e, f, s);
}


lenv *lenv_new(void) {
	lenv *retVal = (lenv *) malloc(sizeof(lenv));
	retVal->par = NULL;
	retVal->count = 0;
	retVal->syms = NULL;
	retVal->vals = NULL;
	return retVal;
}


void lenv_del(lenv *v) {
	for (int i = 0; i < v->count; ++i) {
		free(v->syms[i]);
		lval_del(v->vals[i]);
	}

	free(v->syms);
	free(v->vals);
	free(v);
}


lval *lenv_get(const lenv *e, const lval *k) {
	for (int i = 0; i < e->count; ++i) {
		if (!strcmp(e->syms[i], k->info.sym)) //neden sym'ye bakıyoruz?
			return lval_copy(e->vals[i]); //sonuçta aynı isimli degiskeni ariyor. k'nin kopyasın da döndürebilir miydik?
	}

	if (e->par) 
		return lenv_get(e->par, k);
	else 
		return lval_err("unbound symbol '%s'", k->info.sym);
}


void lenv_put(lenv *e, const lval *f, const lval *s) {
	for (int i = 0; i < e->count; ++i) {
		if (!strcmp(e->syms[i], s->info.sym)) {
			lval_del(e->vals[i]);
			e->vals[i] = lval_copy(f);
			return;
		}
	}
	++e->count;
	e->vals = (lval **) realloc(e->vals, e->count * sizeof(lval *));
	e->syms = (char **) realloc(e->syms, e->count * sizeof(char *));
	e->vals[e->count - 1] = lval_copy(f);
	e->syms[e->count - 1] = (char *) malloc(strlen(s->info.sym) + 1);
	strcpy(e->syms[e->count -1], s->info.sym);
}




PRIVATE lval *lval_read_num(const char *c) { //kitapta mpc_ast_t ile sig. cagirirken dikkat!
	errno = 0;
	double x = strtod(c, NULL);
	return (errno == ERANGE)
		   ? lval_err("Operand is out of range!")
		   : lval_num(x);
}


PRIVATE void lval_expr_print(const lval* v, char open, char close) {
	putchar(open);

	for (int i = 0; i < v->count; i++) {
    	lval_print(v->cell[i]);
    	if (i != (v->count-1)) 
			putchar(' ');
	}

	putchar(close);
}

