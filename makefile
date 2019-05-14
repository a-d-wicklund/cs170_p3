output: autograder_main.o threads.o
	gcc -g autograder_main.o threads.o -o app

autograder2_main.o: autograder_main.c
	gcc -c -g autograder_main.c

threads.o: threads.c
	gcc -c -g threads.c

clean:
	rm *.o
