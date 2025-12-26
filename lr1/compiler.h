#pragma once
#ifndef COMPILER_H
#define COMPILER_H

#include "common.h"
#include "lexer.h"
#include "lr1_parser.h"
#include "semantic.h"

class Compiler {
private:
    Lexer lexer;
    LR1Parser parser;
    SemanticAnalyzer semantic;

    vector<Token> tokens;

    // LR(1)分析用的栈
    stack<int> stateStack;
    stack<string> symbolStack;
    stack<SemanticRecord> semStack;

    // 执行语义动作
    void executeSemanticAction(int prodIndex, vector<SemanticRecord>& poppedRecords);

public:
    Compiler();

    bool compile(const string& source);
    bool lr1Parse();  // LR(1)驱动的语法分析

    void printAll();
};

#endif
