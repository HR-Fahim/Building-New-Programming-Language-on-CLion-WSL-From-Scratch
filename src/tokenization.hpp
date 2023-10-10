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
    div,
    open_curly,
    close_curly,
    if_
};

// Refactored bin_prec function to comply with line length limit and improve readability

std::optional<int> bin_prec(TokenType type) {
    if (type == TokenType::sub || type == TokenType::plus) {
        return 0;
    } else if (type == TokenType::div || type == TokenType::star) {
        return 1;
    } else {
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
    std::string buf;
    std::vector<Token> tokens;

    // Loop until there are no more characters to process
    while (peek().has_value()) {

        if (std::isalpha(peek().value())) {
            buf.push_back(consume());
            // Keep consuming characters until a non-alphanumeric character is encountered
            while (peek().has_value() && std::isalnum(peek().value())) {
                buf.push_back(consume());
            }
            // Check if the identifier is a keyword
            if (buf == "exit") {
                tokens.push_back({.type = TokenType::exit});
                buf.clear();
            }
            else if (buf == "let") {
                tokens.push_back({.type = TokenType::let});
                buf.clear();
            }
            else if (buf == "if") {
                tokens.push_back({.type = TokenType::if_});
                buf.clear();
            }
            else {
                // If the identifier is not a keyword, treat it as an identifier token
                tokens.push_back({.type = TokenType::ident, .value = buf});
                buf.clear();
            }
        }
        else if (std::isdigit(peek().value())) {
            buf.push_back(consume());
            // Keep consuming characters until a non-digit character is encountered
            while (peek().has_value() && std::isdigit(peek().value())) {
                buf.push_back(consume());
            }
            // Add the integer literal token to the vector
            tokens.push_back({.type = TokenType::int_lit, .value = buf});
            buf.clear();
        }
        else {
            // Handle other types of tokens
            char c = consume();
            switch (c) {
                case '(':
                    tokens.push_back({.type = TokenType::open_paren});
                    break;
                case ')':
                    tokens.push_back({.type = TokenType::close_paren});
                    break;
                case ';':
                    tokens.push_back({.type = TokenType::semi});
                    break;
                case '=':
                    tokens.push_back({.type = TokenType::eq});
                    break;
                case '+':
                    tokens.push_back({.type = TokenType::plus});
                    break;
                case '-':
                    tokens.push_back({.type = TokenType::sub});
                    break;
                case '*':
                    tokens.push_back({.type = TokenType::star});
                    break;
                case '/':
                    tokens.push_back({.type = TokenType::div});
                    break;
                case '{':
                    tokens.push_back({.type = TokenType::open_curly});
                    break;
                case '}':
                    tokens.push_back({.type = TokenType::close_curly});
                    break;
                case ' ':
                case '\t':
                case '\n':
                case '\r':
                    // Ignore whitespace characters
                    break;
                default:
                    // Handle unexpected characters
                    std::cerr << "You messed up!" << std::endl;
                    exit(EXIT_FAILURE);
            }
        }
    }

    // Reset the index and return the tokens
    m_index = 0;
    return tokens;
}

    private:

// Refactored code to adhere to the 66 characters per line rule
[[nodiscard]]
inline std::optional<char> peek(int offset = 0) const
{
    // Check if the index with the offset exceeds the length of the source string
    if (m_index + offset >= m_src.length()) {
        return {}; // Return an empty optional if out of bounds
    } else {
        // Return the character at the index with the offset
        return m_src.at(m_index + offset);
    }
}

    inline char consume() { return m_src.at(m_index++); }

    const std::string& m_src;
    size_t m_index = 0;
};
