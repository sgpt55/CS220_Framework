#include "../Headers/grid.h"
#include "../Headers/edge.h"
#include "../Headers/claim.h"

//Takes an x and y coordinate as input and creates a grid of that size filled with default nodes
Utilities::Grid::Grid(ProblemObject* problem_object) {
	this->num_connections = problem_object->get_connections().size();
	this->connections = problem_object->get_connections();
	int height = problem_object->get_height();
	int width = problem_object->get_width();
	for (int y = 0; y < height; y++) {
		vector<Node*> temp_row;
		for (int x = 0; x < width; x++) {
			Node* new_node = new Node(x, y, 0);
			/*if (x > 0) {
			 Edge* west = new Edge(new_node,temp_row.at(temp_row.size()-1));
			 new_node->add_connection(west);
			 }
			 if (y > 0) {
			 Edge* north = new Edge(new_node,grid.at(y-1).at(x));
			 new_node->add_connection(north);
			 }*/
			temp_row.push_back(new_node);
		}
		this->grid.push_back(temp_row);
	}
	vector<Blocker> blockers = problem_object->get_blockers();
	for (int i = 0; i < blockers.size(); i++) {
		int y = blockers.at(i).height;
		int x = blockers.at(i).width;
		for (int m = 0; m < y; m++) {
			for (int n = 0; n < x; n++) {
				grid.at(blockers.at(i).location.y - 1 + m).at(
						blockers.at(i).location.x - 1 + n)->set_cost(-1);
				//printf("%d %d %d\n",blockers.at(i).location.y+m,blockers.at(i).location.x+n,i);
			}
		}
	}
	//print_grid();
}

//Destructs the grid by deleting each node individually, the node destructors will delete their own set of edges
Utilities::Grid::~Grid() {
	int width = this->get_width();
	int height = this->get_height();
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			delete grid.at(y).at(x);
		}
	}
}

int Utilities::Grid::get_width() {
	//Assumes a perfect rectangle
	return grid.empty() ? 0 : grid.at(0).size();
}

int Utilities::Grid::get_height() {
	//Assumes a perfect rectangle
	return this->grid.size();
}

int Utilities::Grid::get_num_connections() {
	return this->num_connections;
}

Node* Utilities::Grid::get_node(int x, int y) {
	if (y >= this->grid.size()) {
		claim(
				"Attemping to access a node outside of the grid's range (y-value out of range)",
				kError);
		return NULL;
	}
	if (x >= this->grid.at(y).size()) {
		claim(
				"Attemping to access a node outside of the grid's range (x-value out of range)",
				kError);
		return NULL;
	}
	return this->grid.at(y).at(x); //at(...) will throw an out of range exception
}

Node* Utilities::Grid::get_node(Point coord) {
	return this->get_node(coord.x, coord.y);
}

vector<Path*> Utilities::Grid::get_paths() {
	return this->paths;
}

Path* Utilities::Grid::get_path(int i) {
	if (i >= paths.size()) {
		claim("Attempting to access a path outside of the path list's range",
				kError);
	}
	return this->paths.at(i); //at(...) will throw an out of range exception
}

/*
 Since each node has an x/y coordinate, if you pass a Node* into replace node, it will take the node at the passed in
 Nodes location, delete it, and then place the passed in node into the grid at its proper location.
 */
void Utilities::Grid::replace_node(Node* replacement_node) {
	delete this->grid.at(replacement_node->get_y()).at(
			replacement_node->get_x());
	this->grid.at(replacement_node->get_y()).at(replacement_node->get_x())
			= replacement_node;
}

void Utilities::Grid::set_paths(vector<Path*> paths) {
	this->paths = paths;
}

void Utilities::Grid::add_path(Path* path) {
	this->paths.push_back(path);
}

void Utilities::Grid::replace_path(int i, Path* path) {
	if (i >= this->paths.size()) {
		claim("Attemping to replace path outside of the path list's range",
				kError);
	}
	paths.at(i) = path;
}

void Utilities::Grid::remove_path(int i) {
	if (i >= paths.size()) {
		claim("Attempting to remove a path outside of the path list's range",
				kError);
	}
	vector<Path*>::iterator it = this->paths.begin();
	it += i;
	paths.erase(it);
}

//Note, we create random paths just as an example of how to create paths, netlists are created similarly
vector<Path*> Utilities::Grid::test_algorithm() {
	vector<Path*> paths;
	srand(time(NULL));
	Node* node = NULL;
	int number_paths = this->get_num_connections();
	for (int i = 0; i < number_paths; i++) {
		Path* new_path = new Path();
		/*int x = rand() % this->get_width();
		 int y = rand() % this->get_height();
		 int path_length = 1+rand()%10;
		 for (unsigned j = 0;j < path_length;j++) {
		 bool direction = rand()%2;
		 Point head(x,y);
		 direction?x+=1:y+=1;
		 Point tail(x,y);
		 PathSegment* new_segment = new PathSegment(head,tail);
		 new_path->add_segment(new_segment);
		 }*/
		int num = fill(i);
		if (num == -1) {
			printf("source is same as sink\n");
			paths.push_back(new_path);
			this->clear();
			continue;
		}
		printf("\n");
		this->print_grid();//输出二维路线的值
		//calculate path;
		try {

			node = this->grid.at(connections.at(i).source.y).at(connections.at(
					i).source.x);
			num++;//每什么含义，就是让下面从num开始
			while (num != 0) {
				num--;
				int r = rand() % 4;
				if (r == 0)
					goto up;
				if (r == 1)
					goto right;
				if (r == 2)
					goto down;
				if (r == 3)
					goto left;
				start: up: //up
				try {
					if (this->grid.at(node->get_y() - 1).at(node->get_x())->get_cost()
							== num - 1) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x(), node->get_y() - 1);
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y() - 1).at(node->get_x());
						continue;
					}
				} catch (...) {
				}
				right: //right
				try {
					if (this->grid.at(node->get_y()).at(node->get_x() + 1)->get_cost()
							== num - 1) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x() + 1, node->get_y());
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y()).at(node->get_x() + 1);
						continue;
					}
				} catch (...) {
				}
				down: //down
				try {
					if (this->grid.at(node->get_y() + 1).at(node->get_x())->get_cost()
							== num - 1) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x(), node->get_y() + 1);
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y() + 1).at(node->get_x());
						continue;
					}
				} catch (...) {
				}
				left: //left
				try {
					if (this->grid.at(node->get_y()).at(node->get_x() - 1)->get_cost()
							== num - 1) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x() - 1, node->get_y());
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y()).at(node->get_x() - 1);
						continue;
					}
				} catch (...) {
				}
				//		printf("node %d num %d x %d y %d\n",node->get_cost(),num,node->get_x(),node->get_y());
				if (grid.at(node->get_y()).at(node->get_x() - 1)->get_cost()
						!= -3
						&& grid.at(node->get_y() + 1).at(node->get_x())->get_cost()
								!= -3 && grid.at(node->get_y()).at(
						node->get_x() + 1)->get_cost() != -3 && grid.at(
						node->get_y() - 1).at(node->get_x())->get_cost() != -3) {
					goto start;
				}
			}
		} catch (...) {
		}
		Point head(node->get_x(), node->get_y());
		Point tail(connections.at(i).sink.x, connections.at(i).sink.y);
		PathSegment* new_segment = new PathSegment(head, tail);
		new_path->add_segment(new_segment);
		paths.push_back(new_path);
		this->clear();
	}
	return paths;
}

void Utilities::Grid::print_grid() {
	for (int y = 0; y < grid.size(); y++) {
		for (int x = 0; x < grid.at(y).size(); x++) {
			printf("%f ", grid.at(y).at(x)->get_cost());
		}
		printf("\n");
	}
}
//填充表格，直到找到为止
int Utilities::Grid::fill(int i) {
	if (connections.at(i).source.y == connections.at(i).sink.y
			&& connections.at(i).source.x == connections.at(i).sink.x) {
		return -1;
	}
	this->grid.at(connections.at(i).source.y).at(connections.at(i).source.x)->set_cost(
			-2);//0 nothing,-1 blocker,-2 source,-3 dest
	this->grid.at(connections.at(i).sink.y).at(connections.at(i).sink.x)->set_cost(
			-3);
	vector<Node*> list;
	list.push_back(this->grid.at(connections.at(i).sink.y).at(
			connections.at(i).sink.x));
	int j = 0;
	bool flag = false;
	while (true) {//循环查找，每次从list中取值（NodeList先把值全部拷贝过来），然后清楚list，新的结果又放入list。
		j++;
		//printf("list size %d j %d\n",list.size(),j);
		vector<Node*> NodeList(list);
		list.clear();
		for (int i = 0; i < NodeList.size(); i++) {
			//up
			try {
				if (this->grid.at(NodeList.at(i)->get_y() - 1).at(
						NodeList.at(i)->get_x())->get_cost() == 0) {
					this->grid.at(NodeList.at(i)->get_y() - 1).at(
							NodeList.at(i)->get_x()) ->set_cost(j);
					list.push_back(
							this->grid.at(NodeList.at(i)->get_y() - 1).at(
									NodeList.at(i)->get_x()));
				} else if (this->grid.at(NodeList.at(i)->get_y() - 1).at(
						NodeList.at(i)->get_x())->get_cost() == -2) {
					return j;
				}
			} catch (...) {
			}
			//right
			try {
				if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() + 1)->get_cost() == 0) {
					this->grid.at(NodeList.at(i)->get_y()).at(
							NodeList.at(i)->get_x() + 1) ->set_cost(j);
					list.push_back(this->grid.at(NodeList.at(i)->get_y()).at(
							NodeList.at(i)->get_x() + 1));
				} else if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() + 1)->get_cost() == -2) {
					return j;
				}
			} catch (...) {
			}
			//down
			try {
				if (this->grid.at(NodeList.at(i)->get_y() + 1).at(
						NodeList.at(i)->get_x())->get_cost() == 0) {
					this->grid.at(NodeList.at(i)->get_y() + 1).at(
							NodeList.at(i)->get_x()) ->set_cost(j);
					list.push_back(
							this->grid.at(NodeList.at(i)->get_y() + 1).at(
									NodeList.at(i)->get_x()));
				} else if (this->grid.at(NodeList.at(i)->get_y() + 1).at(
						NodeList.at(i)->get_x())->get_cost() == -2) {
					return j;
				}
			} catch (...) {
			}
			//left
			try {
				if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() - 1)->get_cost() == 0) {
					this->grid.at(NodeList.at(i)->get_y()).at(
							NodeList.at(i)->get_x() - 1) ->set_cost(j);
					list.push_back(this->grid.at(NodeList.at(i)->get_y()).at(
							NodeList.at(i)->get_x() - 1));
				} else if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() - 1)->get_cost() == -2) {
					return j;
				}
			} catch (...) {
			}
		}
	}
}
//保留障碍，其他清0
void Utilities::Grid::clear() {
	for (int y = 0; y < grid.size(); y++) {
		for (int x = 0; x < grid.at(y).size(); x++) {
			if (grid.at(y).at(x)->get_cost() != -1) {
				grid.at(y).at(x)->set_cost(0);
			}
		}
	}
}

//填充表格，直到找到为止
int Utilities::Grid::fill2bit(int i) {
	if (connections.at(i).source.y == connections.at(i).sink.y
			&& connections.at(i).source.x == connections.at(i).sink.x) {
		return -1;
	}
	this->grid.at(connections.at(i).source.y).at(connections.at(i).source.x)->set_cost(
			-2);//0 nothing,-1 blocker,-2 source,-3 dest
	this->grid.at(connections.at(i).sink.y).at(connections.at(i).sink.x)->set_cost(
			-3);
	vector<Node*> list;
	list.push_back(this->grid.at(connections.at(i).sink.y).at(
			connections.at(i).sink.x));
	int j = 0;
	int p[4] = { 1, 1, 2, 2 };
	int q[4] = { 1, 2, 2, 1 };
	bool flag = false;
	while (true) {//循环查找，每次从list中取值（NodeList先把值全部拷贝过来），然后清楚list，新的结果又放入list。
		j++;
		j = (j == 5 ? 1 : j);
		//printf("list size %d j %d\n",list.size(),j);
		vector<Node*> NodeList(list);
		list.clear();
		for (int i = 0; i < NodeList.size(); i++) {
			//up
			try {
				if (this->grid.at(NodeList.at(i)->get_y() - 1).at(
						NodeList.at(i)->get_x())->get_cost() == 0) {
					this->grid.at(NodeList.at(i)->get_y() - 1).at(
							NodeList.at(i)->get_x()) ->set_cost(q[p[j - 1] - 1]);
					list.push_back(
							this->grid.at(NodeList.at(i)->get_y() - 1).at(
									NodeList.at(i)->get_x()));
				} else if (this->grid.at(NodeList.at(i)->get_y() - 1).at(
						NodeList.at(i)->get_x())->get_cost() == -2) {
					return j;
				}
			} catch (...) {
			}
			//right
			try {
				if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() + 1)->get_cost() == 0) {
					this->grid.at(NodeList.at(i)->get_y()).at(
							NodeList.at(i)->get_x() + 1) ->set_cost(q[p[j - 1]
							- 1]);
					list.push_back(this->grid.at(NodeList.at(i)->get_y()).at(
							NodeList.at(i)->get_x() + 1));
				} else if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() + 1)->get_cost() == -2) {
					return j;
				}
			} catch (...) {
			}
			//down
			try {
				if (this->grid.at(NodeList.at(i)->get_y() + 1).at(
						NodeList.at(i)->get_x())->get_cost() == 0) {
					this->grid.at(NodeList.at(i)->get_y() + 1).at(
							NodeList.at(i)->get_x()) ->set_cost(q[p[j - 1] - 1]);
					list.push_back(
							this->grid.at(NodeList.at(i)->get_y() + 1).at(
									NodeList.at(i)->get_x()));
				} else if (this->grid.at(NodeList.at(i)->get_y() + 1).at(
						NodeList.at(i)->get_x())->get_cost() == -2) {
					return j;
				}
			} catch (...) {
			}
			//left
			try {
				if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() - 1)->get_cost() == 0) {
					this->grid.at(NodeList.at(i)->get_y()).at(
							NodeList.at(i)->get_x() - 1) ->set_cost(q[p[j - 1]
							- 1]);
					list.push_back(this->grid.at(NodeList.at(i)->get_y()).at(
							NodeList.at(i)->get_x() - 1));
				} else if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() - 1)->get_cost() == -2) {
					return j;
				}
			} catch (...) {
			}
		}
	}
}

//填充表格，直到找到为止
int Utilities::Grid::fill3bit(int i) {
	if (connections.at(i).source.y == connections.at(i).sink.y
			&& connections.at(i).source.x == connections.at(i).sink.x) {
		return -1;
	}
	this->grid.at(connections.at(i).source.y).at(connections.at(i).source.x)->set_cost(
			-2);//0 nothing,-1 blocker,-2 source,-3 dest
	this->grid.at(connections.at(i).sink.y).at(connections.at(i).sink.x)->set_cost(
			-3);
	vector<Node*> list;
	list.push_back(this->grid.at(connections.at(i).sink.y).at(
			connections.at(i).sink.x));
	int j = -1;
	bool flag = false;
	while (true) {//循环查找，每次从list中取值（NodeList先把值全部拷贝过来），然后清楚list，新的结果又放入list。
		j = (j + 1) % 3;
		//printf("list size %d j %d\n",list.size(),j);
		vector<Node*> NodeList(list);
		list.clear();
		for (int i = 0; i < NodeList.size(); i++) {
			//up
			try {
				if (this->grid.at(NodeList.at(i)->get_y() - 1).at(
						NodeList.at(i)->get_x())->get_cost() == 0) {
					this->grid.at(NodeList.at(i)->get_y() - 1).at(
							NodeList.at(i)->get_x()) ->set_cost(j + 1);
					list.push_back(
							this->grid.at(NodeList.at(i)->get_y() - 1).at(
									NodeList.at(i)->get_x()));
				} else if (this->grid.at(NodeList.at(i)->get_y() - 1).at(
						NodeList.at(i)->get_x())->get_cost() == -2) {
					return j + 1;
				}
			} catch (...) {
			}
			//right
			try {
				if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() + 1)->get_cost() == 0) {
					this->grid.at(NodeList.at(i)->get_y()).at(
							NodeList.at(i)->get_x() + 1) ->set_cost(j + 1);
					list.push_back(this->grid.at(NodeList.at(i)->get_y()).at(
							NodeList.at(i)->get_x() + 1));
				} else if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() + 1)->get_cost() == -2) {
					return j + 1;
				}
			} catch (...) {
			}
			//down
			try {
				if (this->grid.at(NodeList.at(i)->get_y() + 1).at(
						NodeList.at(i)->get_x())->get_cost() == 0) {
					this->grid.at(NodeList.at(i)->get_y() + 1).at(
							NodeList.at(i)->get_x()) ->set_cost(j + 1);
					list.push_back(
							this->grid.at(NodeList.at(i)->get_y() + 1).at(
									NodeList.at(i)->get_x()));
				} else if (this->grid.at(NodeList.at(i)->get_y() + 1).at(
						NodeList.at(i)->get_x())->get_cost() == -2) {
					return j + 1;
				}
			} catch (...) {
			}
			//left
			try {
				if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() - 1)->get_cost() == 0) {
					this->grid.at(NodeList.at(i)->get_y()).at(
							NodeList.at(i)->get_x() - 1) ->set_cost(j + 1);
					list.push_back(this->grid.at(NodeList.at(i)->get_y()).at(
							NodeList.at(i)->get_x() - 1));
				} else if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() - 1)->get_cost() == -2) {
					return j + 1;
				}
			} catch (...) {
			}
		}
	}
}

vector<Path*> Utilities::Grid::test_algorithm2bit() {
	vector<Path*> paths;
	srand(time(NULL));
	Node* node = NULL;
	int p[4] = { 1, 1, 2, 2 };
	int q[4] = { 1, 2, 2, 1 };
	int number_paths = this->get_num_connections();
	for (int i = 0; i < number_paths; i++) {
		Path* new_path = new Path();
		/*int x = rand() % this->get_width();
		 int y = rand() % this->get_height();
		 int path_length = 1+rand()%10;
		 for (unsigned j = 0;j < path_length;j++) {
		 bool direction = rand()%2;
		 Point head(x,y);
		 direction?x+=1:y+=1;
		 Point tail(x,y);
		 PathSegment* new_segment = new PathSegment(head,tail);
		 new_path->add_segment(new_segment);
		 }*/
		int num = fill2bit(i);
		if (num == -1) {
			printf("source is same as sink\n");
			paths.push_back(new_path);
			this->clear();
			continue;
		}
		printf("\n");
		this->print_grid();//输出二维路线的值
		//calculate path;
		try {

			node = this->grid.at(connections.at(i).source.y).at(connections.at(
					i).source.x);
			while (node->get_cost() != -3) {
				num = num - 1;
				num = (num == 0 ? 4 : num);
				int r = q[p[num - 1] - 1];
				int rr = rand() % 4;
				if (rr == 0)
					goto up;
				if (rr == 1)
					goto right;
				if (rr == 2)
					goto down;
				if (rr == 3)
					goto left;
				start: up:
				//    	printf("%d %d %d %d\n",num,node->get_x(),node->get_y(),node->get_cost());
				//up
				try {
					if (this->grid.at(node->get_y() - 1).at(node->get_x())->get_cost()
							== r) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x(), node->get_y() - 1);
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y() - 1).at(node->get_x());
						continue;
					} else if (this->grid.at(node->get_y() - 1).at(
							node->get_x())->get_cost() == -3) {
						break;
					}
				} catch (...) {
				}
				right: //right
				try {
					if (this->grid.at(node->get_y()).at(node->get_x() + 1)->get_cost()
							== r) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x() + 1, node->get_y());
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y()).at(node->get_x() + 1);
						continue;
					} else if (this->grid.at(node->get_y()).at(node->get_x()
							+ 1)->get_cost() == -3) {
						break;
					}
				} catch (...) {
				}
				down: //down
				try {
					if (this->grid.at(node->get_y() + 1).at(node->get_x())->get_cost()
							== r) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x(), node->get_y() + 1);
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y() + 1).at(node->get_x());
						continue;
					} else if (this->grid.at(node->get_y() + 1).at(
							node->get_x())->get_cost() == -3) {
						break;
					}
				} catch (...) {
				}
				left: //left
				try {
					if (this->grid.at(node->get_y()).at(node->get_x() - 1)->get_cost()
							== r) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x() - 1, node->get_y());
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y()).at(node->get_x() - 1);
						continue;
					} else if (this->grid.at(node->get_y()).at(node->get_x()
							- 1)->get_cost() == -3) {
						break;
					}
				} catch (...) {
				}
				if (grid.at(node->get_y()).at(node->get_x() - 1)->get_cost()
						!= -3
						&& grid.at(node->get_y() + 1).at(node->get_x())->get_cost()
								!= -3 && grid.at(node->get_y()).at(
						node->get_x() + 1)->get_cost() != -3 && grid.at(
						node->get_y() - 1).at(node->get_x())->get_cost() != -3) {
					goto start;
				}
			}
		} catch (...) {
		}
		Point head(node->get_x(), node->get_y());
		Point tail(connections.at(i).sink.x, connections.at(i).sink.y);
		PathSegment* new_segment = new PathSegment(head, tail);
		new_path->add_segment(new_segment);
		paths.push_back(new_path);
		this->clear();
	}
	return paths;
}

vector<Path*> Utilities::Grid::test_algorithm3bit() {
	vector<Path*> paths;
	srand(time(NULL));
	Node* node = NULL;
	int number_paths = this->get_num_connections();
	for (int i = 0; i < number_paths; i++) {
		Path* new_path = new Path();
		/*int x = rand() % this->get_width();
		 int y = rand() % this->get_height();
		 int path_length = 1+rand()%10;
		 for (unsigned j = 0;j < path_length;j++) {
		 bool direction = rand()%2;
		 Point head(x,y);
		 direction?x+=1:y+=1;
		 Point tail(x,y);
		 PathSegment* new_segment = new PathSegment(head,tail);
		 new_path->add_segment(new_segment);
		 }*/
		int num = fill3bit(i);
		if (num == -1) {
			printf("source is same as sink\n");
			paths.push_back(new_path);
			this->clear();
			continue;
		}
		printf("%d\n", num);
		this->print_grid();//输出二维路线的值
		//calculate path;
		try {

			node = this->grid.at(connections.at(i).source.y).at(connections.at(
					i).source.x);
			while (node->get_cost() != -3) {
				bool find = false;
				num--;
				num = (num == 0 ? 3 : num);
				int rr = rand() % 4;
				if (rr == 0)
					goto up;
				if (rr == 1)
					goto right;
				if (rr == 2)
					goto down;
				if (rr == 3)
					goto left;
				start: up:
				//up
				try {
					if (this->grid.at(node->get_y() - 1).at(node->get_x())->get_cost()
							== num) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x(), node->get_y() - 1);
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y() - 1).at(node->get_x());
						find = true;
						continue;
					} else if (this->grid.at(node->get_y() - 1).at(
							node->get_x())->get_cost() == -3) {
						break;
					}
				} catch (...) {
				}
				right: try {
					if (this->grid.at(node->get_y()).at(node->get_x() + 1)->get_cost()
							== num) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x() + 1, node->get_y());
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y()).at(node->get_x() + 1);
						find = true;
						continue;
					} else if (this->grid.at(node->get_y()).at(node->get_x()
							+ 1)->get_cost() == -3) {
						break;
					}
				} catch (...) {
				}
				down: try {
					if (this->grid.at(node->get_y() + 1).at(node->get_x())->get_cost()
							== num) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x(), node->get_y() + 1);
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y() + 1).at(node->get_x());
						find = true;
						continue;
					} else if (this->grid.at(node->get_y() + 1).at(
							node->get_x())->get_cost() == -3) {
						break;
					}
				} catch (...) {
				}
				left: try {
					if (this->grid.at(node->get_y()).at(node->get_x() - 1)->get_cost()
							== num) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x() - 1, node->get_y());
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y()).at(node->get_x() - 1);
						find = true;
						continue;
					} else if (this->grid.at(node->get_y()).at(node->get_x()
							- 1)->get_cost() == -3) {
						break;
					}
				} catch (...) {
				}
				//		printf("node %d num %d x %d y %d\n",node->get_cost(),num,node->get_x(),node->get_y());
				if (grid.at(node->get_y()).at(node->get_x() - 1)->get_cost()
						!= -3
						&& grid.at(node->get_y() + 1).at(node->get_x())->get_cost()
								!= -3 && grid.at(node->get_y()).at(
						node->get_x() + 1)->get_cost() != -3 && grid.at(
						node->get_y() - 1).at(node->get_x())->get_cost() != -3) {
					goto start;
				}
				if (node->get_cost() == -3) {
					break;
				}
				if (!find) {
					PathSegment* new_segment = new_path->at(
							new_path->get_length() - 1);
					new_path->remove_segment(new_path->get_length() - 1);
					Point tail = new_segment->get_sink();
					//delete new_segment;
					node->set_coord(tail.x, tail.y);
					num = node->get_cost();
					node->set_cost(0);
				}
			}
		} catch (...) {
		}
		Point head(node->get_x(), node->get_y());
		Point tail(connections.at(i).sink.x, connections.at(i).sink.y);
		PathSegment* new_segment = new PathSegment(head, tail);
		new_path->add_segment(new_segment);
		paths.push_back(new_path);
		this->clear();
	}
	return paths;
}

vector<Path*> Utilities::Grid::test_algorithmE() {
	vector<Path*> paths;
	srand(time(NULL));
	Node* node = NULL;
	int number_paths = this->get_num_connections();
	for (int i = 0; i < number_paths; i++) {
		Path* new_path = new Path();
		int num = fill(i);
		if (num == -1) {
			printf("source is same as sink\n");
			paths.push_back(new_path);
			this->clear();
			continue;
		}
		printf("\n");
		this->print_grid();//输出二维路线的值
		//calculate path;
		try {

			node = this->grid.at(connections.at(i).source.y).at(connections.at(
					i).source.x);
			num++;//每什么含义，就是让下面从num开始
			int d = -1;
			while (num != 0) {
				num--;
				int r = rand() % 4;
				d = r;
				start: if (d != -1) {
					if (d == 0)
						goto up;
					if (d == 1)
						goto right;
					if (d == 2)
						goto down;
					if (d == 3)
						goto left;
				}
				up: //up
				try {
					if (this->grid.at(node->get_y() - 1).at(node->get_x())->get_cost()
							== num - 1) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x(), node->get_y() - 1);
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y() - 1).at(node->get_x());
						d = 0;
						continue;
					}
				} catch (...) {
				}
				right: //right
				try {
					if (this->grid.at(node->get_y()).at(node->get_x() + 1)->get_cost()
							== num - 1) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x() + 1, node->get_y());
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y()).at(node->get_x() + 1);
						d = 1;
						continue;
					}
				} catch (...) {
				}
				down: //down
				try {
					if (this->grid.at(node->get_y() + 1).at(node->get_x())->get_cost()
							== num - 1) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x(), node->get_y() + 1);
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y() + 1).at(node->get_x());
						d = 2;
						continue;
					}
				} catch (...) {
				}
				left: //left
				try {
					if (this->grid.at(node->get_y()).at(node->get_x() - 1)->get_cost()
							== num - 1) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x() - 1, node->get_y());
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y()).at(node->get_x() - 1);
						d = 3;
						continue;
					}
				} catch (...) {
				}
				//		printf("node %d num %d x %d y %d\n",node->get_cost(),num,node->get_x(),node->get_y());
				if (grid.at(node->get_y()).at(node->get_x() - 1)->get_cost()
						!= -3
						&& grid.at(node->get_y() + 1).at(node->get_x())->get_cost()
								!= -3 && grid.at(node->get_y()).at(
						node->get_x() + 1)->get_cost() != -3 && grid.at(
						node->get_y() - 1).at(node->get_x())->get_cost() != -3) {
					d = -1;
					goto start;
				}
			}
		} catch (...) {
		}
		Point head(node->get_x(), node->get_y());
		Point tail(connections.at(i).sink.x, connections.at(i).sink.y);
		PathSegment* new_segment = new PathSegment(head, tail);
		new_path->add_segment(new_segment);
		paths.push_back(new_path);
		this->clear();
	}
	return paths;
}
vector<Path*> Utilities::Grid::test_algorithm2bitE() {
	vector<Path*> paths;
	srand(time(NULL));
	Node* node = NULL;
	int p[4] = { 1, 1, 2, 2 };
	int q[4] = { 1, 2, 2, 1 };
	int number_paths = this->get_num_connections();
	for (int i = 0; i < number_paths; i++) {
		Path* new_path = new Path();
		/*int x = rand() % this->get_width();
		 int y = rand() % this->get_height();
		 int path_length = 1+rand()%10;
		 for (unsigned j = 0;j < path_length;j++) {
		 bool direction = rand()%2;
		 Point head(x,y);
		 direction?x+=1:y+=1;
		 Point tail(x,y);
		 PathSegment* new_segment = new PathSegment(head,tail);
		 new_path->add_segment(new_segment);
		 }*/
		int num = fill2bit(i);
		if (num == -1) {
			printf("source is same as sink\n");
			paths.push_back(new_path);
			this->clear();
			continue;
		}
		printf("\n");
		this->print_grid();//输出二维路线的值
		//calculate path;
		try {

			node = this->grid.at(connections.at(i).source.y).at(connections.at(
					i).source.x);
			int d = -1;
			while (node->get_cost() != -3) {
				num = num - 1;
				num = (num == 0 ? 4 : num);
				int r = q[p[num - 1] - 1];
				int rr = rand() % 4;
				d = rr;
				start: if (d != -1) {
					if (d == 0)
						goto up;
					if (d == 1)
						goto right;
					if (d == 2)
						goto down;
					if (d == 3)
						goto left;
				}
				up:
				//    	printf("%d %d %d %d\n",num,node->get_x(),node->get_y(),node->get_cost());
				//up
				try {
					if (this->grid.at(node->get_y() - 1).at(node->get_x())->get_cost()
							== r) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x(), node->get_y() - 1);
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y() - 1).at(node->get_x());
						d = 0;
						continue;
					} else if (this->grid.at(node->get_y() - 1).at(
							node->get_x())->get_cost() == -3) {
						break;
					}
				} catch (...) {
				}
				right: //right
				try {
					if (this->grid.at(node->get_y()).at(node->get_x() + 1)->get_cost()
							== r) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x() + 1, node->get_y());
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y()).at(node->get_x() + 1);
						d = 1;
						continue;
					} else if (this->grid.at(node->get_y()).at(node->get_x()
							+ 1)->get_cost() == -3) {
						break;
					}
				} catch (...) {
				}
				down: //down
				try {
					if (this->grid.at(node->get_y() + 1).at(node->get_x())->get_cost()
							== r) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x(), node->get_y() + 1);
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y() + 1).at(node->get_x());
						d = 2;
						continue;
					} else if (this->grid.at(node->get_y() + 1).at(
							node->get_x())->get_cost() == -3) {
						break;
					}
				} catch (...) {
				}
				left: //left
				try {
					if (this->grid.at(node->get_y()).at(node->get_x() - 1)->get_cost()
							== r) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x() - 1, node->get_y());
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y()).at(node->get_x() - 1);
						d = 3;
						continue;
					} else if (this->grid.at(node->get_y()).at(node->get_x()
							- 1)->get_cost() == -3) {
						break;
					}
				} catch (...) {
				}
				if (grid.at(node->get_y()).at(node->get_x() - 1)->get_cost()
						!= -3
						&& grid.at(node->get_y() + 1).at(node->get_x())->get_cost()
								!= -3 && grid.at(node->get_y()).at(
						node->get_x() + 1)->get_cost() != -3 && grid.at(
						node->get_y() - 1).at(node->get_x())->get_cost() != -3) {
					d = -1;
					goto start;
				}
			}
		} catch (...) {
		}
		Point head(node->get_x(), node->get_y());
		Point tail(connections.at(i).sink.x, connections.at(i).sink.y);
		PathSegment* new_segment = new PathSegment(head, tail);
		new_path->add_segment(new_segment);
		paths.push_back(new_path);
		this->clear();
	}
	return paths;
}

vector<Path*> Utilities::Grid::test_algorithm3bitE() {
	vector<Path*> paths;
	srand(time(NULL));
	Node* node = NULL;
	int number_paths = this->get_num_connections();
	for (int i = 0; i < number_paths; i++) {
		Path* new_path = new Path();
		int num = fill3bit(i);
		if (num == -1) {
			printf("source is same as sink\n");
			paths.push_back(new_path);
			this->clear();
			continue;
		}
		printf("%d\n", num);
		this->print_grid();//输出二维路线的值
		//calculate path;
		try {

			node = this->grid.at(connections.at(i).source.y).at(connections.at(
					i).source.x);
			int d = -1;
			while (node->get_cost() != -3) {
				bool find = false;
				num--;
				num = (num == 0 ? 3 : num);
				int rr = rand() % 4;
				d = rr;
				start: if (d != -1) {
					if (d == 0)
						goto up;
					if (d == 1)
						goto right;
					if (d == 2)
						goto down;
					if (d == 3)
						goto left;
				}
				up:
				//up
				try {
					if (this->grid.at(node->get_y() - 1).at(node->get_x())->get_cost()
							== num) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x(), node->get_y() - 1);
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y() - 1).at(node->get_x());
						find = true;
						d = 0;
						continue;
					} else if (this->grid.at(node->get_y() - 1).at(
							node->get_x())->get_cost() == -3) {
						break;
					}
				} catch (...) {
				}
				right: try {
					if (this->grid.at(node->get_y()).at(node->get_x() + 1)->get_cost()
							== num) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x() + 1, node->get_y());
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y()).at(node->get_x() + 1);
						find = true;
						d = 1;
						continue;
					} else if (this->grid.at(node->get_y()).at(node->get_x()
							+ 1)->get_cost() == -3) {
						break;
					}
				} catch (...) {
				}
				down: try {
					if (this->grid.at(node->get_y() + 1).at(node->get_x())->get_cost()
							== num) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x(), node->get_y() + 1);
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y() + 1).at(node->get_x());
						find = true;
						d = 2;
						continue;
					} else if (this->grid.at(node->get_y() + 1).at(
							node->get_x())->get_cost() == -3) {
						break;
					}
				} catch (...) {
				}
				left: try {
					if (this->grid.at(node->get_y()).at(node->get_x() - 1)->get_cost()
							== num) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x() - 1, node->get_y());
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y()).at(node->get_x() - 1);
						find = true;
						d = 3;
						continue;
					} else if (this->grid.at(node->get_y()).at(node->get_x()
							- 1)->get_cost() == -3) {
						break;
					}
				} catch (...) {
				}
				//		printf("node %d num %d x %d y %d\n",node->get_cost(),num,node->get_x(),node->get_y());
				if (grid.at(node->get_y()).at(node->get_x() - 1)->get_cost()
						!= -3
						&& grid.at(node->get_y() + 1).at(node->get_x())->get_cost()
								!= -3 && grid.at(node->get_y()).at(
						node->get_x() + 1)->get_cost() != -3 && grid.at(
						node->get_y() - 1).at(node->get_x())->get_cost() != -3) {
					d = -1;
					goto start;
				}
				if (node->get_cost() == -3) {
					break;
				}
				if (!find) {
					PathSegment* new_segment = new_path->at(
							new_path->get_length() - 1);
					new_path->remove_segment(new_path->get_length() - 1);
					Point tail = new_segment->get_sink();
					//delete new_segment;
					node->set_coord(tail.x, tail.y);
					num = node->get_cost();
					node->set_cost(0);
				}
			}
		} catch (...) {
		}
		Point head(node->get_x(), node->get_y());
		Point tail(connections.at(i).sink.x, connections.at(i).sink.y);
		PathSegment* new_segment = new PathSegment(head, tail);
		new_path->add_segment(new_segment);
		paths.push_back(new_path);
		this->clear();
	}
	return paths;
}

int Utilities::Grid::fill23(int i) {
	if (connections.at(i).source.y == connections.at(i).sink.y
			&& connections.at(i).source.x == connections.at(i).sink.x) {
		return -1;
	}
	this->grid.at(connections.at(i).source.y).at(connections.at(i).source.x)->set_cost(
			-2);//0 nothing,-1 blocker,-2 source,-3 dest
	this->grid.at(connections.at(i).sink.y).at(connections.at(i).sink.x)->set_cost(
			-3);
	vector<Node*> list;
	list.push_back(this->grid.at(connections.at(i).sink.y).at(
			connections.at(i).sink.x));
	int j = 0;
	bool flag = false;
	int d = 0;
	while (true) {//循环查找，每次从list中取值（NodeList先把值全部拷贝过来），然后清楚list，新的结果又放入list。
		j++;
		//printf("list size %d j %d\n",list.size(),j);
		vector<Node*> NodeList(list);
		list.clear();
		for (int i = 0; i < NodeList.size(); i++) {
			//up
			try {
				if (this->grid.at(NodeList.at(i)->get_y() - 1).at(
						NodeList.at(i)->get_x())->get_cost() == 0) {
					this->grid.at(NodeList.at(i)->get_y() - 1).at(
							NodeList.at(i)->get_x()) ->set_cost(j);
					d = 0;
					list.push_back(
							this->grid.at(NodeList.at(i)->get_y() - 1).at(
									NodeList.at(i)->get_x()));
				} else if (this->grid.at(NodeList.at(i)->get_y() - 1).at(
						NodeList.at(i)->get_x())->get_cost() == -2) {
					return j;
				} else if (this->grid.at(NodeList.at(i)->get_y() - 1).at(
						NodeList.at(i)->get_x())->get_cost() == -1) {
					this->grid.at(NodeList.at(i)->get_y() - 1).at(
							NodeList.at(i)->get_x()) ->set_cost(12);
				}
			} catch (...) {
			}
			//right
			try {
				if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() + 1)->get_cost() == 0) {
					this->grid.at(NodeList.at(i)->get_y()).at(
							NodeList.at(i)->get_x() + 1) ->set_cost(j);
					d = 1;
					list.push_back(this->grid.at(NodeList.at(i)->get_y()).at(
							NodeList.at(i)->get_x() + 1));
				} else if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() + 1)->get_cost() == -2) {
					return j;
				} else if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() + 1)->get_cost() == -1) {
					this->grid.at(NodeList.at(i)->get_y()).at(
							NodeList.at(i)->get_x() + 1) ->set_cost(12);
				}
			} catch (...) {
			}
			//down
			try {
				if (this->grid.at(NodeList.at(i)->get_y() + 1).at(
						NodeList.at(i)->get_x())->get_cost() == 0) {
					this->grid.at(NodeList.at(i)->get_y() + 1).at(
							NodeList.at(i)->get_x()) ->set_cost(j);
					d = 2;
					list.push_back(
							this->grid.at(NodeList.at(i)->get_y() + 1).at(
									NodeList.at(i)->get_x()));
				} else if (this->grid.at(NodeList.at(i)->get_y() + 1).at(
						NodeList.at(i)->get_x())->get_cost() == -2) {
					return j;
				} else if (this->grid.at(NodeList.at(i)->get_y() + 1).at(
						NodeList.at(i)->get_x())->get_cost() == -1) {
					this->grid.at(NodeList.at(i)->get_y() + 1).at(
							NodeList.at(i)->get_x()) ->set_cost(12);
				}
			} catch (...) {
			}
			//left
			try {
				if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() - 1)->get_cost() == 0) {
					this->grid.at(NodeList.at(i)->get_y()).at(
							NodeList.at(i)->get_x() - 1) ->set_cost(j);
					d = 3;
					list.push_back(this->grid.at(NodeList.at(i)->get_y()).at(
							NodeList.at(i)->get_x() - 1));
				} else if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() - 1)->get_cost() == -2) {
					return j;
				} else if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() - 1)->get_cost() == -1) {
					this->grid.at(NodeList.at(i)->get_y()).at(
							NodeList.at(i)->get_x() - 1) ->set_cost(12);
				}
			} catch (...) {
			}
		}
	}
}
vector<Path*> Utilities::Grid::test_algorithm23() {
	vector<Path*> paths;
	srand(time(NULL));
	Node* node = NULL;
	int number_paths = this->get_num_connections();
	for (int i = 0; i < number_paths; i++) {
		Path* new_path = new Path();
		int num = fill23(i);
		if (num == -1) {
			printf("source is same as sink\n");
			paths.push_back(new_path);
			this->clear();
			continue;
		}
		printf("\n");
		this->print_grid();//输出二维路线的值
		//calculate path;
		try {

			node = this->grid.at(connections.at(i).source.y).at(connections.at(
					i).source.x);
			num++;
			int d = -1;
			while (num != 0) {
				num--;
				int r = rand() % 4;
				d = r;
				start:
				//printf("node %d num %d x %d y %d\n",node->get_cost(),num,node->get_x(),node->get_y());
				if (d != -1) {
					if (d == 0)
						goto up;
					if (d == 1)
						goto right;
					if (d == 2)
						goto down;
					if (d == 3)
						goto left;
				}
				up: //up
				try {
					if (this->grid.at(node->get_y() - 1).at(node->get_x())->get_cost()
							== num - 1) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x(), node->get_y() - 1);
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y() - 1).at(node->get_x());
						d = 0;
						continue;
					}
				} catch (...) {
				}
				right: //right
				try {
					if (this->grid.at(node->get_y()).at(node->get_x() + 1)->get_cost()
							== num - 1) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x() + 1, node->get_y());
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y()).at(node->get_x() + 1);
						d = 1;
						continue;
					}
				} catch (...) {
				}
				down: //down
				try {
					if (this->grid.at(node->get_y() + 1).at(node->get_x())->get_cost()
							== num - 1) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x(), node->get_y() + 1);
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y() + 1).at(node->get_x());
						d = 2;
						continue;
					}
				} catch (...) {
				}
				left: //left
				try {
					if (this->grid.at(node->get_y()).at(node->get_x() - 1)->get_cost()
							== num - 1) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x() - 1, node->get_y());
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y()).at(node->get_x() - 1);
						d = 3;
						continue;
					}
				} catch (...) {
				}
				//printf("end node %d num %d x %d y %d\n",node->get_cost(),num,node->get_x(),node->get_y());
				if (grid.at(node->get_y()).at(node->get_x() - 1)->get_cost()
						!= -3
						&& grid.at(node->get_y() + 1).at(node->get_x())->get_cost()
								!= -3 && grid.at(node->get_y()).at(
						node->get_x() + 1)->get_cost() != -3 && grid.at(
						node->get_y() - 1).at(node->get_x())->get_cost() != -3) {
					d = -1;
					goto start;
				} else {
					break;
				}
			}
		} catch (...) {
		}
		//      printf("end node %d num %d x %d y %d\n",node->get_cost(),num,node->get_x(),node->get_y());
		Point head(node->get_x(), node->get_y());
		Point tail(connections.at(i).sink.x, connections.at(i).sink.y);
		PathSegment* new_segment = new PathSegment(head, tail);
		new_path->add_segment(new_segment);
		paths.push_back(new_path);
		this->clear();
	}
	return paths;
}
int Utilities::Grid::fill22(int i) {
	if (connections.at(i).source.y == connections.at(i).sink.y
			&& connections.at(i).source.x == connections.at(i).sink.x) {
		return -1;
	}
	this->grid.at(connections.at(i).source.y).at(connections.at(i).source.x)->set_cost(
			-2);//0 nothing,-1 blocker,-2 source,-3 dest
	this->grid.at(connections.at(i).sink.y).at(connections.at(i).sink.x)->set_cost(
			-3);
	vector<Node*> list;
	list.push_back(this->grid.at(connections.at(i).sink.y).at(
			connections.at(i).sink.x));
	int j = 0;
	bool flag = false;
	int d = 0;
	while (true) {//循环查找，每次从list中取值（NodeList先把值全部拷贝过来），然后清楚list，新的结果又放入list。
		j = 10;
		//printf("list size %d j %d\n",list.size(),j);
		vector<Node*> NodeList(list);
		list.clear();
		for (int i = 0; i < NodeList.size(); i++) {
			//up
			try {
				if (this->grid.at(NodeList.at(i)->get_y() - 1).at(
						NodeList.at(i)->get_x())->get_cost() == 0) {
					this->grid.at(NodeList.at(i)->get_y() - 1).at(
							NodeList.at(i)->get_x()) ->set_cost(j);
					d = 0;
					list.push_back(
							this->grid.at(NodeList.at(i)->get_y() - 1).at(
									NodeList.at(i)->get_x()));
					continue;
				} else if (this->grid.at(NodeList.at(i)->get_y() - 1).at(
						NodeList.at(i)->get_x())->get_cost() == -2) {
					return j;
				} else if (this->grid.at(NodeList.at(i)->get_y() - 1).at(
						NodeList.at(i)->get_x())->get_cost() == -1) {
					this->grid.at(NodeList.at(i)->get_y() - 1).at(
							NodeList.at(i)->get_x()) ->set_cost(12);
				}
			} catch (...) {
			}
			//right
			try {
				if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() + 1)->get_cost() == 0) {
					this->grid.at(NodeList.at(i)->get_y()).at(
							NodeList.at(i)->get_x() + 1) ->set_cost(j);
					d = 1;
					list.push_back(this->grid.at(NodeList.at(i)->get_y()).at(
							NodeList.at(i)->get_x() + 1));
					continue;
				} else if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() + 1)->get_cost() == -2) {
					return j;
				} else if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() + 1)->get_cost() == -1) {
					this->grid.at(NodeList.at(i)->get_y()).at(
							NodeList.at(i)->get_x() + 1) ->set_cost(12);
				}
			} catch (...) {
			}
			//down
			try {
				if (this->grid.at(NodeList.at(i)->get_y() + 1).at(
						NodeList.at(i)->get_x())->get_cost() == 0) {
					this->grid.at(NodeList.at(i)->get_y() + 1).at(
							NodeList.at(i)->get_x()) ->set_cost(j);
					d = 2;
					list.push_back(
							this->grid.at(NodeList.at(i)->get_y() + 1).at(
									NodeList.at(i)->get_x()));
					continue;
				} else if (this->grid.at(NodeList.at(i)->get_y() + 1).at(
						NodeList.at(i)->get_x())->get_cost() == -2) {
					return j;
				} else if (this->grid.at(NodeList.at(i)->get_y() + 1).at(
						NodeList.at(i)->get_x())->get_cost() == -1) {
					this->grid.at(NodeList.at(i)->get_y() + 1).at(
							NodeList.at(i)->get_x()) ->set_cost(12);
				}
			} catch (...) {
			}
			//left
			try {
				if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() - 1)->get_cost() == 0) {
					this->grid.at(NodeList.at(i)->get_y()).at(
							NodeList.at(i)->get_x() - 1) ->set_cost(j);
					d = 3;
					list.push_back(this->grid.at(NodeList.at(i)->get_y()).at(
							NodeList.at(i)->get_x() - 1));
					continue;
				} else if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() - 1)->get_cost() == -2) {
					return j;
				} else if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() - 1)->get_cost() == -1) {
					this->grid.at(NodeList.at(i)->get_y()).at(
							NodeList.at(i)->get_x() - 1) ->set_cost(12);
				}
			} catch (...) {
			}
		}
	}
}
vector<Path*> Utilities::Grid::test_algorithm22() {
	vector<Path*> paths;
	srand(time(NULL));
	Node* node = NULL;
	int number_paths = this->get_num_connections();
	for (int i = 0; i < number_paths; i++) {
		Path* new_path = new Path();
		int num = fill22(i);
		if (num == -1) {
			printf("source is same as sink\n");
			paths.push_back(new_path);
			this->clear();
			continue;
		}
		printf("\n");
		this->print_grid();//输出二维路线的值
		//calculate path;
		try {

			node = this->grid.at(connections.at(i).source.y).at(connections.at(
					i).source.x);
			int d = -1;
			while (node->get_cost() != -3) {
				start:
				//		printf("node %d num %d x %d y %d\n",node->get_cost(),num,node->get_x(),node->get_y());
				if (d != -1) {
					if (d == 0)
						goto up;
					if (d == 1)
						goto right;
					if (d == 2)
						goto down;
					if (d == 3)
						goto left;
				}
				up: //up
				try {
					if (this->grid.at(node->get_y() - 1).at(node->get_x())->get_cost()
							== 10) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x(), node->get_y() - 1);
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y() - 1).at(node->get_x());
						d = 0;
						continue;
					}
				} catch (...) {
				}
				right: //right
				try {
					if (this->grid.at(node->get_y()).at(node->get_x() + 1)->get_cost()
							== 10) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x() + 1, node->get_y());
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y()).at(node->get_x() + 1);
						d = 1;
						continue;
					}
				} catch (...) {
				}
				down: //down
				try {
					if (this->grid.at(node->get_y() + 1).at(node->get_x())->get_cost()
							== 10) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x(), node->get_y() + 1);
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y() + 1).at(node->get_x());
						d = 2;
						continue;
					}
				} catch (...) {
				}
				left: //left
				try {
					if (this->grid.at(node->get_y()).at(node->get_x() - 1)->get_cost()
							== 10) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x() - 1, node->get_y());
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y()).at(node->get_x() - 1);
						d = 3;
						continue;
					}
				} catch (...) {
				}
				//printf("node %d num %d x %d y %d\n",node->get_cost(),num,node->get_x(),node->get_y());
				if (grid.at(node->get_y()).at(node->get_x() - 1)->get_cost()
						!= -3
						&& grid.at(node->get_y() + 1).at(node->get_x())->get_cost()
								!= -3 && grid.at(node->get_y()).at(
						node->get_x() + 1)->get_cost() != -3 && grid.at(
						node->get_y() - 1).at(node->get_x())->get_cost() != -3) {
					d = -1;
					goto start;
				} else {
					break;
				}
			}
		} catch (...) {
		}
		//      printf("end node %d num %d x %d y %d\n",node->get_cost(),num,node->get_x(),node->get_y());
		Point head(node->get_x(), node->get_y());
		Point tail(connections.at(i).sink.x, connections.at(i).sink.y);
		PathSegment* new_segment = new PathSegment(head, tail);
		new_path->add_segment(new_segment);
		paths.push_back(new_path);
		this->clear();
	}
	return paths;
}

int Utilities::Grid::fill24(int k, double m) {
	if (connections.at(k).source.y == connections.at(k).sink.y
			&& connections.at(k).source.x == connections.at(k).sink.x) {
		return -1;
	}
	this->grid.at(connections.at(k).source.y).at(connections.at(k).source.x)->set_cost(
			-2);//0 nothing,-1 blocker,-2 source,-3 dest
	this->grid.at(connections.at(k).sink.y).at(connections.at(k).sink.x)->set_cost(
			-3);
	vector<Node*> list;
	list.push_back(this->grid.at(connections.at(k).sink.y).at(
			connections.at(k).sink.x));
	int j = 0;
	bool flag = false;
	int d = -1;
	while (true) {//循环查找，每次从list中取值（NodeList先把值全部拷贝过来），然后清楚list，新的结果又放入list。
		j++;
		//printf("list size %d j %d m %f\n", list.size(), j, m);
		vector<Node*> NodeList(list);
		list.clear();
		for (int i = 0; i < NodeList.size(); i++) {
			if (d != -1) {
				if (d == 0)
					goto up;
				if (d == 1)
					goto right;
				if (d == 2)
					goto down;
				if (d == 3)
					goto left;
			}
			up: //up
//			printf("NodeList size %d j %d m %f\n", NodeList.size(), j, m);
			try {
				if (this->grid.at(NodeList.at(i)->get_y() - 1).at(
						NodeList.at(i)->get_x())->get_cost() == 0) {
					this->grid.at(NodeList.at(i)->get_y() - 1).at(
							NodeList.at(i)->get_x()) ->set_cost(j + m
							* (abs(NodeList.at(i)->get_y() - 1
									- connections.at(k).sink.y) + abs(
									NodeList.at(i)->get_x()
											- connections.at(k).sink.x)));
					d = 0;
//					this->grid.at(NodeList.at(i)->get_y() - 1).at(
//														NodeList.at(i)->get_x())->direction = 0;
					list.push_back(
							this->grid.at(NodeList.at(i)->get_y() - 1).at(
									NodeList.at(i)->get_x()));
					//left
					if (grid.at(NodeList.at(i)->get_y() - 1).at(
							NodeList.at(i)->get_x() - 1) == 0)
						this->grid.at(NodeList.at(i)->get_y() - 1).at(
								NodeList.at(i)->get_x() - 1) ->set_cost(j + m
								* (abs(NodeList.at(i)->get_y() - 1
										- connections.at(k).sink.y) + abs(
										NodeList.at(i)->get_x() - 1
												- connections.at(k).sink.x)));
					//right
					if (grid.at(NodeList.at(i)->get_y() - 1).at(
							NodeList.at(i)->get_x() + 1) == 0)
						this->grid.at(NodeList.at(i)->get_y() - 1).at(
								NodeList.at(i)->get_x() + 1) ->set_cost(j + m
								* (abs(NodeList.at(i)->get_y() - 1
										- connections.at(k).sink.y) + abs(
										NodeList.at(i)->get_x() + 1
												- connections.at(k).sink.x)));
					continue;
				} else if (this->grid.at(NodeList.at(i)->get_y() - 1).at(
						NodeList.at(i)->get_x())->get_cost() == -2) {
					return j;
				} else if (this->grid.at(NodeList.at(i)->get_y() - 1).at(
						NodeList.at(i)->get_x())->get_cost() == -1) {
					//left
					if (grid.at(NodeList.at(i)->get_y() - 1).at(
							NodeList.at(i)->get_x() - 1) == 0){
						this->grid.at(NodeList.at(i)->get_y() - 1).at(
								NodeList.at(i)->get_x() - 1) ->set_cost(j + m
								* (abs(NodeList.at(i)->get_y() - 1
										- connections.at(k).sink.y) + abs(
										NodeList.at(i)->get_x() - 1
												- connections.at(k).sink.x)));
						list.push_back(grid.at(NodeList.at(i)->get_y() - 1).at(
							NodeList.at(i)->get_x() - 1));
					}
					//right
					if (grid.at(NodeList.at(i)->get_y() - 1).at(
							NodeList.at(i)->get_x() + 1) == 0){
						this->grid.at(NodeList.at(i)->get_y() - 1).at(
								NodeList.at(i)->get_x() + 1) ->set_cost(j + m
								* (abs(NodeList.at(i)->get_y() - 1
										- connections.at(k).sink.y) + abs(
										NodeList.at(i)->get_x() + 1
												- connections.at(k).sink.x)));
						list.push_back(grid.at(NodeList.at(i)->get_y() - 1).at(
													NodeList.at(i)->get_x() + 1));
					}
				}
			} catch (...) {
			}
			right: //right
			try {
				if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() + 1)->get_cost() == 0) {
					this->grid.at(NodeList.at(i)->get_y()).at(
							NodeList.at(i)->get_x() + 1) ->set_cost(j + m
							* (abs(NodeList.at(i)->get_y()
									- connections.at(k).sink.y) + abs(
									NodeList.at(i)->get_x() + 1
											- connections.at(k).sink.x)));
					d = 1;
					list.push_back(this->grid.at(NodeList.at(i)->get_y()).at(
							NodeList.at(i)->get_x() + 1));
					//up
					if (grid.at(NodeList.at(i)->get_y() - 1).at(
							NodeList.at(i)->get_x() + 1) == 0)
						this->grid.at(NodeList.at(i)->get_y() - 1).at(
								NodeList.at(i)->get_x() + 1) ->set_cost(j + m
								* (abs(NodeList.at(i)->get_y() - 1
										- connections.at(k).sink.y) + abs(
										NodeList.at(i)->get_x() + 1
												- connections.at(k).sink.x)));
					//down
					if (grid.at(NodeList.at(i)->get_y() + 1).at(
							NodeList.at(i)->get_x() + 1) == 0)
						this->grid.at(NodeList.at(i)->get_y() + 1).at(
								NodeList.at(i)->get_x() + 1) ->set_cost(j + m
								* (abs(NodeList.at(i)->get_y() + 1
										- connections.at(k).sink.y) + abs(
										NodeList.at(i)->get_x() + 1
												- connections.at(k).sink.x)));
					continue;
				} else if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() + 1)->get_cost() == -2) {
					return j;
				} else if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() + 1)->get_cost() == -1) {
				}
			} catch (...) {
			}
			down: //down
			try {
				if (this->grid.at(NodeList.at(i)->get_y() + 1).at(
						NodeList.at(i)->get_x())->get_cost() == 0) {
					this->grid.at(NodeList.at(i)->get_y() + 1).at(
							NodeList.at(i)->get_x()) ->set_cost(j + m
							* (abs(NodeList.at(i)->get_y() + 1
									- connections.at(k).sink.y) + abs(
									NodeList.at(i)->get_x()
											- connections.at(k).sink.x)));
					d = 2;
					list.push_back(
							this->grid.at(NodeList.at(i)->get_y() + 1).at(
									NodeList.at(i)->get_x()));
					//left
					if (grid.at(NodeList.at(i)->get_y() + 1).at(
							NodeList.at(i)->get_x() - 1) == 0)
						this->grid.at(NodeList.at(i)->get_y() + 1).at(
								NodeList.at(i)->get_x() - 1) ->set_cost(j + m
								* (abs(NodeList.at(i)->get_y() + 1
										- connections.at(k).sink.y) + abs(
										NodeList.at(i)->get_x() - 1
												- connections.at(k).sink.x)));
					//right
					if (grid.at(NodeList.at(i)->get_y() + 1).at(
							NodeList.at(i)->get_x() + 1) == 0)
						this->grid.at(NodeList.at(i)->get_y() + 1).at(
								NodeList.at(i)->get_x() + 1) ->set_cost(j + m
								* (abs(NodeList.at(i)->get_y() + 1
										- connections.at(k).sink.y) + abs(
										NodeList.at(i)->get_x() + 1
												- connections.at(k).sink.x)));
					continue;
				} else if (this->grid.at(NodeList.at(i)->get_y() + 1).at(
						NodeList.at(i)->get_x())->get_cost() == -2) {
					return j;
				} else if (this->grid.at(NodeList.at(i)->get_y() + 1).at(
						NodeList.at(i)->get_x())->get_cost() == -1) {

				}
			} catch (...) {
			}
			left: //left
			try {
				if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() - 1)->get_cost() == 0) {
					this->grid.at(NodeList.at(i)->get_y()).at(
							NodeList.at(i)->get_x() - 1) ->set_cost(j + m
							* (abs(NodeList.at(i)->get_y()
									- connections.at(k).sink.y) + abs(
									NodeList.at(i)->get_x() - 1
											- connections.at(k).sink.x)));
					d = 3;
					list.push_back(this->grid.at(NodeList.at(i)->get_y()).at(
							NodeList.at(i)->get_x() - 1));
					//up
					if (grid.at(NodeList.at(i)->get_y() - 1).at(
							NodeList.at(i)->get_x() - 1) == 0)
						this->grid.at(NodeList.at(i)->get_y() - 1).at(
								NodeList.at(i)->get_x() - 1) ->set_cost(j + m
								* (abs(NodeList.at(i)->get_y() - 1
										- connections.at(k).sink.y) + abs(
										NodeList.at(i)->get_x() - 1
												- connections.at(k).sink.x)));
					//down
					if (grid.at(NodeList.at(i)->get_y() + 1).at(
							NodeList.at(i)->get_x() - 1) == 0)
						this->grid.at(NodeList.at(i)->get_y() + 1).at(
								NodeList.at(i)->get_x() - 1) ->set_cost(j + m
								* (abs(NodeList.at(i)->get_y() + 1
										- connections.at(k).sink.y) + abs(
										NodeList.at(i)->get_x() - 1
												- connections.at(k).sink.x)));
					continue;
				} else if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() - 1)->get_cost() == -2) {
					return j;
				} else if (this->grid.at(NodeList.at(i)->get_y()).at(
						NodeList.at(i)->get_x() - 1)->get_cost() == -1) {

				}
			} catch (...) {
			}
			goto up;
		}
	}
}
vector<Path*> Utilities::Grid::test_algorithm24(double m) {
	vector<Path*> paths;
	srand(time(NULL));
	Node* node = NULL;
	int number_paths = this->get_num_connections();
	for (int i = 0; i < number_paths; i++) {
		Path* new_path = new Path();
		int num = fill24(i, m);
		if (num == -1) {
			printf("source is same as sink\n");
			paths.push_back(new_path);
			this->clear();
			continue;
		}
		printf("\n");
		this->print_grid();//输出二维路线的值
		return paths;
		//calculate path;
		try {

			node = this->grid.at(connections.at(i).source.y).at(connections.at(
					i).source.x);
			int d = -1;
			while (node->get_cost() != -3) {
				start:
				//		printf("node %d num %d x %d y %d\n",node->get_cost(),num,node->get_x(),node->get_y());
				if (d != -1) {
					if (d == 0)
						goto up;
					if (d == 1)
						goto right;
					if (d == 2)
						goto down;
					if (d == 3)
						goto left;
				}
				up: //up
				try {
					if (this->grid.at(node->get_y() - 1).at(node->get_x())->get_cost()
							== 10) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x(), node->get_y() - 1);
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y() - 1).at(node->get_x());
						d = 0;
						continue;
					}
				} catch (...) {
				}
				right: //right
				try {
					if (this->grid.at(node->get_y()).at(node->get_x() + 1)->get_cost()
							== 10) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x() + 1, node->get_y());
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y()).at(node->get_x() + 1);
						d = 1;
						continue;
					}
				} catch (...) {
				}
				down: //down
				try {
					if (this->grid.at(node->get_y() + 1).at(node->get_x())->get_cost()
							== 10) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x(), node->get_y() + 1);
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y() + 1).at(node->get_x());
						d = 2;
						continue;
					}
				} catch (...) {
				}
				left: //left
				try {
					if (this->grid.at(node->get_y()).at(node->get_x() - 1)->get_cost()
							== 10) {
						Point head(node->get_x(), node->get_y());
						Point tail(node->get_x() - 1, node->get_y());
						PathSegment* new_segment = new PathSegment(head, tail);
						new_path->add_segment(new_segment);
						node = grid.at(node->get_y()).at(node->get_x() - 1);
						d = 3;
						continue;
					}
				} catch (...) {
				}
				//printf("node %d num %d x %d y %d\n",node->get_cost(),num,node->get_x(),node->get_y());
				if (grid.at(node->get_y()).at(node->get_x() - 1)->get_cost()
						!= -3
						&& grid.at(node->get_y() + 1).at(node->get_x())->get_cost()
								!= -3 && grid.at(node->get_y()).at(
						node->get_x() + 1)->get_cost() != -3 && grid.at(
						node->get_y() - 1).at(node->get_x())->get_cost() != -3) {
					d = -1;
					goto start;
				} else {
					break;
				}
			}
		} catch (...) {
		}
		//      printf("end node %d num %d x %d y %d\n",node->get_cost(),num,node->get_x(),node->get_y());
		Point head(node->get_x(), node->get_y());
		Point tail(connections.at(i).sink.x, connections.at(i).sink.y);
		PathSegment* new_segment = new PathSegment(head, tail);
		new_path->add_segment(new_segment);
		paths.push_back(new_path);
		this->clear();
	}
	return paths;
}
