#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"
#include "error.h"
#include <memory>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<ASTNode> parse();

private:
    bool isAtEnd() const {
            return current_pos >= tokens.size() - 1;
        }
    std::vector<Token> tokens;
    size_t current_pos = 0;
    size_t current_line = 1;

    const Token& current() const;
    const Token& peek() const;
    void advance();
    bool match(TokenType type, const std::string& value = "");
    void consume(TokenType type, const std::string& value, const std::string& error_msg);

    std::unique_ptr<ASTNode> parseProgram();
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseCompound();
    std::unique_ptr<ASTNode> parseAssignment();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseFactor();
    std::unique_ptr<ASTNode> parseVariableDeclaration();
    std::unique_ptr<ASTNode> parseWriteln();
};

#endif // PARSER_H
