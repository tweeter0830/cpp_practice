
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <string>
#include <vector>
#include <queue>
#include <utility>

#include <csignal> 

using std::string;
using std::vector;
using std::pair;

char kInputFile1[] = "park_ranger_input_1.txt";
char kInputFile2[] = "park_ranger_input_2.txt";
char kInputFile3[] = "park_ranger_input_3.txt";

typedef vector<pair<int, int>> pair_vector;

void pair_comb_recursive(vector<pair_vector>* output,
			 pair_vector* pairs_so_far,
			 const vector<int>& remainder) {
  if (remainder.size() == 0) {
    output->push_back(*pairs_so_far);
  } else {
    for (auto sec_iter = ++remainder.cbegin();
         sec_iter != remainder.cend(); ++sec_iter) {
      pair<int, int> cur_pair = std::make_pair(*(remainder.cbegin()),
                                               *sec_iter);
      pairs_so_far->push_back(cur_pair);
      vector<int> new_remainder(remainder.size() - 2);
      auto end_iter = std::copy(++remainder.cbegin(), sec_iter, new_remainder.begin());
      std::copy(sec_iter + 1, remainder.cend(), end_iter);
      pair_comb_recursive(output, pairs_so_far, new_remainder);
    }
  }
  pairs_so_far->pop_back();
}

vector<pair_vector> pair_comb(int n_elements) {
  pair_vector pairs_so_far;
  vector<pair_vector> all_combinations;
  if (n_elements%2 == 0) {
    vector<int> remainder(n_elements);
    std::iota(remainder.begin(), remainder.end(), 0);
    pair_comb_recursive(&all_combinations, &pairs_so_far, remainder);
  }
  return all_combinations;
}


struct DirectedEdge {
  int weight;
  int from;
  int to;
};

// Adjacency based directed graph representation. Stolen in large part
// from Sedgwick's implementation
class DirectedGraph {
 public:
  // Construct the graph from a text file that uses the representation
  // given on reddit. The first line gives the number of nodes and
  // then a n x n matrix is given
  explicit DirectedGraph(std::ifstream* in_file) {
    string line;
    if (getline(*in_file, line))
      n_nodes_ = std::stoi(line);
    else
      n_nodes_ = 0;
    adj_list_.resize(n_nodes_);
    int row = 0;
    n_edges_ = 0;
    // Edge goes from row to collumn
    while (getline(*in_file, line)) {
      std::istringstream line_stream(line);
      string edge_str;
      int collumn = 0;
      // For each line, loop over each comma separated value
      while (getline(line_stream, edge_str, ',')) {
        int weight = std::stoi(edge_str);
        if (weight != -1) {
          DirectedEdge temp_edge = {weight, row, collumn};
          edges_.push_back(temp_edge);
          adj_list_[row].push_back(n_edges_);
          ++n_edges_;
        }
        ++collumn;
      }
      ++row;
    }
  }

  // Return all the edges emminating from this node
  vector<DirectedEdge> adj(int node) const {
    vector<DirectedEdge> return_val;
    if (node < n_nodes_) {
      vector<int> adj_indices(adj_list_[node]);
      for (auto edge_it = adj_indices.cbegin();
           edge_it < adj_indices.cend(); ++edge_it)
        return_val.push_back(edges_[*edge_it]);
    }
    return return_val;
  }

  vector<DirectedEdge> edges() const { return vector<DirectedEdge>(edges_); }

  string to_string() const {
    string out_string;
    int from_node = 0;
    // Loop over each node
    for (auto node_it = adj_list_.cbegin();
         node_it < adj_list_.cend(); ++node_it) {
      // Print the current node number
      out_string += "From: " + std::to_string(from_node) + " To: ";
      vector<int> cur_v = *node_it;
      // Loop over each edge from this node
      for (auto edge_it = cur_v.cbegin(); edge_it < cur_v.cend(); ++edge_it) {
        out_string += std::to_string(edges_[*edge_it].to) + ":"
                   + std::to_string(edges_[*edge_it].weight) + ", ";
      }
      out_string += '\n';
      ++from_node;
    }
    return out_string;
  }

  int out_degree(int i) const {
    return adj_list_[i].size();
  }

  int n_nodes() const { return n_nodes_; }
  int n_edges() const { return n_edges_; }

 private:
  vector<vector<int>> adj_list_;
  vector<DirectedEdge> edges_;
  int n_nodes_;
  int n_edges_;
};

// Uses Sedgwick's Dijkstra's algorithm to find the shortest paths
// from a single node in a graph to every other node in the graph.
class ShortestPaths {
 public:
  explicit ShortestPaths(const DirectedGraph& in_graph, int from_node) {
    dist_to_.resize(in_graph.n_nodes(), std::numeric_limits<int>::max());
    dist_to_[from_node] = 0;
    min_queue_.push(std::make_pair(0, from_node));
    while (!min_queue_.empty()) {
      pair<int, int> cur_pair = min_queue_.top();
      min_queue_.pop();
      relax_node(in_graph, cur_pair.second);
    }
  }

  int min_dist(int node) { return dist_to_[node]; }

 private:
  void relax_node(const DirectedGraph& in_graph, int node_n) {
    vector<DirectedEdge> adj = in_graph.adj(node_n);
    int ini_dist = dist_to_[node_n];
    for (auto edge_it = adj.cbegin(); edge_it < adj.cend(); ++edge_it) {
      int to_node = edge_it->to;
      int new_dist = edge_it->weight + ini_dist;
      if (dist_to_[to_node] > new_dist) {
        dist_to_[to_node] = new_dist;
        min_queue_.push(std::make_pair(dist_to_[to_node], to_node));
      }
    }
  }

  vector<int> dist_to_;
  std::priority_queue<pair<int, int>,
                      vector<pair<int, int>>,
                      std::greater<pair<int, int>>> min_queue_;
};


// Solves a varient of the route inspection problem. Given a
// connected, undirected graph with positive edge weights. Find the
// two nodes which minimize the distance covered to visit all paths
// when starting at one of these nodes and finishing at another.
//
// This implemenation is woking on the assumption that the optimal
// nodes come from the odd-node pair combination with the smallest
// value total distance between the nodes in each pair. Further, the
// pair of nodes in this combination with the largest path between
// them are the optimal nodes.
class RouteInspection {
 public:
  explicit RouteInspection(const DirectedGraph& in_graph) {
    // TODO(Jacob) Check if graph is connected
    // TODO(Jacob) Check that graph is undirected
    // TODO(Jacob) Check if edge weights are positive
    // Find number of odd degree vertices
    for (int i = 0; i < in_graph.n_nodes(); ++i) {
      if (in_graph.out_degree(i)%2 == 1) {
        odd_nodes_.push_back(i);
        odd_shortest_paths_.push_back(ShortestPaths(in_graph, i));
      }
    }
    // The way we find our answer depends heavily on n_odd_nodes
    n_odd_nodes_ = odd_nodes_.size();
    if (n_odd_nodes_ == 0) {
      // No odd nodes, so any two nodes are optimal
      is_eulerian_ == true;
      optimal_nodes_ = std::make_pair(-1, -1);
    } else if (n_odd_nodes_ == 2) {
      // only two odd nodes, these are the optimal nodes
      is_eulerian_ = false;
      optimal_nodes_ = std::make_pair(odd_nodes_[0], odd_nodes_[1]);
    } else {
      // > 2 odd nodes. This is where it gets interesting
      is_eulerian_ = false;
      // Find the combinations of pairs which cover every odd vertex
      vector<pair_vector> pair_combinations = pair_comb(n_odd_nodes_);
      // Find the minimum distance combination excluding one pair
      int min_dist = 999999999;
      pair<int, int> min_pair;
      // Loop over ever possible pair combination of odd nodes
      for (auto comb_iter = pair_combinations.cbegin();
           comb_iter != pair_combinations.cend(); comb_iter++) {
	pair<int, int> max_pair;
	int dist;
	// for this pair combination, find the distance associated
	// with traversing the odd nodes
        cost_pair_vect(*comb_iter, &max_pair, &dist);
	// Keep track of the pair combination with the lowest distance
	if (min_dist > dist) {
	  min_dist = dist;
	  min_pair = max_pair;
	}
      }
      optimal_nodes_ = std::make_pair(odd_nodes_[min_pair.first],
                                      odd_nodes_[min_pair.second]);
    }
  }

  pair<int, int> optimal_nodes() { return optimal_nodes_; }
  bool is_eulerian() { return is_eulerian_; }

 private:
  // For a vector of odd node pairs, find the sum of the distances
  // between each pair, with the largest diatance pair excluded.
  // Return the pair with the largest distance.
  void cost_pair_vect(const pair_vector& in_pair_vect,
		      pair<int, int>* max_pair, int* dist) {
    int tot_dist = 0;
    int max_dist = -1;
    for (auto pair_it = in_pair_vect.cbegin();
	 pair_it != in_pair_vect.cend(); ++pair_it) {
      int first_node_idx = pair_it->first;
      int second_node_idx = pair_it->second;
      int second_node = odd_nodes_[second_node_idx];
      int cur_dist = odd_shortest_paths_[first_node_idx].min_dist(second_node);
      tot_dist += cur_dist;
      if (max_dist < cur_dist) {
	max_dist = cur_dist;
	*max_pair = *pair_it;
      }
    }
    *dist = tot_dist - max_dist;
  }

  vector<int> odd_nodes_;
  pair<int, int> optimal_nodes_;
  vector<ShortestPaths> odd_shortest_paths_;
  int n_odd_nodes_;
  bool is_eulerian_;
};


int main(int argc, char *argv[]) {
  vector<string> file_strings = {kInputFile1,
                                 kInputFile2,
                                 kInputFile3};
  for (auto iter = file_strings.cbegin();
       iter < file_strings.cend(); ++iter) {
    std::ifstream in_file(*iter);
    if (in_file) {
      DirectedGraph cur_graph(&in_file);

      printf("File: %s\n", iter->c_str());
      printf("Number of nodes: %d\n", cur_graph.n_nodes());
      string print_string = cur_graph.to_string();
      printf("Graph\n%s\n", print_string.c_str());

      RouteInspection cur_route(cur_graph);
      bool is_eulerian = cur_route.is_eulerian();
      pair<int, int> optimal_nodes = cur_route.optimal_nodes();
      
      printf("Is Eulerian: %s\n", cur_route.is_eulerian() ? "True" : "False");
      printf("Optimal Nodes: %c, %c\n\n",
             optimal_nodes.first + 'A', optimal_nodes.second + 'A');
      
    }
    in_file.close();
  }
  return 0;
}
