#include "include/car.h"
#define RAD2DEG 57.325

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

	uint frame = 0;

	while (window.isOpen()){

		frame++;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			c.applyAction("w");
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			c.applyAction("s");
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
			c.applyAction("a");
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			c.applyAction("d");
		}


		c.setPosition(c.getPosition().x, -c.getPosition().y);
		c.setRotation(RAD2DEG * c.getAngle());

		sf::Time delta = clock.restart();
		float d = delta.asSeconds();
		world.Step(d, 2, 2);

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
