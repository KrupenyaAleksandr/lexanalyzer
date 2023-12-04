#include <iostream>
#include <regex>
#include <string>
#include <sstream>

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

int main() {
    setlocale(LC_ALL, "");
    std::vector<std::pair<std::string, std::string>> lexems;
    std::string input = "loop do { x = 10 + 25 ; } y = x * 30 ;";
    lexicalAnalyzer(input, lexems);
    for (int i = 0; i < lexems.size(); ++i)
        std::cout << lexems[i].first << " -> " << lexems[i].second << std::endl;
    return 0;
}