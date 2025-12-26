#pragma once
#ifndef LR1_PARSER_H
#define LR1_PARSER_H

#include "common.h"

class LR1Parser {
private:
    // 文法
    vector<Production> productions;
    set<string> terminals;
    set<string> nonTerminals;
    string startSymbol;

    // FIRST集和FOLLOW集
    map<string, set<string>> firstSet;
    map<string, set<string>> followSet;

    // LR(1)项目集族
    vector<set<LR1Item>> states;

    // ACTION和GOTO表
    map<pair<int, string>, string> actionTable;
    map<pair<int, string>, int> gotoTable;

    // 辅助函数
    void initGrammar();
    void computeFirstSets();
    void computeFollowSets();
    set<string> getFirstOfSequence(const vector<string>& seq, size_t start);

    set<LR1Item> closure(const set<LR1Item>& items);
    set<LR1Item> goTo(const set<LR1Item>& items, const string& symbol);
    int findState(const set<LR1Item>& items);
    void buildStates();
    void buildTable();

    bool isTerminal(const string& s);
    bool isNonTerminal(const string& s);

public:
    LR1Parser();
    void init();

    // 获取分析表
    string getAction(int state, const string& symbol);
    int getGoto(int state, const string& symbol);
    const Production& getProduction(int index) const { return productions[index]; }

    // 打印函数
    void printGrammar();
    void printFirstSets();
    void printFollowSets();
    void printStates();
    void printTable();
};

#endif
