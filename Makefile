CC=g++
a.out : main.o draw.o
	$(CC) -o a.out -ljpeg main.o draw.o
main.o : main.cpp
	$(CC) -o main.o -c main.cpp
draw.o : draw.cpp
	$(CC) -o draw.o -c draw.cpp
clean : 
	rm *.o

