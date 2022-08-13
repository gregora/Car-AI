#include "box2d/box2d.h"
#include <stdio.h>
#include <vector>
#include <string>

#include <SFML/Graphics.hpp>

class Car : public sf::Drawable, public sf::Transformable {

	public:
		b2World* world;
		b2Body* car;

		Car(b2World* world, b2Vec2 position = b2Vec2(0.0f, 0.0f));

		b2Vec2 getPosition();
		float getAngle();
		float getSpeed();

		void applyAction(std::string action);
		void calculateLateralForces();

		void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	private:
		sf::Texture body;
};
