#include "include/car.h"
#include "include/ui.h"
#include "include/algorithms.h"

#define RAD2DEG 57.29577951308232
#define RAYS 20

#define FULLSCREEN false
#define RENDER false

using namespace nnlib;
using namespace std;

uint sample(Matrix& probabilites){

	float sampled = nnlib::random();

	float sum = 0;

	for(uint ind = 0; ind < probabilites.height; ind++){
		sum += probabilites.get(0, ind);
	}

	if(sum != 0)
		probabilites = probabilites / sum; //normalize given vector

	for(uint ind = 0; ind < probabilites.height; ind++){
		sampled -= probabilites.get(0, ind);
		if(sampled <= 0){
			return ind;
		}
	}

	return 0;
}

uint action(Matrix& probabilites, float epsilon){

	float explore = nnlib::random();
	if(explore >= epsilon){
		return (int) min((double) nnlib::random(0, probabilites.height - 1), (double) probabilites.height - 1);
	}

	return sample(probabilites);

}

void episode(Network* network, vector<Matrix*>& inputs, vector<Matrix*>& outputs, vector<uint>& actions, vector<float>& rewards, bool training){

	int WIDTH = 800;
	int HEIGHT = 600;

	b2World world(b2Vec2(0.0f, 0.0f));



	Car c(&world, b2Vec2(-25, 0));

	Track t(&world);
	t.loadChain("tracks/handmade-inner.ch");
	t.loadChain("tracks/handmade-outer.ch");

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;

	sf::RenderTexture render_text;
	render_text.create(WIDTH, HEIGHT);

	sf::RenderWindow* window_ptr = nullptr;

	if(!training)
		window_ptr = new sf::RenderWindow(sf::VideoMode(WIDTH, HEIGHT), "Car AI", sf::Style::Default, settings);

	sf::View view(sf::Vector2f(0, 0), sf::Vector2f(WIDTH / 2, HEIGHT / 2));
	if(!training){
		
		window_ptr -> setFramerateLimit(60);

		view.zoom(0.2);
		sf::View default_view = window_ptr -> getView();
		window_ptr -> setView(view);
	}

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

	Bar bar1("W", 0, 1, false);
	bar1.setScale(0.5, 0.5);
	bar1.setPosition(50, HEIGHT - 80);
	Bar bar2("A", 0, 1, false);
	bar2.setScale(0.5, 0.5);
	bar2.setPosition(95, HEIGHT - 80);
	Bar bar3("S", 0, 1, false);
	bar3.setScale(0.5, 0.5);
	bar3.setPosition(140, HEIGHT - 80);
	Bar bar4("D", 0, 1, false);
	bar4.setScale(0.5, 0.5);
	bar4.setPosition(185, HEIGHT - 80);

	FPS fps;
	fps.setPosition(WIDTH - 100, 20);
	fps.setScale(0.5, 0.5);
	fps.frame_average = 60;

	while (true){

		sf::Time delta = clock.restart();
		float d = 1.0f / 60;


		if(!training && !RENDER){
			if(!window_ptr -> isOpen()){
				break;
			}

			d = delta.asSeconds();			
		}

		float reward = 0;

		frame++;

		b1.active = false;
		b2.active = false;
		b3.active = false;
		b4.active = false;

		c.castRays(t, RAYS, 100);

		Matrix input(1, RAYS + 1);

		for(uint i = 0; i < c.rays.size(); i++){
			input.set(0, i, c.rays[i]);
		}

		input.set(0, RAYS, c.getSpeed());

		Matrix output = network -> eval(input);

		uint ind = 0;
		if(training){
			ind = action(output, 0.9f);
		}else{
			ind = action(output, 1.0f);
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
		}

		if(ind == 1){
			c.applyAction("a");
			b2.active = true;


		}

		if(ind == 2){
			c.applyAction("s");
			b3.active = true;

		}

		if(ind == 3){
			c.applyAction("d");
			b4.active = true;

		}

		world.Step(d, 2, 2);


		reward = c.getForwardSpeed() * d;

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

		bar1.value = output.get(0, 0);
		bar2.value = output.get(0, 1);
		bar3.value = output.get(0, 2);
		bar4.value = output.get(0, 3);

		
		if(!training){
			sf::RenderWindow& window = *window_ptr;

			const sf::View& default_view = window.getDefaultView();
			
			view.setCenter(c.getLocation().x, -c.getLocation().y);

			//view.setRotation(- c.getAngle() * RAD2DEG);


			window.clear(sf::Color::Black);
			window.draw(t);
			window.draw(c);

			window.setView(default_view);
			window.draw(g);
			window.draw(b1);
			window.draw(b2);
			window.draw(b3);
			window.draw(b4);

			window.draw(bar1);
			window.draw(bar2);
			window.draw(bar3);
			window.draw(bar4);

			fps.update();
			if(!RENDER){
				window.draw(fps);
			}

			window.setView(view);

			if(RENDER){
				sf::Texture texture;
				texture.create(window.getSize().x, window.getSize().y);
				texture.update(window);
				if (texture.copyToImage().saveToFile("render/" + to_string(frame) + ".png")){}
			}

			window.display();

			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed){
					window.close();
					return;
				}
			}
		}

		if(frame >= 600){
			break;
		}

	}

	if(!training){
		delete window_ptr;
	}
}

void reinforcement(Network* network){

	float alpha = 0.999;
	int training_period = 20;

	for(uint e = 0; e < 10000; e++){
		vector<Matrix*> inputs;
		vector<Matrix*> outputs;
		vector<uint> actions;
		vector<float> rewards;

		episode(network, inputs, outputs, actions, rewards, e % training_period != 0);

		vector<Matrix*> targets;

		network -> save("networks/network" + std::to_string(e) + ".AI");

		for(uint i = 0; i < inputs.size(); i++){

			float reward = 0;

			for(uint j = i; j < rewards.size(); j++){
				reward += pow(alpha, j - i) * rewards[j];
			}

			uint act = actions[i];

			float val = outputs[i] -> get(0, act);

			uint n = outputs[i] -> height;
			Matrix* target = new Matrix(1, n);

			if(reward > 0){
				target -> set(0, act, reward > 0);
			}else {
				for(uint i = 0; i < n; i++){
					if(i != act){
						target -> set(0, i, 1.0f / (n - 1));
					}
				}
			}


			targets.push_back(target);

			//cout << reward << endl;
			//cout << target -> serialize() << endl;


		}

		float c_reward = 0;
		for(uint i = 0; i < rewards.size(); i++){
			c_reward += rewards[i];
		}

		fit_settings settings = {
			//backpropagation settings
			epochs: 10,
			batch_size: outputs.size(),
			speed: 0.0001,
			//output
			output: "none",
		};

		if(e % training_period != 0){
			printf("Training score: %f\n", c_reward);
			fit(network, inputs, targets, settings);
		}else{
			printf("Score: %f\n", c_reward);
		}

		for(uint i = 0; i < inputs.size(); i++){

			delete outputs[i];
			delete inputs[i];
			delete targets[i];

		}
	}

}

int main(){

		Network network;
		Dense* layer1 = new Dense(RAYS + 1, 40);
		Dense* layer3 = new Dense(40, 4);

		layer1 -> setActivationFunction("relu");
		layer1 -> setErrorFunction("categoricalCrossentropy");
		//layer2 -> setActivationFunction("relu");
		layer3 -> setErrorFunction("categoricalCrossentropy");
		layer3 -> setActivationFunction("softmax");

		network.addLayer(layer1);
		//network.addLayer(layer2);
		network.addLayer(layer3);

		reinforcement(&network);
}
