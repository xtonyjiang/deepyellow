#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <cstdlib>
#include <regex>

using namespace std;

unordered_map<string, vector<string> > dict;
vector<pair<string, int> > words;
vector<string> letters;
vector<pair<int, string> > sols;
string result = "";

bool comp(char c1, char c2) {
    return tolower(c1) < tolower(c2);
}

// Sort string by its characters e.g. "hello" -> "ehllo"
string sortString(string s) {
    string ret = s;
    sort(ret.begin(), ret.end(), comp);
    return ret;
}

// Populates the unordered_map
void init(int size) {
    ifstream infile("dict.txt");
    string tmp;
    while (infile >> tmp) {
        if(tmp.length() >= size) {
            if(dict.count(sortString(tmp)) == 0) {
                vector<string> v;
                dict[sortString(tmp)] = v;
            }
            dict[sortString(tmp)].push_back(tmp);
        }
    }
}

bool contains(string s) {
    return dict.count(s) > 0;
}

// Finds new words
void exec() {
    sols.clear();
    for(int bitmask = 1; bitmask < (1 << letters.size()); ++bitmask) {
        string cur = "";
        int tmp = bitmask;
        int cnt = 0;
        // Use the bitmask to build the subset of letters
        while(tmp > 0) {
            if((tmp & 1) == 1) cur += letters[cnt];
            tmp >>= 1;
            cnt++;
        }
        // Existing words with letters
        for(int i = 0; i < words.size(); ++i) {
            string w = words[i].first;
            if(contains(sortString(w + cur))) {
                for(int j = 0; j < dict[sortString(w + cur)].size(); ++j) {
                    if(dict[sortString(w + cur)][j].find(w) <
                        dict[sortString(w + cur)][j].length()) continue;
                    int score = -1;
                    if(words[i].second == 0) {
                        score = cur.length();
                    }
                    else {
                        score = cur.length() + w.length();
                    }
                    sols.push_back(make_pair(-score, w + "." +
                        dict[sortString(cur + w)][j]));
                }
            }
        }
        // Just the letters on the board
        if(contains(sortString(cur))) {
            for(int i = 0; i < dict[sortString(cur)].size(); ++i) {
                sols.push_back(make_pair(-cur.length(), "." +
                    dict[sortString(cur)][i]));
            }
        }
    }
    // List top 3 words
    sort(sols.begin(), sols.end());
    int start = 2;
    if(sols.size() < 3) start = sols.size() - 1;
    for(int i = start; i >= 0; --i) {
        int split = sols[i].second.find(".");
        string s1 = sols[i].second.substr(0, split);
        string s2 = sols[i].second.substr(split + 1);
        cout << (i + 1) << ") " << s2;
        if(s1.length() > 0 ) cout << " from " + s1;
        cout << "\n";
    }
    if(sols.size() > 0) {
        string next;
        cout << "Enter (0 for me, 1 for others)(word)(.)(word): ";
        cin >> next;
        regex r0("[0-9]");
        result = next;
        if(regex_match(next, r0)) {
            int tmp = next[0] - '0';
            cout << "0" + sols[tmp - 1].second << "\n";
            result = "0" + sols[tmp - 1].second;
        }
    }
}

int main() {
    int minletters;
    cout << "Minimum letter count?: ";
    cin >> minletters;
    init(minletters);
    while(true) {
        string cur = "";
        if(result.length() > 0) {
            cur = result;
        }
        else {
            cout << "Enter (0 for me, 1 for others)(word)(.)(word): ";
            cin >> cur;
        }
        result = "";
        // Input parsing
        if(cur == ".") break;
        regex r1("[a-z]");
        if(regex_match(cur, r1)) {
            letters.push_back(cur);
            exec();
            continue;
        }
        regex r2("[0-1][a-z]*\\.[a-z]+");
        if(!regex_match(cur, r2)) {
            cout << "Invalid syntax. Please try again.\n";
            continue;
        }
        int tmp = cur[0] - '0';
        string stmp = cur.substr(1);
        int split = stmp.find(".");
        string s1 = stmp.substr(0, split);
        string s2 = stmp.substr(split + 1);
        if((s1.length() > 0 && !contains(sortString(s1))) || 
            !contains(sortString(s2))) {
            cout << contains(s2) << "\n";
            cout << "Did you make a typo?\n";
            continue;
        }
        // Stealing words
        if(s1.length() > 0) {
            for(int i = 0; i < s1.length(); ++i) {
                if(s2.find(s1[i]) == string::npos) {
                    cout << "Invalid entry. Please try again.\n";
                    continue;
                }
            }
            vector< pair<string, int> >::iterator it = std::find(
                words.begin(), words.end(), make_pair(s1, 1));
            if(it != words.end()) {
                words.erase(it);
            }
            else {
                vector< pair<string, int> >::iterator it = std::find(
                    words.begin(), words.end(), make_pair(s1, 0));
                if(it != words.end()) {
                    words.erase(it);
                }
                else {
                    cout << "Invalid entry. Please try again.\n";
                    continue;
                }
            }
            string s2copy = s2;
            string toRemove = "";
            for(int i = 0; i < s1.length(); ++i) {
                int index = s2copy.find(s1.substr(i, 1));
                s2copy[index] = '-';
            }
            for(int i = 0; i < s2copy.length(); ++i) {
                if(s2copy[i] != '-') toRemove += s2copy[i];
            }
            bool cool = true;
            for(int i = 0; i < toRemove.length(); ++i) {
                vector<string>::iterator it = find(letters.begin(),
                    letters.end(), toRemove.substr(i,1));
                if(it == letters.end()) {
                    cout << "Word not found. Please try again.\n";
                    cool = false;
                    break;
                }
            }
            if(!cool) continue;
            for(int i = 0; i < toRemove.length(); ++i) {
                vector<string>::iterator it = find(letters.begin(),
                    letters.end(), toRemove.substr(i,1));
                if(it != letters.end()) {
                    letters.erase(it);
                }
            }
            words.push_back(make_pair(s2, tmp));
        }
        // Word entirely from individual letters
        else {
            bool cool = true;
            for(int i = 0; i < s2.length(); ++i) {
                vector<string>::iterator it = find(letters.begin(),
                    letters.end(), s2.substr(i,1));
                if(it == letters.end()) {
                    cout << "Word not found. Please try again.\n";
                    cool = false;
                    break;
                }
            }
            if(!cool) continue;
            for(int i = 0; i < s2.length(); ++i) {
                vector<string>::iterator it = find(letters.begin(),
                    letters.end(), s2.substr(i,1));
                if(it != letters.end()) {
                    letters.erase(it);
                }
            }
            words.push_back(make_pair(s2, tmp));
        }
        cout << "Letters in play:\n";
        for(int i = 0; i < letters.size(); ++i) cout << letters[i] << " ";
        cout << "\nWords in play:\n";
        for(int i = 0; i < words.size(); ++i) cout << words[i].first << " ";
        cout << "\n";
        exec();
    }
}
