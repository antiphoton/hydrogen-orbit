CC=g++
a.out : draw.o main.o geom.o
	$(CC) -o a.out -ljpeg draw.o main.o geom.o
draw.o : draw.cpp
	$(CC) -o draw.o -c draw.cpp
main.o : main.cpp
	$(CC) -o main.o -c main.cpp
geom.o : geom.cpp
	$(CC) -o geom.o -c geom.cpp
clean : 
	rm *.o

