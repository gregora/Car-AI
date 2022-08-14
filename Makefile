main.out: main.o lib/libcar.a include/car.h gauge.o include/gauge.h track.o include/track.h
	g++ main.o gauge.o track.o -o main.out -Llib/ -lcar -lbox2d -lsfml-graphics -lsfml-window -lsfml-system

main.o: main.cpp include/car.h
	g++ -c main.cpp

lib/libcar.a: car.o
	ar rcs lib/libcar.a car.o

car.o: include/car.h source/car.cpp lib/libbox2d.a include/box2d/box2d.h
	g++ -c source/car.cpp -Llib/ -lbox2d -Wall

gauge.o: source/gauge.cpp include/gauge.h
	g++ -c source/gauge.cpp

track.o: source/track.cpp include/track.h
	g++ -c source/track.cpp
