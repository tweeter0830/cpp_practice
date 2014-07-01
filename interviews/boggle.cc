/* boggle.cc
This code solves boggle. The goal is to find all of the words
contained in a 4 by 4 square of characters according to Boggle
rules. It requires a file named "words" to be in the same directory
from which it is launched. This file should contain text word data
with one word per line. I stole my word list from
/usr/share/dict/words.  For the test solution presented, I find 248
points worth of words.

Result:
Number of dictionary words:
97531
Words Found: bee, beer, beers, beet, beg, begin, being, belt, beret,
bering, bern, bernie, bet, blew, blog, boeing, boer, boers, bog,
boggier, bogie, bole, bolt, eel, eerie, egg, ego, eire, elbe, ere,
erie, erin, ernie, ervin, ewe, ewer, ewers, gee, geeing, gel, gelt,
geo, ger, gere, get, gig, gin, ginger, gob, goblet, goering, going,
ing, inge, ire, lee, leer, leering, leers, leg, leggier, leggin, lego,
lei, leo, let, lew, lob, lobe, log, loge, loin, loire, nigel, niger,
nigger, niggers, niobe, reel, reggie, reign, rein, riel, rig, rigel,
ring, ringo, rio, sweet, sweeter, sweetie, swelter, sweltering,
swerving, tee, teeing, teri, tern, tie, tieing, tier, tiers, vie,
view, viewer, viewers, views, viol, violet, vireo, wee, weeing, weer,
wei, weir, weirs, welt, welter, weltering, welters, were, wet, wring,
Score:248
*/

#include<cstdio>
#include<fstream>

#include<string>
#include<vector>
#include<set>
#include<unordered_set>
#include<algorithm>

using std::set;
using std::unordered_set;
using std::string;
using std::vector;

// Loads in a set of words (One word for each line) from a file. These
// words are converted to lowercase and then appended to an unordered
// set. If the words are less than min_word_len, they are ignored
void load_words_to_set(string file_name, int min_word_len,
                       unordered_set<string>* word_set) {
  std::ifstream in_file(file_name);
  if (!in_file.is_open()) {
    printf("File could not be opened\n");
    return;
  }
  string str;
  while (std::getline(in_file, str)) {
    if (str.size() < min_word_len)
      continue;
    std::transform(str.begin(), str.end(), str.begin(), tolower);
    word_set->insert(str);
  }
  in_file.close();
  return;
}

// For a given i,j on the board, this recursive function will do a
// depth-first scan of all the possible branches emanating from this
// i,j. If a string is found in the dictionary, it will be appended to
// the solution set.
void search_recursive(const unordered_set<string>& word_set,
                      const string& board, const int side_len,
                      int i, int j, vector<bool>* visited,
                      set<string>* solutions, string* cur_string) {
  cur_string->push_back(board[i*side_len+j]);
  (*visited)[i*side_len+j] = true;
  // Check if this word is in the dictionary
  if (word_set.count(*cur_string) > 0) {
    solutions->insert(*cur_string);
  }
  // Loop through a square around current point
  for (int i_scan = i-1; i_scan <= i+1; ++i_scan) {
    for (int j_scan = j-1; j_scan <= j+1; ++j_scan) {
      // Ignore out of bounds points
      if (i_scan < 0 || j_scan < 0 ||
          i_scan >= side_len || j_scan >= side_len)
        continue;
      // Ignore points that we've already visited
      if ((*visited)[i_scan*side_len+j_scan])
        continue;
      // Apply recursion at the next valid point
      search_recursive(word_set, board, side_len, i_scan, j_scan, visited,
                       solutions, cur_string);
    }
  }
  // Adjust string and visited matrix
  cur_string->pop_back();
  (*visited)[i*side_len+j] = false;
  return;
}

// Finds all of the words that are common to the boggle board and dictionary
void search_board(const unordered_set<string>& word_set, const string& board,
                  const int side_len, set<string>* words_found) {
  string recurse_string;
  // Initialize visited array to false
  vector<bool> visited(side_len*side_len, false);
  // Search over every array element
  for (int i = 0; i < side_len; ++i) {
    for (int j = 0; j < side_len; ++j) {
      search_recursive(word_set, board, side_len, i, j,
                       &visited, words_found, &recurse_string);
    }
  }
  return;
}

// Calculates a boggle score for a set of words
int calc_score(set<string>* words) {
  int score = 0;
  for (auto iter = words->cbegin();
       iter != words->cend(); ++iter) {
    int word_len = iter->size();
    if (word_len == 3 || word_len == 4)
      score+=1;
    else if (word_len == 5)
      score+=2;
    else if (word_len == 6)
      score+=3;
    else if (word_len == 7)
      score+=5;
    else if (word_len > 7)
      score+=11;
  }
  return score;
}

// Print comma separated words in a set
void print_words(set<string>* solutions) {
  for (auto iter = solutions->cbegin();
       iter != solutions->cend(); ++iter)
    printf("%s, ", iter->c_str());
}

int main() {
  string file_name = "words";
  const int min_word_len = 3;
  const int side_len = 4;
  const string test_board = "boggleinterviews";
  // In 2d the board looks like this:
  // {{'b','o','g','g'},
  //  {'l','e','i','n'},
  //  {'t','e','r','v'},
  //  {'i','e','w','s'}};

  // Fill dictionary set
  unordered_set<string> word_set;
  load_words_to_set(file_name, min_word_len, &word_set);
  printf("Number of dictionary words:\n%lu\n", word_set.size());
  // Find all the words in the test board
  set<string> test_solutions;
  search_board(word_set, test_board, side_len, &test_solutions);
  // Print all of the found words;
  printf("Words Found:\n");
  print_words(&test_solutions);
  // Find score: 192
  printf("\nScore:%d\n", calc_score(&test_solutions));
  return 0;
}


