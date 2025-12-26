#include "lr1_parser.h"

LR1Parser::LR1Parser() {
    startSymbol = "S'";
}

// 初始化文法 - 严格按照课设定义的产生式
void LR1Parser::initGrammar() {
    productions.clear();

    // (0)  S' → S
    productions.push_back(Production("S'", { "S" }));

    // (1)  S → id = E
    productions.push_back(Production("S", { "id", "=", "E" }));

    // (2)  S → if ( C ) M { L } N
    productions.push_back(Production("S", { "if", "(", "C", ")", "M", "{", "L", "}", "N" }));

    // (3)  S → if ( C ) M { L } N else M { L }
    productions.push_back(Production("S", { "if", "(", "C", ")", "M", "{", "L", "}", "N", "else", "M", "{", "L", "}" }));

    // (4)  L → L M S
    productions.push_back(Production("L", { "L", "M", "S" }));

    // (5)  L → S
    productions.push_back(Production("L", { "S" }));

    // (6)  C → E rop E
    productions.push_back(Production("C", { "E", "rop", "E" }));

    // (7)  M → ε
    productions.push_back(Production("M", { "ε" }));

    // (8)  N → ε
    productions.push_back(Production("N", { "ε" }));

    // (9)  E → E + T
    productions.push_back(Production("E", { "E", "+", "T" }));

    // (10) E → E - T
    productions.push_back(Production("E", { "E", "-", "T" }));

    // (11) E → T
    productions.push_back(Production("E", { "T" }));

    // (12) T → T * F
    productions.push_back(Production("T", { "T", "*", "F" }));

    // (13) T → T / F
    productions.push_back(Production("T", { "T", "/", "F" }));

    // (14) T → F
    productions.push_back(Production("T", { "F" }));

    // (15) F → ( E )
    productions.push_back(Production("F", { "(", "E", ")" }));

    // (16) F → id
    productions.push_back(Production("F", { "id" }));

    // (17) F → num
    productions.push_back(Production("F", { "num" }));

    // 终结符集合（不含分号，因为文法中不使用）
    terminals = { "if", "else", "id", "num", "+", "-", "*", "/",
                 "(", ")", "{", "}", "=", "rop", "#" };

    // 非终结符集合
    nonTerminals = { "S'", "S", "L", "C", "E", "T", "F", "M", "N" };
}

bool LR1Parser::isTerminal(const string& s) {
    return terminals.find(s) != terminals.end();
}

bool LR1Parser::isNonTerminal(const string& s) {
    return nonTerminals.find(s) != nonTerminals.end();
}

// 计算FIRST集
void LR1Parser::computeFirstSets() {
    // 终结符的FIRST集是自身
    for (const string& t : terminals) {
        firstSet[t].insert(t);
    }
    firstSet["ε"].insert("ε");

    // 初始化非终结符的FIRST集为空
    for (const string& nt : nonTerminals) {
        firstSet[nt] = set<string>();
    }

    bool changed = true;
    while (changed) {
        changed = false;

        for (const Production& prod : productions) {
            const string& A = prod.left;
            const vector<string>& alpha = prod.right;

            // A → ε
            if (alpha.size() == 1 && alpha[0] == "ε") {
                if (firstSet[A].find("ε") == firstSet[A].end()) {
                    firstSet[A].insert("ε");
                    changed = true;
                }
                continue;
            }

            // A → X1 X2 ... Xn
            bool allCanBeEmpty = true;
            for (size_t i = 0; i < alpha.size() && allCanBeEmpty; i++) {
                const string& Xi = alpha[i];

                // 将FIRST(Xi) - {ε} 加入 FIRST(A)
                for (const string& f : firstSet[Xi]) {
                    if (f != "ε" && firstSet[A].find(f) == firstSet[A].end()) {
                        firstSet[A].insert(f);
                        changed = true;
                    }
                }

                // 检查Xi能否推导出ε
                if (firstSet[Xi].find("ε") == firstSet[Xi].end()) {
                    allCanBeEmpty = false;
                }
            }

            // 如果所有Xi都能推导出ε
            if (allCanBeEmpty) {
                if (firstSet[A].find("ε") == firstSet[A].end()) {
                    firstSet[A].insert("ε");
                    changed = true;
                }
            }
        }
    }
}

// 计算FOLLOW集
void LR1Parser::computeFollowSets() {
    // 初始化
    for (const string& nt : nonTerminals) {
        followSet[nt] = set<string>();
    }

    // FOLLOW(S') = {#}
    followSet["S'"].insert("#");
    followSet["S"].insert("#");

    bool changed = true;
    while (changed) {
        changed = false;

        for (const Production& prod : productions) {
            const string& A = prod.left;
            const vector<string>& alpha = prod.right;

            if (alpha.size() == 1 && alpha[0] == "ε") continue;

            for (size_t i = 0; i < alpha.size(); i++) {
                const string& B = alpha[i];

                if (!isNonTerminal(B)) continue;

                // 计算β = alpha[i+1...]的FIRST集
                set<string> firstBeta;
                bool betaCanBeEmpty = true;

                for (size_t j = i + 1; j < alpha.size() && betaCanBeEmpty; j++) {
                    const string& Xj = alpha[j];
                    for (const string& f : firstSet[Xj]) {
                        if (f != "ε") firstBeta.insert(f);
                    }
                    if (firstSet[Xj].find("ε") == firstSet[Xj].end()) {
                        betaCanBeEmpty = false;
                    }
                }

                // FOLLOW(B) ∪= FIRST(β) - {ε}
                for (const string& f : firstBeta) {
                    if (followSet[B].find(f) == followSet[B].end()) {
                        followSet[B].insert(f);
                        changed = true;
                    }
                }

                // 如果β为空或β =>* ε，则 FOLLOW(B) ∪= FOLLOW(A)
                if (i == alpha.size() - 1 || betaCanBeEmpty) {
                    for (const string& f : followSet[A]) {
                        if (followSet[B].find(f) == followSet[B].end()) {
                            followSet[B].insert(f);
                            changed = true;
                        }
                    }
                }
            }
        }
    }
}

// 获取符号序列的FIRST集
set<string> LR1Parser::getFirstOfSequence(const vector<string>& seq, size_t start) {
    set<string> result;

    if (start >= seq.size()) {
        result.insert("ε");
        return result;
    }

    bool allCanBeEmpty = true;
    for (size_t i = start; i < seq.size() && allCanBeEmpty; i++) {
        const string& Xi = seq[i];

        for (const string& f : firstSet[Xi]) {
            if (f != "ε") result.insert(f);
        }

        if (firstSet[Xi].find("ε") == firstSet[Xi].end()) {
            allCanBeEmpty = false;
        }
    }

    if (allCanBeEmpty) {
        result.insert("ε");
    }

    return result;
}

// 求闭包
set<LR1Item> LR1Parser::closure(const set<LR1Item>& items) {
    set<LR1Item> result = items;
    bool changed = true;

    while (changed) {
        changed = false;
        set<LR1Item> toAdd;

        for (const LR1Item& item : result) {
            const Production& prod = productions[item.prodIndex];

            // 如果是ε产生式或点已在最后，跳过
            if (prod.right[0] == "ε" || item.dotPos >= (int)prod.right.size()) {
                continue;
            }

            string B = prod.right[item.dotPos];  // 点后面的符号

            if (isNonTerminal(B)) {
                // 计算 βa 的 FIRST
                vector<string> betaA;
                for (size_t i = item.dotPos + 1; i < prod.right.size(); i++) {
                    betaA.push_back(prod.right[i]);
                }
                betaA.push_back(item.lookahead);

                set<string> firstBetaA = getFirstOfSequence(betaA, 0);

                // 对B的每个产生式，添加新项目
                for (size_t i = 0; i < productions.size(); i++) {
                    if (productions[i].left == B) {
                        for (const string& b : firstBetaA) {
                            if (b != "ε") {
                                LR1Item newItem(i, 0, b);
                                if (result.find(newItem) == result.end() &&
                                    toAdd.find(newItem) == toAdd.end()) {
                                    toAdd.insert(newItem);
                                    changed = true;
                                }
                            }
                        }
                    }
                }
            }
        }

        for (const LR1Item& item : toAdd) {
            result.insert(item);
        }
    }

    return result;
}

// GOTO函数
set<LR1Item> LR1Parser::goTo(const set<LR1Item>& items, const string& symbol) {
    set<LR1Item> result;

    for (const LR1Item& item : items) {
        const Production& prod = productions[item.prodIndex];

        // ε产生式不移进
        if (prod.right[0] == "ε") continue;

        // 如果点后面是symbol
        if (item.dotPos < (int)prod.right.size() && prod.right[item.dotPos] == symbol) {
            LR1Item newItem(item.prodIndex, item.dotPos + 1, item.lookahead);
            result.insert(newItem);
        }
    }

    return closure(result);
}

// 查找状态
int LR1Parser::findState(const set<LR1Item>& items) {
    for (size_t i = 0; i < states.size(); i++) {
        if (states[i] == items) return i;
    }
    return -1;
}

// 构造项目集族
void LR1Parser::buildStates() {
    states.clear();

    // 初始状态
    set<LR1Item> initItems;
    initItems.insert(LR1Item(0, 0, "#"));
    states.push_back(closure(initItems));

    // 所有文法符号
    set<string> allSymbols;
    for (const string& t : terminals) {
        if (t != "#") allSymbols.insert(t);
    }
    for (const string& nt : nonTerminals) {
        if (nt != "S'") allSymbols.insert(nt);
    }

    bool changed = true;
    while (changed) {
        changed = false;
        size_t stateCount = states.size();

        for (size_t i = 0; i < stateCount; i++) {
            for (const string& X : allSymbols) {
                set<LR1Item> gotoIX = goTo(states[i], X);

                if (!gotoIX.empty() && findState(gotoIX) == -1) {
                    states.push_back(gotoIX);
                    changed = true;
                }
            }
        }
    }
}

// 构造分析表
void LR1Parser::buildTable() {
    actionTable.clear();
    gotoTable.clear();

    for (size_t i = 0; i < states.size(); i++) {
        for (const LR1Item& item : states[i]) {
            const Production& prod = productions[item.prodIndex];

            // 情况1: [A → α·aβ, b]，a是终结符，ACTION[i,a] = shift j
            if (prod.right[0] != "ε" && item.dotPos < (int)prod.right.size()) {
                string a = prod.right[item.dotPos];
                if (isTerminal(a)) {
                    set<LR1Item> gotoIa = goTo(states[i], a);
                    int j = findState(gotoIa);
                    if (j != -1) {
                        actionTable[{i, a}] = "s" + to_string(j);
                    }
                }
            }

            // 情况2: [A → α·, a] 且 A ≠ S'，ACTION[i,a] = reduce j
            if ((prod.right[0] == "ε" || item.dotPos == (int)prod.right.size()) &&
                prod.left != "S'") {
                actionTable[{i, item.lookahead}] = "r" + to_string(item.prodIndex);
            }

            // 情况3: [S' → S·, #]，ACTION[i,#] = acc
            if (item.prodIndex == 0 && item.dotPos == 1 && item.lookahead == "#") {
                actionTable[{i, "#"}] = "acc";
            }
        }

        // GOTO表
        for (const string& A : nonTerminals) {
            if (A == "S'") continue;
            set<LR1Item> gotoIA = goTo(states[i], A);
            int j = findState(gotoIA);
            if (j != -1) {
                gotoTable[{i, A}] = j;
            }
        }
    }
}

// 初始化
void LR1Parser::init() {
    cout << "正在初始化文法..." << endl;
    initGrammar();

    cout << "正在计算FIRST集..." << endl;
    computeFirstSets();

    cout << "正在计算FOLLOW集..." << endl;
    computeFollowSets();

    cout << "正在构造LR(1)项目集族..." << endl;
    buildStates();

    cout << "正在构造LR(1)分析表..." << endl;
    buildTable();

    cout << "初始化完成！共 " << states.size() << " 个状态" << endl;
}

// 获取ACTION
string LR1Parser::getAction(int state, const string& symbol) {
    auto it = actionTable.find({ state, symbol });
    if (it != actionTable.end()) {
        return it->second;
    }
    return "";
}

// 获取GOTO
int LR1Parser::getGoto(int state, const string& symbol) {
    auto it = gotoTable.find({ state, symbol });
    if (it != gotoTable.end()) {
        return it->second;
    }
    return -1;
}

// 打印文法
void LR1Parser::printGrammar() {
    cout << "\n==================== 文法产生式 ====================" << endl;
    for (size_t i = 0; i < productions.size(); i++) {
        cout << "(" << i << ") " << productions[i].left << " → ";
        for (size_t j = 0; j < productions[i].right.size(); j++) {
            cout << productions[i].right[j];
            if (j < productions[i].right.size() - 1) cout << " ";
        }
        cout << endl;
    }
    cout << "===================================================\n" << endl;
}

// 打印FIRST集
void LR1Parser::printFirstSets() {
    cout << "\n==================== FIRST集 ====================" << endl;
    vector<string> order = { "S'", "S", "L", "C", "E", "T", "F", "M", "N" };
    for (const string& nt : order) {
        cout << "FIRST(" << nt << ") = { ";
        bool first = true;
        for (const string& f : firstSet[nt]) {
            if (!first) cout << ", ";
            cout << f;
            first = false;
        }
        cout << " }" << endl;
    }
    cout << "=================================================\n" << endl;
}

// 打印FOLLOW集
void LR1Parser::printFollowSets() {
    cout << "\n==================== FOLLOW集 ====================" << endl;
    vector<string> order = { "S'", "S", "L", "C", "E", "T", "F", "M", "N" };
    for (const string& nt : order) {
        cout << "FOLLOW(" << nt << ") = { ";
        bool first = true;
        for (const string& f : followSet[nt]) {
            if (!first) cout << ", ";
            cout << f;
            first = false;
        }
        cout << " }" << endl;
    }
    cout << "==================================================\n" << endl;
}

// 打印项目集
void LR1Parser::printStates() {
    cout << "\n==================== LR(1)项目集族 ====================" << endl;
    for (size_t i = 0; i < states.size(); i++) {
        cout << "I" << i << ":" << endl;
        for (const LR1Item& item : states[i]) {
            const Production& prod = productions[item.prodIndex];
            cout << "  [" << prod.left << " → ";

            if (prod.right[0] == "ε") {
                cout << "·";
            }
            else {
                for (size_t j = 0; j < prod.right.size(); j++) {
                    if ((int)j == item.dotPos) cout << "·";
                    cout << prod.right[j] << " ";
                }
                if (item.dotPos == (int)prod.right.size()) cout << "·";
            }

            cout << ", " << item.lookahead << "]" << endl;
        }
        cout << endl;
    }
    cout << "======================================================\n" << endl;
}

// 打印分析表
void LR1Parser::printTable() {
    cout << "\n==================== LR(1)分析表 ====================" << endl;

    vector<string> termList = { "id", "num", "if", "else", "=", "rop", "+", "-", "*", "/",
                               "(", ")", "{", "}", "#" };
    vector<string> ntList = { "S", "L", "C", "E", "T", "F", "M", "N" };

    // 表头
    cout << setw(6) << "状态" << " |";
    for (const string& t : termList) {
        cout << setw(5) << t;
    }
    cout << " |";
    for (const string& nt : ntList) {
        cout << setw(4) << nt;
    }
    cout << endl;

    cout << string(6 + 1 + termList.size() * 5 + 1 + ntList.size() * 4 + 2, '-') << endl;

    // 每一行
    for (size_t i = 0; i < states.size(); i++) {
        cout << setw(6) << i << " |";

        for (const string& t : termList) {
            string action = getAction(i, t);
            cout << setw(5) << action;
        }

        cout << " |";

        for (const string& nt : ntList) {
            int g = getGoto(i, nt);
            if (g >= 0) {
                cout << setw(4) << g;
            }
            else {
                cout << setw(4) << "";
            }
        }
        cout << endl;
    }
    cout << "======================================================\n" << endl;
}
