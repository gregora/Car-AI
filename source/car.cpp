#include "../include/car.h"

#define RAD2DEG 57.325

Car::Car(b2World* world, b2Vec2 position){

	this -> world = world;
	body.loadFromFile("textures/car.png");

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position = position;
	car = world -> CreateBody(&bodyDef);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(1, 2);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1;
	fixtureDef.friction = 0.1;


	car -> CreateFixture(&fixtureDef);

	car -> SetLinearDamping(0.5f);
	car -> SetAngularDamping(20);

}

b2Vec2 Car::getPosition(){
	return car -> GetPosition();
}

float Car::getAngle(){
	return car -> GetAngle();
}

float Car::getSpeed(){
	return car -> GetLinearVelocity().Length();
}

void Car::applyAction(std::string action){

	float angle = car -> GetAngle();

	if(action == "w"){
		car -> ApplyForceToCenter(b2Vec2(sin(angle) * 10, cos(angle) * 10), true);
	}

	if(action == "s"){
		car -> ApplyForceToCenter(b2Vec2(-sin(angle) * 10, -cos(angle) * 10), true);
	}

	if(action == "a"){
		car -> ApplyTorque(20, true);
	}

	if(action == "d"){
		car -> ApplyTorque(20, true);
	}


}


void Car::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	sf::Sprite car_body;
	car_body.setTexture(body);

	car_body.setScale(0.004, 0.004);
	car_body.setOrigin(250, 500);
	car_body.setRotation(RAD2DEG * car -> GetAngle());

	target.draw(car_body, states.transform*getTransform());
}
