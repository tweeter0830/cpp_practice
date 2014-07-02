
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <vector>
#include <queue>
#include <utility>

using std::string;
using std::vector;
using std::pair;

char kInputFile1[] = "park_ranger_input_1.txt";
char kInputFile2[] = "park_ranger_input_2.txt";
char kInputFile3[] = "park_ranger_input_3.txt";


vector<vector<int>> comb(int N, int K) {
  // Modified from Rosetta code
  vector<bool> bitmask(K, 1);  // K leading 1's
  bitmask.resize(N, 0);  // N-K trailing 0's

  vector<vector<int>> out_vals;
  // print integers and permute bitmask
  do {
    vector<int> temp_vector;
    for (int i = 0; i < N; ++i) {  // [0..N-1] integers
      if (bitmask[i])
        temp_vector.push_back(i);
    }
    out_vals.push_back(temp_vector);
  } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
  return out_vals;
}


struct DirectedEdge {
  int weight;
  int from;
  int to;
};


class DirectedGraph {
 public:
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

  vector<DirectedEdge> adj(int node) const {
    vector<DirectedEdge> return_val;
    if (node < n_nodes_) {
      vector<int> adj_indices(adj_list_[node]);
      for (auto edge_iter = adj_indices.cbegin();
           edge_iter < adj_indices.cend(); ++edge_iter)
        return_val.push_back(edges_[*edge_iter]);
    }
    return return_val;
  }

  vector<DirectedEdge> edges() const { return vector<DirectedEdge>(edges_); }

  void to_string(string* out_string) const {
    *out_string = "";
    int from_node = 0;
    // Loop over each node
    for (auto from_iter = adj_list_.cbegin();
         from_iter < adj_list_.cend(); ++from_iter) {
      // Print the current node number
      *out_string += "From: " + std::to_string(from_node) + " To: ";
      vector<int> cur_vect = *from_iter;
      // Loop over each edge from this node
      for (auto edge_iter = cur_vect.cbegin();
           edge_iter < cur_vect.cend(); ++edge_iter) {
        *out_string += std::to_string(edges_[*edge_iter].to) + ":"
            + std::to_string(edges_[*edge_iter].weight) + ", ";
      }
      *out_string += '\n';
      ++from_node;
    }
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


class ShortestPaths {
 public:
  explicit ShortestPaths(const DirectedGraph& in_graph, int from_node) {
    dist_to_.resize(in_graph.n_nodes(), std::numeric_limits<int>::max());
    dist_to_[from_node] = 0;
    min_queue_.push(std::make_pair(0, from_node));
    while (!min_queue_.empty()) {
      pair<int, int> cur_pair = min_queue_.top();
      relax_node(in_graph, cur_pair.second);
    }
  }

  int min_dist(int node) { return dist_to_[node]; }

 private:
  void relax_node(const DirectedGraph& in_graph, int node_n) {
    vector<DirectedEdge> adj = in_graph.adj(node_n);
    int ini_dist = dist_to_[node_n];
    for (auto edge_iter = adj.cbegin(); edge_iter < adj.cend(); ++edge_iter) {
      int to_node = edge_iter->to;
      int new_dist = edge_iter->weight + ini_dist;
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


class RouteInspection {
 public:
  explicit RouteInspection(const DirectedGraph& in_graph) {
    // TODO(Jacob) Check if graph is connected
    // TODO(Jacob) Check that graph is undirected
    // Find number of odd degree vertices
    for (int i = 0; i < in_graph.n_nodes(); ++i) {
      if (in_graph.out_degree(i)%2 == 1) {
        odd_nodes_.push_back(i);
        odd_shortest_paths_.push_back(ShortestPaths(in_graph, i));
      }
    }
    // The way we find our answer depends heavily on n_odd_nodes
    n_odd_nodes_ = odd_nodes_.size();
    if (n_odd_nodes_ == 0)
      is_eulerian_ == true;
    else if (n_odd_nodes_ == 2) {
      is_eulerian_ = false;
      optimal_nodes_ = vector<int>(odd_nodes_);
    } else {
      // Find all possible pairings of odd vertices
      // vector<vector<int>> combinations = comb(n_odd_nodes_, 2);
      // Find shortest path value between each pair
      vector<int> combination_dist;
      for (auto pair_iter = combinations.cbegin();
           pair_iter < combinations.cend(); ++pair_iter) {
        int from = (*pair_iter)[0];
        int to = (*pair_iter)[1];
        int min_dist = odd_shortest_paths_[from].min_dist(to);
        combination_dist.push_back(min_dist);
      }
      // Find the combinations of pairs which cover every vertex
      // Find the minimum distance combination
      // Find the minimum distance combination excluding one pair
      // The start/stop nodes will be the nodes in the exluded pair

    }
  }

 private:
  vector<int> odd_nodes_;
  vector<int> optimal_nodes_;
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
      string print_string;
      cur_graph.to_string(&print_string);
      printf("Graph\n%s\n", print_string.c_str());
    }
    in_file.close();
  }
  return 0;
}
