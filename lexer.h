#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <string>
#include <unordered_set>

enum TokenType {
    KEYWORD, OPERATOR, DELIMITER, IDENTIFIER, NUMBER, BOOLEAN
};

struct Token {
    TokenType type;
    std::string value;
    size_t line;
};

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();
    static std::string tokenTypeToString(TokenType type);
private:
    std::string source;
    size_t pos = 0;
    size_t line = 1;
    const std::unordered_set<std::string> KEYWORDS = {
        "begin", "end", "if", "else", "for", "while",
        "readln", "writeln", "%", "!", "$"
    };

    char current() const;
    char peek() const;
    void advance();
    void skipWhitespace();
    void skipComment();
    Token nextToken();
    Token readIdentifier();
    Token readNumber();
    Token readOperator();
    Token readDelimiter();
};

#endif // LEXER_H
