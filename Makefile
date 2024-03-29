all: genetic.out reinforcement.out track_editor.out

# Genetic algorithm
genetic.out: genetic.o lib/libcar.a include/car.h ui.o include/ui.h track.o include/track.h
	g++ genetic.o ui.o track.o  -o genetic.out -Llib/ -lcar -lnn -lbox2d -lsfml-graphics -lsfml-window -lsfml-system -pthread
genetic.o: include/car.h genetic.cpp
	g++ -c genetic.cpp

# Reinforcement learning algorithm
reinforcement.o: include/car.h reinforcement.cpp
	g++ -c reinforcement.cpp
reinforcement.out: reinforcement.o lib/libcar.a include/car.h ui.o include/ui.h track.o include/track.h
	g++ reinforcement.o ui.o track.o  -o reinforcement.out -Llib/ -lcar -lnn -lbox2d -lsfml-graphics -lsfml-window -lsfml-system -pthread

# Track editor
track_editor.out: track_editor.cpp include/track.h
	g++ track_editor.cpp track.o -o track_editor.out -Llib/ -lbox2d -lsfml-graphics -lsfml-window -lsfml-system -pthread

# Misc files
lib/libcar.a: car.o
	ar rcs lib/libcar.a car.o
car.o: include/car.h source/car.cpp lib/libbox2d.a include/box2d/box2d.h
	g++ -c source/car.cpp -Llib/ -lbox2d -Wall

track.o: source/track.cpp include/track.h
	g++ -c source/track.cpp

ui.o: source/ui.cpp include/ui.h
	g++ -c source/ui.cpp
