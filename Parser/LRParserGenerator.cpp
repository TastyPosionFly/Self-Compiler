#include <fstream>
#include <sstream>
#include "LRParserGenerator.h"

// 计算符号的FIRST集合
std::set<LRParserGenerator::Symbol> LRParserGenerator::first(const std::vector<Symbol> &symbols) {
    std::set<Symbol> firstSet;

    for (const Symbol &symbol: symbols) {
        if (symbol.type == SymbolType::TERMINAL) {
            firstSet.insert(symbol);
            break;
        } else {
            std::set<Symbol> firstOfSymbol = LRParserGenerator::firstOfSymbol(symbol);
            for (const Symbol &s: firstOfSymbol) {
                if (s.name != "epsilon") {
                    firstSet.insert(s);
                }
            }
            if (firstOfSymbol.find({"epsilon", SymbolType::TERMINAL}) != firstOfSymbol.end()) {
                continue;
            } else {
                break;
            }
        }
    }
    return firstSet;
}

std::set<LRParserGenerator::Symbol> LRParserGenerator::firstOfSymbol(const LRParserGenerator::Symbol &symbol) {
    if (firstCache.find(symbol) != firstCache.end()) {
        return firstCache[symbol];
    }

    std::set<Symbol> firstSet;
    if (symbol.type == SymbolType::TERMINAL) {
        firstSet.insert(symbol);
    } else {
        for (const Production &prod: productions) {
            if (prod.lhs.name == symbol.name) {
                std::set<Symbol> firstOfRhs = first(prod.rhs);
                firstSet.insert(firstOfRhs.begin(), firstOfRhs.end());
            }
        }
    }

    firstCache[symbol] = firstSet;
    return firstSet;
}

std::set<LRParserGenerator::Symbol> LRParserGenerator::follow(const Symbol &symbol) {
    if (followCache.find(symbol) != followCache.end()) {
        return followCache[symbol];
    }

    std::set<Symbol> followSet;
    if (symbol.name == startSymbol.name) {
        followSet.insert({"$", SymbolType::TERMINAL});
    }
    for (const Production &prod: productions) {
        for (size_t i = 0; i < prod.rhs.size(); ++i) {
            if (prod.rhs[i].name == symbol.name) {
                if (i + 1 < prod.rhs.size()) {
                    std::set<Symbol> firstOfNext = first({prod.rhs.begin() + i + 1, prod.rhs.end()});
                    followSet.insert(firstOfNext.begin(), firstOfNext.end());
                    followSet.erase({"epsilon", SymbolType::TERMINAL});
                }
                if (i + 1 == prod.rhs.size() ||
                    first({prod.rhs.begin() + i + 1, prod.rhs.end()}).count({"epsilon", SymbolType::TERMINAL})) {
                    std::set<Symbol> followOfLhs = follow(prod.lhs);
                    followSet.insert(followOfLhs.begin(), followOfLhs.end());
                }
            }
        }
    }

    followCache[symbol] = followSet;
    return followSet;
}

LRParserGenerator::LRItem LRParserGenerator::shiftDot(const LRParserGenerator::LRItem &item) {
    LRItem newItem = item;
    newItem.dotIndex++;
    return newItem;
}

std::set<LRParserGenerator::Symbol> LRParserGenerator::getSymbolsAfterDot(const LRParserGenerator::LRItem &item) {
    std::set<Symbol> symbols;
    if (item.dotIndex < item.prod.rhs.size()) {
        symbols.insert(item.prod.rhs[item.dotIndex]);
    }
    return symbols;
}

LRParserGenerator::LRState LRParserGenerator::closure(const LRParserGenerator::LRState &state) {
    LRState closureState = state;
    std::stack<LRItem> itemStack;

    for (const LRItem &lrItem: closureState.items) {
        itemStack.push(lrItem);
    }

    while (!itemStack.empty()) {
        LRItem currentItem = itemStack.top();
        itemStack.pop();

        if (currentItem.dotIndex < currentItem.prod.rhs.size()) {
            Symbol symbolAfterDot = currentItem.prod.rhs[currentItem.dotIndex];
            if (symbolAfterDot.type == SymbolType::NON_TERMINAL) {
                for (const Production &prod: productions) {
                    if (prod.lhs.name == symbolAfterDot.name) {
                        LRItem newItem = {prod, 0};
                        if (closureState.items.find(newItem) == closureState.items.end()) {
                            closureState.items.insert(newItem);
                            itemStack.push(newItem);
                        }
                    }
                }
            }
        }
    }

    return closureState;
}

LRParserGenerator::LRState
LRParserGenerator::goTo(const LRParserGenerator::LRState &state, const LRParserGenerator::Symbol &symbol) {
    LRState nextState;
    for (const LRItem &item: state.items) {
        if (item.dotIndex < item.prod.rhs.size() && item.prod.rhs[item.dotIndex].name == symbol.name) {
            LRItem shiftedItem = shiftDot(item);
            nextState.items.insert(shiftedItem);
        }
    }
    return closure(nextState);
}

void LRParserGenerator::buildLRItems() {
    LRItem initialItem = {productions[0], 0};
    LRState initialState = {0, {initialItem}};
    LRState startState = closure(initialState);
    states.push_back(startState);

    int stateId = 0;
    while (stateId < states.size()) {
        LRState currentState = states[stateId];
        currentState.stateId = stateId;
        for (const Symbol &symbol: terminals) {
            LRState nextState = goTo(currentState, symbol);
            if (!nextState.items.empty() && std::find(states.begin(), states.end(), nextState) == states.end()) {
                nextState.stateId = states.size();
                states.push_back(nextState);
            }
        }
        for (const Symbol &symbol: nonTerminals) {
            LRState nextState = goTo(currentState, symbol);
            if (!nextState.items.empty() && std::find(states.begin(), states.end(), nextState) == states.end()) {
                nextState.stateId = states.size();
                states.push_back(nextState);
            }
        }
        stateId++;
    }
}

void LRParserGenerator::buildActionTable() {
    for (int i = 0; i < states.size(); ++i) {
        LRState state = states[i];
        for (const LRItem &item: state.items) {
            if (item.dotIndex == item.prod.rhs.size()) {
                if (item.prod.lhs.name == startSymbol.name) {
                    actionTable[{i, {"$", SymbolType::TERMINAL}}] = {'A', 0};
                } else {
                    std::set<Symbol> followSet = follow(item.prod.lhs);
                    for (const Symbol &symbol: followSet) {
                        int prodIndex =
                                std::find(productions.begin(), productions.end(), item.prod) - productions.begin();
                        actionTable[{i, symbol}] = {'R', prodIndex};
                    }
                }
            }
        }
        for (const Symbol &symbol: terminals) {
            LRState nextState = goTo(state, symbol);
            if (!nextState.items.empty()) {
                int nextStateId = std::find(states.begin(), states.end(), nextState) - states.begin();
                actionTable[{i, symbol}] = {'S', nextStateId};
            }
        }
    }
}

void LRParserGenerator::buildGotoTable() {
    for (int i = 0; i < states.size(); i++) {
        LRState state = states[i];
        for (const Symbol &symbol: nonTerminals) {
            LRState nextState = goTo(state, symbol);
            if (!nextState.items.empty()) {
                int nextStateId = std::find(states.begin(), states.end(), nextState) - states.begin();
                gotoTable[{i, symbol}] = nextStateId;
            }
        }
    }
}

void LRParserGenerator::readGrammar(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        exit(1);
    }

    string input;
    while (getline(file, input)) {
        if (input.empty()) continue;

        size_t arrowIndex = input.find("->");
        if (arrowIndex == string::npos) {
            cout << "Invalid input format. Use '->' to separate left and right parts of the production." << endl;
            continue;
        }
        string lhs = input.substr(0, arrowIndex - 1);
        string rhs = input.substr(arrowIndex + 2);

        // Trim whitespace from lhs
        lhs.erase(lhs.find_last_not_of(" \n\r\t") + 1);
        lhs.erase(0, lhs.find_first_not_of(" \n\r\t"));

        LRParserGenerator::Symbol lhsSymbol = {lhs, LRParserGenerator::SymbolType::NON_TERMINAL};

        // Split the right-hand side by '|'
        stringstream ss(rhs);
        string subRhs;
        while (getline(ss, subRhs, '|')) {
            stringstream subRhsStream(subRhs);
            vector<LRParserGenerator::Symbol> rhsSymbols;
            string symbolName;
            while (subRhsStream >> symbolName) {
                LRParserGenerator::SymbolType type = (isupper(symbolName[0])
                                                      ? LRParserGenerator::SymbolType::NON_TERMINAL
                                                      : LRParserGenerator::SymbolType::TERMINAL);
                LRParserGenerator::Symbol symbol = {symbolName, type};
                rhsSymbols.push_back(symbol);
                if (type == LRParserGenerator::SymbolType::TERMINAL &&
                    find(terminals.begin(), terminals.end(), symbol) == terminals.end()) {
                    terminals.push_back(symbol);
                } else if (type == LRParserGenerator::SymbolType::NON_TERMINAL &&
                           find(nonTerminals.begin(), nonTerminals.end(), symbol) == nonTerminals.end()) {
                    nonTerminals.push_back(symbol);
                }
            }
            LRParserGenerator::Production production = {lhsSymbol, rhsSymbols};
            productions.push_back(production);
        }
    }

    file.close();

    // 扩展文法，和添加终止符
    LRParserGenerator::Symbol startSymbol_temp = {"S'", LRParserGenerator::SymbolType::NON_TERMINAL};
    LRParserGenerator::Production startProduction = {startSymbol_temp, {productions[0].lhs}};
    productions.insert(productions.begin(), startProduction);
    startSymbol = startSymbol_temp;

}
