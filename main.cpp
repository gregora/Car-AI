#include "qlearn.cpp"
#include "genetic.cpp"

int main(uint argc, char** args){

	/*
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
	*/

	//qlearn(&network);

	uint population = 100;
	Network* networks[population];

	bool train = true;

	for(uint i = 0; i < population; i++){

		networks[i] = new Network();

		Dense* layer1 = new Dense(RAYS, 30);
		Dense* layer2 = new Dense(30, 4);
		Dense* layer3 = new Dense(4, 4);

		layer1 -> setActivationFunction("relu");
		layer2 -> setActivationFunction("relu");
		layer3 -> setActivationFunction("softmax");

		networks[i] -> addLayer(layer1);
		networks[i] -> addLayer(layer2);
		networks[i] -> addLayer(layer3);
	}

	for(int i = 0; i < argc; i++){
		if(strcmp(args[i], "-load") == 0){
			load_population(networks, population, string("networks/Generation")+args[i + 1] + "/");
		}else if(strcmp(args[i], "-display") == 0){
			train = false;
		}
	}

	gen_settings settings = {
		//general settings
		population: population,
		generations: 100, //number of generations to run
		mutation_rate: 0.1, //number of mutations on each child

		rep_coef: 0.1, //percent of population to reproduce

		delta: 0.2, //maximum weight/bias change for mutations
		recompute_parents: false, //recompute parents (for non-deterministic evaluation functions)

		multithreading: false,

		//file saving settings
		save_period: 10, //how often networks are saved (0 == never)
		path: "./networks/", //empty folder for saving
		start_generation: 1, //affects save files

		//output settings
		output: true
	};

	if(train){
		genetic(networks, evaluate, settings);
	}else{
		render(population, networks);
	}

}
