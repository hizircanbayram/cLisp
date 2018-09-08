#ifndef AST_OPRS_H
#define AST_OPRS_H 

#include	<stdlib.h>
#include	"/home/hizircan/Desktop/cLisp/mpc/mpc.h"

#define MIN -1
#define MAX 1


typedef enum bool { false = 0, true } bool ;



void findFirstNumber(const mpc_ast_t *a, double *nPtr, bool *aPtr);
void findMinMax(const mpc_ast_t *a, double *mPtr, int operationType);
void findMCprv(const mpc_ast_t *a, const mpc_ast_t **theTree);
mpc_ast_t *findMostChildren(const mpc_ast_t *a);
int number_of_nodes(const mpc_ast_t *a);
int number_of_leaves(const mpc_ast_t *a);
int number_of_branches(const mpc_ast_t *a);
double min(const mpc_ast_t *a);
double max(const mpc_ast_t *a);


#endif
