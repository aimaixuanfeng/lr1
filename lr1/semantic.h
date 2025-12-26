#pragma once
#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "common.h"

class SemanticAnalyzer {
private:
    vector<Quadruple> code;     // 三地址码序列
    int nextquad;               // 下一条指令地址
    int tempCount;              // 临时变量计数

public:
    SemanticAnalyzer();
    // 在 SemanticAnalyzer 类中添加一行：
    void removeLastQuad();  // 删除最后一条指令（用于消除冗余goto）
    void reset();
    string newtemp();
    int emit(const string& op, const string& arg1, const string& arg2, const string& result);
    void backpatch(int addr, int target);
    int merge(int p1, int p2);

    int getNextQuad() const { return nextquad; }
    const vector<Quadruple>& getCode() const { return code; }

    void printCode();
    void printQuadruple();  // 打印四元式形式
};

#endif
