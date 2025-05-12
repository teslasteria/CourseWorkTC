#include "lexer.h"
#include <cctype>
#include <stdexcept>
#include <cstring>
#include <iostream>

using namespace std;

Lexer::Lexer(const std::string& source) : source(source) {}
std::vector<Token> Lexer::tokenize() {
    std::cout << "\n=== DEBUG: Source content ===\n";
        for (char c : source) {
            std::cout << (c == '\n' ? "\\n" :
                         c == '\r' ? "\\r" :
                         std::string(1, c));
        }
        std::cout << "\n=== END DEBUG ===\n";

    std::vector<Token> tokens;
    std::cout << "\n=== Begin ===\n" << endl;

    while (pos < source.length()) {
        try {
            std::cout << "Position: " << pos
                      << ", Symbol: '" << current() << "'"
                      << " (Code: " << (int)current() << ")" << endl;

            Token token = nextToken();

            if (token.type != DELIMITER || token.value != " ") {
                tokens.push_back(token);
                std::cout << "token added: "
                          << tokenTypeToString(token.type)
                          << " '" << token.value << "'" << endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "ERROR at position " << pos
                      << ": " << e.what() << endl;
            throw;
        }
    }

    tokens.push_back({KEYWORD, "end", line});
    std::cout << "=== Completed. Found: "
              << tokens.size() << " tokens ===" << endl;

    return tokens;
}

std::string Lexer::tokenTypeToString(TokenType type) {
    switch(type) {
        case KEYWORD: return "KEYWORD";
        case OPERATOR: return "OPERATOR";
        case DELIMITER: return "DELIMITER";
        case IDENTIFIER: return "IDENTIFIER";
        case NUMBER: return "NUMBER";
        case BOOLEAN: return "BOOLEAN";
        default: return "UNKNOWN";
    }
}

char Lexer::current() const {
    return (pos < source.length()) ? source[pos] : '\0';
}

char Lexer::peek() const {
    return (pos + 1 < source.length()) ? source[pos + 1] : '\0';
}

void Lexer::advance() {
    if (pos < source.length()) {
               if (source[pos] == '\n') line++;
               pos++;
     }
}

void Lexer::skipWhitespace() {
    while (isspace(current()) && current() != '\n') advance();
}


void Lexer::skipComment() {
    advance(); // Пропускаем '('
    advance(); // Пропускаем '*'

    while (pos < source.length()) {
        if (current() == '*' && peek() == ')') {
            advance();
            advance();

            return;
        }

        if (current() == '\n') {
            line++;
        }
        advance();
    }

    throw std::runtime_error("Uncompleted comment");
}

Token Lexer::nextToken() {
    while (pos < source.length()) {
        char c = current();
        std::cout << "Processing char '" << c << "' at pos " << pos << std::endl;

        // Пропускаем пробелы и табы
        if (c == ' ' || c == '\t') {
            advance();
            continue;
        }

        if (c == '%' || c == '!' || c == '$') {
            Token t = {KEYWORD, std::string(1, c), line};
            advance();
            return t;
        }

        // Обрабатываем переводы строк
        if (c == '\n') {
            advance();
            return {DELIMITER, "\\n", line-1};
        }

        // Идентификаторы и ключевые слова
        if (isalpha(c)) {
            return readIdentifier();
        }

        // Числа
        if (isdigit(c)) {
            return readNumber();
        }

        // Операторы
        if (strchr("+-*/=<>!:&|", c)) {
            return readOperator();
        }

        // Разделители
        if (strchr(";,:.(){}", c)) {
            Token t = {DELIMITER, std::string(1, c), line};
            advance();
            return t;
        }

        throw std::runtime_error("Unknown character: '" + std::string(1, c) + "'");
    }

    return {KEYWORD, "end", line};
}

Token Lexer::readIdentifier() {
    size_t start = pos;

    while (isalnum(current()) || current() == '_') {
        advance();
    }
    std::string value = source.substr(start, pos - start);
    TokenType type = KEYWORDS.count(value) ? KEYWORD : IDENTIFIER;

    return {type, value, line};
}

Token Lexer::readNumber() {
    size_t start = pos;
    while (isdigit(current())) advance();
    if (current() == '.') {
        advance();
        while (isdigit(current())) advance();
    }

    return {NUMBER, source.substr(start, pos - start), line};
}

Token Lexer::readOperator() {
    switch (current()) {
        case ':':
            advance();
            if (current() == '=') {
                advance();
                return {OPERATOR, ":=", line};
            }
            return {DELIMITER, ":", line};

        case '!':
            advance();
            if (current() == '=') {
                advance();
                return {OPERATOR, "!=", line};
            }
            return {KEYWORD, "!", line};  // Тип данных, а не оператор

        case '%':
        case '$': {
            // Фиксим ошибку: создаем область видимости с помощью {}
            Token t = {KEYWORD, std::string(1, current()), line};
            advance();
            return t;
        }

        default: {
            char op = current();
            advance();
            return {OPERATOR, std::string(1, op), line};
        }
    }
}

Token Lexer::readDelimiter() {
    char delim = current();
    advance();

    return {DELIMITER, std::string(1, delim), line};
}
