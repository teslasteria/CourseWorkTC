#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <ios>
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "semanticanalyzer.h"


using namespace std;

std::string readSourceFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary); // Важно открывать в бинарном режиме
    std::string content;

    if (file) {
        char ch;
        while (file.get(ch)) {
            if (ch != '\r') {
                content += ch;
            }
        }
    } else {
        throw std::runtime_error("Cannot open file: " + path);
    }

    return content;
}

void testLexer() {
    std::string testCode = "x := 42;";
    Lexer lexer(testCode);
    auto tokens = lexer.tokenize();

    for (const auto& t : tokens) {
        std::cout << t.value << " | "
                  << Lexer::tokenTypeToString(t.type)
                  << " | line " << t.line << "\n";
    }
}

//int argc, char* argv[])

int main(int argc, char* argv[]) {
    try {
        std::string inputFile = "C:\\Users\\fyt68\\OneDrive\\Documents\\CourseWW\\program.txt"; // По умолчанию
        if (argc > 1) {
            inputFile = argv[1];
        }
        std::string sourceCode = readSourceFile(inputFile);
        std::cout << "Source code:\n\n" << sourceCode << "\n";

        std::cout << "Start tokenize" << std::endl;
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();
        std::cout << "=== Tokens ===" << std::endl;

        for (const auto& t : tokens) {
            std::cout << "[" << t.line << "] " << Lexer::tokenTypeToString(t.type)
                      << ": " << t.value << std::endl;
        }
        std::cout << "\n";


        std::cout << "=== Start Parsing ===" << std::endl;
        Parser parser(tokens);
        std::unique_ptr<ASTNode> ast = parser.parse();

        std::cout << "Abstract syntax tree (AST):\n";
        printAST(*ast);

        SemanticAnalyzer analyzer;
        analyzer.analyze(*ast);
        std::cout << "Semantic analysis passed successfully!\n";

        std::cout << "\nSuccessfully!\n";
    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << std::endl;
        return 1;

    } catch (...) {
        std::cerr << "\n!!! UNEXPECTED ERROR\n";
        return 2;

    }

    return 0;

}
