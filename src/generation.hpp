// //
// // Created by Asus on 9/28/2023.
// //

// #ifndef HYDROGEN_GENERATION_H
// #define HYDROGEN_GENERATION_H

// #endif //HYDROGEN_GENERATION_H

#pragma once

#include "./parser.hpp"

class Generator {
    public:
    inline Generator(NodeExit root)
    : m_root(std::move(root))
    {

    }

    [[nodiscard]] std::string generate() const
    {
        std::stringstream output;
        output << "global_start\n_start:\n";
        output << "   mov rax, 60\n";
        output << "   mov rdi, " << m_root.expr.int_lit.value.value() << "\n";
        output << "   syscall";
        return output.str();
    }

    private:
    const NodeExit m_root;
};
