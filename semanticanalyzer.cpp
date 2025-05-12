#include "semanticanalyzer.h"
#include "error.h"
#include <iostream>

void SemanticAnalyzer::analyze(const ASTNode& node) {
    checkNode(node);
}

void SemanticAnalyzer::checkNode(const ASTNode& node) {
    if (node.name == "VariableDeclaration") {
        checkVariableDeclaration(node);
    }
    else if (node.name == "Assignment") {
        checkAssignment(node);
    }
//    else if (node.name == "Writeln") {
//        checkWriteln(node);
//    }

    // Рекурсивно проверяем детей
    for (const auto& child : node.children) {
        checkNode(*child);
    }
}

VarType SemanticAnalyzer::getExpressionType(const ASTNode& node) {
    if (node.name == "Variable") {
        auto it = symbolTable.find(node.token.value);
        if (it == symbolTable.end()) {
            throwSemanticError("Variable '" + node.token.value + "' is not declared", node.token.line);
        }
        return it->second;
    }
    else if (node.name == "Number") {
        return VarType::INTEGER; // Упрощённо: все числа целые
    }
    else if (node.name == "Operator") {
        // Проверяем типы операндов
        VarType left = getExpressionType(*node.children[0]);
        VarType right = getExpressionType(*node.children[1]);

        if (node.token.value == "+" || node.token.value == "-" || node.token.value == "*" || node.token.value == "/") {
            if (left != VarType::INTEGER || right != VarType::INTEGER) {
                throwSemanticError("Arithmetic operations require integer operands", node.token.line);
            }
            return VarType::INTEGER;
        }
        else if (node.token.value == "&&" || node.token.value == "||") {
            if (left != VarType::BOOLEAN || right != VarType::BOOLEAN) {
                throwSemanticError("Logical operations require boolean operands", node.token.line);
            }
            return VarType::BOOLEAN;
        }
    }
    return VarType::UNKNOWN;
}

void SemanticAnalyzer::checkAssignment(const ASTNode& node) {
    std::string varName = node.children[0]->token.value;
    VarType varType = getExpressionType(*node.children[1]);

    if (symbolTable.find(varName) == symbolTable.end()) {
        throwSemanticError("Variable '" + varName + "' is not declared", node.children[0]->token.line);
    }

    VarType declaredType = symbolTable[varName];
    if (declaredType != varType) {
        throwSemanticError("Type mismatch for variable '" + varName + "'", node.children[0]->token.line);
    }
}

void SemanticAnalyzer::checkVariableDeclaration(const ASTNode& node) {
    VarType type;
    if (node.children.back()->token.value == "%") type = VarType::INTEGER;
    else if (node.children.back()->token.value == "!") type = VarType::REAL;
    else if (node.children.back()->token.value == "$") type = VarType::BOOLEAN;

    for (size_t i = 0; i < node.children.size() - 1; ++i) {
        const auto& child = node.children[i];
        if (child->name == "Variable") {
            symbolTable[child->token.value] = type;
        }
    }
}

void SemanticAnalyzer::throwSemanticError(const std::string& message, size_t line) {
    throw std::runtime_error("Semantic error at line " + std::to_string(line) + ": " + message);
}
