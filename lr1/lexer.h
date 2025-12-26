#pragma once
#ifndef LEXER_H
#define LEXER_H

#include "common.h"

class Lexer {
private:
    string input;
    size_t pos;
    int line;

    char peek();
    char advance();
    void skipWhitespace();
    bool isLetter(char c);
    bool isDigit(char c);
    Token scanIdentifier();
    Token scanNumber();
    Token scanOperator();

public:
    Lexer();
    void setInput(const string& src);
    vector<Token> tokenize();
    void printTokens(const vector<Token>& tokens);
};

#endif
