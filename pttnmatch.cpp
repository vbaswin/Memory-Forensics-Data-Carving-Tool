#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <climits>

using namespace std;

const int q = INT_MAX;
const int d = 256;

int rabinKarp(string pattern, string text) 
{
    int M = pattern.length();
    int N = text.length();
    int i, j;
    int p = 0; // hash value for pattern
    int t = 0; // hash value for txt
    int h = 1;

     for (i = 0; i < M - 1; i++)
        h = (h * d) % q;

    for (i = 0; i < M; i++) {
        p = (d * p + pattern[i]) % q;
        t = (d * t + text[i]) % q;
    }

    for (i = 0; i <= N - M; i++) 
    {
        if (p == t) {
            for (j = 0; j < M; j++) {
                if (text[i + j] != pattern[j]) {
                    break;
                }
            }
 
            // if p == t and pat[0...M-1] = txt[i, i+1,
            // ...i+M-1]
 
            if (j == M)
            {
               /*  cout << "Pattern found at index " << i
                     << endl; */

                return i;
            }
                
        }
 
        // Calculate hash value for next window of text:
        // Remove leading digit, add trailing digit
        if (i < N - M) {
            t = (d * (t - text[i] * h) + text[i + M]) % q;
 
            // We might get negative value of t, converting
            // it to positive
            if (t < 0)
                t = (t + q);
        }
    }
    return -1;
}

int main() {
    vector<string> patterns = {"FF D8", "47 49 46 38", "25 50 44 46"};//jpeg,gif,pdf
    string text = "00 00 00 10 45 2F 8A 11 C8 FF D8 58 AA 9A FF D9 87 47 49 46 38 48 93 F9 E3 F8 7F 00 00 48 93 F9 E3 F8 7F 00 00 93 F9 E3 F8 7F 00 00 87 74 07 25 50 44 46 ";
    vector<pair<string, int>> res;

    /* cout << text << endl;
    for (auto x : patterns)
        cout << x << endl;
    cout << endl<< endl; */

    for (auto pattern : patterns) {
        //cout << "invoking rabin karp algo" << endl;
        int pos;
        pos = rabinKarp(pattern, text);
        res.push_back(make_pair(pattern, pos));
        
        //cout << "rabinKarp finish for 1 pattern\t" << pos << endl;
    }

    for (auto x : res)
        cout << x.first << " : " << x.second << endl;
    return 0;
}
