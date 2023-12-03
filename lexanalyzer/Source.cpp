#include <iostream>
#include <regex>
#include <string>
#include <sstream>

void lexicalAnalyzer(std::string input) {
    std::regex identifiers("^[a-zA-Z_][a-zA-Z0-9_]*$");
    std::regex numbers("^[0-9]+$");
    std::regex operators("^[+*/=-]$");
    std::regex reserved_words("loop|do");
    std::regex characters(";|\\{|\\}");

    std::istringstream stringstream(input);
    std::string token;

    while (stringstream >> token) {
        if (std::regex_match(token, reserved_words)) {
            std::cout << token << " -> зарезервированное слово" << std::endl;
        }
        else if (std::regex_match(token, characters)) {
            std::cout << token << " -> вспомогательный символ" << std::endl;
        }
        else if (std::regex_match(token, identifiers)) {
            std::cout << token << " -> идентификатор" << std::endl;
        }
        else if (std::regex_match(token, numbers)) {
            std::cout << token << " -> число" << std::endl;
        }
        else if (std::regex_match(token, operators)) {
            std::cout << token << " -> оператор" << std::endl;
        }
        else {
            std::cout << token << " -> ошибка" << std::endl;
            return;
        }
    }
}

int main() {
    setlocale(LC_ALL, "");
    std::string input = "loop do { x = 10 + 25 ; } y = x * 30 ;";
    lexicalAnalyzer(input);
    return 0;
}