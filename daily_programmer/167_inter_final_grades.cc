
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

static char input_file_name[] = "test_scores.txt";
static char output_file_name[] = "report_card.txt";
static int kNumTests = 5;

struct GradeHistory {
  string first_name;
  string last_name;
  vector<int> test_grades;
  int average;
  string letter_grade;
};

double mean(const std::vector<int>& numbers) {
    if (numbers.empty())
        return 0;
    int sum = std::accumulate(numbers.cbegin(), numbers.cend(), 0);
    return static_cast<double>(sum) / numbers.size();
}


string assign_letter_grade(int score) {
  if (score < 60) {
    return string("F");
  } else if (score < 63) {
    return string("D-");
  } else if (score < 67) {
    return string("D");
  } else if (score < 70) {
    return string("D+");
  } else if (score < 73) {
    return string("C-");
  } else if (score < 77) {
    return string("C");
  } else if (score < 80) {
    return string("C+");
  } else if (score < 83) {
    return string("B-");
  } else if (score < 87) {
    return string("B");
  } else if (score < 90) {
    return string("B+");
  } else if (score < 93) {
    return string("A-");
  } else if (score <= 100) {
    return string("A");
  } else {
    printf("Error, grade outside of range");
  }
}


void parse_input_file(string file_name, vector<GradeHistory>* histories) {
  char name_delim = ',';
  char score_delim = '\t';
  std::ifstream in_file(file_name);
  if (!in_file.is_open()) {
    printf("File could not be opened\n");
    return;
  }
  string str;
  while (std::getline(in_file, str)) {
    GradeHistory cur_history;
    // Get First and last names
    size_t name_delim_loc = str.find(name_delim);
    size_t score_delim_loc = str.find(score_delim, name_delim_loc);
    cur_history.first_name = str.substr(0, name_delim_loc);
    cur_history.last_name = str.substr(name_delim_loc+1,
                                       score_delim_loc-name_delim_loc);
    // Get all the test scores
    size_t score_start = score_delim_loc+1;
    string score_string;
    for (int i = 0; i < kNumTests-1; i++) {
      size_t score_end = str.find(score_delim, score_start);
      score_string = str.substr(score_start,
                                score_end - score_start);
      cur_history.test_grades.push_back(std::atoi(score_string.c_str()));
      score_start = score_end + 1;
    }
    score_string = str.substr(score_start);
    cur_history.test_grades.push_back(std::atoi(score_string.c_str()));
    // Sort scores in ascending order
    std::sort(cur_history.test_grades.begin(), cur_history.test_grades.end());
    cur_history.average = std::round(mean(cur_history.test_grades));
    // Assign letter grade
    cur_history.letter_grade = assign_letter_grade(cur_history.average);
    histories->push_back(cur_history);
  }
}


void save_report_card(string file_name, const vector<GradeHistory>& histories) {
  std::ofstream out_file(file_name);
  for (auto iter_ = histories.cbegin();
       iter_ != histories.cend(); ++iter_) {
    out_file << iter_->first_name << ","
             << iter_->last_name << "\t("
             << iter_->average << "%)\t("
             << iter_->letter_grade << "):";
    for (int i = 0; i < kNumTests; ++i)
      out_file << '\t' << iter_->test_grades[i];
    out_file << std::endl;
  }
}


bool history_sort(const GradeHistory& lhs, const GradeHistory& rhs) {
  return lhs.average > rhs.average;
}


int main(int argc, char *argv[]) {
  vector<GradeHistory> histories;
  parse_input_file(input_file_name, &histories);
  std::sort(histories.begin(), histories.end(), history_sort);
  save_report_card(output_file_name, histories);
  return 0;
}
