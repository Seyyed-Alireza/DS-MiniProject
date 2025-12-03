#include <bits/stdc++.h>
#include <regex>
#include <string>
#include <cmath>
#include <utility>
#include <fstream>
#include <cstdlib>
#include <filesystem>

using namespace std;
using namespace std::filesystem;

// sample input: (++2 + 3 / 22) ++-+- √3 * (+90 - 22 / 2) ++-+- 5 * √(3+2)
// long input: (++2 + 3 / 22) ++-+- √3 * ((++2 + 3 / 22) ++-+- √3 * (+90 - 22 / 2) ++-+- 5 * √(3+2) - 22 / 2) ++-+- 5 * √(3+2) + ((++2 + 3 / 22) ++-+- √3 * (+90 - 22 / 2) ++-+- 5 * √(3+2))

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
    while (('0' <= result[index] && result[index] <= '9') || result[index] == '.') {
        new_number.push_back(result[index]);
        index++;
    }
    if (new_number[0] == '.') new_number = '0' + new_number;
    else if (new_number[new_number.length() - 1] == '.') new_number += '0';
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

        if (('0' <= math[index] && math[index] <= '9') || math[index] == '.') {
            string new_number = newNumberValue(index, math);
            Token new_token;
            new_token.kind = "number";
            new_token.str_value = "";
            new_token.num_value = stof(new_number);
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

void deleteZero(Node* node) {
    while (node->str_value[node->str_value.length() - 1] == '0') {
        node->str_value.pop_back();
    }
    if (node->str_value[node->str_value.length() - 1] == '.') node->str_value.pop_back();
}

int before_r = 200, before_g = 200, before_b = 200;
string randomLightColor() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(128, 255);

    int r, b, g;
    do r = dist(gen);
    while (abs(r - before_r) < 20);

    do g = dist(gen);
    while (abs(g - before_g) < 20);

    do b = dist(gen);
    while (abs(b - before_b) < 20);

    before_r = r; before_g = g; before_b = b;
    
    char buf[8];
    snprintf(buf, sizeof(buf), "#%02X%02X%02X", r, g, b);
    return string(buf);
}

bool isDarkColor(const string& hexColor) {
    if (hexColor.size() != 7 || hexColor[0] != '#') {
        cerr << "Invalid color format. Use #RRGGBB.\n";
        return false;
    }

    int r = stoi(hexColor.substr(1,2), nullptr, 16);
    int g = stoi(hexColor.substr(3,2), nullptr, 16);
    int b = stoi(hexColor.substr(5,2), nullptr, 16);

    double L = 0.299*r + 0.587*g + 0.114*b;

    return L < 128;
}

vector<string> level_colors;
vector<string> level_colors_default = {"#ffffff", "#fc3567", "#e81416", "#ffa500", "#faeb36", "#79c314", "#487de7", "#4b369d", "#70369d"};
void writeFile1(ofstream& file, Node* node, int level) {
    if (node == nullptr) return;
    if (node->str_value.find('.') != string::npos) deleteZero(node);
    while (level_colors.size() <= level + 1) {
        level_colors.push_back(randomLightColor());
    }
    string font_color = "#000000";
    if ((isDarkColor(level_colors[level]))) font_color = "#ffffff";
    
    file << node->id << " [label=\"" << node->str_value << "\", fillcolor=\"" << level_colors[level] << "\", fontcolor=\"" << font_color << "\"]" << endl;
    // color = randomLightColor();
    level++;
    writeFile1(file, node->left_child, level);
    writeFile1(file, node->right_child, level);
}

void writeFile2(ofstream& file, Node* node) {
    if (node == nullptr) return;
    if (node->left_child != nullptr) {
        file << node->id << " -> " << node->left_child->id << ";" << endl;
    }
    if (node->right_child != nullptr) {
        file << node->id << " -> " << node->right_child->id << ";" << endl;
    }
    writeFile2(file, node->left_child);
    writeFile2(file, node->right_child);
}

int graph_file_count = 1;
void drawTree(Node* head) {
    string picture_name = "graph" + to_string(graph_file_count); graph_file_count++;
    path base_path = current_path();
    path tree_path = base_path / "tree.dot";
    path picture_path = base_path / "pictures" / picture_name;
    ofstream file("tree.dot");
    file << "digraph G {\n";
    file << "graph [ dpi=120, rankdir=TB, bgcolor=\"#FAFAFA\", splines=true, nodesep=0.2, ranksep=0.5 ];\n";
    file << "node [ shape=oval, style=filled, fontname=\"Calibri\", fontsize=24, penwidth=1.4, width=1, height=0.6, color=\"#7393B3\", fillcolor=\"#DDE7F0\", fontcolor=\"#2B3A42\" ];\n";
    file << "edge [ color=\"#78909C\", penwidth=1, arrowhead=vee, arrowsize=1.0 ];\n";
    string color = randomLightColor();
    // level = 0;
    level_colors.clear();
    level_colors = level_colors_default;
    writeFile1(file, head, 0);
    writeFile2(file, head);
    file << "}\n";
    file.close();
    string command = "dot -Tpng \"" + tree_path.string() + "\" -o \"" + picture_path.string() + ".png\"";
    system(command.c_str());

    cout << "Graph of step " << graph_file_count - 1 << " created" << endl;
}

void calculate (Node* root, Node* main) {
    if (!root->left_child && !root->right_child) {
        return;
    }
    calculate(root->left_child, main);
    calculate(root->right_child, main);
    if (root->str_value == "+") {
        root->str_value = to_string(stof(root->left_child->str_value) + stof(root->right_child->str_value));
    }
    else if (root->str_value == "-") {
        root->str_value = to_string(stof(root->left_child->str_value) - stof(root->right_child->str_value));
    }
    else if (root->str_value == "*") {
        root->str_value = to_string(stof(root->left_child->str_value) * stof(root->right_child->str_value));
    }
    else if (root->str_value == "/") {
        root->str_value = to_string(stof(root->left_child->str_value) / stof(root->right_child->str_value));
    }
    else if (root->str_value == "$" || root->str_value == "^") {
        root->str_value = to_string(pow(stof(root->left_child->str_value), stof(root->right_child->str_value)));
    }
    Node* temp1 = root->left_child;
    Node* temp2 = root->right_child;
    root->left_child = nullptr;
    root->right_child = nullptr;
    delete temp1;
    delete temp2;

    drawTree(main);
    return;
}

int main() {
    operation['$' - 36] = 7;
    operation['+' - 36] = 1;
    operation['-' - 36] = 1;
    operation['*' - 36] = 2;
    operation['/' - 36] = 2;
    operation['^' - 36] = 6;
    operation[']' - 36] = 10;
    system("cls");
    cout << "Please enter a valid mathematical expression: ";
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

    cout << "Your input after normalising: " << result << endl;
    // for (int i = 0; i < tokens.size(); i++) {
    //     if (tokens[i].kind == "number") cout << tokens[i].num_value << ' ';
    //     else cout << tokens[i].str_value << ' ';
    // }
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
    cout << "Building tree completed" << endl;
    path pictures = current_path() / "pictures";
    if (exists(pictures)) {
        remove_all(pictures);
    }
    create_directory(pictures);
    
    cout << "Drawing trees and calculating..." <<endl;
    drawTree(head);
    calculate(head, head);
    cout << "All graphs were drawn" << endl;
    cout << "Final result --> " << head->str_value << endl;
    
    return 0;
}