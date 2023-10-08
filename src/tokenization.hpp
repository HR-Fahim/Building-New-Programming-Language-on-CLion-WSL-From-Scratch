//
// Created by Asus on 9/28/2023.
//

//#ifndef HYDROGEN_TOKENIZATION_HPP
//#define HYDROGEN_TOKENIZATION_HPP
//
//#endif //HYDROGEN_TOKENIZATION_HPP

#pragma once

#include <string>
#include <vector>

enum class TokenType {
    exit,
    int_lit,
    semi,
    open_paren,
    close_paren,
    ident,
    let,
    eq,
    plus,
    star,
    sub,
    div
};

// bool is_bin_op(TokenType type) {
//     switch (type) {
//         case TokenType::plus:
//         case TokenType::star:
//             return true;
//         default:
//             return false;
//     }
// }

std::optional<int> bin_prec(TokenType type) {
    switch (type) {
        case TokenType::sub:
        case TokenType::plus:
            return 0;
        case TokenType::div:
        case TokenType::star:
            return 1;
        default:
            return {};
    }
}

struct Token {
    TokenType type;
    std::optional<std::string> value;
};

class Tokenizer {
    public:
        inline explicit Tokenizer(const std::string& src)
            : m_src(std::move(src))
        {

        }

        inline std::vector<Token> tokenize()
        {
    //        std::vector<Token> tokens;

            // for (char c : str) {
            //     std::cout << c << std::endl;
            // }

            // std::vector<char> buf {};

            std::string buf;
            std::vector<Token> tokens;

            while (peek().has_value()){

                if (std::isalpha(peek().value())) {
                    buf.push_back(consume());
                    while (peek().has_value() && std::isalnum(peek().value())) {
                        buf.push_back(consume());
                    }
                    if (buf == "exit") {
                        tokens.push_back({.type = TokenType::exit});
                        buf.clear();
                        // continue;
                    }
                    else if (buf == "let") {
                        tokens.push_back({.type = TokenType::let});
                        buf.clear();
                        // continue;
                    }
                    else
                    {
                        // std::cerr << "You messed up!" << std::endl;
                        // exit(EXIT_FAILURE);
                        
                        tokens.push_back({.type = TokenType::ident, .value = buf});
                        buf.clear();
                        // continue;
                    }
                }
                else if (std::isdigit(peek().value())) {
                    buf.push_back(consume());
                    while (peek().has_value() && std::isdigit(peek().value())) {
                        buf.push_back(consume());
                    }
                    tokens.push_back({.type = TokenType::int_lit, .value = buf});
                    buf.clear();
                    // continue;
                }
                else if (peek().value() == '(') {
                    consume();
                    tokens.push_back({.type = TokenType::open_paren});
                    // continue;
                }
                else if (peek().value() == ')') {
                    consume();
                    tokens.push_back({.type = TokenType::close_paren});
                    // continue;
                }
                else if (peek().value() == ';') {
                    consume();
                    tokens.push_back({.type = TokenType::semi});
                    // continue;
                }
                else if (peek().value() == '=') {
                    consume();
                    tokens.push_back({.type = TokenType::eq});
                    // continue;
                }
                else if (peek().value() == '+') {
                    consume();
                    tokens.push_back({.type = TokenType::plus});
                    // continue;
                }
                else if (peek().value() == '-') {
                    consume();
                    tokens.push_back({.type = TokenType::sub});
                    // continue;
                }
                else if (peek().value() == '*') {
                    consume();
                    tokens.push_back({.type = TokenType::star});
                    // continue;
                }
                else if (peek().value() == '/') {
                    consume();
                    tokens.push_back({.type = TokenType::div});
                    // continue;
                }
                else if (std::isspace(peek().value())) {
                    consume();
                    // continue;
                }
                else {
                    std::cerr << "You messed up!" << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
            m_index = 0;
            return tokens;

            // // for (int i = 0; i < str.length(); i++) {
            // //     char c = str.at(i);
            //     if (std::isalpha(c)) {
            //         buf.push_back(c);
            //         i++;

            //         while (std::isalnum(str.at(i))) {
            //             buf.push_back(str.at(i));
            //             i++;
            //         }
            //         i--;

            //         if (buf == "exit") {
            //             tokens.push_back({.type = TokenType::exit});
            //             buf.clear();
            //             continue;
            //         }
            //         else {
            //             std::cerr << "You messed up!" << std::endl;
            //             exit(EXIT_FAILURE);
            //         }
            //     }
            //     else if (std::isspace(c)) {
            //         continue;
            //     }
            //     else if(std::isdigit(c)) {
            //         buf.push_back(c);
            //         i++;
            //         while (std::isdigit(str.at(i))) {
            //             buf.push_back(str.at(i));
            //             i++;
            //         }
            //         i--;
            //         tokens.push_back({.type = TokenType::int_lit, .value = buf});
            //         buf.clear();
            //     }
            //     else if (c == ';') {
            //         tokens.push_back({.type = TokenType::semi});
            //     }
            //     else {
            //         std::cerr << "You messed up!" << std::endl;
            //         exit(EXIT_FAILURE);
            //     }
            // }
            // return tokens;
        }

    private:

        [[nodiscard]] inline std::optional<char> peek(int offset = 0) const
        {
            if (m_index + offset >= m_src.length()) {
                return {};
            }
            else {
                return m_src.at(m_index + offset);
            }
        }

    inline char consume() { return m_src.at(m_index++); }

    const std::string& m_src;
    size_t m_index = 0;
};
