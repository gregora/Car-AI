#include "include/car.h"
#include "include/gauge.h"
#define RAD2DEG 57.325


int main(){

	int WIDTH = 800;
	int HEIGHT = 600;

	b2World world(b2Vec2(0.0f, 0.0f));

	Car c(&world, b2Vec2(-90, 0));

	Track t(&world);
	t.loadChain("tracks/track1-1.ch");
	t.loadChain("tracks/track1-2.ch");

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Car AI", sf::Style::Default, settings);
	sf::View view(sf::Vector2f(0, 0), sf::Vector2f(WIDTH / 2, HEIGHT / 2));
	view.zoom(0.4);
	sf::View default_view = window.getView();
	window.setView(view);

	sf::Clock clock;
	float passed = 0;

	uint frame = 0;

	Gauge g("Speed", 0, 150);
	g.unit = "km/h";
	g.setPosition(WIDTH - 150, HEIGHT - 100);
	g.setScale(0.2, 0.2);

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

		c.applyLateralForces();


		c.setPosition(c.getPosition().x, -c.getPosition().y);
		c.setRotation(- RAD2DEG * c.getAngle());
		c.castRays(t, 30, 100);

		g.value = c.getSpeed() * 3.6;

		sf::Time delta = clock.restart();
		float d = delta.asSeconds();
		world.Step(d, 2, 2);

		view.setCenter(c.getPosition().x, -c.getPosition().y);
		//view.setRotation(- c.getAngle() * RAD2DEG);

		window.clear(sf::Color::Black);
		window.draw(t);
		window.draw(c);

		window.setView(default_view);
		window.draw(g);
		window.setView(view);

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
