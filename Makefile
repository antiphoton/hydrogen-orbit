CC=g++
a.out : main.o color.o draw.o geom.o
	$(CC) -o a.out -ljpeg main.o color.o draw.o geom.o
main.o : main.cpp
	$(CC) -o main.o -c main.cpp
color.o : color.cpp
	$(CC) -o color.o -c color.cpp
draw.o : draw.cpp
	$(CC) -o draw.o -c draw.cpp
geom.o : geom.cpp
	$(CC) -o geom.o -c geom.cpp
clean : 
	rm *.o

