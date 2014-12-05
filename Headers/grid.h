#ifndef _GRID_BASE_H_
#define _GRID_BASE_H_

#include "node.h"
#include "path.h"
#include "problem_object.h"
#include <vector>

using std::cerr;
using std::endl;
using std::string;
using Utilities::Node;
using Utilities::Path;

namespace Utilities {
    class Grid {
        private:
            vector<vector<Node*> > grid;
            int num_connections;
            vector<Path*> paths;

				vector<Connection> connections;

        public:
            /* Constructors/Destructors */
            Grid(ProblemObject* problem_object);
            ~Grid();

            /* Accessors */
            int get_width();
            int get_height();
            int get_num_connections();
            Node* get_node(int x, int y);
            Node* get_node(Point coord);
            vector<Path*> get_paths();
            Path* get_path(int i);

            /* Mutators */
            void replace_node(Node* replacement_node);
            void set_paths(vector<Path*> paths);
            void add_path(Path* path);
            void replace_path(int i, Path* path);
            void remove_path(int i);
				void print_grid();
				void clear();

				int fill(int i);
				int fill2bit(int i);
				int fill3bit(int i);
            /* Algorithms */
            vector<Path*> lee_algorithm();
            vector<Path*> lee_algorithm2bit();
            vector<Path*> lee_algorithm3bit();
				//enhanced
            vector<Path*> lee_algorithm_rubin();
            vector<Path*> lee_algorithm2bit_rubin();
            vector<Path*> lee_algorithm3bit_rubin();

            int fill_lwr(int i);
            vector<Path*> rubin_algorithm_impv();
            int fill_rubin(int i);
            vector<Path*> rubin_algorithm();
            /*int fill_korn(int i,double m);
            vector<Path*> korn_algorithm(double m);*/
    };
}

#endif  //_GRID_BASE_H_
