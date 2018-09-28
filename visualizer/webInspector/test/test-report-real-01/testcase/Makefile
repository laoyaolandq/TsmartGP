all : good bad mixed

SUPPORT_DIR = ../../../support
SUPPORT_SRC = $(SUPPORT_DIR)/io.c
SUPPORT_OBJ = $(SUPPORT_SRC:.c=.o)

SOURCES = CWE476_NULL_Pointer_Dereference__binary_if_01.c

make-support : $(SUPPORT_OBJ)
	cd $(SUPPORT_DIR) && make

good : $(SOURCES) make-support
	gcc -D INCLUDEMAIN -D OMITBAD -I $(SUPPORT_DIR) $(SUPPORT_OBJ) $(SOURCES) -o good-exec

bad : $(SOURCES) make-support
	gcc -D INCLUDEMAIN -D OMITGOOD -I $(SUPPORT_DIR) $(SUPPORT_OBJ) $(SOURCES) -o bad-exec

mixed : $(SOURCES) make-support
	gcc -D INCLUDEMAIN -I $(SUPPORT_DIR) $(SUPPORT_OBJ) $(SOURCES) -o mixed-exec

clean :
	rm -rf *.o
	rm -rf good-exec bad-exec mixed-exec
	rm -rf *.i

preprocess :
	gcc -E -c $(SUPPORT_DIR)/io.c -o io.i
	gcc -E -D INCLUDEMAIN -I $(SUPPORT_DIR) CWE476_NULL_Pointer_Dereference__binary_if_01.c -o 1.i
