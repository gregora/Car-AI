#define RAD2DEG 57.325
#define RAYS 20

using namespace nnlib;
using namespace std;


void evaluate(uint population, Network** networks, float* scores){

		int WIDTH = 800;
		int HEIGHT = 600;

		b2World* worlds[population];
		Track* tracks[population];


		for(uint i = 0; i < population; i++){
			worlds[i] = new b2World(b2Vec2(0.0f, 0.0f));
			tracks[i] = new Track(worlds[i]);
			tracks[i] -> loadChain("tracks/track1-1.ch");
			tracks[i] -> loadChain("tracks/track1-2.ch");

		}

		Track t(worlds[0]);
		t.loadChain("tracks/track1-1.ch");
		t.loadChain("tracks/track1-2.ch");


		Car cars[population];

		for(uint i = 0; i < population; i++){
			cars[i] = Car(worlds[i], b2Vec2(-90, 0));
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

				if(collision){
					reward -= 50.0f / 60;
				}


				reward += cars[i].getForwardSpeed() / 60;

				cars[i].applyLateralForces();


				cars[i].setPosition(cars[i].getLocation().x, -cars[i].getLocation().y);
				cars[i].setRotation(- RAD2DEG * cars[i].getAngle());

				scores[i] -= reward;

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
