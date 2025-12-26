#include "semantic.h"

SemanticAnalyzer::SemanticAnalyzer() {
    reset();
}

void SemanticAnalyzer::reset() {
    code.clear();
    nextquad = 100;
    tempCount = 0;
}

string SemanticAnalyzer::newtemp() {
    return "t" + to_string(++tempCount);
}

int SemanticAnalyzer::emit(const string& op, const string& arg1, const string& arg2, const string& result) {
    code.push_back(Quadruple(op, arg1, arg2, result));
    return nextquad++;
}

void SemanticAnalyzer::backpatch(int addr, int target) {
    if (addr >= 100 && addr - 100 < (int)code.size()) {
        code[addr - 100].result = to_string(target);
    }
}

int SemanticAnalyzer::merge(int p1, int p2) {
    if (p1 == -1) return p2;
    if (p2 == -1) return p1;
    return p1;  // 简化处理
}

void SemanticAnalyzer::printCode() {
    cout << "\n==================== 三地址码 ====================" << endl;

    for (size_t i = 0; i < code.size(); i++) {
        cout << "(" << (100 + i) << ") ";

        const Quadruple& q = code[i];

        if (q.op == "j") {
            cout << "goto " << q.result;
        }
        else if (q.op.length() > 1 && q.op[0] == 'j') {
            // 条件跳转 j>, j<, j>=, j<=, j==, j!=
            string rop = q.op.substr(1);
            cout << "if " << q.arg1 << " " << rop << " " << q.arg2 << " goto " << q.result;
        }
        else if (q.op == "=") {
            cout << q.result << " = " << q.arg1;
        }
        else {
            // 算术运算
            cout << q.result << " = " << q.arg1 << " " << q.op << " " << q.arg2;
        }
        cout << endl;
    }

    cout << "=================================================\n" << endl;
}

void SemanticAnalyzer::printQuadruple() {
    cout << "\n==================== 四元式序列 ====================" << endl;
    cout << setw(8) << "序号" << setw(10) << "op" << setw(10) << "arg1"
        << setw(10) << "arg2" << setw(10) << "result" << endl;
    cout << "----------------------------------------------------" << endl;

    for (size_t i = 0; i < code.size(); i++) {
        cout << setw(8) << (100 + i)
            << setw(10) << code[i].op
            << setw(10) << code[i].arg1
            << setw(10) << code[i].arg2
            << setw(10) << code[i].result << endl;
    }

    cout << "====================================================\n" << endl;
}
void SemanticAnalyzer::removeLastQuad() {
    if (!code.empty()) {
        code.pop_back();
        nextquad--;
    }
}