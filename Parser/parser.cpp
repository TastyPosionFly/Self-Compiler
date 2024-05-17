#include "parser.h"

bool parser::parse(const vector<LRParserGenerator::Symbol> &input) {
    std::stack<int> stateStack;
    std::stack<LRParserGenerator::Symbol> symbolStack;
    stateStack.push(0); // 初始状态

    size_t index = 0;
    while (index < input.size()) {
        int currentState = stateStack.top();
        LRParserGenerator::Symbol currentSymbol = input[index];

        cout << "Current State: " << currentState << ", Current Symbol: " << currentSymbol.name << endl;

        auto actionIt = actionTable.find({currentState, currentSymbol});
        if (actionIt == actionTable.end()) {
            cout << "Syntax error: No action found for state " << currentState << " and symbol " << currentSymbol.name
                 << endl;
            return false; // 语法错误
        }

        char action = actionIt->second.first;
        int nextState = actionIt->second.second;

        if (action == 'S') { // 移进
            cout << "Shift to state " << nextState << endl;
            stateStack.push(nextState);
            symbolStack.push(currentSymbol);
            index++;
        } else if (action == 'R') { // 规约
            const LRParserGenerator::Production &production = productions[nextState];
            cout << "Reduce by production: " << production.lhs.name << " -> ";
            for (const auto &sym: production.rhs) {
                cout << sym.name << "'";
            }
            cout << endl;

            for (size_t i = 0; i < production.rhs.size(); ++i) {
                stateStack.pop();
                symbolStack.pop();
            }
            currentState = stateStack.top();
            symbolStack.push(production.lhs);

            auto gotoIt = gotoTable.find({currentState, production.lhs});
            if (gotoIt == gotoTable.end()) {
                cout << "Syntax error: No goto found for state " << currentState << " and symbol "
                     << production.lhs.name << endl;
                return false; // 语法错误
            }
            stateStack.push(gotoIt->second);
        } else if (action == 'A') { // 接受
            cout << "Accept" << endl;
            return true;
        } else {
            cout << "Syntax error: Invalid action " << action << endl;
            return false; // 语法错误
        }
    }

    return false; // 语法错误
}