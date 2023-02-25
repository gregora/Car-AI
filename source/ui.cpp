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


Bar::Bar(std::string name, float min, float max, bool horizontal){
	this -> name = name;
	this -> min = min;
	this -> max = max;
	this -> horizontal = horizontal;

	if(!font.loadFromFile("fonts/Prototype.ttf")){
		printf("WARNING: Gauge object could not load \"fonts/Prototype.ttf\"");
	}

}

Bar::~Bar(){}

void Bar::draw(sf::RenderTarget& target, sf::RenderStates states) const {

	float bar_width = 200.0f;
	float bar_height = 70.0f;

	int font_size = 30;

	sf::RectangleShape background(sf::Vector2f(bar_width, bar_height));
	background.setFillColor(sf::Color(50, 50, 50, 255));


	float value_width = bar_width * (value - min) / (max - min);
	sf::RectangleShape bar(sf::Vector2f(value_width, bar_height));
	bar.setFillColor(sf::Color(150, 150, 150, 255));
	
	sf::Text text;
	text.setFont(font);
	text.setFillColor(sf::Color(255, 255, 255));
	text.setCharacterSize(font_size);
	text.setString(name.c_str());
	sf::FloatRect bounds = text.getLocalBounds();
	text.setOrigin(bounds.width / 2, bounds.height / 2);

	// Orient correctly if bar is not set to horizontal
	if(!horizontal){
		background.setOrigin(0, bar_height/2);
		bar.setOrigin(0, bar_height/2);

		background.rotate(90.0f);
		bar.rotate(90.0f);

		bar.setPosition(0, (bar_width - value_width) / 2);

		text.setPosition(0, bar_width/2 + bounds.height);
	}else{
		bar.setPosition(- (bar_width - value_width) / 2, 0);
		text.setPosition(-bar_width/2 - bounds.width, 0);
	}

	background.setOrigin(bar_width / 2, bar_height / 2);
	bar.setOrigin(value_width / 2, bar_height / 2);


	target.draw(background, states.transform*getTransform());
	target.draw(bar, states.transform*getTransform());
	target.draw(text, states.transform*getTransform());

}



FPS::FPS(){
	c.restart();

	if(!font.loadFromFile("fonts/Prototype.ttf")){
		printf("WARNING: FPS object could not load \"fonts/Prototype.ttf\"");
	}

}

void FPS::update(){

	current_frame++;

	if(current_frame >= frame_average){
		fps = (uint) 1 / c.getElapsedTime().asSeconds() * current_frame;
		c.restart();
		current_frame = 0;
	}

}

void FPS::draw(sf::RenderTarget& target, sf::RenderStates states) const {

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(45);
	text.setString(std::to_string(fps) + " FPS");
	text.setFillColor(sf::Color::White);

	target.draw(text, states.transform*getTransform());

}
