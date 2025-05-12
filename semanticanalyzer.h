#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include <unordered_map>
#include <string>

enum class VarType { INTEGER, REAL, BOOLEAN, UNKNOWN };

class SemanticAnalyzer {
public:
    void analyze(const ASTNode& node);

private:
    std::unordered_map<std::string, VarType> symbolTable;

    void checkNode(const ASTNode& node);
    VarType getExpressionType(const ASTNode& node);
    void checkAssignment(const ASTNode& node);
    void checkVariableDeclaration(const ASTNode& node);
    void checkWriteln(const ASTNode& node);
    void throwSemanticError(const std::string& message, size_t line);
};

#endif // SEMANTIC_H
