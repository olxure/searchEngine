#include "../include/KeyRecommander.h"

KeyRecommander::KeyRecommander(string& query, Dictionary& dic)
    : _queryWord(query),
      _dic(dic)
{
    _dic_res = _dic.doAndQuery(query);
}

vector<string> KeyRecommander::getResult()
{
    vector<string> result;
    for (int i = 0; i < 10; ++i) {
        if (_prique.empty()) {
            break;
        }
        CandidateResult test = _prique.top();
        result.push_back(test._word);
        _prique.pop();
    }
    return result;
}

void KeyRecommander::statistic()
{
    for (auto& elem : _dic_res) {
        CandidateResult temp_can;
        temp_can._word = elem.first;
        temp_can._freq = elem.second;
        temp_can._dist = distance(_queryWord, elem.first);
        _prique.push(temp_can);
    }
}

int KeyRecommander::distance(const std::string& lhs, const std::string& rhs)
{
    size_t lhs_len = length(lhs);
    size_t rhs_len = length(rhs);
    int editDist[lhs_len + 1][rhs_len + 1];
    for (size_t idx = 0; idx <= lhs_len; ++idx) {
        editDist[idx][0] = idx;
    }
    for (size_t idx = 0; idx <= rhs_len; ++idx) {
        editDist[0][idx] = idx;
    }
    std::string sublhs, subrhs;
    for (std::size_t dist_i = 1, lhs_idx = 0; dist_i <= lhs_len; ++dist_i, ++lhs_idx) {
        size_t nBytes = nBytesCode(lhs[lhs_idx]);
        sublhs = lhs.substr(lhs_idx, nBytes);
        lhs_idx += (nBytes - 1);
        for (std::size_t dist_j = 1, rhs_idx = 0; dist_j <= rhs_len; ++dist_j, ++rhs_idx) {
            nBytes = nBytesCode(rhs[rhs_idx]);
            subrhs = rhs.substr(rhs_idx, nBytes);
            rhs_idx += (nBytes - 1);
            if (sublhs == subrhs) {
                editDist[dist_i][dist_j] = editDist[dist_i - 1][dist_j - 1];
            } else {
                editDist[dist_i][dist_j] = triple_min(editDist[dist_i][dist_j - 1] + 1,
                                                      editDist[dist_i - 1][dist_j] + 1,
                                                      editDist[dist_i - 1][dist_j - 1] + 1);
            }
        }
    }
    return editDist[lhs_len][rhs_len];
}

// Calculates the number of bytes in a character based on its first byte.
size_t KeyRecommander::nBytesCode(const char ch)
{
    if (ch & (1 << 7)) {
        int nBytes = 1;
        for (int idx = 0; idx != 6; ++idx) {
            if (ch & (1 << (6 - idx))) {
                ++nBytes;
            } else {
                break;
            }
        }
        return nBytes;
    }
    return 1;
}

// Calculates the length of a string in number of characters.
std::size_t KeyRecommander::length(const std::string& str)
{
    std::size_t ilen = 0;
    for (std::size_t idx = 0; idx != str.size(); ++idx) {
        int nBytes = nBytesCode(str[idx]);
        idx += (nBytes - 1);
        ++ilen;
    }
    return ilen;
}

int KeyRecommander::triple_min(const int& a, const int& b, const int& c)
{
    return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

