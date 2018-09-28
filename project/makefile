helloworld : main.o print.o
	gcc -o hello main.o print.o
		
main.o : main.c print.h
	gcc -c main.c
print.o : print.c print.h
	gcc -c print.c
	
clean :
	rm hello main.o print.o
