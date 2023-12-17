#include <iostream>
#include <regex>
#include <string>
#include <sstream>
#include <vector>
#include <string>
#include <utility>

void lexicalAnalyzer(std::string input, std::vector<std::pair<std::string, std::string>>& vec) {
    std::regex identifiers("^[a-zA-Z_][a-zA-Z0-9_]*$");
    std::regex numbers("^[0-9]+$");
    std::regex operators("^[+*/=-]$");
    std::regex reserved_words("loop|do");
    std::regex characters(";|\\{|\\}|\\(|\\)");

    std::istringstream stringstream(input);
    std::string token;

    while (stringstream >> token) {
        if (std::regex_match(token, reserved_words)) {
            vec.push_back(std::make_pair(token, "reserved_word"));
        }
        else if (std::regex_match(token, characters)) {
            vec.push_back(std::make_pair(token, "character"));
        }
        else if (std::regex_match(token, identifiers)) {
            vec.push_back(std::make_pair(token, "identifier"));
        }
        else if (std::regex_match(token, numbers)) {
            vec.push_back(std::make_pair(token, "number"));
        }
        else if (std::regex_match(token, operators)) {
            vec.push_back(std::make_pair(token, "operator"));
        }
        else {
            vec.push_back(std::make_pair(token, "unknown_character"));
            return;
        }
    }
}

struct Node {
    std::string value;
    Node* left;
    Node* right;
    std::string oper;

    Node(std::string val) : value(val), left(nullptr), right(nullptr) {}
};

class SyntaxParser {
private:
    std::vector<Node*> priority_nodes;
    std::vector<std::vector<std::pair<std::string, std::string>>> tokens;
public:
    std::vector<Node*> rootes;
public:
    SyntaxParser(const std::vector<std::pair<std::string, std::string>> vec) {
        std::vector<std::pair<std::string, std::string>> tmp;
        for (auto it : vec) {
            if (it.first == ";") {
                tokens.push_back(tmp);
                tmp.clear();
                continue;
            }
            if (it.second != "character" && it.second != "reserved_word") {
                tmp.push_back(it);
            }
        }
    }
    void buildParseTree() {
        for (int i = 0; i < tokens.size(); ++i) {
            if (tokens[i][1].first == "=") {
                tokens[i].push_back(tokens[i][1]);
                tokens[i].push_back(tokens[i][0]);
                tokens[i].erase(tokens[i].begin());
                tokens[i].erase(tokens[i].begin());
            }
        }

        for (int i = 0; i < tokens.size(); ++i) {
            std::vector<std::pair<std::string, std::string>> vec;
            vec = tokens[i];
            int node_num = 1;
            while (true) {
                for (int i = 0; i < vec.size(); ++i) {
                    if (vec[i].first == "*" || vec[i].first == "/") {
                        if (i != 0 && i != vec.size()) {
                            if ((vec[i - 1].second == "identifier" || vec[i - 1].second == "number" || vec[i - 1].second == "priority_node") &&
                                (vec[i + 1].second == "identifier" || vec[i + 1].second == "number" || vec[i + 1].second == "priority_node")) {
                                Node* tmp = new Node("n" + std::to_string(node_num));
                                if (vec[i - 1].second == "priority_node") {
                                    for (auto it : priority_nodes) {
                                        if (it->value == vec[i - 1].first)
                                            tmp->left = it;
                                    }
                                }
                                else {
                                    tmp->left = new Node(vec[i - 1].first);
                                }
                                if (vec[i + 1].second == "priority_node") {
                                    for (auto it : priority_nodes) {
                                        if (it->value == vec[i + 1].first)
                                            tmp->right = it;
                                    }
                                }
                                else {
                                    tmp->right = new Node(vec[i + 1].first);
                                }
                                tmp->oper = vec[i].first;
                                priority_nodes.push_back(tmp);
                                vec.erase(vec.begin() + i - 1, vec.begin() + i + 2);
                                vec.insert(vec.begin() + i - 1, std::make_pair("n" + std::to_string(node_num), "priority_node"));
                                i = i - 3;
                                node_num++;
                                continue;
                            }
                        }
                    }
                }
                break;
            }

            while (vec.size() != 1) {
                for (int i = 0; i < vec.size(); ++i) {
                    if (vec[i].first == "+" || vec[i].first == "-" || vec[i].first == "=") {
                        if (i != 0 && i != vec.size()) {
                            if ((vec[i - 1].second == "identifier" || vec[i - 1].second == "number" || vec[i - 1].second == "priority_node") &&
                                (vec[i + 1].second == "identifier" || vec[i + 1].second == "number" || vec[i + 1].second == "priority_node")) {
                                Node* tmp = new Node("n" + std::to_string(node_num));
                                if (vec[i - 1].second == "priority_node") {
                                    for (auto it : priority_nodes) {
                                        if (it->value == vec[i - 1].first)
                                            tmp->left = it;
                                    }
                                }
                                else {
                                    tmp->left = new Node(vec[i - 1].first);
                                }
                                if (vec[i + 1].second == "priority_node") {
                                    for (auto it : priority_nodes) {
                                        if (it->value == vec[i + 1].first)
                                            tmp->right = it;
                                    }
                                }
                                else {
                                    tmp->right = new Node(vec[i + 1].first);
                                }
                                tmp->oper = vec[i].first;
                                priority_nodes.push_back(tmp);
                                vec.erase(vec.begin() + i - 1, vec.begin() + i + 2);
                                if (i > vec.size()) {
                                    vec.insert(vec.end(), std::make_pair("n" + std::to_string(node_num), "priority_node"));
                                }
                                else {
                                    vec.insert(vec.begin() + i - 1, std::make_pair("n" + std::to_string(node_num), "priority_node"));
                                }
                                i = i - 3;
                                node_num++;
                                continue;
                            }
                        }
                    }
                }
            }
            rootes.push_back(priority_nodes.back());
        }
    }
    void printTree(Node* node, int depth) {
        if (node) {
            printTree(node->right, depth + 1);
            for (int i = 0; i < depth; i++) {
                std::cout << "   ";
            }
            std::cout << "|--" << node->value;
            if (!node->oper.empty()) {
                std::cout << "(" << node->oper << ")";
            }
            std::cout << std::endl;
            printTree(node->left, depth + 1);
        }
    }
};



int main() {
    setlocale(LC_ALL, "");
    std::vector<std::pair<std::string, std::string>> lexems;
    std::string input = 
        R"(loop do
        {
            x = 10 + 25 * 5 + 35 / 5 * 2 + 12 ; 
            y = x * 30 ;
        }
        )";
    lexicalAnalyzer(input, lexems);

    SyntaxParser syntaxParser(lexems);
    syntaxParser.buildParseTree();
    for (int i = 0; i < syntaxParser.rootes.size(); ++i) {
        std::cout << "Root num -> " << i + 1 << "\n\n";
        syntaxParser.printTree(syntaxParser.rootes[i], 0);
        std::cout << "\n\n";
    }
    return 0;
}