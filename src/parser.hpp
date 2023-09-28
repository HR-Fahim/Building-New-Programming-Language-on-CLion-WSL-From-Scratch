////
//// Created by Asus on 9/28/2023.
////
//
//#ifndef HYDROGEN_PERSER_H
//#define HYDROGEN_PERSER_H
//
//#endif //HYDROGEN_PERSER_H

#pragma once
#include "./tokenization.hpp"

struct NodeExpr {
    Token int_lit;
};

struct NodeExit {
    NodeExpr expr;
};


class Parser {
    public:
    inline explicit Parser(std::vector<Token> tokens)
    : m_tokens(std::move(tokens))
    {

    }

    std::optional<NodeExpr> parse_expr() {

        if (peek().has_value() && peek().value().type == TokenType::int_lit) {
            return NodeExpr{.int_lit = consume()};
        }
        else {
            return {};
        }
    }

    std::optional<NodeExit> parse() {
        std::optional<NodeExit> exit_node;
        while (peek().has_value()) {
            if (peek().value().type == TokenType::exit) {
                consume();
                if (auto node_expr = parse_expr()) {
                    exit_node = NodeExit{.expr = node_expr.value()};
                } else {
                    std::cerr << "Invalid Expression" << std::endl;
                    exit(EXIT_FAILURE);
                }
                if (peek().has_value() && peek().value().type == TokenType::semi) {
                    consume(); // Consume the semicolon
                } else {
                    std::cerr << "Semicolon expected" << std::endl;
                    exit(EXIT_FAILURE);
                }
            } else {
                std::cerr << "Invalid Statement" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        m_index = 0;
        return exit_node;
    }

//    std::optional<NodeExit> parse() {
//        std::optional<NodeExit> exit_node;
//        while (peek().has_value()) {
//            if (peek().value().type == TokenType::exit) {
//                consume();
//                if (auto node_expr = parse_expr()) {
//                    exit_node = NodeExit{.expr = node_expr.value()};
//                } else {
//                    std::cerr << "Invalid Expression" << std::endl;
//                    exit(EXIT_FAILURE);
//                }
//                if (peek().has_value() && peek().value().type == TokenType::semi) {
//                    consume(); // Consume the semicolon
//                } else {
//                    std::cerr << "Semicolon expected" << std::endl;
//                    exit(EXIT_FAILURE);
//                }
//            } else {
//                std::cerr << "Invalid Statement" << std::endl;
//                exit(EXIT_FAILURE);
//            }
//        }
//        m_index = 0;
//        return exit_node;
//    }


private:

    [[nodiscard]] inline std::optional<Token> peek(int ahead = 1) const
            {
                if (m_index + ahead > m_tokens.size()) {
                    return {};
                }
                else {
                    return m_tokens.at(m_index);
                }
            }

        inline Token consume() { return m_tokens.at(m_index++); }

    const std::vector<Token> m_tokens;
    size_t m_index = 0;
};