#include "box2d/box2d.h"
#include <stdio.h>
#include <vector>
#include <string>

#include <SFML/Graphics.hpp>

#include "../include/track.h"

class Car : public sf::Drawable, public sf::Transformable {

	public:
		b2World* world;
		b2Body* car;

		std::vector<float> rays;

		float power = 150;
		float turning = 40;

		Car(b2World* world, b2Vec2 position = b2Vec2(0.0f, 0.0f));

		b2Vec2 getPosition() const;
		float getAngle() const;
		float getSpeed() const;

		void applyAction(std::string action);
		void applyLateralForces();
		void calculateLateralForces();

		void castRays(Track track, uint number, float length);

		void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	private:
		sf::Texture body;
};
