////
//// Created by Asus on 9/28/2023.
////
//
//#ifndef HYDROGEN_PERSER_H
//#define HYDROGEN_PERSER_H
//
//#endif //HYDROGEN_PERSER_H

#pragma once

#include <variant>

#include "./tokenization.hpp"

struct NodeExprInLit {
    Token int_lit;
};

struct NodeExprIdent {
    Token ident;
};

struct NodeExpr {
    std::variant<NodeExprInLit, NodeExprIdent> var;
};

struct NodeStmtExit {
    NodeExpr expr;
};

struct NodeStmtLet {
    Token ident;
    NodeExpr expr;
};

struct NodeStmt {
    std::variant<NodeStmtExit, NodeStmtLet> var;
};

struct NodeProg {
    std::vector<NodeStmt> stmts;
};

// struct NodeExit {
//     NodeExpr expr;
// };


class Parser {
    public:
    inline explicit Parser(std::vector<Token> tokens)
    : m_tokens(std::move(tokens))
    {

    }

    std::optional<NodeExpr> parse_expr() {

        if (peek().has_value() && peek().value().type == TokenType::int_lit) {
            // return NodeExpr{.int_lit = consume()};
            return NodeExpr {.var = NodeExprInLit{.int_lit = consume()}};
        }
        else if (peek().has_value() && peek().value().type == TokenType::ident) {
            return NodeExpr {.var = NodeExprIdent{.ident = consume()}};
        }
        else {
            return {};
        }
    }

    std::optional<NodeStmt> parse_stmt() {
        if (peek().has_value() && peek().value().type == TokenType::exit && peek(1).has_value()
            && peek(1).value().type == TokenType::open_paren) {
            consume();
            consume();
            NodeStmtExit stmt_exit;

            if (auto node_expr = parse_expr()) {
                stmt_exit = {.expr = node_expr.value()};
            } else {
                std::cerr << "Invalid Expression" << std::endl;
                exit(EXIT_FAILURE);
            }
            if (peek().has_value() && peek().value().type == TokenType::close_paren) {
                consume();
            } else {
                std::cerr << "Expected `)`" << std::endl;
                exit(EXIT_FAILURE);
            }
            if (peek().has_value() && peek().value().type == TokenType::semi) {
                consume(); // Consume the semicolon
            } else {
                std::cerr << "Expected `;`" << std::endl;
                exit(EXIT_FAILURE);
            }
            return NodeStmt{.var = stmt_exit};
        } else if (peek().has_value() && peek().value().type == TokenType::let
                   && peek(1).has_value() && peek(1).value().type == TokenType::ident
                   && peek(2).has_value() && peek(2).value().type == TokenType::eq) {
            consume();
            auto stmt_let = NodeStmtLet {.ident = consume()};
            consume();
            if (auto node_expr = parse_expr()) {
                stmt_let.expr = node_expr.value();
            } else {
                std::cerr << "Invalid Expression" << std::endl;
                exit(EXIT_FAILURE);
            }
            if (peek().has_value() && peek().value().type == TokenType::semi) {
                consume(); // Consume the semicolon
            } else {
                std::cerr << "Expected `;`" << std::endl;
                exit(EXIT_FAILURE);
            }
            return NodeStmt{.var = stmt_let};
        } else {
            std::cerr << "Invalid Statement" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    std::optional<NodeProg> parse_prog() {
        NodeProg prog;
        while (peek().has_value()) {
            if (auto stmt = parse_stmt()) {
                prog.stmts.push_back(stmt.value());
            }
            else {
                std::cerr << "Invalid Statement" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        return prog;
    }


    // std::optional<NodeExit> parse() {
    //     std::optional<NodeExit> exit_node;
    //     while (peek().has_value()) {
    //         if (peek().value().type == TokenType::exit && peek(1).has_value()
    //         && peek(1).value().type == TokenType::open_paren) {
    //             consume();
    //             consume();
    //             if (auto node_expr = parse_expr()) {
    //                 exit_node = NodeExit{.expr = node_expr.value()};
    //             } else {
    //                 std::cerr << "Invalid Expression" << std::endl;
    //                 exit(EXIT_FAILURE);
    //             }
    //             if (peek().has_value() && peek().value().type == TokenType::close_paren) {
    //                 consume();
    //             } else {
    //                 std::cerr << "Expected `)`" << std::endl;
    //                 exit(EXIT_FAILURE);
    //             }
    //             if (peek().has_value() && peek().value().type == TokenType::semi) {
    //                 consume(); // Consume the semicolon
    //             } else {
    //                 std::cerr << "Expected `;`" << std::endl;
    //                 exit(EXIT_FAILURE);
    //             }
    //         } else {
    //             std::cerr << "Invalid Statement" << std::endl;
    //             exit(EXIT_FAILURE);
    //         }
    //     }
    //     m_index = 0;
    //     return exit_node;
    // }

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

        [[nodiscard]] inline std::optional<Token> peek(int offset = 0) const
                {
                if (m_index + offset >= m_tokens.size()) {
                    return {};
                }
                else {
                    return m_tokens.at(m_index + offset);
                }
            }

        inline Token consume() { return m_tokens.at(m_index++); }

    const std::vector<Token> m_tokens;
    size_t m_index = 0;
};