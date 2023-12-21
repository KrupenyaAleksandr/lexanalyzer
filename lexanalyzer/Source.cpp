#include <iostream>
#include <regex>
#include <string>
#include <sstream>
#include <vector>
#include <string>
#include <utility>
#include <format>
#include <fstream>

void lexicalAnalyzer(std::string input, std::vector<std::pair<std::string, std::string>>& vec) {
    std::regex identifiers("^[a-zA-Z_][a-zA-Z0-9_]*$");
    std::regex numbers("^[0-9]+$");
    std::regex operators("^[+*/=-]$");
    std::regex reserved_words("loop|do");
    std::regex characters(";|\\{|\\}");

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
    std::vector<Node*> roots;
public:
    SyntaxParser(const std::vector<std::pair<std::string, std::string>> vec) {
        std::vector<std::pair<std::string, std::string>> tmp;
        for (auto it : vec) {
            if (it.first == "loop") {
                tmp.push_back(std::make_pair("start_cycle", "cycle"));
                tokens.push_back(tmp);
                tmp.clear();
                continue;
            }
            if (it.first == "}") {
                tmp.push_back(std::make_pair("end_cycle", "cycle"));
                tokens.push_back(tmp);
                tmp.clear();
                continue;
            }
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
        // перемещаем приравние в конец чтобы в итоге оно получилось в корне
        for (int i = 0; i < tokens.size(); ++i) {
            if (tokens[i][0].second == "cycle") {
                roots.push_back(new Node(tokens[i][0].first));
                continue;
            }
            if (tokens[i][1].first == "=") {
                tokens[i].push_back(tokens[i][1]);
                tokens[i].push_back(tokens[i][0]);
                tokens[i].erase(tokens[i].begin());
                tokens[i].erase(tokens[i].begin());
            }
            std::vector<std::pair<std::string, std::string>> vec;
            vec = tokens[i];
            int node_num = 1;

            for (int i = 0; i < vec.size(); ++i) {
                if (vec[i].first == "*" || vec[i].first == "/") {
                    if (i != 0 && i != vec.size()) {
                        Node* tmp = new Node("__n" + std::to_string(node_num));
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
                        vec.insert(vec.begin() + i - 1, std::make_pair("__n" + std::to_string(node_num), "priority_node"));
                        i = i - 3;
                        node_num++;
                        continue;
                    }
                }
            }

            bool flag = true;
            while (vec.size() > 1) {
                for (int i = 0; i < vec.size(); ++i) {
                    if (vec[i].first == "+" || vec[i].first == "-" || vec[i].first == "=") {
                        if (i != 0 && i != vec.size()) {
                                Node* tmp = new Node("__n" + std::to_string(node_num));
                                if (vec[i - 1].second == "priority_node") {
                                    for (auto it : priority_nodes) {
                                        if (it->value == vec[i - 1].first) {
                                            if (vec[i].first == "=") {
                                                tmp->left = new Node(vec[i + 1].first);
                                                tmp->right = it;
                                                tmp->oper = vec[i].first;
                                                priority_nodes.push_back(tmp);
                                                vec.clear();
                                                flag = false;
                                                break;
                                            }
                                            else {
                                                tmp->left = it;
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (!flag) break;
                                    tmp->left = new Node(vec[i - 1].first);
                                }
                                if (!flag) break;
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
                                    vec.insert(vec.end(), std::make_pair("__n" + std::to_string(node_num), "priority_node"));
                                }
                                else {
                                    vec.insert(vec.begin() + i - 1, std::make_pair("__n" + std::to_string(node_num), "priority_node"));
                                }
                                i = i - 3;
                                node_num++;
                                continue;
                        }
                    }
                }
            }
            roots.push_back(priority_nodes.back());
            priority_nodes.clear();
        }
    }
    void generateCode() {
        std::string output = "";
        output += "#include <iostream>\n\nint main() {\n";
        for (int i = 0; i < tokens.size(); ++i) {
            for (auto it : tokens[i]) {
                if (it.second == "identifier") {
                    std::string tmp = "double " + it.first;
                    if (output.find(tmp) == std::string::npos) {
                        output += "double ";
                        output += it.first;
                        output += ";\n";
                    }
                }
            }
        }
        for (int i = 0; i < roots.size(); ++i) {
            if (roots[i]->value == "start_cycle") {
                output += "while (true) {\n";
                continue;
            }
            if (roots[i]->value == "end_cycle") {
                output += " }\n";
                continue;
            }
            generateCodeRecursive(roots[i], output);
            output += ";\n";
        }
        output += "}";
        std::ofstream out("output.txt");
        out << output;
        out.close();
    }
    void generateCodeRecursive(Node* node, std::string& output) {
        if (node) {
            if (node->left) {
                generateCodeRecursive(node->left, output);
            }
            if (node->value.rfind("__n", 0) == 0) {
                output += node->oper;
                output += " ";
            }
            else {
                output += node->value;
                output += " ";
            }
            if (node->right) {
                generateCodeRecursive(node->right, output);
            }
        }
    }
    void printTree() {
        for (int i = 0; i < roots.size(); ++i) {
            std::cout << "Root num -> " << i + 1 << "\n\n";
            printTreeRecursive(roots[i], 0);
            std::cout << "\n\n";
        }
    }
    void printTreeRecursive(Node* node, int depth) {
        if (node) {
            printTreeRecursive(node->right, depth + 1);
            for (int i = 0; i < depth; i++) {
                std::cout << "   ";
            }
            std::cout << "|" << node->value;
            if (!node->oper.empty()) {
                std::cout << "(" << node->oper << ")";
            }
            std::cout << std::endl;
            printTreeRecursive(node->left, depth + 1);
        }
    }
};



int main() {
    setlocale(LC_ALL, "");
    std::vector<std::pair<std::string, std::string>> lexems;
    std::string input =
        R"(
        a = 10 + 23 - 3 ;
        loop do
        {
            x = 10 - 25 * 5 + 35 / 5 * 2 + 12 ; 
            y = x * 30 ;
        }
        )";
    lexicalAnalyzer(input, lexems);

    /*
    * double a, x, y;
    * a = 10 + 23 - 3;
    * while (true)
    * {
    *  x = 10 - 25 * 5 + 35 / 5 * 2 + 12;
    *  y = x * 30;
    * }
    */

    SyntaxParser syntaxParser(lexems);
    syntaxParser.buildParseTree();
    syntaxParser.printTree();
    syntaxParser.generateCode();
    return 0;
}