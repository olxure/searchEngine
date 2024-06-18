#ifndef OLX_PROJECT_SEARCH_ENGINES_ONLINE_INCLUDE_KEYRECOMMANDER_H_
#define OLX_PROJECT_SEARCH_ENGINES_ONLINE_INCLUDE_KEYRECOMMANDER_H_

#include "Dictionary.h"

#include <set>
#include <queue>
#include <vector>
#include <string>

using std::priority_queue;
using std::string;
using std::set;
using std::vector;

struct CandidateResult
{
    string _word;  // The suggested word
    int _freq;  // The frequency of the word in the dictionary
    int _dist;  // The distance between the suggested word and the query word
};

struct CompareCandidate {
    bool operator()(const CandidateResult& a, const CandidateResult& b) {
        if (a._dist != b._dist) {
            return a._dist > b._dist;  // Sort in ascending order based on _dist
        } else if (a._freq != b._freq) {
            return a._freq < b._freq;  // Sort in descending order based on _freq
        } else {
            return a._word > b._word;  // Sort lexicographically based on _word
        }
    }
};

class KeyRecommander
{
public:
    // Constructor
    // Initializes KeyRecommander with a query word and a dictionary
    KeyRecommander(string& query, Dictionary& dic);

    // getResult
    // Returns a vector of suggested words based on the query
    vector<string> getResult();

    // statistic
    // Performs statistic analysis on the dictionary
    void statistic();

    // distance
    // Calculates the Levenshtein distance between two strings
    int distance(const std::string & lhs, const std::string &rhs);

private:
    // nBytesCode
    // Calculates the number of bytes used to encode a character
    size_t nBytesCode(const char ch);

    // length
    // Calculates the length of a string in terms of characters
    std::size_t length(const std::string &str);

    // triple_min
    // Returns the minimum value among three integers
    int triple_min(const int &a, const int &b, const int &c);

private:
    string _queryWord;  // The query word
    Dictionary & _dic;  // Reference to the dictionary
    priority_queue<CandidateResult, vector<CandidateResult>, CompareCandidate> _prique;  // Priority queue for candidate results
    vector<std::pair<string, int>> _dic_res;  // Vector to store dictionary results
};

#endif

