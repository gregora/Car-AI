#include "box2d/box2d.h"
#include <stdio.h>
#include <vector>
#include <string>
#include <fstream>


#include <SFML/Graphics.hpp>


class Track : public sf::Drawable, public sf::Transformable {
	public:
		b2World* world;
		b2Body* body;

		Track(b2World* world);

		void loadChain(std::string path);

		void draw(sf::RenderTarget& target, sf::RenderStates states) const;

};
