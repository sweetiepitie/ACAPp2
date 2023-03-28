# ACAPp2

Compilation:

	ej1.c > mpicc ej1.c -o ej1 -lm
	
	ej2.c > mpicc ej2.c pgm.c -o ej2

Execution:
	ej1 > mpiexec --oversubscribe -n 4 ej1
	
	ej2 > mpiexec --oversubscribe -n num_processors ej2
