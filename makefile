all: control.o write.o
	gcc -o control control.o
	gcc -o write write.o
	rm *.o

control.o: control.c
	gcc -c control.c

write.o: write.c
	gcc -c write.c
