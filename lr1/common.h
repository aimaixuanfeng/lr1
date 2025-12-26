#pragma once
#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <fstream>

using namespace std;

// ==================== Token定义 ====================
enum TokenType {
    TOKEN_IF = 1,       // if
    TOKEN_ELSE = 2,     // else
    TOKEN_ID = 3,       // 标识符
    TOKEN_NUM = 4,      // 整数
    TOKEN_PLUS = 5,     // +
    TOKEN_MINUS = 6,    // -
    TOKEN_MUL = 7,      // *
    TOKEN_DIV = 8,      // /
    TOKEN_ASSIGN = 9,   // =
    TOKEN_LT = 10,      // <
    TOKEN_LE = 11,      // <=
    TOKEN_GT = 12,      // >
    TOKEN_GE = 13,      // >=
    TOKEN_EQ = 14,      // ==
    TOKEN_NE = 15,      // !=
    TOKEN_LPAREN = 16,  // (
    TOKEN_RPAREN = 17,  // )
    TOKEN_LBRACE = 18,  // {
    TOKEN_RBRACE = 19,  // }
    TOKEN_SEMI = 20,    // ;
    TOKEN_END = 21,     // #
    TOKEN_ERROR = -1    // 错误
};

// Token结构
struct Token {
    TokenType type;
    string value;
    int line;

    Token() : type(TOKEN_ERROR), value(""), line(0) {}
    Token(TokenType t, string v, int l) : type(t), value(v), line(l) {}
};

// ==================== 产生式定义 ====================
struct Production {
    string left;
    vector<string> right;
    int len;  // 右部长度（ε产生式为0）

    Production() : len(0) {}
    Production(string l, vector<string> r) : left(l), right(r) {
        if (r.size() == 1 && r[0] == "ε") {
            len = 0;
        }
        else {
            len = r.size();
        }
    }
};

// ==================== LR(1)项目定义 ====================
struct LR1Item {
    int prodIndex;
    int dotPos;
    string lookahead;

    LR1Item() : prodIndex(0), dotPos(0), lookahead("#") {}
    LR1Item(int p, int d, string l) : prodIndex(p), dotPos(d), lookahead(l) {}

    bool operator==(const LR1Item& other) const {
        return prodIndex == other.prodIndex &&
            dotPos == other.dotPos &&
            lookahead == other.lookahead;
    }

    bool operator<(const LR1Item& other) const {
        if (prodIndex != other.prodIndex) return prodIndex < other.prodIndex;
        if (dotPos != other.dotPos) return dotPos < other.dotPos;
        return lookahead < other.lookahead;
    }
};

// ==================== 四元式定义 ====================
struct Quadruple {
    string op;
    string arg1;
    string arg2;
    string result;

    Quadruple() {}
    Quadruple(string o, string a1, string a2, string r)
        : op(o), arg1(a1), arg2(a2), result(r) {}
};

// ==================== 语义记录定义 ====================
struct SemanticRecord {
    string place;       // E,T,F: 变量名或临时变量
    int quad;           // M,N: 指令地址
    int trueList;       // C: 真出口
    int falseList;      // C: 假出口
    int nextList;       // S,L: 后继地址
    string idName;      // 保存id名
    string numVal;      // 保存num值
    string rop;         // 保存关系运算符

    SemanticRecord() : place(""), quad(0), trueList(-1), falseList(-1), nextList(-1),
        idName(""), numVal(""), rop("") {}
};

// 辅助函数
string tokenTypeToString(TokenType type);
string tokenToSymbol(const Token& tok);

#endif
