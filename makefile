output: autograder2_main.o threads.o
	gcc -g autograder2_main.o threads.o -o application

autograder2_main.o: autograder2_main.c
	gcc -c -g autograder2_main.c

threads.o: threads.c
	gcc -c -g threads.c

clean:
	rm *.o
