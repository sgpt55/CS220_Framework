
#include "../Headers/grid.h"
#include "../Headers/problem_object.h"
#include <time.h>
#include <cstdlib>
#include <iostream>

using std::cerr;
using std::cout;
using std::endl;

int main(int argc,char* argv[]) {

	// DO NOT CHANGE THIS SECTION OF CODE
	if(argc < 3) {
		cout << "Usage: ./grid_router <test_file> integer" << endl;
		exit(1);
	}
	Utilities::ProblemObject* first_problem = new Utilities::ProblemObject(std::string(argv[1]));
	// EDIT FROM HERE DOWN

	//Create your problem map object (in our example, we use a simple grid, you should create your own)
	Utilities::Grid g(first_problem);

	/*
	Note: we do not take into account the connections or blockers that exist in the Project Object
	You should be accouting for these in your problem map objects (you should not be using Grid). You
	should be using both the Node and Edge classes as the background for whatever problem map object
	you create.
	*/

	/*
	Run your algorithm after creating your problem map object. You should be returning from your algorithm 
	either a Path or a Netlist

	Path: a series of straight line segments, with a single source and a single sink
	Netlist: a series of stright line segments, with a single source and more than one sink
	*/
	int e = atoi(argv[2]);
	vector<Path*> paths;
	if(e==0){
		paths = g.test_algorithm();
	}else if(e==1){
		paths = g.test_algorithm2bit();
	}else if(e==2){
		paths = g.test_algorithm3bit();
	}else if(e==3){
		paths = g.test_algorithmE();
	}else if(e==4){
		paths = g.test_algorithm2bitE();
	}else if(e==5){
		paths = g.test_algorithm3bitE();
	}else if(e==6){
		paths = g.test_algorithm22();
	}else if(e==7){
		paths = g.test_algorithm23();
	}else if(e==8){
		paths = g.test_algorithm24(atof(argv[3]));
	}
	//Print the paths/netlists that you return from your algorithm
	for(unsigned i = 0; i < paths.size(); i++) {
		cout << "Path " << i << ": ";
		paths.at(i)->print();
		Path* temp = paths.at(i);
		delete temp;
	}

	paths.clear();

	delete first_problem;

	return 0;
}
