#include "ast.h"
#include <iostream>

void printAST(const ASTNode& node, int depth) {
    std::cout << std::string(depth * 2, ' ') << node.name;
    if (!node.token.value.empty()) {
        std::cout << " [" << node.token.value << "]";
    }
    std::cout << std::endl;

    for (const auto& child : node.children) {
        printAST(*child, depth + 1);
    }
}
