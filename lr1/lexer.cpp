#include "lexer.h"

Lexer::Lexer() : input(""), pos(0), line(1) {}

void Lexer::setInput(const string& src) {
    input = src;
    pos = 0;
    line = 1;
}

char Lexer::peek() {
    if (pos >= input.length()) return '\0';
    return input[pos];
}

char Lexer::advance() {
    if (pos >= input.length()) return '\0';
    return input[pos++];
}

void Lexer::skipWhitespace() {
    while (pos < input.length()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r') {
            pos++;
        }
        else if (c == '\n') {
            pos++;
            line++;
        }
        else {
            break;
        }
    }
}

bool Lexer::isLetter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::isDigit(char c) {
    return c >= '0' && c <= '9';
}

Token Lexer::scanIdentifier() {
    string word = "";
    int startLine = line;
    while (pos < input.length() && (isLetter(peek()) || isDigit(peek()))) {
        word += advance();
    }
    if (word == "if") return Token(TOKEN_IF, word, startLine);
    if (word == "else") return Token(TOKEN_ELSE, word, startLine);
    return Token(TOKEN_ID, word, startLine);
}

Token Lexer::scanNumber() {
    string num = "";
    int startLine = line;
    while (pos < input.length() && isDigit(peek())) {
        num += advance();
    }
    return Token(TOKEN_NUM, num, startLine);
}

Token Lexer::scanOperator() {
    char c = advance();
    int startLine = line;

    switch (c) {
    case '+': return Token(TOKEN_PLUS, "+", startLine);
    case '-': return Token(TOKEN_MINUS, "-", startLine);
    case '*': return Token(TOKEN_MUL, "*", startLine);
    case '/': return Token(TOKEN_DIV, "/", startLine);
    case '(': return Token(TOKEN_LPAREN, "(", startLine);
    case ')': return Token(TOKEN_RPAREN, ")", startLine);
    case '{': return Token(TOKEN_LBRACE, "{", startLine);
    case '}': return Token(TOKEN_RBRACE, "}", startLine);
    case ';': return Token(TOKEN_SEMI, ";", startLine);  // 分号单独处理
    case '=':
        if (peek() == '=') { advance(); return Token(TOKEN_EQ, "==", startLine); }
        return Token(TOKEN_ASSIGN, "=", startLine);
    case '<':
        if (peek() == '=') { advance(); return Token(TOKEN_LE, "<=", startLine); }
        return Token(TOKEN_LT, "<", startLine);
    case '>':
        if (peek() == '=') { advance(); return Token(TOKEN_GE, ">=", startLine); }
        return Token(TOKEN_GT, ">", startLine);
    case '!':
        if (peek() == '=') { advance(); return Token(TOKEN_NE, "!=", startLine); }
        cerr << "词法错误：非法字符 '!' 在第 " << startLine << " 行" << endl;
        return Token(TOKEN_ERROR, "!", startLine);
    default:
        cerr << "词法错误：非法字符 '" << c << "' 在第 " << startLine << " 行" << endl;
        return Token(TOKEN_ERROR, string(1, c), startLine);
    }
}

vector<Token> Lexer::tokenize() {
    vector<Token> tokens;
    pos = 0;
    line = 1;

    while (pos < input.length()) {
        skipWhitespace();
        if (pos >= input.length()) break;

        char c = peek();
        Token token;

        if (isLetter(c)) {
            token = scanIdentifier();
        }
        else if (isDigit(c)) {
            token = scanNumber();
        }
        else {
            token = scanOperator();
        }

        // 跳过分号，不加入token序列（分号作为语句分隔符但不参与语法分析）
        if (token.type == TOKEN_SEMI) {
            continue;
        }

        tokens.push_back(token);
        if (token.type == TOKEN_ERROR) break;
    }

    tokens.push_back(Token(TOKEN_END, "#", line));
    return tokens;
}

string tokenTypeToString(TokenType type) {
    switch (type) {
    case TOKEN_IF: return "IF";
    case TOKEN_ELSE: return "ELSE";
    case TOKEN_ID: return "ID";
    case TOKEN_NUM: return "NUM";
    case TOKEN_PLUS: return "PLUS";
    case TOKEN_MINUS: return "MINUS";
    case TOKEN_MUL: return "MUL";
    case TOKEN_DIV: return "DIV";
    case TOKEN_ASSIGN: return "ASSIGN";
    case TOKEN_LT: return "LT";
    case TOKEN_LE: return "LE";
    case TOKEN_GT: return "GT";
    case TOKEN_GE: return "GE";
    case TOKEN_EQ: return "EQ";
    case TOKEN_NE: return "NE";
    case TOKEN_LPAREN: return "LPAREN";
    case TOKEN_RPAREN: return "RPAREN";
    case TOKEN_LBRACE: return "LBRACE";
    case TOKEN_RBRACE: return "RBRACE";
    case TOKEN_SEMI: return "SEMI";
    case TOKEN_END: return "END";
    default: return "ERROR";
    }
}

// Token转文法符号
string tokenToSymbol(const Token& tok) {
    switch (tok.type) {
    case TOKEN_IF: return "if";
    case TOKEN_ELSE: return "else";
    case TOKEN_ID: return "id";
    case TOKEN_NUM: return "num";
    case TOKEN_PLUS: return "+";
    case TOKEN_MINUS: return "-";
    case TOKEN_MUL: return "*";
    case TOKEN_DIV: return "/";
    case TOKEN_ASSIGN: return "=";
    case TOKEN_LT: case TOKEN_LE: case TOKEN_GT:
    case TOKEN_GE: case TOKEN_EQ: case TOKEN_NE: return "rop";
    case TOKEN_LPAREN: return "(";
    case TOKEN_RPAREN: return ")";
    case TOKEN_LBRACE: return "{";
    case TOKEN_RBRACE: return "}";
    case TOKEN_SEMI: return ";";
    case TOKEN_END: return "#";
    default: return "ERROR";
    }
}

void Lexer::printTokens(const vector<Token>& tokens) {
    cout << "\n===================== 词法分析结果 =====================" << endl;
    cout << setw(8) << "序号" << setw(12) << "类别码" << setw(12) << "类别名"
        << setw(12) << "值" << setw(8) << "行号" << endl;
    cout << "--------------------------------------------------------" << endl;

    for (size_t i = 0; i < tokens.size(); i++) {
        cout << setw(8) << i
            << setw(12) << tokens[i].type
            << setw(12) << tokenTypeToString(tokens[i].type)
            << setw(12) << tokens[i].value
            << setw(8) << tokens[i].line << endl;
    }
    cout << "========================================================\n" << endl;
}
