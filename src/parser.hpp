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

struct NodeTermParen {
    NodeExpr* expr;
};

struct NodeBinExprAdd {
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExprMulti {
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExprSub {
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExprDiv {
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExpr {
    std::variant<NodeBinExprAdd*, NodeBinExprMulti*, NodeBinExprSub*, NodeBinExprDiv*> var;
    // NodeBinExprAdd* add;
};

struct NodeTerm {
    std::variant<NodeTermIntLit*, NodeTermIdent*, NodeTermParen*> var;
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

struct NodeStmt;

struct NodeScope {
    std::vector<NodeStmt*> stmts;
};

struct NodeStmtIf {
    NodeExpr* expr;
    NodeScope* scope;
};

struct NodeStmt {
    std::variant<NodeStmtExit*, NodeStmtLet*, NodeScope*, NodeStmtIf*> var;
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
        if (auto int_lit = try_consume(TokenType::int_lit)) {
            
            auto term_int_lit = m_allocator.alloc<NodeTermIntLit>();
            term_int_lit->int_lit = int_lit.value();
            auto term = m_allocator.alloc<NodeTerm>();
            term->var = term_int_lit;
            return term;
        }
        // if (peek().has_value() && peek().value().type == TokenType::int_lit) {
        //     // return NodeExpr{.int_lit = consume()};
        //     auto term_int_lit = m_allocator.alloc<NodeTermIntLit>();
        //     // return NodeExpr {.var = NodeExprIntLit{.int_lit = consume()}};
        //     term_int_lit->int_lit = consume();
        //     auto term = m_allocator.alloc<NodeTerm>();
        //     term->var = term_int_lit;
        //     // return NodeExpr {.var = node_expr_int_lit};
        //     return term;
        // }
        else if (auto ident = try_consume(TokenType::ident)) {
            // return NodeExpr {.var = NodeExprIdent{.ident = consume()}};
            auto expr_ident = m_allocator.alloc<NodeTermIdent>();
            expr_ident->ident = ident.value();
            auto term = m_allocator.alloc<NodeTerm>();
            term->var = expr_ident;
            return term;
        } else if (auto open_paren = try_consume(TokenType::open_paren)) {
            auto expr = parse_expr();
            if (!expr.has_value()) {
                std::cerr << "Unable to Parse Expression" << std::endl;
                exit(EXIT_FAILURE);
            } 
            try_consume(TokenType::close_paren, "Expected Close Paren");
            auto term_paren = m_allocator.alloc<NodeTermParen>();
            term_paren->expr = expr.value();
            auto term = m_allocator.alloc<NodeTerm>();
            term->var = term_paren;
            return term;
        } else {
            return {};
        }
    }

    std::optional<NodeExpr*> parse_expr(int min_prec = 0) {
        std::optional<NodeTerm*> term_lhs = parse_term();
        if (!term_lhs.has_value()) {
            return {};
        }
        auto expr_lhs = m_allocator.alloc<NodeExpr>();
        expr_lhs->var = term_lhs.value();

        //auto expr = m_allocator.alloc<NodeBinExpr>();

        while (true) {
            std::optional<Token> curr_tok = peek();
            std::optional<int> prec;
            if (curr_tok.has_value()) {
                prec = bin_prec(curr_tok->type);
                if(!prec.has_value() || prec < min_prec) {
                    break;
                }
            }
            else {
                break;
            }
            Token op = consume();
            int next_min_prec = prec.value() + 1;
            auto expr_rhs = parse_expr(next_min_prec);

            if(!expr_rhs.has_value()) {
                std::cerr << "Unable to Parse Expression" << std::endl;
                exit(EXIT_FAILURE);
            }


            // expr_lhs->var = term_lhs.value();

            auto expr = m_allocator.alloc<NodeBinExpr>();

            auto expr_lhs2 = m_allocator.alloc<NodeExpr>();
            
            if(op.type == TokenType::plus) {
                auto add = m_allocator.alloc<NodeBinExprAdd>();
                // auto expr_lhs = m_allocator.alloc<NodeExpr>();
                // expr_lhs->var = term_lhs.value();
                expr_lhs2->var = expr_lhs->var;
                add->lhs = expr_lhs2;
                add->rhs = expr_rhs.value();
                expr->var = add;
            } else if (op.type == TokenType::star) {
                auto multi = m_allocator.alloc<NodeBinExprMulti>();
                expr_lhs2->var = expr_lhs->var;
                multi->lhs = expr_lhs2;
                
                multi->rhs = expr_rhs.value();
                expr->var = multi;
            } else if (op.type == TokenType::sub) {
                auto sub = m_allocator.alloc<NodeBinExprSub>();
                expr_lhs2->var = expr_lhs->var;
                sub->lhs = expr_lhs2;
                
                sub->rhs = expr_rhs.value();
                expr->var = sub;
            } else if (op.type == TokenType::div) {
                auto div = m_allocator.alloc<NodeBinExprDiv>();
                expr_lhs2->var = expr_lhs->var;
                div->lhs = expr_lhs2;
                
                div->rhs = expr_rhs.value();
                expr->var = div;
            } else {
                assert(false); // Unreachable
            }
            // int next_min_prec = prec.value() + 1;
            // auto expr_rhs = parse_expr(next_min_prec);
            expr_lhs->var = expr;
        }
        return expr_lhs;

        // if (auto term = parse_term()) {
        //     if (try_consume(TokenType::plus).has_value()) {
        //         auto bin_expr = m_allocator.alloc<NodeBinExpr>();
        //          auto bin_expr_add = m_allocator.alloc<NodeBinExprAdd>();
        //          auto lhs_expr = m_allocator.alloc<NodeExpr>();
        //          lhs_expr->var = term.value();
        //          bin_expr_add->lhs = lhs_expr;
        //         //  consume();
        //          if (auto rhs = parse_expr()) {
        //              bin_expr_add->rhs = rhs.value();
        //              bin_expr->var = bin_expr_add;
        //              auto expr = m_allocator.alloc<NodeExpr>();
        //              expr->var = bin_expr;
        //              return expr;
        //          }
        //          else {
        //              std::cerr << "Expected Expression" << std::endl;
        //              exit(EXIT_FAILURE);
        //             }
            
        //     } else {
        //         auto expr = m_allocator.alloc<NodeExpr>();
        //         expr->var = term.value();
        //         return expr;
        //     }
        // }
        // else {
        //     return {};
        // }
        // // if (peek().has_value() && peek().value().type == TokenType::int_lit) {
        // //     // return NodeExpr{.int_lit = consume()};
        // //     auto expr_int_lit = m_allocator.alloc<NodeExprIntLit>();
        // //     // return NodeExpr {.var = NodeExprIntLit{.int_lit = consume()}};
        // //     expr_int_lit->int_lit = consume();
        // //     auto expr = m_allocator.alloc<NodeExpr>();
        // //     expr->var = expr_int_lit;
        // //     // return NodeExpr {.var = node_expr_int_lit};
        // //     return expr;
        // // }
        // // else if (peek().has_value() && peek().value().type == TokenType::ident) {
        // //     // return NodeExpr {.var = NodeExprIdent{.ident = consume()}};
        // //     auto expr_ident = m_allocator.alloc<NodeExprIdent>();
        // //     expr_ident->ident = consume();
        // //     auto expr = m_allocator.alloc<NodeExpr>();
        // //     expr->var = expr_ident;
        // //     return expr;
        // // }
        // // else if (auto bin_expr = parse_bin_expr()) {
        // //     auto expr = m_allocator.alloc<NodeExpr>();
        // //     expr->var = bin_expr.value();
        // //     return expr;
        // // }
        // // else {
        // //     return {};
        // // }
    }

    std::optional<NodeScope*> parse_scope() {
        if (!try_consume(TokenType::open_curly).has_value()) {
            return {};
        }
        auto scope = m_allocator.alloc<NodeScope>();
        while (auto stmt = parse_stmt()) {
            scope->stmts.push_back(stmt.value());
        }
        try_consume(TokenType::close_curly, "Expected `}`");
        return scope;
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
            try_consume(TokenType::close_paren, "Expected `)`");
            // if (peek().has_value() && peek().value().type == TokenType::close_paren) {
            //     consume();
            // } else {
            //     std::cerr << "Expected `)`" << std::endl;
            //     exit(EXIT_FAILURE);
            // }
            try_consume(TokenType::semi, "Expected `;`");
            // if (peek().has_value() && peek().value().type == TokenType::semi) {
            //     consume(); // Consume the semicolon
            // } else {
            //     std::cerr << "Expected `;`" << std::endl;
            //     exit(EXIT_FAILURE);
            // }
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
            try_consume(TokenType::semi, "Expected `;`");
            // if (peek().has_value() && peek().value().type == TokenType::semi) {
            //     consume(); // Consume the semicolon
            // } else {
            //     std::cerr << "Expected `;`" << std::endl;
            //     exit(EXIT_FAILURE);
            // }
            // return NodeStmt{.var = stmt_let};
            auto stmt = m_allocator.alloc<NodeStmt>();
            stmt->var = stmt_let;
            return stmt;
        } else if (peek().has_value() && peek().value().type == TokenType::open_curly) {
            if (auto scope = parse_scope()) {
//                 auto stmt = m_allocator.alloc<NodeStmt>();
//                 stmt->var = scope.value();
//                 return stmt;
            } else {
                std::cerr << "Invalid Scope" << std::endl; 
                exit(EXIT_FAILURE);
            }
            // auto scope = m_allocator.alloc<NodeScope>();
            // while (auto stmt = parse_stmt()) {
            //     scope->stmts.push_back(stmt.value());
            // }
            // try_consume(TokenType::close_curly, "Expected `}`");
            
        } else if (auto if_ = try_consume(TokenType::if_)) {
            try_consume(TokenType::open_paren, "Expected `(`");
            auto stmt_if = m_allocator.alloc<NodeStmtIf>();
            if (auto expr = parse_expr()) {
                stmt_if->expr = expr.value();
            } else {
                std::cerr << "Invalid Expression" << std::endl;
                exit(EXIT_FAILURE);
            }
            try_consume(TokenType::close_paren, "Expected `)`");
            if (auto scope = parse_scope()) {
                stmt_if->scope = scope.value();
            } else {
                std::cerr << "Invalid Scope" << std::endl;
                exit(EXIT_FAILURE);
            }
            auto stmt = m_allocator.alloc<NodeStmt>();
            stmt->var = stmt_if;
            return stmt;
        }
        else {
            // std::cerr << "Invalid Statement" << std::endl;
            // exit(EXIT_FAILURE);
            return {};
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

        inline Token try_consume(TokenType type, const std::string& err_msg) {
            if (peek().has_value() && peek().value().type == type) {
                return consume();
            }
            else {
                std:: cerr << err_msg << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        inline std::optional<Token>try_consume(TokenType type) {
            if (peek().has_value() && peek().value().type == type) {
                return consume();
            }
            else {
                return {};
            }
        }

    const std::vector<Token> m_tokens;
    size_t m_index = 0;
    ArenaAllocator m_allocator;
};