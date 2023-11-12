# Compiler Design

Compiler design refers to the process of creating a compiler, which is a specialized software program that translates high-level source code written in a programming language into machine code or an intermediate code. The primary goal of a compiler is to facilitate the execution of the source code on a target computer system.

![Compiler Design](https://github.com/HR-Fahim/Constructing-New-Programming-Language-in-CLion-From-Scratch-WSL/assets/66734379/7c7ea2b5-8c5f-41d6-b334-a86453be88b8)

Here are the key components and stages involved in compiler design:

1. **Lexical Analysis:**
   - The first phase of a compiler is lexical analysis, where the source code is broken down into tokens. Tokens are the smallest units of meaning in a programming language, such as keywords, identifiers, operators, and literals.
   - A lexer or scanner is responsible for this phase, and it generates a stream of tokens as output.

2. **Syntax Analysis:**
   - The syntax analysis phase, also known as parsing, checks the syntactic structure of the source code based on a specified grammar. It creates a parse tree or an abstract syntax tree (AST) that represents the hierarchical structure of the code.
   - A parser is used to implement this phase.

3. **Semantic Analysis:**
   - Semantic analysis follows syntax analysis and checks the meaning of the program. It ensures that the code adheres to the language's semantics and performs context-sensitive checks.
   - The output of this phase is an annotated syntax tree, where additional information about the semantics of the code is attached to the nodes.

4. **Intermediate Code Generation:**
   - The compiler generates an intermediate code that serves as an intermediate representation of the source code. This code is independent of the target machine and facilitates easier optimization.
   - Common intermediate code forms include three-address code or an abstract machine code.

5. **Code Optimization:**
   - The compiler performs various optimizations on the intermediate code to improve the efficiency of the generated machine code. This includes optimizations such as constant folding, loop optimization, and dead code elimination.

6. **Code Generation:**
   - In this phase, the compiler generates the target machine code or assembly code from the optimized intermediate code. The output is specific to the target architecture or platform.
   - The code generator takes care of mapping the intermediate code to the corresponding machine instructions.

7. **Code Linking and Assembly:**
   - If the compilation process involves multiple source files, a linker combines the generated code with libraries and produces an executable file.
   - In the case of interpreted languages, this step may not be necessary, as the interpreter can directly execute the intermediate code.

8. **Error Handling:**
   - Throughout the compilation process, the compiler must handle errors gracefully. It should provide meaningful error messages to help programmers identify and fix issues in their code.

Efficient compilers play a crucial role in software development by translating high-level code into executable machine code while optimizing performance.

# Project Details

- Compiler design involves several key aspects. In the lexical analysis phase, a set of rules is defined using regular expressions to identify and tokenize the source code. Hydrogen is used to create a lexer or scanner that processes the source code and produces a stream of tokens.

![Tokenization](https://github.com/HR-Fahim/Constructing-New-Programming-Language-in-CLion-From-Scratch-WSL/assets/66734379/2c70b30e-1e18-4bc9-8b04-46d38b62bd37)

- For syntax analysis, Hydrogen is employed to define the grammar of the programming language. Rules are written for parsing the tokens generated in the lexical analysis phase, building a parse tree or an abstract syntax tree (AST). This step involves creating a parser.

![Parser](https://github.com/HR-Fahim/Constructing-New-Programming-Language-in-CLion-From-Scratch-WSL/assets/66734379/39189cd5-ffb2-48bb-82a7-ac338aeca3b6)

- After obtaining the AST, semantic analysis is implemented. This involves checking the correctness of the code in terms of its meaning. Hydrogen is used to traverse the AST and perform various checks like type checking, scoping, and other semantic rules.

![AST](https://github.com/HR-Fahim/Constructing-New-Programming-Language-in-CLion-From-Scratch-WSL/assets/66734379/4482f137-760b-475a-b8be-139d817fd9ca)

- Hydrogen is also used to generate an intermediate representation of the code in the intermediate code generation phase. This could be a simple three-address code or any other representation that simplifies further optimization and code generation.

![Generation](https://github.com/HR-Fahim/Constructing-New-Programming-Language-in-CLion-From-Scratch-WSL/assets/66734379/205406c2-1d68-4c25-b34e-87acfb589b9d)

- To optimize the code, Hydrogen can be utilized to implement various techniques. Common optimizations include constant folding, common subexpression elimination, and loop optimization. The intermediate representation is traversed, and these optimizations are applied.

- In the code generation phase, Hydrogen is used to generate the target code, whether it's assembly code or machine code. The code generator maps the intermediate representation to the specific instructions of the target architecture.

![ASM](https://github.com/HR-Fahim/Constructing-New-Programming-Language-in-CLion-From-Scratch-WSL/assets/66734379/7a72e50f-49f4-46b1-b3fa-f4d90e71a0aa)

- For code linking and assembly, Hydrogen or other tools are used to handle the linking and assembly process. This involves combining the generated code with any necessary libraries and producing the final executable.

Throughout the entire process, error handling is crucial. Hydrogen is used to report meaningful error messages when issues are encountered during lexical analysis, parsing, semantic analysis, or any other phase.

# License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.


# Resources

- *[Linux System Call Table](https://chromium.googlesource.com/chromiumos/docs/+/master/constants/syscalls.md)*
- *[Lexical analysis](https://en.wikipedia.org/wiki/Lexical_analysis)*
- *[Syntax analysis](https://en.wikipedia.org/wiki/Syntax_analysis)*
- *[Intermediate code generation](https://en.wikipedia.org/wiki/Intermediate_code)*
- *[Code generation](https://en.wikipedia.org/wiki/Code_generation)*
- *[Code linking and assembly](https://en.wikipedia.org/wiki/Code_linking_and_assembly)*
- *[Precedence Climbing (Eli Bendersky)](https://eli.thegreenplace.net/2012/08/02/parsing-expressions-by-precedence-climbing)*
- *[Support Tutorial](https://www.youtube.com/watch?v=vcSijrRsrY0&list=PLUDlas_Zy_qC7c5tCgTMYq2idyyT241qs)*

Please let me know if have any questions or suggestions!

