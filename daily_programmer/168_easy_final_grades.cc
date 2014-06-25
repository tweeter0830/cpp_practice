#include <random>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <fstream>
#include <algorithm>
#include <string>
#include <vector>

using std::string;
using std::vector;
using std::pair;

static char out_file_name[] = "test_scores.txt";
static char name_file_name[] = "name_list.txt";
static double mean = 75;
static double std_dev = 15;

void get_name_list(string file_name,
                   vector<pair<string, string>>* name_vector) {
  std::ifstream in_file(file_name);
  char delim = ',';

  if (!in_file.is_open()) {
    printf("File could not be opened\n");
    return;
  }
  string str;
  while (std::getline(in_file, str)) {
    size_t delim_loc = str.find(delim);
    string first_name = str.substr(0, delim_loc);
    string last_name = str.substr(delim_loc+1);
    last_name.pop_back();
    name_vector->push_back(std::make_pair(first_name, last_name));
  }
}


float get_random_grade(std::normal_distribution<double> dist) {
  static std::random_device rd;
  static std::mt19937 e2(rd());
  int num = std::ceil(dist(e2));
  if (num > 100)
    num = 100;
  else if (num < 0)
    num = 0;
  return num;
}


void gen_random_scores(std::string file_out_str,
                       std::string name_file_str, int n) {
  std::srand(unsigned(std::time(0)));
  vector<pair<string, string>> name_vector;
  get_name_list(name_file_str, &name_vector);
  int num_names = name_vector.size();
  if (num_names < n) {
    printf("Error, n is larger than the number of names");
    return;
  }
  std::random_shuffle(name_vector.begin(), name_vector.end());
  vector<pair<string, string>> rand_name_vector;
  for (int i = 0; i < n; ++i) {
    rand_name_vector.push_back(name_vector[i]);
  }
  string out_str;
  std::normal_distribution<double> dist(mean, std_dev);
  std::ofstream out_file(file_out_str);
  for (auto iter_ = rand_name_vector.cbegin();
       iter_ != rand_name_vector.cend(); ++iter_) {
    out_file << iter_->first << ","
             << iter_->second << "\t"
             << get_random_grade(dist) << "\t"
             << get_random_grade(dist) << "\t"
             << get_random_grade(dist) << "\t"
             << get_random_grade(dist) << "\t"
             << get_random_grade(dist) << "\t" << std::endl;
  }
}


int main(int argc, char *argv[]) {
  if (argc != 2) {
        printf("You must provide exactly one argument\n");
        exit(0);
  }
  int n = std::atoi(argv[1]);
  printf("n: %d\n", n);
  string out_file_str = out_file_name;
  string name_file_str = name_file_name;
  gen_random_scores(out_file_str, name_file_str, n);
  return 0;
}


