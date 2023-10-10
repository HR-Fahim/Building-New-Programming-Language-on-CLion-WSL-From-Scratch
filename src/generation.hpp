// Created by Asus on 9/28/2023.
// This comment provides information about the author and creation date of the file.

#ifndef HYDROGEN_GENERATION_H
#define HYDROGEN_GENERATION_H
// These lines are part of an include guard to prevent double inclusion of this header file.

#endif // HYDROGEN_GENERATION_H
// This line ends the include guard.

#pragma once
// This pragma ensures that the header is included only once, similar to include guards.

#include <cassert>
// Include the assert header for debugging purposes.

#include "./parser.hpp"
// Include the "parser.hpp" header file.

class Generator
{
public:
    // Constructor that takes a NodeProg as an argument.
    inline explicit Generator(NodeProg prog)
        : m_prog(std::move(prog))
    {
    }

    // Generate code for a term node.
    void gen_term(const NodeTerm *term)
    {
        struct TermVisitor
        {
            Generator &gen;
            void operator()(const NodeTermIntLit *term_int_lit) const
            {
                // Generate code for an integer literal.
                gen.m_output << "   mov rax, " << term_int_lit->int_lit.value.value() << "\n";
                gen.push("rax");
            }
            void operator()(const NodeTermIdent *term_ident) const
            {
                // Generate code for an identifier.
                auto it = std::find_if(gen.m_vars.cbegin(), gen.m_vars.cend(), [&](const Var &var)
                                       { return var.name == term_ident->ident.value.value(); });
                if (it == gen.m_vars.cend())
                {
                    std::cerr << "Identifier not used: " << term_ident->ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }

                std::stringstream offset;
                offset << "QWORD [rsp + " << (gen.m_stack_size - (*it).stack_loc - 1) * 8 << "]\n";
                gen.push(offset.str());
            }
            void operator()(const NodeTermParen *term_paren) const
            {
                // Generate code for a parenthesized expression.
                gen.gen_expr(term_paren->expr);
            }
        };
        TermVisitor visitor{.gen = *this};
        std::visit(visitor, term->var);
    }

    // Generate code for a binary expression node.
    void gen_bin_expr(const NodeBinExpr *bin_expr)
    {
        struct BinExprVisitor
        {
            Generator &gen;
            void operator()(const NodeBinExprSub *sub) const
            {
                // Generate code for subtraction.
                gen.gen_expr(sub->rhs);
                gen.gen_expr(sub->lhs);

                gen.pop("rax");
                gen.pop("rbx");
                gen.m_output << "   sub rax, rbx\n";
                gen.push("rax");
            }
            void operator()(const NodeBinExprAdd *add) const
            {
                // Generate code for addition.
                gen.gen_expr(add->rhs);
                gen.gen_expr(add->lhs);

                gen.pop("rax");
                gen.pop("rbx");
                gen.m_output << "   add rax, rbx\n";
                gen.push("rax");
            }
            void operator()(const NodeBinExprMulti *multi) const
            {
                // Generate code for multiplication.
                gen.gen_expr(multi->rhs);
                gen.gen_expr(multi->lhs);

                gen.pop("rax");
                gen.pop("rbx");
                gen.m_output << "   mul rbx\n";
                gen.push("rax");
            }
            void operator()(const NodeBinExprDiv *div) const
            {
                // Generate code for division.
                gen.gen_expr(div->rhs);
                gen.gen_expr(div->lhs);

                gen.pop("rax");
                gen.pop("rbx");
                gen.m_output << "   div rbx\n";
                gen.push("rax");
            }
        };
        BinExprVisitor visitor{.gen = *this};
        std::visit(visitor, bin_expr->var);
    }

    // Generate code for an expression node.
    void gen_expr(const NodeExpr *expr)
    {
        struct ExprVisitor
        {
            Generator &gen;
            void operator()(const NodeTerm *term) const
            {
                // Generate code for a term node.
                gen.gen_term(term);
            }
            void operator()(const NodeBinExpr *bin_expr) const
            {
                // Generate code for a binary expression node.
                gen.gen_bin_expr(bin_expr);
            }
        };
        ExprVisitor visitor{.gen = *this};
        std::visit(visitor, expr->var);
    }

    // Generate code for a scope node.
    void gen_scope(const NodeScope *scope)
    {
        begin_scope();
        for (const NodeStmt *stmt : scope->stmts)
        {
            gen_stmt(stmt);
        }
        end_scope();
    }

    // Generate code for a statement node.
    void gen_stmt(const NodeStmt *stmt)
    {
        struct StmtVisitor
        {
            Generator &gen;
            void operator()(const NodeStmtExit *stmt_exit) const
            {
                // Generate code for an exit statement.
                gen.gen_expr(stmt_exit->expr);
                gen.m_output << "   mov rax, 60\n";
                gen.pop("rdi");
                gen.m_output << "   syscall\n";
            }
            void operator()(const NodeStmtLet *stmt_let) const
            {
                // Generate code for a let statement.
                auto it = std::find_if(gen.m_vars.cbegin(), gen.m_vars.cend(), [&](const Var &var)
                                       { return var.name == stmt_let->ident.value.value(); });
                if (it != gen.m_vars.cend())
                {
                    std::cerr << "Identifier already used: " << stmt_let->ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }
                gen.m_vars.push_back({.name = stmt_let->ident.value.value(), .stack_loc = gen.m_stack_size});
                gen.gen_expr(stmt_let->expr);
            }
            void operator()(const NodeScope *scope) const
            {
                // Generate code for a scope.
                gen.gen_scope(scope);
            }
            void operator()(const NodeStmtIf *stmt_if) const
            {
                // Generate code for an if statement.
                gen.gen_expr(stmt_if->expr);
                gen.pop("rax");
                std::string label = gen.create_label();
                gen.m_output << "   test rax, rax\n";
                gen.m_output << "   jz " << label << "\n";
                gen.gen_scope(stmt_if->scope);
                gen.m_output << label << ":\n";
            }
        };
        StmtVisitor visitor{.gen = *this};
        std::visit(visitor, stmt->var);
    }

    // Generate code for the entire program.
    [[nodiscard]] std::string gen_prog()
    {
        m_output << "global _start\n_start:\n";

        for (const auto &stmt : m_prog.stmts)
        {
            gen_stmt(stmt);
        }

        m_output << "   mov rax, 60\n";
        m_output << "   mov rdi, 0\n";
        m_output << "   syscall\n";
        return m_output.str();
    }

private:
    // Push a value onto the stack.
    void push(const std::string &reg)
    {
        m_output << "   push " << reg << "\n";
        m_stack_size++;
    }

    // Pop a value from the stack.
    void pop(const std::string &reg)
    {
        m_output << "   pop " << reg << "\n";
        m_stack_size--;
    }

    // Begin a new scope.
    void begin_scope()
    {
        m_scopes.push_back(m_stack_size);
    }

    // End the current scope.
    void end_scope()
    {
        size_t pop_count = m_vars.size() - m_scopes.back();
        m_output << "   add rsp, " << pop_count * 8 << "\n";
        m_stack_size -= pop_count;
        for (int i = 0; i < pop_count; i++)
        {
            m_vars.pop_back();
        }
        m_scopes.pop_back();
    }

    // Create a unique label.
    std::string create_label()
    {
        std::stringstream ss;
        ss << "label_" << m_label_count++;
        return ss.str();
    }

    // Struct to represent variables.
    struct Var
    {
        std::string name;
        size_t stack_loc;
    };

    const NodeProg m_prog;
    std::stringstream m_output;
    size_t m_stack_size = 0;
    std::vector<Var> m_vars{};
    std::vector<size_t> m_scopes{};
    int m_label_count = 0;
};
