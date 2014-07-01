#include <cstdio>

#include <string>
#include <vector>
#include <unordered_map>

using std::vector;
using std::string;

static char kInputFileName[] = "ASCIIMap.txt";
static int kBlockLength = 10;

class ASCIIMatrix {
public:
  explicit ASCIIMatrix(FILE* p_file) {
    n_rows_ = 0;
    n_cols_ = 0;
    int char_count = 0;
    int ch;
    while ((ch = fgetc(p_file)) != EOF) {
      if (ch == '\n') {
	if (n_cols_ == 0)
	  n_cols_ = char_count;
	else if (n_cols_ != char_count)
	  printf("Error, collumns are not even\n");
	n_rows_++;
	char_count = 0;
      } else {
	ascii_map_.push_back(ch);
	char_count++;
      }
    }
    size_ = n_cols_*n_rows_;
  }

  char get_element (int idx) const {
    if (idx > n_rows_ * n_cols_ -1)
      return 0;
    else
      return ascii_map_[idx];
  }

  void get_adjacent_idxs (int idx, vector<int>* adj_idx) const {
    int row = idx / n_cols_;
    int col = idx % n_cols_;
    if (col > 0)
      adj_idx->push_back(idx-1);
    if (col < n_cols_-1)
      adj_idx->push_back(idx+1);
    if (row > 0)
      adj_idx->push_back(idx-n_cols_);
    if (row < n_rows_ - 1)
      adj_idx->push_back(idx+n_cols_);
  }

  int get_num_elements() {
    return n_rows_ * n_cols_;
  }

  int n_rows() const { return n_rows_; }
  int n_cols() const { return n_cols_; }
  int size() const { return size_; }
private:
  vector<char> ascii_map_;
  int n_rows_;
  int n_cols_;
  int size_;
};


struct BlockMetrics {
  int area;
  int circumference;
  int number_blobs;
};


class ASCIIBlockCount{
public:
  explicit ASCIIBlockCount(const ASCIIMatrix& in_matrix) {
    count_ = 0;
    for (int i = 0; i < in_matrix.size(); ++i) {
      marked_.push_back(false);
    }
    for (int i = 0; i < in_matrix.size(); ++i) {
      if (!marked_[i]){
	char cur_char = in_matrix.get_element(i);
	auto found_iter = block_data_.find(cur_char);
	// This character hasn't been seen before
	if (found_iter == block_data_.end()) {
	  BlockMetrics new_metrics;
	  new_metrics.area = 0;
	  new_metrics.circumference = 0;
	  new_metrics.number_blobs = 1;
	  block_data_[cur_char] = new_metrics;
	} else {
	  found_iter->second.number_blobs += 1;
	}
	depth_first_search(in_matrix, i, 
			   &(block_data_[cur_char]));
      }
    }
  }
  
  void print_block_data(){
    printf("Block data\n");
    for (auto iter = block_data_.cbegin();
	 iter != block_data_.cend(); ++iter) {
      char key = iter->first;
      BlockMetrics met = iter->second;
      printf("%c: Area: %d\tCirc: %d\tBlobs: %d\n", 
	     key, met.area, met.circumference, met.number_blobs);
    }
  }
    
private:
  int count_;
  std::unordered_map<char, BlockMetrics> block_data_;
  std::vector<bool> marked_;
  
  void depth_first_search(const ASCIIMatrix& mat, int idx, 
			  BlockMetrics* cur_char_metrics) {
    char cur_char = mat.get_element(idx);
    int circum = kBlockLength*4;
    marked_[idx] = true;
    vector<int> adjacent_idx;
    mat.get_adjacent_idxs(idx, &adjacent_idx);
    for (auto iter = adjacent_idx.cbegin(); 
	 iter != adjacent_idx.cend(); ++iter) {
      int loop_idx = *iter;
      char loop_char = mat.get_element(loop_idx);
      if (loop_char == cur_char)
	circum -= kBlockLength;
      if (!marked_[loop_idx] && loop_char == cur_char)
	depth_first_search(mat, loop_idx, cur_char_metrics);
    }
    cur_char_metrics->area += kBlockLength * kBlockLength;
    cur_char_metrics->circumference += circum;
  }
};


int main( int argc, const char* argv[] ) {
  FILE* p_file = fopen(kInputFileName, "r");
  if (p_file == NULL) {
    printf("Error opening file\n");
    return -1;
  }
  ASCIIMatrix ascii_matrix(p_file);
  printf("Number of Elements: %d\n", ascii_matrix.get_num_elements());
  ASCIIBlockCount block_count(ascii_matrix);
  block_count.print_block_data();
}

