#include "compiler.h"

Compiler::Compiler() {}

bool Compiler::compile(const string& source) {
    cout << "\n========================================" << endl;
    cout << "      IF-ELSE条件语句翻译程序" << endl;
    cout << "      LR(1)方法 + 三地址码输出" << endl;
    cout << "========================================\n" << endl;

    cout << "源程序：" << source << endl << endl;

    // 1. 词法分析
    cout << ">>> 阶段1：词法分析" << endl;
    lexer.setInput(source);
    tokens = lexer.tokenize();

    for (const Token& t : tokens) {
        if (t.type == TOKEN_ERROR) {
            cerr << "词法分析出错！" << endl;
            return false;
        }
    }

    lexer.printTokens(tokens);

    // 2. 构造LR(1)分析表
    cout << ">>> 阶段2：构造LR(1)分析表" << endl;
    parser.init();
    cout << endl;

    // 3. LR(1)语法分析 + 语义分析
    cout << ">>> 阶段3：LR(1)语法分析与语义翻译" << endl;
    if (!lr1Parse()) {
        cerr << "语法分析出错！" << endl;
        return false;
    }

    // 4. 输出结果
    cout << "\n>>> 阶段4：输出中间代码" << endl;
    semantic.printCode();

    cout << "编译完成！" << endl;
    return true;
}

// LR(1)驱动的语法分析
bool Compiler::lr1Parse() {
    // 清空栈
    while (!stateStack.empty()) stateStack.pop();
    while (!symbolStack.empty()) symbolStack.pop();
    while (!semStack.empty()) semStack.pop();

    semantic.reset();

    // 初始化
    stateStack.push(0);
    symbolStack.push("#");

    SemanticRecord initRec;
    semStack.push(initRec);

    int ip = 0;  // 输入指针
    int step = 0;

    cout << "\nLR(1)分析过程：" << endl;
    cout << setw(6) << "步骤" << setw(20) << "状态栈" << setw(25) << "符号栈"
        << setw(18) << "当前输入" << setw(12) << "动作" << endl;
    cout << string(81, '-') << endl;

    while (true) {
        step++;
        int s = stateStack.top();
        string a = tokenToSymbol(tokens[ip]);

        // 打印当前状态
        // 状态栈
        string stateStr = "";
        stack<int> tempState = stateStack;
        vector<int> stateVec;
        while (!tempState.empty()) {
            stateVec.push_back(tempState.top());
            tempState.pop();
        }
        for (int i = stateVec.size() - 1; i >= 0; i--) {
            stateStr += to_string(stateVec[i]) + " ";
        }

        // 符号栈
        string symbolStr = "";
        stack<string> tempSymbol = symbolStack;
        vector<string> symbolVec;
        while (!tempSymbol.empty()) {
            symbolVec.push_back(tempSymbol.top());
            tempSymbol.pop();
        }
        for (int i = symbolVec.size() - 1; i >= 0; i--) {
            symbolStr += symbolVec[i] + " ";
        }

        // 当前输入
        string inputStr = tokens[ip].value;

        // 获取动作
        string action = parser.getAction(s, a);

        cout << setw(6) << step
            << setw(20) << stateStr.substr(0, 18)
            << setw(25) << symbolStr.substr(0, 23)
            << setw(18) << inputStr
            << setw(12);

        if (action.empty()) {
            cout << "错误" << endl;
            cerr << "\n语法错误！在第 " << tokens[ip].line << " 行，'" << tokens[ip].value << "' 附近" << endl;
            return false;
        }

        if (action[0] == 's') {
            // 移进
            int nextState = stoi(action.substr(1));
            cout << action << endl;

            stateStack.push(nextState);
            symbolStack.push(a);

            // 创建语义记录
            SemanticRecord rec;
            if (tokens[ip].type == TOKEN_ID) {
                rec.idName = tokens[ip].value;
                rec.place = tokens[ip].value;
            }
            else if (tokens[ip].type == TOKEN_NUM) {
                rec.numVal = tokens[ip].value;
                rec.place = tokens[ip].value;
            }
            else if (tokens[ip].type >= TOKEN_LT && tokens[ip].type <= TOKEN_NE) {
                rec.rop = tokens[ip].value;
            }
            semStack.push(rec);

            ip++;
        }
        else if (action[0] == 'r') {
            // 归约
            int prodIndex = stoi(action.substr(1));
            const Production& prod = parser.getProduction(prodIndex);

            cout << action << " (" << prod.left << "→";
            for (size_t i = 0; i < prod.right.size(); i++) {
                cout << prod.right[i];
                if (i < prod.right.size() - 1) cout << " ";
            }
            cout << ")" << endl;

            // 弹出 |β| 个状态和符号
            int popCount = prod.len;
            vector<SemanticRecord> poppedRecords;

            for (int i = 0; i < popCount; i++) {
                stateStack.pop();
                symbolStack.pop();
                poppedRecords.insert(poppedRecords.begin(), semStack.top());
                semStack.pop();
            }

            // 执行语义动作
            executeSemanticAction(prodIndex, poppedRecords);

            // 压入左部符号
            int topState = stateStack.top();
            int gotoState = parser.getGoto(topState, prod.left);

            if (gotoState == -1) {
                cerr << "\nGOTO表错误！状态 " << topState << "，符号 " << prod.left << endl;
                return false;
            }

            stateStack.push(gotoState);
            symbolStack.push(prod.left);
        }
        else if (action == "acc") {
            cout << "acc" << endl;
            cout << "\n语法分析成功！" << endl;
            return true;
        }

        // 防止死循环
        if (step > 500) {
            cerr << "\n分析步骤过多，可能存在错误" << endl;
            return false;
        }
    }

    return false;
}

// 执行语义动作
void Compiler::executeSemanticAction(int prodIndex, vector<SemanticRecord>& rec) {
    SemanticRecord newRec;

    switch (prodIndex) {
    case 0: {
        // S' → S
        // 不需要特殊处理
        if (!rec.empty()) newRec = rec[0];
        break;
    }

    case 1: {
        // S → id = E
        // rec[0]=id, rec[1]='=', rec[2]=E
        string id = rec[0].idName;
        string place = rec[2].place;
        semantic.emit("=", place, "_", id);
        newRec.nextList = -1;
        break;
    }

    case 2: {
        // S → if ( C ) M { L } N
        int C_true = rec[2].trueList;
        int C_false = rec[2].falseList;
        int M_quad = rec[4].quad;

        // 对于简单if语句（无else），N生成的goto是冗余的，删除它
        semantic.removeLastQuad();

        // 回填C.true到M.quad（then分支起始）
        semantic.backpatch(C_true, M_quad);
        // C.false跳到语句结束（then分支之后）
        semantic.backpatch(C_false, semantic.getNextQuad());

        newRec.nextList = -1;
        break;
    }

    case 3: {
        // S → if ( C ) M { L } N else M { L }
        // rec: 0=if, 1=(, 2=C, 3=), 4=M1, 5={, 6=L1, 7=}, 8=N, 9=else, 10=M2, 11={, 12=L2, 13=}
        int C_true = rec[2].trueList;
        int C_false = rec[2].falseList;
        int M1_quad = rec[4].quad;
        int M2_quad = rec[10].quad;
        int N_quad = rec[8].quad;

        // 回填C.true到M1.quad（then分支起始）
        semantic.backpatch(C_true, M1_quad);
        // 回填C.false到M2.quad（else分支起始）
        semantic.backpatch(C_false, M2_quad);
        // N跳到语句结束
        semantic.backpatch(N_quad, semantic.getNextQuad());

        newRec.nextList = -1;
        break;
    }

    case 4: {
        // L → L M S
        // rec: 0=L, 1=M, 2=S
        newRec.nextList = rec[2].nextList;
        break;
    }

    case 5: {
        // L → S
        if (!rec.empty()) newRec = rec[0];
        break;
    }

    case 6: {
        // C → E rop E
        // rec: 0=E1, 1=rop, 2=E2
        string E1_place = rec[0].place;
        string rop = rec[1].rop;
        string E2_place = rec[2].place;

        newRec.trueList = semantic.getNextQuad();
        semantic.emit("j" + rop, E1_place, E2_place, "0");

        newRec.falseList = semantic.getNextQuad();
        semantic.emit("j", "_", "_", "0");
        break;
    }

    case 7: {
        // M → ε
        newRec.quad = semantic.getNextQuad();
        break;
    }

    case 8: {
        // N → ε
        newRec.quad = semantic.getNextQuad();
        semantic.emit("j", "_", "_", "0");
        break;
    }

    case 9: {
        // E → E + T
        // rec: 0=E1, 1='+', 2=T
        string E1_place = rec[0].place;
        string T_place = rec[2].place;
        newRec.place = semantic.newtemp();
        semantic.emit("+", E1_place, T_place, newRec.place);
        break;
    }

    case 10: {
        // E → E - T
        string E1_place = rec[0].place;
        string T_place = rec[2].place;
        newRec.place = semantic.newtemp();
        semantic.emit("-", E1_place, T_place, newRec.place);
        break;
    }

    case 11: {
        // E → T
        if (!rec.empty()) newRec.place = rec[0].place;
        break;
    }

    case 12: {
        // T → T * F
        string T1_place = rec[0].place;
        string F_place = rec[2].place;
        newRec.place = semantic.newtemp();
        semantic.emit("*", T1_place, F_place, newRec.place);
        break;
    }

    case 13: {
        // T → T / F
        string T1_place = rec[0].place;
        string F_place = rec[2].place;
        newRec.place = semantic.newtemp();
        semantic.emit("/", T1_place, F_place, newRec.place);
        break;
    }

    case 14: {
        // T → F
        if (!rec.empty()) newRec.place = rec[0].place;
        break;
    }

    case 15: {
        // F → ( E )
        // rec: 0='(', 1=E, 2=')'
        newRec.place = rec[1].place;
        break;
    }

    case 16: {
        // F → id
        if (!rec.empty()) {
            newRec.place = rec[0].idName;
            newRec.idName = rec[0].idName;
        }
        break;
    }

    case 17: {
        // F → num
        if (!rec.empty()) {
            newRec.place = rec[0].numVal;
            newRec.numVal = rec[0].numVal;
        }
        break;
    }

    default:
        break;
    }

    semStack.push(newRec);
}

void Compiler::printAll() {
    parser.printGrammar();
    parser.printFirstSets();
    parser.printFollowSets();
    // parser.printStates();  // 项目集太多，可选
    parser.printTable();
}
