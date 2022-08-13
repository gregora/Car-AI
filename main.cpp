#include "include/car.h"

int main(){

	int WIDTH = 500;
	int HEIGHT = 500;

	b2World world(b2Vec2(0.0f, 0.0f));

	Car c(&world);

	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Arm");
	sf::View view(sf::Vector2f(0, 0), sf::Vector2f(WIDTH / 2, HEIGHT / 2));
	view.zoom(0.3);
	window.setView(view);

	sf::Clock clock;
	float passed = 0;

	while (window.isOpen()){

		c.applyAction("w");
		c.applyAction("d");

		printf("%f %f\n", c.getPosition().x, c.getPosition().y);
		c.setPosition(c.getPosition().x, -c.getPosition().y);
		world.Step(1/60.0f, 2, 2);

		window.clear(sf::Color::Black);
		window.draw(c);
		window.display();


		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed){
				window.close();
				return 0;
			}
		}
	}

}
