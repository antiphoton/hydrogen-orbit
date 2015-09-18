CC=g++
a.out : main.o color.o draw.o geom.o monte_carlo.o wave.o
	$(CC) -o a.out -ljpeg main.o color.o draw.o geom.o monte_carlo.o wave.o
main.o : main.cpp
	$(CC) -o main.o -c main.cpp
color.o : color.cpp
	$(CC) -o color.o -c color.cpp
draw.o : draw.cpp
	$(CC) -o draw.o -c draw.cpp
geom.o : geom.cpp
	$(CC) -o geom.o -c geom.cpp
monte_carlo.o : monte_carlo.cpp
	$(CC) -o monte_carlo.o -c monte_carlo.cpp
wave.o : wave.cpp
	$(CC) -o wave.o -c wave.cpp
clean : 
	rm *.o

