#ifndef AST_H
#define AST_H

#include "lexer.h"
#include <memory>
#include <vector>

struct ASTNode {
    std::string name;
    Token token;
    std::vector<std::unique_ptr<ASTNode>> children;
    ASTNode(const std::string& name) : name(name) {}
    ASTNode(const std::string& name, const Token& token) : name(name), token(token) {}
};

void printAST(const ASTNode& node, int depth = 0);

#endif // AST_H
