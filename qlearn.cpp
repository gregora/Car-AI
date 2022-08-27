#include "include/car.h"
#include "include/ui.h"
#include "include/algorithms.h"

#define RAD2DEG 57.325
#define RAYS 20

using namespace nnlib;
using namespace std;

uint action(Matrix& matrix){

	float explore = nnlib::random();
	if(explore >= 0.0){
		return nnlib::random(0, matrix.height - 1);
	}


	float rand = nnlib::random();

	for(uint i = 0; i < matrix.height; i++){
		float value = matrix.get(0, i);

		rand -= value;

		if(rand <= 0){
			return i;
		}
	}



	return - 1;

}

uint action_max(Matrix& matrix){

	float max = 0;
	uint index = -1;

	for(uint i = 0; i < matrix.height; i++){
		if(matrix.get(0, i) > max){
			max = matrix.get(0, i);
			index = i;
		}
	}

	return index;
}

void episode(Network* network, vector<Matrix*>& inputs, vector<Matrix*>& outputs, vector<uint>& actions, vector<float>& rewards, bool training){


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
		view.zoom(0.2);
		sf::View default_view = window.getView();
		window.setView(view);

		sf::Clock clock;
		float passed = 0;

		uint frame = 0;

		Gauge g("Speed", 0, 150);
		g.unit = "km/h";
		g.setPosition(WIDTH - 150, HEIGHT - 110);
		g.setScale(0.2, 0.2);

		Button b1('W');
		b1.setScale(0.5, 0.5);
		b1.setPosition(WIDTH - 300, HEIGHT - 110);

		Button b2('A');
		b2.setScale(0.5, 0.5);
		b2.setPosition(WIDTH - 360, HEIGHT - 50);

		Button b3('S');
		b3.setScale(0.5, 0.5);
		b3.setPosition(WIDTH - 300, HEIGHT - 50);

		Button b4('D');
		b4.setScale(0.5, 0.5);
		b4.setPosition(WIDTH - 240, HEIGHT - 50);

		FPS fps;
		fps.setPosition(WIDTH - 100, 20);
		fps.setScale(0.5, 0.5);
		fps.frame_average = 60;

		while (window.isOpen()){

			float reward = 0;


			frame++;

			b1.active = false;
			b2.active = false;
			b3.active = false;
			b4.active = false;

			c.castRays(t, RAYS, 100);

			Matrix input(1, RAYS);

			for(uint i = 0; i < c.rays.size(); i++){
				input.set(0, i, c.rays[i]);
			}

			Matrix output = network -> eval(input);

			uint ind = 0;
			if(training){
				ind = action(output);
			}else{
				ind = action_max(output);
			}

			//printf("OUT: %f\n", output.get(0, ind));

			if(output.get(0, ind) != output.get(0, ind)){
				std::cout << network -> serialize() << std::endl;
				throw 1;
			}

			inputs.push_back(input.clone());
			outputs.push_back(output.clone());


			actions.push_back(ind);


			if(ind == 0){
				c.applyAction("w");
				b1.active = true;
				reward += 0.1;
			}

			if(ind == 1){
				c.applyAction("a");
				b2.active = true;
				reward -= 0.1;


			}

			if(ind == 2){
				c.applyAction("s");
				b3.active = true;
				reward -= 0.1;

			}

			if(ind == 3){
				c.applyAction("d");
				b4.active = true;
				reward -= 0.1;

			}

			world.Step(1/60.0f, 2, 2);

			//reward += c.getForwardSpeed() / 60;

			rewards.push_back(reward);

			/*
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
				c.applyAction("w");
				b1.active = true;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
				c.applyAction("s");
				b3.active = true;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
				c.applyAction("a");
				b2.active = true;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
				c.applyAction("d");
				b4.active = true;
			}
			*/

			c.applyLateralForces();


			c.setPosition(c.getLocation().x, -c.getLocation().y);
			c.setRotation(- RAD2DEG * c.getAngle());


			//printf("%s\n", output.serialize().c_str());

			g.value = c.getSpeed() * 3.6;

			sf::Time delta = clock.restart();
			//float d = delta.asSeconds();

			view.setCenter(c.getLocation().x, -c.getLocation().y);
			//view.setRotation(- c.getAngle() * RAD2DEG);

			if(!training){
				window.clear(sf::Color::Black);
				window.draw(t);
				window.draw(c);

				window.setView(default_view);
				window.draw(g);
				window.draw(b1);
				window.draw(b2);
				window.draw(b3);
				window.draw(b4);

				fps.update();
				window.draw(fps);
				window.setView(view);

				window.display();
			}



			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed){
					window.close();
					return;
				}
			}

			if(frame >= 600){
				return;
			}

		}
}

void qlearn(Network* network){

	float alpha = 0.001;

	for(uint e = 0; e < 1000; e++){
		vector<Matrix*> inputs;
		vector<Matrix*> outputs;
		vector<uint> actions;
		vector<float> rewards;

		episode(network, inputs, outputs, actions, rewards, e % 5 != 0);
		//printf("REWARD %f\n", reward);

		network -> save("networks/network" + std::to_string(e) + ".AI");

		for(uint i = 0; i < inputs.size(); i++){

			float reward = 0;

			for(uint j = i; j < rewards.size(); j++){
				reward += pow(alpha, j - i) * rewards[j];
			}

			reward /= 10;

			uint act = actions[i];

			//printf("Action: %d, Reward %f\n", act, reward);

			float val = outputs[i] -> get(0, act);

			//printf("val: %d %f\n", act, outputs[i] -> get(0, act));

			float target = std::min((double) 0.9, (double) std::max((double) val + reward*(1 - val)/100, (double)0.1));

			//printf("Target %d %f\n", act, target);

			outputs[i] -> set(0, act, target);

			if(i == 0){
				//printf("Reward: %f\n", reward);
			}

		}

		float c_reward = 0;
		for(uint i = 0; i < rewards.size(); i++){
			c_reward += rewards[i];
		}
		//printf("Reward: %f\n", c_reward);



		fit_settings settings = {
			//backpropagation settings
			epochs: 1,
			batch_size: inputs.size(),
			speed: 0.0000001,
			//output
			output: "none",
		};

		//printf("%s\n", network -> serialize().c_str());

		printf("\n\n\n\n\n\n");

		fit(network, inputs, outputs, settings);

		//printf("%s\n", network -> serialize().c_str());

		for(uint i = 0; i < inputs.size(); i++){

			delete outputs[i];
			delete inputs[i];

		}
	}

}

int main(){

		Network network;
		Dense* layer1 = new Dense(RAYS, 30);
		Dense* layer2 = new Dense(30, 4);
		Dense* layer3 = new Dense(4, 4);

		layer1 -> setActivationFunction("relu");
		layer2 -> setActivationFunction("relu");
		layer3 -> setActivationFunction("softmax");

		network.addLayer(layer1);
		network.addLayer(layer2);
		network.addLayer(layer3);

		qlearn(&network);
}
