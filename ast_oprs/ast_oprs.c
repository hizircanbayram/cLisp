#include "ast_oprs.h"


mpc_ast_t *findMostChildren(const mpc_ast_t *a) {
	const mpc_ast_t *retVal = a;
	findMCprv(a, &retVal);
	return (mpc_ast_t *) retVal;
}


void findMinMax(const mpc_ast_t *a, double *mPtr, int operationType) {
	if (strstr(a->tag, "number")) {
		if (operationType == MIN && atof(a->contents) < *mPtr)
			*mPtr = atof(a->contents);
		else if (operationType == MAX && atof(a->contents) > *mPtr)
			*mPtr = atof(a->contents);
	} 
	for (int i = 0; i < a->children_num; ++i) 
		findMinMax(a->children[i], mPtr, operationType);
}


void findFirstNumber(const mpc_ast_t *a, double *nPtr, bool *aPtr) {
	if (strstr(a->tag, "number") && *aPtr == false) {
		*nPtr = atof(a->contents);
		*aPtr = true;
	return;

	for (int i = 0; i < a->children_num; ++i) 
		 findFirstNumber(a->children[i], nPtr, aPtr); //yukarıdaki ifadeleri döngünün içine koyduğunda, çocuğu olmayan node'lar için hiçbiri gerçeklenmiyordu. Bu ağaç yapısında ilerleme bu şekilde gerçekleşiyor. Döngünün içinde sadece recursive olacak.
	}
}


void findMCprv(const mpc_ast_t *a, const mpc_ast_t **theTree) {
	if ((*theTree)->children_num < a->children_num)
		*theTree = a;
	for (int i = 0; i < a->children_num; ++i)
		findMCprv(a->children[i], theTree);	
}


int number_of_nodes(const mpc_ast_t *a) {
	if (a->children_num == 0)
		return 1;
	if (a->children_num >= 1) {
		int total = 1;
		int i;
		for (i = 0; i < a->children_num; ++i)
			total += number_of_nodes(a->children[i]);
		return total;
	}
	return 0; //neden var anlamadım. belki bazı derleyici warninglerini onlemek icin
}


int number_of_leaves(const mpc_ast_t *a) {
	if (a->children_num == 0)
		return 1;
	int total = 0;
	for (int i = 0; i < a->children_num; ++i) 
		total += number_of_leaves(a->children[i]);
	
	return total;
}


int number_of_branches(const mpc_ast_t *a) {
	if (a->children_num == 0)
		return 0;
	int i;
	int total = 1;
	for (i = 0; i < a->children_num; ++i) 
		total += number_of_branches(a->children[i]);
	
	return total;
}


double min(const mpc_ast_t *a) {
	double min;
	bool assigned = false;
	findFirstNumber(a, &min, &assigned);
	findMinMax(a, &min, MIN);
	return min;
}


double max(const mpc_ast_t *a) {
	double max;
	bool assigned = false;
	findFirstNumber(a, &max, &assigned);
	findMinMax(a, &max, MAX);
	return max;
}
