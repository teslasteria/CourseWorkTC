#include <iostream>
#include "parser.h"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::unique_ptr<ASTNode> Parser::parse() {
    return parseProgram();
}

const Token& Parser::current() const {
    return tokens[current_pos];
}

const Token& Parser::peek() const {
    static Token empty{TokenType::DELIMITER, "", 0};
    return current_pos + 1 < tokens.size() ? tokens[current_pos + 1] : empty;
}

void Parser::advance() {
    current_line = current().line;
    if (current_pos < tokens.size()) current_pos++;
}

bool Parser::match(TokenType type, const std::string& value) {
    if (current().type == type && (value.empty() || current().value == value)) {
        return true;
    }
    return false;
}

void Parser::consume(TokenType type, const std::string& value, const std::string& error_msg) {
    if (!match(type, value)) {
        throwError(error_msg, current().line);
    }
    advance();

}

std::unique_ptr<ASTNode> Parser::parseProgram() {
    auto node = std::make_unique<ASTNode>("Program");

    // Ожидаем начало программы
    consume(KEYWORD, "begin", "Program must start with 'begin'");

    // Парсим основной блок
    node->children.push_back(parseCompound());

    // Проверяем конец программы
    if (isAtEnd()) {
        throwError("Unexpected end of file", current().line);
    }

    if (!match(KEYWORD, "end")) {
        throwError("Expected 'end' at program termination", current().line);
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseCompound() {
    auto node = std::make_unique<ASTNode>("Compound");

    while (!match(KEYWORD, "end")) {
        // Пропускаем пустые строки и лишние разделители
        while (match(DELIMITER, "\\n") || match(DELIMITER, ";")) {
            advance();
        }

        // Если достигли конца, выходим
        if (match(KEYWORD, "end")) break;

        node->children.push_back(parseStatement());
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    std::cout << "Current token: " << current().value << ", next token: " << peek().value << std::endl;
    if (match(KEYWORD, "begin")) {
        return parseCompound();
    }
    else if (match(IDENTIFIER) && peek().value == ":") {  // Объявление типа: 'x : %'
        return parseVariableDeclaration();
    }
    else if (match(IDENTIFIER) && peek().value == ":=") {  // Присваивание: 'x := 10'
        return parseAssignment();
    }
    else if (match(KEYWORD, "writeln")) {
        return parseWriteln();
    }
    throwError("Unknown statement: " + current().value, current().line);
}

std::unique_ptr<ASTNode> Parser::parseAssignment() {
    auto node = std::make_unique<ASTNode>("Assignment");
    node->children.push_back(std::make_unique<ASTNode>("Variable", current()));
    advance();
    consume(OPERATOR, ":=", "Expected ':=' in assignment");
    node->children.push_back(std::move(parseExpression()));

    return node;
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
    auto node = parseTerm();

    while (match(OPERATOR, "+") || match(OPERATOR, "-") || match(OPERATOR, "||")) {
        auto op_node = std::make_unique<ASTNode>("Operator", current());
        advance();
        op_node->children.push_back(std::move(node));
        op_node->children.push_back(std::move(parseTerm()));
        node = std::move(op_node);
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseVariableDeclaration() {
    auto node = std::make_unique<ASTNode>("VariableDeclaration");
    // Чтение идентификатора (например, 'x')
    node->children.push_back(std::make_unique<ASTNode>("Variable", current()));
    advance();
    // Пропуск ':' и типа ('%', '!', '$')
    consume(DELIMITER, ":", "Expected ':' in declaration");
    if (match(KEYWORD, "%") || match(KEYWORD, "!") || match(KEYWORD, "$")) {
        node->children.push_back(std::make_unique<ASTNode>("Type", current()));
        advance();
    }
    return node;
}

std::unique_ptr<ASTNode> Parser::parseTerm() {
    auto node = parseFactor();

    while (match(OPERATOR, "*") || match(OPERATOR, "/") || match(OPERATOR, "&&")) {
        auto op_node = std::make_unique<ASTNode>("Operator", current());
        advance();
        op_node->children.push_back(std::move(node));
        op_node->children.push_back(std::move(parseFactor()));
        node = std::move(op_node);
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseFactor() {
    if (match(IDENTIFIER)) {
        auto node = std::make_unique<ASTNode>("Variable", current());
        advance();
        return node;
    }
    if (match(NUMBER)) {
        auto node = std::make_unique<ASTNode>("Number", current());
        advance();

        return node;
    }
    if (match(DELIMITER, "(")) {
        advance();
        auto node = parseExpression();
        consume(DELIMITER, ")", "Expected ')' after expression");

        return node;
    }
    throwError("Expected variable, number or expression", current().line);

    return nullptr;
}

std::unique_ptr<ASTNode> Parser::parseWriteln() {
    auto node = std::make_unique<ASTNode>("Writeln");
    consume(KEYWORD, "writeln", "Expected 'writeln'");

    // Парсим список выражений для вывода
    while (!match(DELIMITER, "\\n") && !match(KEYWORD, "end")) {
        node->children.push_back(parseExpression());
        if (match(DELIMITER, ",")) {
            advance();
        } else {
            break;
        }
    }

    return node;
}
