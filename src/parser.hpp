#pragma once

#include <variant>
#include <vector>
#include <iostream> // Include for error messages and exit calls

#include "./arena.hpp"
#include "./tokenization.hpp"

// Define the various node structs for the abstract syntax tree (AST)
struct NodeTermIntLit
{
    Token int_lit;
};

struct NodeTermIdent
{
    Token ident;
};

struct NodeExpr;

struct NodeTermParen
{
    NodeExpr *expr;
};

struct NodeBinExprAdd
{
    NodeExpr *lhs;
    NodeExpr *rhs;
};

struct NodeBinExprMulti
{
    NodeExpr *lhs;
    NodeExpr *rhs;
};

struct NodeBinExprSub
{
    NodeExpr *lhs;
    NodeExpr *rhs;
};

struct NodeBinExprDiv
{
    NodeExpr *lhs;
    NodeExpr *rhs;
};

struct NodeBinExpr
{
    std::variant<NodeBinExprAdd *, NodeBinExprMulti *, NodeBinExprSub *, NodeBinExprDiv *> var;
};

struct NodeTerm
{
    std::variant<NodeTermIntLit *, NodeTermIdent *, NodeTermParen *> var;
};

struct NodeExpr
{
    std::variant<NodeTerm *, NodeBinExpr *> var;
};

struct NodeStmtExit
{
    NodeExpr *expr;
};

struct NodeStmtLet
{
    Token ident;
    NodeExpr *expr;
};

struct NodeStmt;

struct NodeScope
{
    std::vector<NodeStmt *> stmts;
};

struct NodeStmtIf
{
    NodeExpr *expr;
    NodeScope *scope;
};

struct NodeStmt
{
    std::variant<NodeStmtExit *, NodeStmtLet *, NodeScope *, NodeStmtIf *> var;
};

struct NodeProg
{
    std::vector<NodeStmt *> stmts;
};

class Parser
{
public:
    // Constructor that takes a vector of tokens as input
    inline explicit Parser(std::vector<Token> tokens)
        : m_tokens(std::move(tokens)),
          m_allocator(1024 * 1024 * 4) // 4MB
    {
    }

    std::optional<NodeTerm *> parse_term()
    {
        // Try to consume an integer literal
        if (auto int_lit = try_consume(TokenType::int_lit))
        {
            // Create a NodeTermIntLit object and assign the consumed value
            auto term_int_lit = m_allocator.alloc<NodeTermIntLit>();
            term_int_lit->int_lit = int_lit.value();

            // Create a NodeTerm object and assign the NodeTermIntLit as its var
            auto term = m_allocator.alloc<NodeTerm>();
            term->var = term_int_lit;

            // Return the created NodeTerm object
            return term;
        }

        // Try to consume an identifier
        else if (auto ident = try_consume(TokenType::ident))
        {
            // Create a NodeTermIdent object and assign the consumed value
            auto expr_ident = m_allocator.alloc<NodeTermIdent>();
            expr_ident->ident = ident.value();

            // Create a NodeTerm object and assign the NodeTermIdent as its var
            auto term = m_allocator.alloc<NodeTerm>();
            term->var = expr_ident;

            // Return the created NodeTerm object
            return term;
        }

        // Try to consume an open parenthesis
        else if (auto open_paren = try_consume(TokenType::open_paren))
        {
            // Parse the expression inside the parenthesis
            auto expr = parse_expr();

            // Check if the expression was successfully parsed
            if (!expr.has_value())
            {
                // Print an error message and exit if parsing failed
                std::cerr << "Unable to Parse Expression" << std::endl;
                exit(EXIT_FAILURE);
            }

            // Try to consume a closing parenthesis
            try_consume(TokenType::close_paren, "Expected Close Paren");

            // Create a NodeTermParen object and assign the parsed expression as its expr
            auto term_paren = m_allocator.alloc<NodeTermParen>();
            term_paren->expr = expr.value();

            // Create a NodeTerm object and assign the NodeTermParen as its var
            auto term = m_allocator.alloc<NodeTerm>();
            term->var = term_paren;

            // Return the created NodeTerm object
            return term;
        }

        // Return an empty optional if none of the above conditions are met
        else
        {
            return {};
        }
    }

    std::optional<NodeExpr *> parse_expr(int min_prec = 0)
    {
        // Parse the left-hand term
        std::optional<NodeTerm *> term_lhs = parse_term();
        if (!term_lhs.has_value())
        {
            return {};
        }
        auto expr_lhs = m_allocator.alloc<NodeExpr>();
        expr_lhs->var = term_lhs.value();

        while (true)
        {
            // Check the current token and its precedence
            std::optional<Token> curr_tok = peek();
            std::optional<int> prec;
            if (curr_tok.has_value())
            {
                prec = bin_prec(curr_tok->type);
                if (!prec.has_value() || prec < min_prec)
                {
                    break;
                }
            }
            else
            {
                break;
            }

            // Consume the operator token
            Token op = consume();

            // Set the next minimum precedence for the recursive call
            int next_min_prec = prec.value() + 1;

            // Parse the right-hand expression
            auto expr_rhs = parse_expr(next_min_prec);

            if (!expr_rhs.has_value())
            {
                std::cerr << "Unable to Parse Expression" << std::endl;
                exit(EXIT_FAILURE);
            }

            auto expr = m_allocator.alloc<NodeBinExpr>();
            auto expr_lhs2 = m_allocator.alloc<NodeExpr>();

            // Construct the appropriate binary expression based on the operator type
            if (op.type == TokenType::plus)
            {
                auto add = m_allocator.alloc<NodeBinExprAdd>();
                expr_lhs2->var = expr_lhs->var;
                add->lhs = expr_lhs2;
                add->rhs = expr_rhs.value();
                expr->var = add;
            }
            else if (op.type == TokenType::star)
            {
                auto multi = m_allocator.alloc<NodeBinExprMulti>();
                expr_lhs2->var = expr_lhs->var;
                multi->lhs = expr_lhs2;
                multi->rhs = expr_rhs.value();
                expr->var = multi;
            }
            else if (op.type == TokenType::sub)
            {
                auto sub = m_allocator.alloc<NodeBinExprSub>();
                expr_lhs2->var = expr_lhs->var;
                sub->lhs = expr_lhs2;
                sub->rhs = expr_rhs.value();
                expr->var = sub;
            }
            else if (op.type == TokenType::div)
            {
                auto div = m_allocator.alloc<NodeBinExprDiv>();
                expr_lhs2->var = expr_lhs->var;
                div->lhs = expr_lhs2;
                div->rhs = expr_rhs.value();
                expr->var = div;
            }
            else
            {
                assert(false); // Unreachable
            }

            // Update the left-hand expression with the new binary expression
            expr_lhs->var = expr;
        }

        return expr_lhs;
    }

    std::optional<NodeScope *> parse_scope()
    {
        // Check if there is an opening curly brace
        if (!try_consume(TokenType::open_curly).has_value())
        {
            return {};
        }

        // Allocate memory for NodeScope object
        auto scope = m_allocator.alloc<NodeScope>();

        // Loop through and parse each statement
        while (auto stmt = parse_stmt())
        {
            // Add parsed statement to the list of statements in the scope
            scope->stmts.push_back(stmt.value());
        }

        // Check if there is a closing curly brace
        try_consume(TokenType::close_curly, "Expected `}`");

        // Return the parsed scope
        return scope;
    }

    std::optional<NodeStmt *> parse_stmt()
    {
        // Check if the next token is "exit"
        if (peek().has_value() && peek().value().type == TokenType::exit && peek(1).has_value() && peek(1).value().type == TokenType::open_paren)
        {
            consume(); // Consume "exit"
            consume(); // Consume "("
            auto stmt_exit = m_allocator.alloc<NodeStmtExit>();

            // Parse the expression
            if (auto node_expr = parse_expr())
            {
                stmt_exit->expr = node_expr.value();
            }
            else
            {
                std::cerr << "Invalid Expression" << std::endl;
                exit(EXIT_FAILURE);
            }

            try_consume(TokenType::close_paren, "Expected `)`");
            try_consume(TokenType::semi, "Expected `;`");

            auto stmt = m_allocator.alloc<NodeStmt>();
            stmt->var = stmt_exit;
            return stmt;
        }

        // Check if the next tokens are "let", identifier, and "="
        else if (peek().has_value() && peek().value().type == TokenType::let && peek(1).has_value() && peek(1).value().type == TokenType::ident && peek(2).has_value() && peek(2).value().type == TokenType::eq)
        {
            consume(); // Consume "let"
            auto stmt_let = m_allocator.alloc<NodeStmtLet>();
            stmt_let->ident = consume(); // Consume identifier
            consume();                   // Consume "="

            // Parse the expression
            if (auto expr = parse_expr())
            {
                stmt_let->expr = expr.value();
            }
            else
            {
                std::cerr << "Invalid Expression" << std::endl;
                exit(EXIT_FAILURE);
            }

            try_consume(TokenType::semi, "Expected `;`");

            auto stmt = m_allocator.alloc<NodeStmt>();
            stmt->var = stmt_let;
            return stmt;
        }

        // Check if the next token is "{"
        else if (peek().has_value() && peek().value().type == TokenType::open_curly)
        {
            if (auto scope = parse_scope())
            {
                // TODO: Handle the parsed scope
            }
            else
            {
                std::cerr << "Invalid Scope" << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        // Check if the next token is "if"
        else if (auto if_ = try_consume(TokenType::if_))
        {
            try_consume(TokenType::open_paren, "Expected `(`");
            auto stmt_if = m_allocator.alloc<NodeStmtIf>();

            // Parse the expression
            if (auto expr = parse_expr())
            {
                stmt_if->expr = expr.value();
            }
            else
            {
                std::cerr << "Invalid Expression" << std::endl;
                exit(EXIT_FAILURE);
            }

            try_consume(TokenType::close_paren, "Expected `)`");

            // Parse the scope
            if (auto scope = parse_scope())
            {
                stmt_if->scope = scope.value();
            }
            else
            {
                std::cerr << "Invalid Scope" << std::endl;
                exit(EXIT_FAILURE);
            }

            auto stmt = m_allocator.alloc<NodeStmt>();
            stmt->var = stmt_if;
            return stmt;
        }

        return {};
    }

    std::optional<NodeProg> parse_prog()
    {
        NodeProg prog; // Create an empty NodeProg object to store the parsed statements

        while (peek().has_value()) // Continue parsing until there are no more tokens left
        {
            if (auto stmt = parse_stmt()) // Parse the next statement
            {
                prog.stmts.push_back(stmt.value()); // Add the parsed statement to the NodeProg object
            }
            else
            {
                std::cerr << "Invalid Statement" << std::endl; // Print an error message if the statement is invalid
                exit(EXIT_FAILURE);                            // Terminate the program
            }
        }

        return prog; // Return the parsed NodeProg object
    }

private:
    // Helper functions for parsing

    // Refactored code that adheres to the given rules
    [[nodiscard]] inline std::optional<Token> peek(int offset = 0) const
    {
        // Check if the index + offset is out of range
        if (m_index + offset >= m_tokens.size())
        {
            return {}; // Return an empty optional
        }
        else
        {
            // Return the token at the index + offset
            return m_tokens.at(m_index + offset);
        }
    }

    inline Token consume() { return m_tokens.at(m_index++); }

    // Refactored the try_consume function to follow the given rules.

    // Added a comment to describe the purpose of the function
    // and its parameters.
    inline Token try_consume(TokenType type, const std::string &err_msg)
    {
        // Check if there is a token to peek at and if its type matches
        // the specified type.
        if (peek().has_value() && peek().value().type == type)
        {
            // Return the consumed token.
            return consume();
        }
        else
        {
            // Print the error message to stderr.
            std::cerr << err_msg << std::endl;

            // Exit the program with a failure status.
            exit(EXIT_FAILURE);
        }
    }

    // Refactored code to improve readability and maintainability

    inline std::optional<Token> try_consume(TokenType type)
    {
        // Check if there is a token to peek at and if its type matches the desired type
        if (peek().has_value() && peek().value().type == type)
        {
            // If the condition is true, consume the token and return it
            return consume();
        }
        else
        {
            // If the condition is false, return an empty optional
            return {};
        }
    }

    const std::vector<Token> m_tokens;
    size_t m_index = 0;
    ArenaAllocator m_allocator;
};
