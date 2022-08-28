#include "include/track.h"
#include "include/algorithms.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace sf;

float detection_radius = 0.5;

void save(b2Vec2* points, int edges, const char* path);

int main(int argc, char** args){

	int WIDTH = 1000;
	int HEIGHT = 500;

	char* name;
	int edges = 10;

	for(int i = 0; i < argc; i++){
		if(strcmp(args[i], "-name") == 0){
			name = args[i + 1];
		}else if(strcmp(args[i], "-points") == 0){
			edges = atoi(args[i + 1]);
		}
	}

	b2World world(b2Vec2(0.0f, 0.0f));
	Track track(&world);

	b2Vec2 points_inner[edges];
	b2Vec2 points_outer[edges];

	for(int i = 0; i < edges; i++){
		points_inner[i] = b2Vec2(20 * sin(2*3.14 * i / edges), 20 * cos(2*3.14 * i / edges));
		points_outer[i] = b2Vec2(30 * sin(2*3.14 * i / edges), 30 * cos(2*3.14 * i / edges));
	}


	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Track editor", sf::Style::Default, settings);
	sf::View view(sf::Vector2f(0, 0), sf::Vector2f(WIDTH / 2, HEIGHT / 2));
	view.zoom(0.2);
	sf::View default_view = window.getView();
	window.setView(view);


	CircleShape node;
	node.setRadius(detection_radius);
	node.setOrigin(detection_radius, detection_radius);
	node.setFillColor(Color::Red);

	int dragged_index = -1;

	while (window.isOpen()){

		if(Mouse::isButtonPressed(Mouse::Button::Left)){
			Vector2i click_i = Mouse::getPosition(window);

			float x = ((float) click_i.x - WIDTH / 2) * 0.2 / 2;
			float y = -((float) click_i.y - HEIGHT / 2) * 0.2 / 2;

			b2Vec2 click(x, y);

			for(int i = 0; i < edges; i++){
				if((click - points_inner[i]).Length() <= detection_radius){
					dragged_index = i;
				}
			}


			for(int i = 0; i < edges; i++){
				if((click - points_outer[i]).Length() <= detection_radius){
					dragged_index = i + 10;
				}
			}

			if(dragged_index != -1){
				if(dragged_index / edges == 0){
					points_inner[dragged_index] = click;
				}else{
					points_outer[dragged_index - 10] = click;
				}
			}

		}else{
			dragged_index = -1;
		}


		track.deleteChains();

		track.setChain(points_inner, edges);
		track.setChain(points_outer, edges);


		window.clear(sf::Color::Black);
		window.draw(track);

		for(int i = 0; i < edges; i++){
			node.setPosition(Vector2f(points_inner[i].x, -points_inner[i].y));
			window.draw(node);
		}

		for(int i = 0; i < edges; i++){
			node.setPosition(Vector2f(points_outer[i].x, -points_outer[i].y));
			window.draw(node);
		}

		window.display();

		//save to file
		if(Keyboard::isKeyPressed(Keyboard::S)){
			save(points_inner, edges, (string("tracks/") + string(name) + string("-inner.ch")).c_str());
			save(points_outer, edges, (string("tracks/") + string(name) + string("-outer.ch")).c_str());
		}

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed){
				window.close();
			}
		}
	}
}


void save(b2Vec2* points, int edges, const char* path) {
	ofstream file;
	file.open(path);

	for(int i = 0; i < edges; i++){
		file << points[i].x;
		file << " ";
		file << points[i].y;
		file << "\n";
	}

	file.close();
}
