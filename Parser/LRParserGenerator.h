//
// Created by w1300 on 2024/5/9.
//

#ifndef COMPILER_LRPARSERGENERATOR_H
#define COMPILER_LRPARSERGENERATOR_H

#include <string>
#include <vector>
#include <stack>
#include <set>
#include <map>
#include <algorithm>
#include <iostream>

using namespace std;

class LRParserGenerator {
public:
    // 符号的类型
    enum class SymbolType{
        TERMINAL,
        NON_TERMINAL
    };

    // 符号
    struct Symbol{
        string name;
        SymbolType type;

        // 用于比较，C++中自定义的结构体无法使用默认的比较方式，特别是自定义的enum数据
        bool operator<(const Symbol& other) const {
            return name < other.name || (name == other.name && type < other.type);
        }

        bool operator==(const Symbol& other) const{
            return name == other.name;
        }
    };

    // 产生式 E -> E+T
    struct Production{
        Symbol lhs;//产生式的左边
        vector<Symbol> rhs;//产生式的右边

        bool operator==(const Production& other) const{
            return lhs == other.lhs && rhs == other.rhs;
        }

        bool operator<(const Production& other) const{
            if (lhs < other.lhs) return true;
            if (other.lhs < lhs) return false;
            return rhs < other.rhs;
        }
    };

    // LR项目
    struct LRItem{
        Production prod;// 产生式 E -> E+T
        int dotIndex;// .所在位置 E -> E.+T
        set<Symbol> lookahead;// 向前看的符号,考虑下一步是移进还是规约 eg. E-> E+T.+R|E+T., 通过判断
                              // 下一个符号是什么来决定是否规约
        bool operator==(const LRItem& other) const{
                                  return prod == other.prod && dotIndex == other.dotIndex && lookahead == other.lookahead;
        }

        bool operator<(const LRItem& other) const {
            if (prod < other.prod) return true;
            if (other.prod < prod) return false;
            if (dotIndex < other.dotIndex) return true;
            if (other.dotIndex < dotIndex) return false;
            return lookahead < other.lookahead;
        }
    };

    // 表示当前的状态
    struct LRState{
        int stateId; // 用于表示状态
        set<LRItem> items; // 用于表示该状态下的所有项目
        map<Symbol, int> transitions; // 用于表示下一步移进之后该转移到什么状态

        bool operator==(const LRState& other) const{
            return items == other.items;
        }
    };

private:
    vector<Production> productions;
    vector<Symbol> terminals;
    vector<Symbol> nonTerminals;
    Symbol startSymbol;
    vector<LRState> states;

    set<Symbol> first(const vector<Symbol>& symbols);
    set<Symbol> firstOfSymbol(const Symbol& symbol);
    LRItem shiftDot(const LRItem& item);
    set<Symbol> getSymbolsAfterDot(const LRItem& item);
    LRState closure(const LRState& state);
    LRState goTo(const LRState& state, const Symbol& symbol);
    void buildLRItems();
    void buildActionTable();
    void buildGotoTable();

public:
    map<pair<int, Symbol>, pair<char, int>> actionTable;
    map<pair<int, Symbol>, int> gotoTable;

    LRParserGenerator(const vector<Production>& productions, const vector<Symbol>& terminals, const vector<Symbol>& nonTerminals, const Symbol& startSymbol)
            : productions(productions), terminals(terminals), nonTerminals(nonTerminals), startSymbol(startSymbol) {}

    void generateParser() {
        buildLRItems();
        buildActionTable();
        buildGotoTable();
    }

    void printTables() {
        cout << "Action Table:" << endl;
        for (const auto& entry : actionTable) {
            cout << "(" << entry.first.first << ", " << entry.first.second.name << ") -> ";
            cout << entry.second.first << entry.second.second << endl;
        }

        cout << endl;

        cout << "Goto Table:" << endl;
        for (const auto& entry : gotoTable) {
            cout << "(" << entry.first.first << ", " << entry.first.second.name << ") -> ";
            cout << entry.second << endl;
        }
    }

    set<Symbol> follow(const Symbol &symbol);

    void printStates() {
        for (const auto& state : states) {
            cout << "State " << state.stateId << ":" << endl;
            for (const auto& item : state.items) {
                cout << item.prod.lhs.name << " -> ";
                for (size_t i = 0; i < item.prod.rhs.size(); ++i) {
                    if (i == item.dotIndex) cout << ".";
                    cout << item.prod.rhs[i].name << " ";
                }
                if (item.dotIndex == item.prod.rhs.size()) cout << ".";
                cout << endl;
            }
            cout << endl;
        }
    }
    void printProductions() const {
        cout << "Productions:" << endl;
        for (size_t i = 0; i < productions.size(); ++i) {
            const Production &prod = productions[i];
            cout << i << ": " << prod.lhs.name << " -> ";
            for (const Symbol &symbol: prod.rhs) {
                cout << symbol.name << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
};


#endif //COMPILER_LRPARSERGENERATOR_H
