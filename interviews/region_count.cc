/* region_count.cc
The problem is, given a number of
ranges (start, end) and a number, find the number of ranges which
contain the number in O(log(n)) time. The range data can be
pre-processed as needed.

The data is pre-processed into a vector of points and a count of the
ranges within those points (counts[i] contains the number of range
overlaps between points[i-1] and points[i]).
Result:
Ranges:
(15.775317,33.607510) (31.323969,31.937601) (7.902055,36.465893)
(13.408910,30.729183) (11.110989,22.158798) (19.095882,25.154837)
(14.591379,20.536036) (36.647804,38.089188) (25.428469,28.691877)
(5.664103,24.278755) (0.652023,9.715470) (5.489263,32.167068)
(6.267164,16.037775) (4.352352,5.191617) (8.730276,39.956978)
Point vector: 0.652023 4.352352 5.191617 5.489263 5.664103 6.267164 7.902055
8.730276 9.715470 11.110989 13.408910 14.591379 15.775317 16.037775
19.095882 20.536036 22.158798 24.278755 25.154837 25.428469 28.691877
30.729183 31.323969 31.937601 32.167068 33.607510 36.465893 36.647804
38.089188 39.956978
Count vector:
1 2 1 2 3 4 5 6 5 6 7 8 9 8 9 8 7 6 5 6 5 4 5 4 3 2 1 2 1
Test points:
-10.000000 10.000000 30.000000 35.000000 90.000000
Number of regions which contain these points:
0 5 5 2 0
*/

#include <cstdio>

#include <cstdlib>
#include <algorithm>
#include <vector>
#include <utility>

using std::vector;
using std::pair;

// Generate random float 
float random_float(float min, float max) {
  return min + (rand()/( RAND_MAX/(max-min)) );
}

// Simple binary search. I could have used the STL sort algorithm for
// this, but chose to implement it myself for practice
int binary_search_range(const vector<float>& sorted, float key) {
  int right = sorted.size()-1;
  int left = 0;
  int mid;
  if (key < sorted[left])
    return 0;
  if (key > sorted[right])
    return right+1;
  while (left < right) {
    mid = (left+right)/2;
    if (key < sorted[mid])
      right = mid;
    else
      left = mid + 1;
  }
  return left;
}

// Makes the data structures for to do fast look up. Points is a vector
// containing all of the range points in sorted order. region_counts
// is a vector with a size one less than the size of points.
// region_counts[i] contains the number of range overlaps
// between points[i-1] and points[i]
void make_range_interlap(const vector<pair<float, float>> & ranges,
                         vector<float>* points,
                         vector<int>* region_counts) {
  vector<pair<float, bool>> point_is_begin;
  // Loop through region point pairs
  for (auto range_iter = ranges.cbegin();
       range_iter < ranges.cend(); ++range_iter) {
    // Record the points and whether they are start or ends of the regions
    point_is_begin.push_back(std::make_pair(range_iter->first, true));
    point_is_begin.push_back(std::make_pair(range_iter->second, false));
  }
  // Sort the points and keep the information on whether
  // they are at the start or the end along for the ride
  std::sort(point_is_begin.begin(), point_is_begin.end());
  // Loop through points while adjusting depth level
  int depth = 0;
  for (auto point_iter = point_is_begin.cbegin();
       point_iter != point_is_begin.cend()-1; ++point_iter) {
    if (point_iter->second)
      ++depth;
    else
      --depth;
    points->push_back(point_iter->first);
    region_counts->push_back(depth);
  }
  // region_counts.size() is one larger than points.size()
  points->push_back((point_is_begin.back()).first);
}

// Finds the number of ranges which contain a value_to_find
int find_count(const vector<float>& points,
               const vector<int>& region_counts,
               float value_to_find) {
  int region_index = binary_search_range(points, value_to_find);
  if (region_index < 1 || points.size() <= region_index)
    return 0;
  else
    return region_counts[region_index-1];
}

void print_float(float const input) {
  printf("%f ", input);
  return;
}

void print_int(int const input) {
  printf("%d ", input);
  return;
}

int main(int argc, char *argv[]) {
  // Create vector containing the regions as pairs
  vector<pair<float, float>> ranges;
  const int num_ranges = 15;
  const int range_min = 0;
  const int range_max = 40;
  printf("Ranges:\n");
  for (int i = 0; i < num_ranges; i++) {
    float val1 = random_float(range_min, range_max);
    float val2 = random_float(range_min, range_max);
    // Sort each pair by value
    if (val1 < val2) {
      ranges.push_back(std::make_pair(val1, val2));
      printf("(%f,%f)\n", val1, val2);
    } else {
      ranges.push_back(std::make_pair(val2, val1));
      printf("(%f,%f)\n", val2, val1);
    }
  }
  // Create the data structures to enable fast lookup of region counts
  vector<float> points;
  vector<int> region_counts;
  make_range_interlap(ranges, &points, &region_counts);
  // Print the data structures
  printf("Point vector:\n");
  std::for_each(points.cbegin(), points.cend(), print_float);
  printf("\nCount vector:\n");
  std::for_each(region_counts.cbegin(), region_counts.cend(), print_int);
  printf("\n");
  // Generate values to test
  vector<float> test_values = {-10.0, 10.0, 30.0, 35.0, 90.0};
  vector<int> test_counts;
  // Find the number of regions each test case is contained within
  for (auto iter = test_values.cbegin(); iter != test_values.cend(); ++iter) {
    test_counts.push_back(find_count(points, region_counts, *iter));
  }
  printf("Test points:\n");
  std::for_each(test_values.cbegin(), test_values.cend(), print_float);
  printf("\nNumber of regions which contain these points:\n");
  std::for_each(test_counts.cbegin(), test_counts.cend(), print_int);
  printf("\n");
  return 0;
}
