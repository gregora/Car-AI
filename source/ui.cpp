#include "../include/ui.h"

Gauge::Gauge(std::string name, float min, float max){
	this -> name = name;
	this -> min = min;
	this -> max = max;

	texture.loadFromFile("textures/gauge.png");
	sprite.setTexture(texture);

	if(!font.loadFromFile("fonts/Prototype.ttf")){
		printf("WARNING: Gauge object could not load \"fonts/Prototype.ttf\"");
	}

}

Gauge::~Gauge(){}

void Gauge::draw(sf::RenderTarget& target, sf::RenderStates states) const {


	float angle = 180 * (value - min) / (max - min);
	sf::RectangleShape pointer;
	pointer.setSize(sf::Vector2f(260, 30));
	pointer.setOrigin(240, 15);
	pointer.setFillColor(sf::Color(150, 150, 150));
	pointer.setRotation(angle);
	pointer.setPosition(250, 250);

	sf::Text text;
	text.setFont(font);
	text.setFillColor(sf::Color(255, 255, 255));
	text.setCharacterSize(60);
	char string[40];
	sprintf(string, "%4.0f %s", value, unit.c_str());
	text.setString(string);
	float t_width = text.getLocalBounds().width;
	text.setPosition((500 - t_width) / 2, 330);


	sf::Text name_text;
	name_text.setFont(font);
	name_text.setFillColor(sf::Color(150, 150, 150));
	name_text.setCharacterSize(45);
	name_text.setString(name);
	float nt_width = name_text.getLocalBounds().width;
	name_text.setPosition((500 - nt_width) / 2, 280);

	sf::CircleShape background;
	background.setFillColor(sf::Color(0, 0, 0, 200));
	background.setOrigin(300, 300);
	background.setRadius(300);
	background.setPosition(250, 250);

	target.draw(background, states.transform*getTransform());
	target.draw(sprite, states.transform*getTransform());
	target.draw(pointer, states.transform*getTransform());
	target.draw(text, states.transform*getTransform());
	target.draw(name_text, states.transform*getTransform());

}

Button::Button(char key){
	this -> key = key;

	if(!font.loadFromFile("fonts/Prototype.ttf")){
		printf("WARNING: Button object could not load \"fonts/Prototype.ttf\"");
	}
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	sf::RectangleShape background;
	background.setSize(sf::Vector2f(80, 80));

	sf::Color dark_grey(20, 20, 20);
	sf::Color grey(50, 50, 50);

	if(active){
		dark_grey = sf::Color(40, 40, 40);
		grey = sf::Color(100, 100, 100);
	}

	background.setFillColor(dark_grey);
	background.setOutlineColor(grey);
	background.setOutlineThickness(10);


	sf::Text text;
	text.setFont(font);
	text.setFillColor(grey);
	text.setCharacterSize(45);
	text.setString(key);
	sf::FloatRect bounds = text.getLocalBounds();
	text.setOrigin(bounds.width / 2, bounds.height / 2);

	text.setPosition(40, 30);

	target.draw(background, states.transform*getTransform());
	target.draw(text, states.transform*getTransform());

}
