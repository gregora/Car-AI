#include "include/car.h"
#include "include/ui.h"
#include "include/algorithms.h"

#define RAD2DEG 57.29577951308232
#define RAYS 20

//char track_inner[] = "tracks/oval-inner.ch";
//char track_outer[] = "tracks/oval-outer.ch";
//b2Vec2 start_position(240, 0);

char track_inner[] = "tracks/track1-1.ch";
char track_outer[] = "tracks/track1-2.ch";
b2Vec2 start_position(-90, 0);


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


void evaluate(Network* network, float* score){
	b2World world(b2Vec2(0.0f, 0.0f));


	Track t(&world);
	t.loadChain(track_inner);
	t.loadChain(track_outer);

	PhysicsCar c(&world, start_position);

	float time = 0;
	while (time <= 20){

		float delta = 1.0f/60;
		time += delta;
		float reward = 0;

		c.castRays(t, RAYS, 100);

		Matrix input(1, RAYS);

		for(uint j = 0; j < c.rays.size(); j++){
			input.set(0, j, c.rays[j]);
		}

		Matrix output = network -> eval(input);

		uint ind = 0;
		ind = action_max(output);

		if(ind == 0){
			c.applyAction("w");
		}

		if(ind == 1){
			c.applyAction("a");
		}

		if(ind == 2){
			c.applyAction("s");
		}

		if(ind == 3){
			c.applyAction("d");
		}

		///check collisions
		bool collision = false;
		b2ContactEdge* ce = c.car -> GetContactList();

		while(ce != NULL)
		{
			 if (ce -> contact -> IsTouching())
			 {
				 collision = true;
				 break;
			 }
			 ce = ce -> next;
		}

		if(collision){
			return;
			reward -= 50.0f / 60;
		}


		reward += c.getForwardSpeed() / 60;

		c.applyLateralForces();

		*score -= reward;

		world.Step(1/60.0f, 2, 2);
	}
}

void render(uint population, Network** networks){

		int WIDTH = 800;
		int HEIGHT = 600;

		b2World* worlds[population];
		Track* tracks[population];

		for(uint i = 0; i < population; i++){
			worlds[i] = new b2World(b2Vec2(0.0f, 0.0f));
			tracks[i] = new Track(worlds[i]);
			tracks[i] -> loadChain(track_outer);
			tracks[i] -> loadChain(track_inner);

		}

		Track t(worlds[0]);
		t.loadChain(track_outer);
		t.loadChain(track_inner);


		Car cars[population];

		for(uint i = 0; i < population; i++){
			cars[i] = Car(worlds[i], start_position);
			if(i != 0){
				cars[i].opacity = 100;
			}else{
				cars[i].render_rays = true;
			}
		}


		sf::ContextSettings settings;
		settings.antialiasingLevel = 8;
		sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Car AI", sf::Style::Default, settings);
		sf::View view(sf::Vector2f(0, 0), sf::Vector2f(WIDTH / 2, HEIGHT / 2));
		view.zoom(0.2);
		sf::View default_view = window.getView();
		window.setView(view);
		window.setFramerateLimit(60);

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

			frame++;

			b1.active = false;
			b2.active = false;
			b3.active = false;
			b4.active = false;

			for(uint i = 0; i < population; i++){
				float reward = 0;

				cars[i].castRays(t, RAYS, 100);

				Matrix input(1, RAYS);

				for(uint j = 0; j < cars[i].rays.size(); j++){
					input.set(0, j, cars[i].rays[j]);
				}

				Matrix output = networks[i] -> eval(input);

				uint ind = 0;
				ind = action_max(output);

				if(ind == 0){
					cars[i].applyAction("w");
					if(i == 0)
						b1.active = true;
				}

				if(ind == 1){
					cars[i].applyAction("a");
					if(i == 0)
						b2.active = true;
				}

				if(ind == 2){
					cars[i].applyAction("s");
					if(i == 0)
						b3.active = true;
				}

				if(ind == 3){
					cars[i].applyAction("d");
					if(i == 0)
						b4.active = true;
				}

				///check collisions
				bool collision = false;
				b2ContactEdge* ce = cars[i].car -> GetContactList();

				while(ce != NULL)
				{
					 if (ce -> contact -> IsTouching())
					 {
						 collision = true;
						 break;
					 }
					 ce = ce -> next;
				}

				cars[i].applyLateralForces();


				cars[i].setPosition(cars[i].getLocation().x, -cars[i].getLocation().y);
				cars[i].setRotation(- RAD2DEG * cars[i].getAngle());

				worlds[i] -> Step(1/60.0f, 2, 2);
			}


			//printf("%s\n", output.serialize().c_str());

			g.value = cars[0].getSpeed() * 3.6;

			sf::Time delta = clock.restart();
			//float d = delta.asSeconds();

			view.setCenter(cars[0].getLocation().x, -cars[0].getLocation().y);
			//view.setRotation(- cars[0].getAngle() * RAD2DEG);

			window.clear(sf::Color::Black);
			window.draw(t);

			for(uint i = 0; i < population; i++){
				window.draw(cars[i]);
			}

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



			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed){
					window.close();
					return;
				}
			}

			if(frame >= 1200){
				for(uint i = 0; i < population; i++){
					delete worlds[i];
					delete tracks[i];
				}

				return;
			}

		}
}






int main(uint argc, char** args){


	bool multithreading = false;
	uint population = 100;
	uint start_generation = 0;
	uint generations = 100;
	bool train = true;

	for(int i = 0; i < argc; i++){
		if(strcmp(args[i], "-load") == 0){
			start_generation = atoi(args[i + 1]);
		}else if(strcmp(args[i], "-display") == 0){
			train = false;
		}else if(strcmp(args[i], "-multithreading") == 0){
			multithreading = true;
		}else if(strcmp(args[i], "-population") == 0){
			population = atoi(args[i + 1]);
		}else if(strcmp(args[i], "-generations") == 0){
			generations = atoi(args[i + 1]);
		}
	}

	Network* networks[population];


	for(uint i = 0; i < population; i++){

		networks[i] = new Network();

		Dense* layer1 = new Dense(RAYS, 100);
		Dense* layer2 = new Dense(100, 30);
		Dense* layer3 = new Dense(30, 4);

		layer1 -> setActivationFunction("relu");
		layer2 -> setActivationFunction("relu");
		layer3 -> setActivationFunction("softmax");

		networks[i] -> addLayer(layer1);
		networks[i] -> addLayer(layer2);
		networks[i] -> addLayer(layer3);
	}

	if(start_generation != 0){
		load_population(networks, population, string("networks/Generation") + to_string(start_generation) + "/");
	}

	gen_settings settings = {
		//general settings
		population: population,
		generations: generations, //number of generations to run
		mutation_rate: 0.1, //number of mutations on each child

		rep_coef: 0.1, //percent of population to reproduce

		delta: 0.2, //maximum weight/bias change for mutations
		recompute_parents: false, //recompute parents (for non-deterministic evaluation functions)

		multithreading: multithreading,

		//file saving settings
		save_period: 10, //how often networks are saved (0 == never)
		path: "./networks/", //empty folder for saving
		start_generation: start_generation, //affects save files

		//output settings
		output: true
	};

	if(train){
		genetic(networks, evaluate, settings);
	}else{
		render(population, networks);
	}

}
