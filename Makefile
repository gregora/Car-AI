main.out: main.o lib/libcar.a include/car.h ui.o include/ui.h track.o include/track.h
	g++ main.o ui.o track.o  -o main.out -Llib/ -lcar -lnn -lbox2d -lsfml-graphics -lsfml-window -lsfml-system -pthread

main.o: main.cpp include/car.h genetic.cpp qlearn.cpp
	g++ -c main.cpp

lib/libcar.a: car.o
	ar rcs lib/libcar.a car.o

car.o: include/car.h source/car.cpp lib/libbox2d.a include/box2d/box2d.h
	g++ -c source/car.cpp -Llib/ -lbox2d -Wall

ui.o: source/ui.cpp include/ui.h
	g++ -c source/ui.cpp

track.o: source/track.cpp include/track.h
	g++ -c source/track.cpp
