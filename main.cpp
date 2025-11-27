#include <bits/stdc++.h>
#include <regex>
#include <string>
#include <cmath>
#include <utility>
using namespace std;
// sample input: (++2 + 3 / 22) ++-+- √3 * (+90 - 22 / 2) ++-+- 5 * √(3+2)

int operation[59];

struct Token {
    string kind;
    string str_value;
    float num_value;
};

pair<vector<Token>, vector<Token>> generate_kid (vector<Token> tokens) {
    char min_op = ']';
    int min_op_index = 0;
    for (int i = 0; i < tokens.size(); i++) {
        if (tokens[i].kind != "operation") {
            continue;
        }
        if (operation[tokens[i].str_value[0] - 36] <= operation[min_op - 36]) {
            min_op = tokens[i].str_value[0];
            min_op_index = i;
        }
    }
    return make_pair(vector<Token>(tokens.begin() + 0, tokens.begin() + min_op_index + 1), vector<Token>(tokens.begin() + min_op_index + 1, tokens.begin() + tokens.size()));
}



struct Node {
    int id;
    string kind;
    string str_value;
    Node* left_child;
    Node* right_child;
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

vector<Token> tokenize(string math) {
    vector<Token> tokens;
    int index = 0;
    while (index < math.length()) {
        if (math[index] == '+' || math[index] == '-' || math[index] == '*' || math[index] == '/' || math[index] == '^') {
            Token new_token;
            new_token.kind = "operation";
            new_token.str_value = math[index];
            new_token.num_value = 0;
            tokens.push_back(new_token);
            index++;
            continue;
        }

        if ('0' <= math[index] && math[index] <= '9') {
            string new_number = newNumberValue(index, math);
            Token new_token;
            new_token.kind = "number";
            new_token.str_value = "";
            new_token.num_value = float(stoi(new_number));
            tokens.push_back(new_token);
            continue;
        }

        if (math[index] == '(') {
            string new_value = newParenthesesValue(index, math);
            Token new_token;
            new_token.kind = "math";
            new_token.str_value = new_value;
            new_token.num_value = 0;
            tokens.push_back(new_token);
            continue;
        }

        if (math[index] == '$') {
            index++;
            if (math[index] == '(') {
                string new_value = "$(";
                new_value += newParenthesesValue(index, math);
                new_value += ')';
                Token new_token;
                new_token.kind = "math";
                new_token.str_value = new_value;
                new_token.num_value = 0;
                tokens.push_back(new_token);
            } else {
                string new_number = newNumberValue(index, math);
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

    return tokens;
}

// void build_tree(vector<Token> tokens) {
//     generate_kid();

// }

int main() {
    operation['$' - 36] = 6;
    operation['+' - 36] = 1;
    operation['-' - 36] = 1;
    operation['*' - 36] = 2;
    operation['/' - 36] = 2;
    operation['^' - 36] = 6;
    operation['^' - 1] = 10;

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
    // auto res = generate_kid(tokens);
    // vector<Token> v1 = res.first;
    // vector<Token> v2 = res.second;
    // for (int i = 0; i < v1.size(); i++) {
    //     if (v1[i].kind == "number") cout << v1[i].num_value << ' ';
    //     else cout << v1[i].str_value << ' ';
    // }
    // cout << endl;
    // for (int i = 0; i < v2.size(); i++) {
    //     if (v2[i].kind == "number") cout << v2[i].num_value << ' ';
    //     else cout << v2[i].str_value << ' ';
    // }
    // cout << endl;
    // cout << result << endl;
    // for (int i = 0; i < tokens.size(); i++) {
    //     if (tokens[i].kind == "number") cout << tokens[i].num_value << ' ';
    //     else cout << tokens[i].str_value << ' ';
    // }
    ofstream file("tree.dot");
    auto root = generate_kid(tokens);
    vector<Token> left_ch = root.first;
    vector<Token> right_ch = root.second;
    
    return 0;
}