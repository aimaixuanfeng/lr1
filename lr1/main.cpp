#include "compiler.h"

void printMenu() {
    cout << "\n╔════════════════════════════════════════════════════════╗" << endl;
    cout << "║       IF-ELSE条件语句翻译程序 (LR1方法)               ║" << endl;
    cout << "╠════════════════════════════════════════════════════════╣" << endl;
    cout << "║  1. 输入源程序进行编译                                 ║" << endl;
    cout << "║  2. 运行示例程序                                       ║" << endl;
    cout << "║  3. 显示文法产生式                                     ║" << endl;
    cout << "║  4. 显示FIRST集和FOLLOW集                              ║" << endl;
    cout << "║  5. 显示LR(1)分析表                                    ║" << endl;
    cout << "║  0. 退出程序                                           ║" << endl;
    cout << "╚════════════════════════════════════════════════════════╝" << endl;
    cout << "请选择: ";
}

void inputAndCompile() {
    Compiler compiler;
    string source;

    cout << "\n请输入源程序 (输入END结束):" << endl;
    cout << "示例: if (a > b) { x = a } else { x = b }" << endl;
    cout << "───────────────────────────────────────────" << endl;
    cout << ">>> ";

    string line;
    source = "";
    while (getline(cin, line)) {
        if (line == "END" || line == "end") {
            break;
        }
        source += line + " ";
        cout << ">>> ";
    }

    if (source.empty()) {
        cout << "输入为空！" << endl;
        return;
    }

    compiler.compile(source);
}

void runExamples() {
    Compiler compiler;
    int choice;

    cout << "\n选择示例程序:" << endl;
    cout << "  1. if (a > b) { x = a }" << endl;
    cout << "  2. if (a > b) { x = a } else { x = b }" << endl;
    cout << "  3. if (a + 1 > b * 2) { x = a + b }" << endl;
    cout << "  4. if (a >= b) { x = a; y = b } else { x = b; y = a }" << endl;
    cout << "请选择 (1-4): ";
    cin >> choice;
    cin.ignore();

    string source;
    switch (choice) {
    case 1: source = "if (a > b) { x = a }"; break;
    case 2: source = "if (a > b) { x = a } else { x = b }"; break;
    case 3: source = "if (a + 1 > b * 2) { x = a + b }"; break;
    case 4: source = "if (a >= b) { x = a; y = b } else { x = b; y = a }"; break;
    default:
        cout << "无效选择！" << endl;
        return;
    }

    compiler.compile(source);
}

void showGrammar() {
    LR1Parser parser;
    cout << "\n正在初始化..." << endl;
    parser.init();
    parser.printGrammar();
}

void showFirstFollow() {
    LR1Parser parser;
    cout << "\n正在计算FIRST集和FOLLOW集..." << endl;
    parser.init();
    parser.printFirstSets();
    parser.printFollowSets();
}

void showLR1Table() {
    LR1Parser parser;
    cout << "\n正在构造LR(1)分析表..." << endl;
    parser.init();
    parser.printTable();
}

int main(int argc, char* argv[]) {
    // 命令行模式
    if (argc >= 2) {
        string arg = argv[1];

        if (arg == "-h" || arg == "--help") {
            cout << "用法：" << endl;
            cout << "  ./compiler              交互模式" << endl;
            cout << "  ./compiler -e \"code\"    直接编译代码" << endl;
            cout << "  ./compiler <file>       编译文件" << endl;
            cout << "  ./compiler -t           显示分析表" << endl;
            return 0;
        }
        else if (arg == "-t") {
            showLR1Table();
            return 0;
        }
        else if (arg == "-e" && argc >= 3) {
            Compiler compiler;
            compiler.compile(argv[2]);
            return 0;
        }
        else {
            // 从文件读取
            ifstream file(arg);
            if (!file.is_open()) {
                cerr << "无法打开文件：" << arg << endl;
                return 1;
            }
            string source, line;
            while (getline(file, line)) {
                source += line + " ";
            }
            file.close();

            Compiler compiler;
            compiler.compile(source);
            return 0;
        }
    }

    // 交互模式
    int choice;

    while (true) {
        printMenu();
        cin >> choice;
        cin.ignore();  // 清除换行符

        switch (choice) {
        case 1:
            inputAndCompile();
            break;
        case 2:
            runExamples();
            break;
        case 3:
            showGrammar();
            break;
        case 4:
            showFirstFollow();
            break;
        case 5:
            showLR1Table();
            break;
        case 0:
            cout << "\n感谢使用，再见！" << endl;
            return 0;
        default:
            cout << "无效选择，请重试！" << endl;
        }

        cout << "\n按回车键继续...";
        cin.get();
    }

    return 0;
}
