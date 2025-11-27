#include <bits/stdc++.h>
#include <regex>
#include <string>
#include <cmath>
#include <utility>
#include <fstream>
#include <cstdlib>
using namespace std;
// sample input: (++2 + 3 / 22) ++-+- √3 * (+90 - 22 / 2) ++-+- 5 * √(3+2)

int operation[59];
int id_counter = 1;

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
        if (operation[tokens[i].str_value[0] - 36] < operation[min_op - 36]) {
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
                string new_value = newParenthesesValue(index, math);
                Token new_token;
                new_token.kind = "math";
                new_token.str_value = new_value;
                new_token.num_value = 0;
                tokens.push_back(new_token);
                new_token.kind = "operation";
                new_token.str_value = "$";
                tokens.push_back(new_token);
                new_token.kind = "number";
                new_token.str_value = "";
                new_token.num_value = 0.5;
                tokens.push_back(new_token);
            } else {
                string new_number = newNumberValue(index, math);
                Token new_token;
                new_token.kind = "number";
                new_token.str_value = "";
                new_token.num_value = stoi(new_number);
                tokens.push_back(new_token);
                new_token.kind = "operation";
                new_token.str_value = "$";
                new_token.num_value = 0;
                tokens.push_back(new_token);
                new_token.kind = "number";
                new_token.str_value = "";
                new_token.num_value = 0.5;
                tokens.push_back(new_token);
            }
            continue;
        }

        index++;
    }

    return tokens;
}

void buildTree(Node* node, vector<Token> left, vector<Token> right);

void build_number_left(Node* node, Token l) {
    node->left_child->id = id_counter; id_counter++;
    node->left_child->kind = "number";
    node->left_child->str_value = to_string(l.num_value);
    node->left_child->left_child = nullptr; node->left_child->right_child = nullptr;
}

void build_number_right(Node* node, Token r) {
    node->right_child->id = id_counter; id_counter++;
    node->right_child->kind = "number";
    node->right_child->str_value = to_string(r.num_value);
    node->right_child->right_child = nullptr; node->right_child->right_child = nullptr;
}

void build_tree_left(Node* node, Token l) {
    node->left_child->id = id_counter; id_counter++;
    node->left_child->kind = "operation";
    auto root = generate_kid(tokenize(l.str_value));
    string opt = root.first.back().str_value;
    root.first.pop_back();
    node->left_child->str_value = opt;
    buildTree(node->left_child, root.first, root.second);
}

void build_tree_right(Node* node, Token r) {
    node->right_child->id = id_counter; id_counter++;
    node->right_child->kind = "operation";
    auto root = generate_kid(tokenize(r.str_value));
    string opt = root.first.back().str_value;
    root.first.pop_back();
    node->right_child->str_value = opt;
    buildTree(node->right_child, root.first, root.second);
}


void buildTree(Node* node, vector<Token> left, vector<Token> right) {
    node->left_child = new Node();
    node->right_child = new Node();
    if (left.size() == 1 && right.size() == 1) {
        Token l = left[0], r = right[0];
        if (l.kind == "number" && r.kind == "number") {
            build_number_left(node, l);
            build_number_right(node, r);
        } else if (l.kind == "number" && r.kind == "math") {
            build_number_left(node, l);
            build_tree_right(node, r);
        } else if (l.kind == "math" && r.kind == "number") {
            build_tree_left(node, l);
            build_number_right(node, r);
        } else {
            build_tree_left(node, l);
            build_tree_right(node, r);
        }
    } else if (left.size() == 1) {
        Token l = left[0];
        if (l.kind == "number") {
            build_number_left(node, l);
        } else {
            build_tree_left(node, l);
        }
        node->right_child->id = id_counter; id_counter++;
        node->right_child->kind = "operation";
        auto root = generate_kid(right);
        string opt = root.first.back().str_value;
        root.first.pop_back();
        node->right_child->str_value = opt;
        buildTree(node->right_child, root.first, root.second);
    } else if (right.size() == 1) {
        node->left_child->id = id_counter; id_counter++;
        node->left_child->kind = "operation";
        auto root = generate_kid(left);
        string opt = root.first.back().str_value;
        root.first.pop_back();
        node->left_child->str_value = opt;
        buildTree(node->left_child, root.first, root.second);
        Token r = right[0];
        if (r.kind == "number") {
            build_number_right(node, r);
        } else {
            build_tree_right(node, r);
        }
    } else {
        node->left_child->id = id_counter; id_counter++;
        node->left_child->kind = "operation";
        auto root1 = generate_kid(left);
        string opt1 = root1.first.back().str_value;
        root1.first.pop_back();
        node->left_child->str_value = opt1;
        buildTree(node->left_child, root1.first, root1.second);
        node->right_child->id = id_counter; id_counter++;
        node->right_child->kind = "operation";
        auto root2 = generate_kid(right);
        string opt2 = root2.first.back().str_value;
        root2.first.pop_back();
        node->right_child->str_value = opt2;
        buildTree(node->right_child, root2.first, root2.second);
    }
}

void writeFile1(ofstream& file, Node* node) {
    if (node == nullptr) return;
    file << node->id << " [label=\"" << node->str_value << "\";]" << endl;
    writeFile1(file, node->left_child);
    writeFile1(file, node->right_child);
}

void writeFile2(ofstream& file, Node* node) {
    if (node == nullptr) return;
    if (node->left_child != nullptr) {
        file << node->id << " -> " << node->left_child->id << ";";
    }
    if (node->right_child != nullptr) {
        file << node->id << " -> " << node->right_child->id << ";";
    }
    writeFile2(file, node->left_child);
    writeFile2(file, node->right_child);
}

int main() {
    operation['$' - 36] = 6;
    operation['+' - 36] = 1;
    operation['-' - 36] = 1;
    operation['*' - 36] = 2;
    operation['/' - 36] = 2;
    operation['^' - 36] = 6;
    operation['!' - 36] = 6;
    operation[']' - 36] = 10;

    string input; getline(cin, input);
    regex sqrt_pattern("\xFB");
    input = regex_replace(input, sqrt_pattern, "$");
    input.erase(remove(input.begin(), input.end(), ' '), input.end());

    regex pattern("([+-]{2,})");
    
    string result;
    auto begin = sregex_iterator(input.begin(), input.end(), pattern);
    auto end = sregex_iterator();

    size_t last_index = 0;
    for (auto now = begin; now != end; ++now) {
        smatch m = *now;

        result += input.substr(last_index, m.position() - last_index);

        string signs = m[1].str();
        size_t next_index = m.position() + m.length();
        if (next_index >= input.size()) continue;
        char next_char = input[next_index];

        int minus_count = count(signs.begin(), signs.end(), '-');

        if (minus_count % 2 == 1)
            result += "-" + string(1, next_char);
        else
            result += "+" + string(1, next_char);

        last_index = m.position() + m.length() + 1;
    }

    result += input.substr(last_index);
    regex new_pattern("\\(\\+(\\d+)");
    result = regex_replace(result, new_pattern, "($1");

    vector<Token> tokens = tokenize(result);

    cout << result << endl;
    for (int i = 0; i < tokens.size(); i++) {
        if (tokens[i].kind == "number") cout << tokens[i].num_value << ' ';
        else cout << tokens[i].str_value << ' ';
    }
    auto root = generate_kid(tokens);
    vector<Token> left_ch = root.first;
    vector<Token> right_ch = root.second;
    string opt = root.first.back().str_value;
    root.first.pop_back();
    Node* head = new Node();
    head->id = id_counter; id_counter++;
    head->kind = "operation";
    head->str_value = opt;
    buildTree(head, root.first, root.second);
    Node* x = new Node();
    x = head;
    cout << endl;
    ofstream file("tree.dot");
    file << "digraph G {\n";
    writeFile1(file, head);
    writeFile2(file, head);
    file << "}\n";
    file.close();
    system("dot -Tpng tree.dot -o graph.png");

    
    return 0;
}