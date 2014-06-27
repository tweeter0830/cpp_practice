#include <cstdio>

#include <string>
#include <vector>

using std::vector;
using std::string;

static char kInputFileName[] = "ASCIIMap.txt";


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
	_ascii_map.push_back(ch);
	char_count++;
      }
    }
  }

  char get_element(int r, int c) {
    if (r >= n_rows_ || c >= n_cols_)
      return 0;
    else
      return _ascii_map[r*n_cols_ + c];
  }

  int get_num_elements() {
    return n_rows_ * n_cols_;
  }

  int n_rows() { return n_rows_; }
  int n_cols() { return n_cols_; }
private:
  vector<char> _ascii_map;
  int n_rows_;
  int n_cols_;
};



int main( int argc, const char* argv[] ) {
  FILE* p_file = fopen(kInputFileName, "r");
  if (p_file == NULL) {
    printf("Error opening file\n");
    return -1;
  }
  ASCIIMatrix ascii_matrix(p_file);
  printf("Number of Elements: %d\n", ascii_matrix.get_num_elements());


}

