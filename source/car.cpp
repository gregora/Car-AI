#include "../include/car.h"

#define RAD2DEG 57.29577951308232

PhysicsCar::PhysicsCar(){

}

PhysicsCar::PhysicsCar(b2World* world, b2Vec2 position){

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

Car::Car(){}

Car::Car(b2World* world, b2Vec2 position) : PhysicsCar(world, position){
	this -> world = world;
	body.loadFromFile("textures/car.png");
}

b2Vec2 PhysicsCar::getLocation() const{
	return car -> GetPosition();
}

float PhysicsCar::getAngle() const{
	return car -> GetAngle();
}

float PhysicsCar::getSpeed() const{
	return car -> GetLinearVelocity().Length();
}

void PhysicsCar::applyAction(std::string action){

	float angle = car -> GetAngle();

	b2Vec2 currentRightNormal = b2Vec2(cos(-angle), -sin(-angle));
	b2Vec2 lateralVel = b2Dot( currentRightNormal, car -> GetLinearVelocity() ) * currentRightNormal;
	b2Vec2 forwardVel = car -> GetLinearVelocity() - lateralVel;

	if(action == "w"){
		car -> ApplyForceToCenter(b2Vec2(sin(-angle) * power, cos(-angle) * power), true);
	}

	if(action == "s"){
		car -> ApplyForceToCenter(b2Vec2(-sin(-angle) * power, -cos(-angle) * power), true);
	}

	if(action == "a"){
		car -> ApplyTorque(forwardVel.Length() * turning, true);
	}

	if(action == "d"){
		car -> ApplyTorque(-forwardVel.Length() * turning, true);
	}

}

void PhysicsCar::applyLateralForces(){
	float angle = car -> GetAngle();

	b2Vec2 currentRightNormal = b2Vec2(cos(-getAngle()), -sin(-getAngle()));
	b2Vec2 lateralVel = b2Dot( currentRightNormal, car -> GetLinearVelocity() ) * currentRightNormal;
	b2Vec2 forwardVel = car -> GetLinearVelocity() - lateralVel;

	b2Vec2 impulse = car -> GetMass() * -lateralVel;
	float maxLateralImpulse = 0.5;


	if (impulse.Length() > maxLateralImpulse){
		impulse *= maxLateralImpulse / impulse.Length(); // drift
	}

	car->ApplyLinearImpulse(impulse, car->GetWorldCenter(), true);
}

float PhysicsCar::getForwardSpeed(){
	b2Vec2 currentRightNormal = b2Vec2(cos(-getAngle()), -sin(-getAngle()));
	b2Vec2 lateralVel = b2Dot( currentRightNormal, car -> GetLinearVelocity() ) * currentRightNormal;
	b2Vec2 forwardVel = car -> GetLinearVelocity() - lateralVel;

	b2Vec2 currentNormal = b2Vec2(sin(-getAngle()), cos(-getAngle()));


	//printf("%2.10f\n", (b2Dot(forwardVel, currentNormal)));

	return b2Dot(forwardVel, currentNormal);

}



void PhysicsCar::castRays(Track track, uint number, float length){

	rays.resize(number);

	float ang_fract = 2*3.14 / number;

	for(uint i = 0; i < number; i++){
		rays[i] = length * track.raycast(getLocation(), length * b2Vec2(sin(- getAngle() - i*ang_fract), cos(- getAngle() - i*ang_fract)));
	}

}


void Car::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	sf::Sprite car_body;
	car_body.setTexture(body);
	car_body.setColor(sf::Color(255, 255, 255, opacity));

	car_body.setOrigin(250, 500);
	car_body.setScale(0.004, 0.004);

	if(render_rays){
		float ang_fract = 2*3.14 / rays.size();

		for(uint i = 0; i < rays.size(); i++){

			float ray_ang = -(i*ang_fract + 1.57) * 180 / 3.14;

			sf::RectangleShape ray;
			ray.setFillColor(sf::Color(50, 50, 50, opacity));
			ray.setSize(sf::Vector2f(rays[i], 0.3));
			ray.setOrigin(0, 0.15);

			ray.setRotation(ray_ang);


			target.draw(ray, states.transform*getTransform());
		}
	}


	target.draw(car_body, states.transform*getTransform());
}
