#include <bits/stdc++.h>
#include <regex>
#include <string>
#include <cmath>

using namespace std;
// sample input: (++2 + 3 / 22) ++-+- √3 * (+90 - 22 / 2) ++-+- 5 * √(3+2)

struct Token {
    string kind;
    string str_value;
    float num_value;
};

string newNumberValue(int& index, string result) {
    int start_number_index = index;
    string new_number = ""; new_number.push_back(result[index]);
    index++;
    while ('0' <= result[index] && result[index] <= '9') {
        new_number.push_back(result[index]);
        index++;
    }
    return new_number;
}

string newParenthesesValue(int& index, string result) {
    string parentheses = "(";
    int start_parentheses_index = index;
    string new_value = "";
    index++;
    while (parentheses != "") {
        new_value.push_back(result[index]);
        if (result[index] == '(') parentheses.push_back('(');
        else if (result[index] == ')') parentheses.pop_back();
        index++;
    }
    new_value.pop_back();
    return new_value;
}

int main() {
    string input; getline(cin, input);
    regex sqrt_pattern("\xFB");
    input = regex_replace(input, sqrt_pattern, "$");
    input.erase(remove(input.begin(), input.end(), ' '), input.end());
    // regex pattern("([+-]{2,})([A-Za-z0-9$])");
    // cout << input << endl;
    regex pattern("([+-]{2,})");
    
    string result;
    auto begin = sregex_iterator(input.begin(), input.end(), pattern);
    auto end = sregex_iterator();

    size_t last_index = 0;
    for (auto now = begin; now != end; ++now) {
        smatch m = *now;

        // cout << "positions: " << m.position() << ' ' << m.length() << ' ' << last_index << endl;
        result += input.substr(last_index, m.position() - last_index);

        string signs = m[1].str();
        size_t next_index = m.position() + m.length();
        if (next_index >= input.size()) continue;
        char next_char = input[next_index];
        // cout << signs << ' ' << next_char  << ' ' << result << endl;

        int minus_count = count(signs.begin(), signs.end(), '-');

        if (minus_count % 2 == 1)
            result += "-" + string(1, next_char);
        else
            result += "+" + string(1, next_char);

        last_index = m.position() + m.length() + 1;
    }

    // cout << endl << input.substr(last_index);
    result += input.substr(last_index);
    // cout << result << endl;
    regex new_pattern("\\(\\+(\\d+)");

    result = regex_replace(result, new_pattern, "($1");
    // cout << result << endl;

    bool open_parentheses = false;

    vector<Token> tokens;

    int index = 0;
    while (index < result.length()) {

        if (result[index] == '+' || result[index] == '-' || result[index] == '*' || result[index] == '/' || result[index] == '^') {
            Token new_token;
            new_token.kind = "operation";
            new_token.str_value = result[index];
            new_token.num_value = 0;
            tokens.push_back(new_token);
            index++;
            continue;
        }

        if ('0' <= result[index] && result[index] <= '9') {
            string new_number = newNumberValue(index, result);
            Token new_token;
            new_token.kind = "number";
            new_token.str_value = "";
            new_token.num_value = float(stoi(new_number));
            tokens.push_back(new_token);
            continue;
        }

        if (result[index] == '(') {
            string new_value = newParenthesesValue(index, result);
            Token new_token;
            new_token.kind = "math";
            new_token.str_value = new_value;
            new_token.num_value = 0;
            tokens.push_back(new_token);
            continue;
        }

        if (result[index] == '$') {
            index++;
            if (result[index] == '(') {
                string new_value = "$(";
                new_value += newParenthesesValue(index, result);
                new_value += ')';
                Token new_token;
                new_token.kind = "math";
                new_token.str_value = new_value;
                new_token.num_value = 0;
                tokens.push_back(new_token);
            } else {
                string new_number = newNumberValue(index, result);
                Token new_token;
                new_token.kind = "number";
                new_token.str_value = "";
                new_token.num_value = sqrt(stoi(new_number));
                tokens.push_back(new_token);
            }
            continue;
        }

        index++;
    }

    cout << result << endl;
    for (int i = 0; i < tokens.size(); i++) {
        if (tokens[i].kind == "number") cout << tokens[i].num_value << ' ';
        else cout << tokens[i].str_value << ' ';
    }
    return 0;
}