all:compile

compile: compile.c
	gcc -c -g main.c -o main.o
	gcc -c -g ~/Desktop/cLisp/mpc/mpc.c -o mpc.o 
	gcc -c -g evaluation/evaluation.c -o evaluation.o 
	gcc -c -g expressions/expressions.c -o expressions.o
	gcc -c -g expressions/builtins/builtins.c -o builtins.o
	gcc -c -g ast_oprs/ast_oprs.c -o ast_oprs.o
	gcc -o cLisp main.o mpc.o evaluation.o expressions.o builtins.o ast_oprs.o -ledit -lm

compile.c: 
	gcc -c -g -std=c99 main.c
	gcc -c -g -std=c99 evaluation/evaluation.c 
	gcc -c -g -std=c99 expressions/expressions.c
	gcc -c -g -std=c99 expressions/builtins/builtins.c
	gcc -c -g -std=c99 ast_oprs/ast_oprs.c
	gcc -c -g -std=c99 ~/Desktop/cLisp/mpc/mpc.c 

clean:
	rm *.o exe
	clear

