#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>

#include "./generation.hpp"
#include "./parser.hpp"
#include "./tokenization.hpp"

// Define TokenType enum and Token struct (provided code)

// Tokenize the input string
std::vector<Token> tokenize(const std::string &str)
{
    std::vector<Token> tokens;
    std::string buf;

    for (int i = 0; i < str.length(); i++)
    {
        char c = str.at(i);
        if (std::isalpha(c))
        {
            // Handle alpha characters (e.g., keywords like 'exit')
            buf.push_back(c);
            i++;
            while (std::isalnum(str.at(i)))
            {
                buf.push_back(str.at(i));
                i++;
            }
            i--;

            if (buf == "exit")
            {
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
        else if (std::isspace(c))
        {
            // Ignore whitespace
            continue;
        }
        else if (std::isdigit(c))
        {
            // Handle integer literals
            buf.push_back(c);
            i++;
            while (std::isdigit(str.at(i)))
            {
                buf.push_back(str.at(i));
                i++;
            }
            i--;
            tokens.push_back({.type = TokenType::int_lit, .value = buf});
            buf.clear();
        }
        else if (c == ';')
        {
            // Handle semicolon
            tokens.push_back({.type = TokenType::semi});
        }
        else
        {
            std::cerr << "You messed up!" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    return tokens;
}

// Generate assembly code from tokens
std::string tokens_to_asm(const std::vector<Token> &tokens)
{
    std::stringstream output;
    output << "global_start\n_start:\n";
    for (int i = 0; i < tokens.size(); i++)
    {
        const Token &token = tokens.at(i);
        if (token.type == TokenType::exit)
        {
            if (i + 1 < tokens.size() && tokens.at(i + 1).type == TokenType::int_lit)
            {
                if (i + 2 < tokens.size() && tokens.at(i + 2).type == TokenType::semi)
                {
                    output << "   mov rax, 60\n";
                    output << "   mov rdi, " << tokens.at(i + 1).value.value() << "\n";
                    output << "   syscall";
                }
            }
        }
    }
    return output.str();
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Incorrect usage. Correct usage is..." << std::endl;
        std::cerr << "hydro <input.hy>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string contents;
    {
        std::stringstream contents_stream;
        std::fstream input(argv[1], std::ios::in);
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
    }

    // Tokenize the input
    Tokenizer tokenizer(std::move(contents));
    std::vector<Token> tokens = tokenizer.tokenize();

    // Parse the tokens
    Parser parser(std::move(tokens));
    std::optional<NodeProg> prog = parser.parse_prog();

    if (!prog.has_value())
    {
        std::cerr << "Invalid Program" << std::endl;
        return EXIT_FAILURE;
    }

    // Generate assembly code from the parsed program
    Generator generator(prog.value());

    // Write the generated assembly code to a file
    {
        std::fstream file("out.asm", std::ios::out);
        file << generator.gen_prog();
    }

    // Assemble and link the generated assembly code
    system("nasm -felf64 out.asm");
    system("ld -o out out.o");

    return EXIT_SUCCESS;
}
