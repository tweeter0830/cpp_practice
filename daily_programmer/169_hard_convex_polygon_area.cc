#include <cmath>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <vector>

using std::string;
using std::vector;
using std::pair;

static char kInputFile1[] = "convex_poly_input_1.txt";
static char kInputFile2[] = "convex_poly_input_2.txt";
static char kInputFile3[] = "convex_poly_input_3.txt";

typedef vector<pair<double, double>> pair_vect;

static int kPointDimension = 2;

void print_pair(pair<double, double> in_pair){
  printf("(%f, %f)\n", in_pair.first, in_pair.second);
}

vector<vector<double>> parse_csv(std::ifstream* in_file) {
  vector<vector<double>> output;
  int n_lines;
  string line;
  if (getline(*in_file, line))
    n_lines = std::stoi(line);
  else
    n_lines = 0;
  output.resize(n_lines);
  int row = 0;
  // Edge goes from row to collumn
  while (getline(*in_file, line)) {
    std::istringstream line_stream(line);
    string elem_str;
    int collumn = 0;
    // For each line, loop over each comma separated value
    while (getline(line_stream, elem_str, ',')) {
      double value = std::stod(elem_str);
      output[row].push_back(value);
      ++collumn;
    }
    ++row;
  }
  return output;
}

pair_vect to_pair_vect(const vector<vector<double>>& in_vect) {
  pair_vect output;
  output.reserve(in_vect.size());
  for (auto it = in_vect.cbegin(); it != in_vect.cend(); ++it){
    if (it->size() < 2) {
      output.resize(0);
      break;
    } else {
      output.push_back(std::make_pair(*(it->cbegin()), 
				      *++(it->cbegin())));
    }
  }
  return output;
}

class CClockwiseCompFunctor {
public:
  CClockwiseCompFunctor(pair<double, double> center) {
    center_ = center;
  }

  bool operator() (pair<double, double> p1,
		   pair<double, double> p2) {
    return poly_angle(p1) < poly_angle(p2);
  }

private:
  double poly_angle(pair<double, double> point) {
    return std::atan2(point.second - center_.second, 
		      point.first - center_.first);
  }

  pair<double, double> center_;
};
  

class ConvexPolygon {
public:
  ConvexPolygon(const pair_vect& in_points) {
    points_ = in_points;
    calc_center();
    std::sort(points_.begin(), points_.end(), 
	      CClockwiseCompFunctor(center_));
    calc_area();
  }
  
  double area () const { return area_; }
private:
  void calc_area() {
    area_ = 0.0;
    for (auto it = points_.cbegin(); it != --points_.cend(); ++it) {
      auto next = it + 1;
      area_ += (it->first * next->second) - (it->second * next->first);
    }
    if (points_.size() > 1) {
      auto sec2last = --points_.cend();
      auto first = points_.cbegin();
      area_ += (sec2last->first * first->second) - (sec2last->second * first->first);
    }
    area_ *= 0.5;
  }
  
  void calc_center() {
    center_ = std::make_pair(0, 0);
    for (auto it = points_.cbegin(); it != points_.cend(); ++it) {
      center_.first += it->first;
      center_.second += it->second;
    }
    center_.first = center_.first/points_.size();
    center_.second = center_.second/points_.size();
  }

  pair_vect points_;
  pair<double, double> center_;
  double area_;
};

int main(int argc, char *argv[]) {
  vector<string> file_strings = {kInputFile1,
                                 kInputFile2,
                                 kInputFile3};
  for (auto iter = file_strings.cbegin();
       iter < file_strings.cend(); ++iter) {
    std::ifstream in_file(*iter);
    if (in_file) {
      pair_vect points = to_pair_vect(parse_csv(&in_file));
      printf("File: %s\nPoints:\n", iter->c_str());
      std::for_each(points.cbegin(), points.cend(), print_pair);
      printf("\n");
      ConvexPolygon cur_poly(points);
      printf("Area: %f\n", cur_poly.area());
    }
    in_file.close();
  }
  return 0;
}
