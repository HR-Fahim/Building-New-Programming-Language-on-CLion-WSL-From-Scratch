// //
// // Created by Asus on 9/28/2023.
// //

// #ifndef HYDROGEN_GENERATION_H
// #define HYDROGEN_GENERATION_H

// #endif //HYDROGEN_GENERATION_H

#pragma once

// #include <unordered_map>
// #include <map>
#include <cassert>
#include "./parser.hpp"

class Generator {
    public:
    // inline Generator(NodeExit root)
    inline explicit Generator(NodeProg prog)
    : m_prog(std::move(prog))
    {

    }

    void gen_term(const NodeTerm* term) {
        struct TermVisitor {
            Generator* gen;
            void operator()(const NodeTermIntLit* term_int_lit) const {
                gen->m_output << "   mov rax, " << term_int_lit->int_lit.value.value() << "\n";
                gen->push("rax");
            }
            void operator()(const NodeTermIdent* term_ident) const {
                auto it = std::find_if(gen->m_vars.cbegin(), gen->m_vars.cend(), [&](const Var& var) {
                    return var.name == term_ident->ident.value.value();
                });
                if (it == gen->m_vars.cend()) {
                    std::cerr << "Identifier not used: " << term_ident->ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }
                      
                // if(!gen->m_vars.contains(term_ident->ident.value.value())){
                //     std::cerr << "Identifier not used: " << term_ident->ident.value.value() << std::endl;
                //     exit(EXIT_FAILURE);
                // }
                // const auto& var = gen->m_vars.at(term_ident->ident.value.value());

                std::stringstream offset;
                offset << "QWORD [rsp + " << (gen->m_stack_size - (*it).stack_loc -1) * 8 << "]\n";
                gen->push(offset.str());
            }
            void operator()(const NodeTermParen* term_paren) const {
                gen->gen_expr(term_paren->expr);
            }
        };
        TermVisitor visitor{.gen = this};
        std::visit(visitor, term->var);
    }

    // [[nodiscard]] std::string gen_expr(const NodeExpr& expr) const
    
    void gen_bin_expr(const NodeBinExpr* bin_expr) {
                struct BinExprVisitor {
                    Generator* gen;

                    void operator()(const NodeBinExprSub* sub) const {
                        gen->gen_expr(sub->rhs);
                        gen->gen_expr(sub->lhs);
                        
                        gen->pop("rax");
                        gen->pop("rbx");
                        gen->m_output << "   sub rax, rbx\n";
                        gen->push("rax");
                    }
                    void operator()(const NodeBinExprAdd* add) const {
                        gen->gen_expr(add->rhs);
                        gen->gen_expr(add->lhs);
                        
                        gen->pop("rax");
                        gen->pop("rbx");
                        gen->m_output << "   add rax, rbx\n";
                        gen->push("rax");
                    }
                    void operator()(const NodeBinExprMulti* multi) const {
                        // assert(false); // TODO
                        gen->gen_expr(multi->rhs);
                        gen->gen_expr(multi->lhs);
                        
                        gen->pop("rax");
                        gen->pop("rbx");
                        gen->m_output << "   mul rbx\n";
                        gen->push("rax");
                    }
                    void operator()(const NodeBinExprDiv* div) const {
                        gen->gen_expr(div->rhs);
                        gen->gen_expr(div->lhs);
                        
                        gen->pop("rax");
                        gen->pop("rbx");
                        gen->m_output << "   div rbx\n";
                        gen->push("rax");
                    }
                };
                BinExprVisitor visitor{.gen = this};
                std::visit(visitor, bin_expr->var);
            }
    
    void gen_expr(const NodeExpr* expr) {
        struct ExprVisitor {
            Generator* gen;
            // ExprVisitor(Generator* gen): gen(gen) {

            // }

            void operator()(const NodeTerm* term) const {
                // gen->m_output << "   mov rax, " << expr_in_lit->int_lit.value.value() << "\n";
                // gen->push("rax");
                // gen->m_output << "   push rax\n";

                gen->gen_term(term);
            }

            // void operator()(const NodeTermIdent* expr_ident) const {
            //     // if(!gen->m_vars.contains(expr_ident->ident.value.value())){
            //     //     std::cerr << "Identifier not used: " << expr_ident->ident.value.value() << std::endl;
            //     //     exit(EXIT_FAILURE);
            //     // }
            //     // const auto& var = gen->m_vars.at(expr_ident->ident.value.value());
            //     // std::stringstream offset;
            //     // offset << "QWORD [rsp + " << (gen->m_stack_size - var.stack_loc -1) * 8 << "]\n";
            //     // gen->push(offset.str());
            // }

            void operator()(const NodeBinExpr* bin_expr) const {
                // assert(false); // TODO
                gen->gen_bin_expr(bin_expr);
                // gen->gen_expr(bin_expr->add->lhs);
                // gen->gen_expr(bin_expr->add->rhs);
                // gen->pop("rax");
                // gen->pop("rbx");
                // gen->m_output << "   add rax, rbx\n";
                // gen->push("rax");
            }
        };

        ExprVisitor visitor{.gen = this};
        std::visit(visitor, expr->var);
    }

    // [[nodiscard]] std::string gen_stmt(const NodeStmt& stmt) const

    void gen_scope(const NodeScope* scope) {
        begin_scope();
        for (const NodeStmt* stmt : scope->stmts) {
            gen_stmt(stmt);
        }
        end_scope();
    }

    void gen_stmt(const NodeStmt* stmt) {
        struct StmtVisitor {
            Generator *gen;

            void operator()(const NodeStmtExit* stmt_exit) const {
                gen->gen_expr(stmt_exit->expr);
                gen->m_output << "   mov rax, 60\n";
                gen->pop("rdi");
                // gen->m_output << "   pop rdi,\n";
                gen->m_output << "   syscall\n";
            }

            void operator()(const NodeStmtLet* stmt_let) const {
                auto it = std::find_if(gen->m_vars.cbegin(), gen->m_vars.cend(), [&](const Var& var) {
                    return var.name == stmt_let->ident.value.value();
                });
                if (it != gen->m_vars.cend()) {
                    std::cerr << "Identifier already used: " << stmt_let->ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }
                gen->m_vars.push_back({.name = stmt_let->ident.value.value(), .stack_loc = gen->m_stack_size});
                gen->gen_expr(stmt_let->expr);
            }
            void operator()(const NodeScope* scope) const {
                // gen->begin_scope();
                // for (const NodeStmt* stmt : scope->stmts) {
                //     gen->gen_stmt(stmt);
                // }
                // gen->end_scope();

                gen->gen_scope(scope);
            }
            void operator()(const NodeStmtIf* stmt_if) const {
                // assert(false && "TODO"); // TODO
                gen->gen_expr(stmt_if->expr);
                gen->pop("rax");
                std::string label = gen->create_label();
                gen->m_output << "   test rax, rax\n";
                gen->m_output << "   jz " << label << "\n";
                gen->gen_scope(stmt_if->scope);
                gen->m_output << label << ":\n";
            }
        };
        StmtVisitor visitor{.gen = this};
        std::visit(visitor, stmt->var);
    }

    // [[nodiscard]] std::string gen_prog() const
    [[nodiscard]] std::string gen_prog()
    {
        // std::stringstream output;
        m_output << "global_start\n_start:\n";

        for (const auto& stmt : m_prog.stmts) {
            gen_stmt(stmt);
        }

        m_output << "   mov rax, 60\n";
        m_output << "   mov rdi, 0\n" ;
        m_output << "   syscall\n";
        return m_output.str();
    }

    private:

    void push(const std::string& reg){
        m_output << "   push " << reg << "\n";
        m_stack_size++;
    }

    void pop(const std::string& reg){
        m_output << "   pop " << reg << "\n";
        m_stack_size--;
    }

    void begin_scope(){
        m_scopes.push_back(m_stack_size);
    }

    void end_scope(){
        size_t pop_count = m_vars.size() - m_scopes.back();
        m_output << "   add rsp, " << pop_count * 8 << "\n";
        m_stack_size -= pop_count;
        for (int i = 0; i < pop_count; i++){
            m_vars.pop_back();
        }
        m_scopes.pop_back();
    }

    std::string create_label() {
        std::stringstream ss;
        ss << "label_" << m_label_count++;
        return ss.str();
    }

    struct Var {
        std::string name;
        size_t stack_loc;
    };

    const NodeProg m_prog;
    std::stringstream m_output;
    size_t m_stack_size = 0;
    // std::unordered_map<std::string, Var> m_vars {};
    // std::map<std::string, Var> m_vars {};
    std::vector<Var> m_vars {};
    std::vector<size_t> m_scopes {};
    int m_label_count = 0;
};
