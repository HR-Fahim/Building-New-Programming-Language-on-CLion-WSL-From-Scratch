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

#include "./arena.hpp"
#include "./tokenization.hpp"

struct NodeTermIntLit {
    Token int_lit;
};

struct NodeTermIdent {
    Token ident;
};

struct NodeExpr;

struct NodeBinExprAdd {
    NodeExpr* lhs;
    NodeExpr* rhs;
};

// struct NodeBinExprMulti {
//     NodeExpr* lhs;
//     NodeExpr* rhs;
// };

struct NodeBinExpr {
    // std::variant<NodeBinExprAdd*, NodeBinExprMulti*> var;
    NodeBinExprAdd* add;
};

struct NodeTerm {
    std::variant<NodeTermIntLit*, NodeTermIdent*> var;
};

struct NodeExpr {
    // std::variant<NodeExprIntLit*, NodeExprIdent*, NodeBinExpr*> var;
    std::variant<NodeTerm*, NodeBinExpr*> var;
};

struct NodeStmtExit {
    NodeExpr* expr;
};

struct NodeStmtLet {
    Token ident;
    NodeExpr* expr;
};

struct NodeStmt {
    std::variant<NodeStmtExit*, NodeStmtLet*> var;
};

struct NodeProg {
    std::vector<NodeStmt*> stmts;
};

// struct NodeExit {
//     NodeExpr expr;
// };


class Parser {
    public:
    inline explicit Parser(std::vector<Token> tokens)
    : m_tokens(std::move(tokens)),
    m_allocator(1024 * 1024 * 4) // 4MB
    {

    }

    // std::optional<NodeBinExpr*> parse_bin_expr() {
    //     if (auto lhs = parse_expr()) {
    //         auto bin_expr = m_allocator.alloc<NodeBinExpr>();
    //         // if(peek().has_value() && peek().value().type == TokenType::plus) {
    //         //     auto bin_expr_add = m_allocator.alloc<NodeBinExprAdd>();
    //         //     bin_expr_add->lhs = lhs.value();
    //         //     consume();
    //         //     if (auto rhs = parse_expr()) {
    //         //         bin_expr_add->rhs = rhs.value();
    //         //         bin_expr->var = bin_expr_add;
    //         //         return bin_expr;
    //         //     }
    //         //     else {
    //         //         std::cerr << "Expected Expression" << std::endl;
    //         //         exit(EXIT_FAILURE);
    //         //     }
    //         }
    //         else{
    //             std::cerr << "Unsupported Binary Operator" << std::endl;
    //             exit(EXIT_FAILURE);
    //         }
    //     } else {
    //         return {};
    //     }
    // }

    std::optional<NodeTerm*> parse_term() {
        if (peek().has_value() && peek().value().type == TokenType::int_lit) {
            // return NodeExpr{.int_lit = consume()};
            auto term_int_lit = m_allocator.alloc<NodeTermIntLit>();
            // return NodeExpr {.var = NodeExprIntLit{.int_lit = consume()}};
            term_int_lit->int_lit = consume();
            auto term = m_allocator.alloc<NodeTerm>();
            term->var = term_int_lit;
            // return NodeExpr {.var = node_expr_int_lit};
            return term;
        }
        else if (peek().has_value() && peek().value().type == TokenType::ident) {
            // return NodeExpr {.var = NodeExprIdent{.ident = consume()}};
            auto expr_ident = m_allocator.alloc<NodeTermIdent>();
            expr_ident->ident = consume();
            auto term = m_allocator.alloc<NodeTerm>();
            term->var = expr_ident;
            return term;
        } else {
            return {};
        }
    }

    std::optional<NodeExpr*> parse_expr() {
        if (auto term = parse_term()) {
            if (peek().has_value() && peek().value().type == TokenType::plus) {
                auto bin_expr = m_allocator.alloc<NodeBinExpr>();
                 auto bin_expr_add = m_allocator.alloc<NodeBinExprAdd>();
                 auto lhs_expr = m_allocator.alloc<NodeExpr>();
                 lhs_expr->var = term.value();
                 bin_expr_add->lhs = lhs_expr;
                 consume();
                 if (auto rhs = parse_expr()) {
                     bin_expr_add->rhs = rhs.value();
                     bin_expr->add = bin_expr_add;
                     auto expr = m_allocator.alloc<NodeExpr>();
                     expr->var = bin_expr;
                     return expr;
                 }
                 else {
                     std::cerr << "Expected Expression" << std::endl;
                     exit(EXIT_FAILURE);
                    }
            
            } else {
                auto expr = m_allocator.alloc<NodeExpr>();
                expr->var = term.value();
                return expr;
            }
        }
        else {
            return {};
        }
        // if (peek().has_value() && peek().value().type == TokenType::int_lit) {
        //     // return NodeExpr{.int_lit = consume()};
        //     auto expr_int_lit = m_allocator.alloc<NodeExprIntLit>();
        //     // return NodeExpr {.var = NodeExprIntLit{.int_lit = consume()}};
        //     expr_int_lit->int_lit = consume();
        //     auto expr = m_allocator.alloc<NodeExpr>();
        //     expr->var = expr_int_lit;
        //     // return NodeExpr {.var = node_expr_int_lit};
        //     return expr;
        // }
        // else if (peek().has_value() && peek().value().type == TokenType::ident) {
        //     // return NodeExpr {.var = NodeExprIdent{.ident = consume()}};
        //     auto expr_ident = m_allocator.alloc<NodeExprIdent>();
        //     expr_ident->ident = consume();
        //     auto expr = m_allocator.alloc<NodeExpr>();
        //     expr->var = expr_ident;
        //     return expr;
        // }
        // else if (auto bin_expr = parse_bin_expr()) {
        //     auto expr = m_allocator.alloc<NodeExpr>();
        //     expr->var = bin_expr.value();
        //     return expr;
        // }
        // else {
        //     return {};
        // }
    }

    std::optional<NodeStmt*> parse_stmt() {
        if (peek().has_value() && peek().value().type == TokenType::exit && peek(1).has_value()
            && peek(1).value().type == TokenType::open_paren) {
            consume();
            consume();
            // NodeStmtExit stmt_exit;

            auto stmt_exit = m_allocator.alloc<NodeStmtExit>();

            if (auto node_expr = parse_expr()) {
//                stmt_exit = {.expr = node_expr.value()};
                stmt_exit->expr = node_expr.value();
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
            // return NodeStmt{.var = stmt_exit};
            auto stmt = m_allocator.alloc<NodeStmt>();
            stmt->var = stmt_exit;
            return stmt;
        } else if (peek().has_value() && peek().value().type == TokenType::let
                   && peek(1).has_value() && peek(1).value().type == TokenType::ident
                   && peek(2).has_value() && peek(2).value().type == TokenType::eq) {
            consume();
//            auto stmt_let = NodeStmtLet {.ident = consume()};
//            consume();
            auto stmt_let = m_allocator.alloc<NodeStmtLet>();
            stmt_let->ident = consume();
            consume();
            if (auto expr = parse_expr()) {
                stmt_let->expr = expr.value();
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
            // return NodeStmt{.var = stmt_let};
            auto stmt = m_allocator.alloc<NodeStmt>();
            stmt->var = stmt_let;
            return stmt;
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
    ArenaAllocator m_allocator;
};