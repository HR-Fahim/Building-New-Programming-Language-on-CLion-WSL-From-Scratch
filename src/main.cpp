#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>

enum class TokenType {
    _return,
    int_lit,
    semi
};

struct Token {
    TokenType type;
    std::optional<std::string> value;
};

std::vector<Token> tokenize(const std::string& str) {

    std::vector<Token> tokens;
    
    // for (char c : str) {
    //     std::cout << c << std::endl;
    // }

    // std::vector<char> buf {};

    std::string buf;

    for (int i = 0; i < str.length(); i++) {
        char c = str.at(i);
        if (std::isalpha(c)) {
            buf.push_back(c);
            i++;

            while (std::isalnum(str.at(i))) {
                buf.push_back(str.at(i));
                i++;
            }
            i--;

            if (buf == "return") {
                tokens.push_back({.type = TokenType::_return});
                buf.clear();
                continue;
            }
            else {
                std::cerr << "You messed up!" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        else if (std::isspace(c)) {
            continue;
        }
        else if(std::isdigit(c)) {
            buf.push_back(c);
            i++;
            while (std::isdigit(str.at(i))) {
                buf.push_back(str.at(i));
                i++;
            }
            i--;
            tokens.push_back({.type = TokenType::int_lit, .value = buf});
            buf.clear();
        }
        else if (c == ';') {
            tokens.push_back({.type = TokenType::semi});
        }
        else {
            std::cerr << "You messed up!" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    return tokens;
}

std::string tokens_to_asm(const std::vector<Token>& tokens) {
    std::stringstream output;
    output << "global_start\nstart:\n";
    for (int i=0; i < tokens.size(); i++) {
        const Token& token = tokens.at(i);
        if (token.type == TokenType::_return) {
            if (i + 1 < tokens.size() && tokens.at(i + 1).type == TokenType::int_lit) {
                if(i + 2 < tokens.size() && tokens.at(i + 2).type == TokenType::semi) {
                    output << "   mov rax, 60\n";
                    output << "   mov rdi, " << tokens.at(i + 1).value.value() << "\n";
                    output << "   syscall";
                }
            }
        }
    }
    return output.str();
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cerr << "Incorrect usage. Correct usage is..." << std::endl;
        std::cerr << "hydro <input.hy>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string contents;
    {
        std::stringstream contents_stream;
        std::fstream input(argv[1], std::ios::in);
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
    }

    // std::cout << contents << std::endl;

    // tokenize(contents);

    // std::vector<Token> thing = tokenize(contents);

    std::vector<Token> tokens = tokenize(contents);

    // std::fstream input(argv[1], std::ios::in);

    // std::cout << tokens_to_asm(tokens) << std::endl;

    {
        std::fstream file("../out.asm", std::ios::out);
        file << tokens_to_asm(tokens);
    }

    return EXIT_SUCCESS;
}