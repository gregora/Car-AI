#include "../include/track.h"
#define RAD2DEG 57.325

float vector2angle(float x, float y){

	float pi = 3.14159265359;

	float veclen = sqrt(pow(x, 2) + pow(y, 2));

	if(y > 0){
		return acos(x / veclen);
	}else{
		return 2*pi - acos(x / veclen);
	}
}

Track::Track(b2World* world){
	this -> world = world;

	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	body = world -> CreateBody(&bodyDef);
}

void Track::loadChain(std::string path){


	int edges = 0;
	std::string line;
	std::ifstream myfile(path);

	if (myfile.is_open()){
		while (std::getline (myfile, line)){
			edges++; //count the number of lines
		}
	}else{
		printf("Unable to open file\n");
	}

	b2Vec2 * box = new b2Vec2[edges];
	std::ifstream myfile2(path);
	int i = 0;
	while (std::getline (myfile2,line)){
		int commapos = line.find(",");
		float x = std::stof(line.substr(0, commapos));
		float y = std::stof(line.substr(commapos + 1, line.length()));
		box[i].Set(x, y);
		i++;
	}

	myfile.close();

	b2ChainShape chain;
	chain.CreateLoop(box, edges);

	body -> CreateFixture(&chain, 1);

}


float Track::raycast(b2Vec2 position, b2Vec2 direction){
	b2Fixture* f = body -> GetFixtureList();

	float min = 1;

	b2RayCastOutput out;
	b2RayCastInput in;
	in.p1 = position;
	in.p2 = position + direction;
	in.maxFraction = 1;

	while(f != nullptr){

		uint children = f -> GetShape() -> GetChildCount();

		for(uint i = 0; i < children; i++){

			bool result = ((b2ChainShape*)  (f -> GetShape())) -> RayCast(&out, in, body -> GetTransform(), i);
			if(out.fraction < min && result){
				min = out.fraction;
			}
		}

		f = f -> GetNext();
	}

	return min;
}


void Track::draw(sf::RenderTarget& target, sf::RenderStates states) const {

	b2Fixture* f = body -> GetFixtureList();

	while(f != nullptr){

		b2ChainShape* chain = (b2ChainShape*) f -> GetShape();

		uint vertices = chain -> GetChildCount();

		sf::Vertex line[vertices];

		for(uint i = 0; i < vertices; i++){
			b2EdgeShape edge;

			chain -> GetChildEdge(&edge, i);

			b2Vec2 direction = edge.m_vertex2 - edge.m_vertex1;

			float width = direction.Length();

			float angle = vector2angle(direction.x, -direction.y);

			sf::RectangleShape rect;
			rect.setSize(sf::Vector2f(width, 1));
			rect.setFillColor(sf::Color(100, 100, 100));
			rect.setOrigin(0, 0.5);
			rect.setPosition(edge.m_vertex1.x, - edge.m_vertex1.y);
			rect.setRotation(angle * RAD2DEG);

			target.draw(rect, states.transform*getTransform());
		}


		f = f -> GetNext();
	}


}
