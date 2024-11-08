#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "include/caesar_dec.h"
#include "include/caesar_enc.h"
#include "include/subst_dec.h"
#include "include/subst_enc.h"
#include "utils.h"

using namespace std;

// Initialize random number generator in .cpp file for One Definition Rule (ODR)
// reasons
std::mt19937 Random::rng;

// Forward declaration of DecryptSubstitutionCipher function
void DecryptSubstitutionCipher(QuadgramScorer& scorer,
                               vector<string> crypto_txt, string fileout);

/**
 * Print instructions for using the program.
 */
void printMenu() {
  cout << "Ciphers Menu" << endl;
  cout << "------------" << endl;
  cout << "C - Encrypt with Caesar Cipher" << endl;
  cout << "D - Decrypt Caesar Cipher" << endl;
  cout << "E - Compute English-ness Score" << endl;
  cout << "A - Apply Random Substitution Cipher" << endl;
  cout << "S - Decrypt Substitution Cipher from Console" << endl;
  cout << "R - Set Random Seed for Testing" << endl;
  cout << "F - Decrypt Substitution Cipher (File)" << endl;
  cout << "X - Exit Program" << endl;
}

int main() {
  // Seed random generator with current time for randomness
  Random::seed(time(NULL));
  string command;
  vector<string> dict;  // Store dictionary words
  string word;

  // Load dictionary from "dictionary.txt"
  ifstream inFS("dictionary.txt");
  while (getline(inFS, word)) {
    dict.push_back(word);
  }
  inFS.close();

  // Load quadgrams and frequencies from "english_quadgrams.txt"
  vector<string> subwords;
  vector<int> appear_time;
  string line;
  string wordd;
  int apr;

  ifstream inFE("english_quadgrams.txt");
  while (getline(inFE, line)) {
    wordd = line.substr(0, 4);  // Extract quadgram
    subwords.push_back(wordd);
    apr = stoi(line.substr(5));  // Extract frequency of quadgram
    appear_time.push_back(apr);
  }
  inFE.close();

  // Initialize the quadgram scorer with loaded data
  QuadgramScorer scorer(subwords, appear_time);
  cout << "Welcome to Ciphers!" << endl;
  cout << "-------------------" << endl;
  cout << endl;

  // Main program loop to process commands
  do {
    printMenu();
    cout << endl << "Enter a command (case does not matter): ";
    getline(cin, command);  // Use getline to handle input properly
    cout << endl;

    // Process each command based on user input
    if (command == "A" || command == "a") {
      applyRandSubstCipherCommand();
    }

    if (command == "R" || command == "r") {
      string seed_str;
      cout << "Enter a non-negative integer to seed the random number "
              "generator: ";
      getline(cin, seed_str);
      Random::seed(stoi(seed_str));  // Set the random seed
    }

    if (command == "C" || command == "c") {
      runCaesarEncrypt();  // Run Caesar encryption
    }

    if (command == "D" || command == "d") {
      runCaesarDecrypt(dict);  // Run Caesar decryption using dictionary
    }

    if (command == "E" || command == "e") {
      computeEnglishnessCommand(scorer);  // Compute the English-ness score
    }

    if (command == "S" || command == "s") {
      decryptSubstCipherCommand(
          scorer);  // Decrypt substitution cipher from console input
    }

    if (command == "F" || command == "f") {
      string filein, fileout;
      cout << "Enter filename with text to substitution-cipher decrypt: "
           << endl;
      cin >> filein;
      cout << "Enter filename to write results to: " << endl;
      cin >> fileout;

      // Read cryptogram file
      vector<string> crypto_lines;
      inFS.open(filein);
      while (getline(inFS, line)) {
        crypto_lines.push_back(line);
      }
      inFS.close();

      // Perform substitution cipher decryption and write to output file
      DecryptSubstitutionCipher(scorer, crypto_lines, fileout);
    }
    cout << endl;

  } while (!(command == "x" || command == "X") &&
           !cin.eof());  // Exit condition

  return 0;
}

// Caesar encryption helper function to rotate a character by a certain amount
char rot(char c, int amount) {
  int char_ind;
  int new_char_ind;
  char new_c;
  if (isalpha(c)) {
    c = toupper(c);
    char_ind = int(c) - 65;  // 'A' in ASCII is 65
    new_char_ind = (char_ind + amount) % 26;
    new_c = char(new_char_ind + 65);
  } else {
    return c;
  }
  return new_c;
}

// Caesar encryption function to rotate a string by a specified amount
string rot(const string& line, int amount) {
  string new_string;
  for (int i = 0; i < line.length(); i++) {
    char index = line[i];
    if (isalpha(index) || isspace(index)) {
      new_string += rot(line[i], amount);
    }
  }
  return new_string;
}

// Command function to perform Caesar encryption based on user input
void runCaesarEncrypt() {
  string en_line;
  int char_num;
  cout << "Enter the text to Caesar encrypt: ";
  getline(cin, en_line);
  cout << "Enter the number of characters to rotate by: ";
  cin >> char_num;
  string newline = rot(en_line, char_num);
  cout << newline;
}

#pragma endregion CaesarEnc

#pragma region CaesarDec

#pragma region CaesarDec

// Function to rotate each string in the vector by a specific amount for Caesar
// decryption
void rot(vector<string>& strings, int amount) {
  for (int y = 0; y < strings.size(); y++) {
    string new_str;
    for (int i = 0; i < strings[y].size(); i++) {
      int char_ind = int(strings[y][i]) - 65;  // ASCII offset for 'A'
      int new_char_ind = (char_ind + amount) % 26;
      char new_c = char(new_char_ind + 65);
      new_str += new_c;
    }
    strings[y] = new_str;  // Update each string with rotated version
  }
}

// Clean function to make a string uppercase and remove non-alphabetic
// characters
string clean(const string& s) {
  string new_s;
  for (int i = 0; i < s.size(); i++) {
    if (isalpha(s[i])) {
      new_s += toupper(s[i]);
    }
  }
  return new_s;
}

// Split a string by spaces and return each word in a vector
vector<string> splitBySpaces(const string& s) {
  vector<string> strings;
  string str;
  for (int i = 0; i < s.size(); i++) {
    if (!isalpha(s[i]) && isalpha(s[i - 1])) {
      strings.push_back(str);
      str.clear();
    } else if (isalpha(s[i])) {
      str += s[i];
    }
  }
  if (isalpha(s[s.size() - 1])) {
    strings.push_back(str);  // Push last word if needed
  }
  return strings;
}

// Join vector of strings with spaces in between to form a single string
string joinWithSpaces(const vector<string>& words) {
  string return_str;
  for (int i = 0; i < words.size() - 1; i++) {
    return_str += words[i] + " ";
  }
  return_str += words[words.size() - 1];
  return return_str;
}

// Count number of words in vector `words` that appear in dictionary `dict`
int numWordsIn(const vector<string>& words, const vector<string>& dict) {
  int counter = 0;
  for (const string& word : words) {
    for (const string& dict_word : dict) {
      if (word == dict_word) {
        counter++;
        break;
      }
    }
  }
  return counter;
}

// Main function to attempt Caesar decryption, looking for the most readable
// result
void runCaesarDecrypt(const vector<string>& dict) {
  string dec_str;
  vector<string> strings;
  cout << "Enter the text to Caesar decrypt: ";
  getline(cin, dec_str);

  strings = splitBySpaces(dec_str);      // Split input string
  for (auto& s : strings) s = clean(s);  // Clean each word

  bool good_dec = false;
  for (int amount = 0; amount < 26; amount++) {
    vector<string> temp_strings = strings;
    rot(temp_strings, amount);
    if (numWordsIn(temp_strings, dict) > temp_strings.size() / 2) {
      cout << joinWithSpaces(temp_strings) << endl;
      good_dec = true;
      break;
    }
  }
  if (!good_dec) {
    cout << "No good decryptions found" << endl;
  }
}

#pragma endregion CaesarDec

#pragma region SubstEnc

// Function to apply a substitution cipher to a string `s`
string applySubstCipher(const vector<char>& cipher, const string& s) {
  string new_s;
  for (char c : s) {
    if (isalpha(c)) {
      int ind = int(toupper(c)) - 65;  // ASCII offset for 'A'
      new_s += cipher[ind];
    } else {
      new_s += c;
    }
  }
  return new_s;
}

// Command to apply a random substitution cipher on user input text
void applyRandSubstCipherCommand() {
  vector<char> cipher = genRandomSubstCipher();  // Generate random cipher
  string input_s;
  cout << "Enter the text to substitution-cipher encrypt: ";
  getline(cin, input_s);
  cout << applySubstCipher(cipher, input_s) << endl;
}

#pragma endregion SubstEnc

#pragma region SubstDec

// Function to calculate a score based on English quadgrams in string `s`
double scoreString(const QuadgramScorer& scorer, const string& s) {
  double total = 0;
  for (int i = 0; i < s.size() - 3; i++) {
    total +=
        scorer.getScore(s.substr(i, 4));  // Calculate score for each quadgram
  }
  return total;
}

// Command to calculate and print English-ness score for user input
void computeEnglishnessCommand(const QuadgramScorer& scorer) {
  string score_string;
  cout << "Enter a string to score: ";
  getline(cin, score_string);
  score_string = clean(score_string);
  cout << scoreString(scorer, score_string);
}

// Function to attempt decryption of substitution cipher
string decryptSubstCipher(const QuadgramScorer& scorer,
                          const string& ciphertext) {
  double best_score = -1e9;
  string best_str;

  for (int i = 0; i < 20; i++) {  // Try 20 random starting ciphers
    vector<char> cipher = genRandomSubstCipher();
    string temp_str = applySubstCipher(cipher, ciphertext);
    double temp_score = scoreString(scorer, clean(temp_str));
    if (temp_score > best_score) {
      best_score = temp_score;
      best_str = temp_str;
    }
  }
  return best_str;
}

// Command to decrypt substitution cipher based on user input text
void decryptSubstCipherCommand(const QuadgramScorer& scorer) {
  string dec_str;
  cout << "Enter text to substitution-cipher decrypt: ";
  getline(cin, dec_str);
  cout << decryptSubstCipher(scorer, clean(dec_str));
}

#pragma endregion SubstDec

// Function to combine lines in crypto_txt for processing as a single string
string combine_line(QuadgramScorer& scorer, vector<string> crypto_txt) {
  string decrypted_txt;
  for (const string& line : crypto_txt) {
    decrypted_txt += line;
  }
  return decryptSubstCipher(scorer, decrypted_txt);
}

// Split a decrypted string back into separate lines, matching input format
vector<string> split_output_line(string dec_txt, vector<string> crypto_txt) {
  vector<string> output;
  int begin_ind = 0;

  for (const string& line : crypto_txt) {
    int end_ind = line.size();
    output.push_back(dec_txt.substr(begin_ind, end_ind));
    begin_ind += end_ind;
  }
  return output;
}

// Write decrypted output to file
void output_str(vector<string> output, string fileout) {
  ofstream OUTF(fileout);
  for (const string& line : output) {
    OUTF << line << endl;
  }
}

// Main function to decrypt substitution cipher from a file input and output to
// file
void DecryptSubstitutionCipher(QuadgramScorer& scorer,
                               vector<string> crypto_txt, string fileout) {
  string combined_text = combine_line(scorer, crypto_txt);
  vector<string> decrypted_lines = split_output_line(combined_text, crypto_txt);
  output_str(decrypted_lines, fileout);
}
