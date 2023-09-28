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
    semi
};

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

            while (peak().has_value()){
                if (std::isalpha(peak().value())) {
                    buf.push_back(consume());
                    while (peak().has_value() && std::isalnum(peak().value())) {
                        buf.push_back(consume());
                    }
                    if (buf == "exit") {
                        tokens.push_back({.type = TokenType::exit});
                        buf.clear();
                        continue;
                    }
                    else
                    {
                        std::cerr << "You messed up!" << std::endl;
                        exit(EXIT_FAILURE);
                    }
                }
                else if (std::isdigit(peak().value())) {
                    buf.push_back(consume());
                    while (peak().has_value() && std::isdigit(peak().value())) {
                        buf.push_back(consume());
                    }
                    tokens.push_back({.type = TokenType::int_lit, .value = buf});
                    buf.clear();
                    continue;
                }
                else if (peak().value() == ';') {
                    consume();
                    tokens.push_back({.type = TokenType::semi});
                    continue;
                }
                else if (std::isspace(peak().value())) {
                    consume();
                    continue;
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

        [[nodiscard]] inline std::optional<char> peak(int ahead = 1) const
        {
            if (m_index + ahead > m_src.length()) {
                return {};
            }
            else {
                return m_src.at(m_index);
            }
        }

    inline char consume() { return m_src.at(m_index++); }

    const std::string& m_src;
    size_t m_index = 0;
};
