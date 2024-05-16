#include "LRParserGenerator.h"

// 计算符号的FIRST集合
set<LRParserGenerator::Symbol> LRParserGenerator::first(const vector<Symbol> &symbols) {
    set<Symbol> firstSet;

    for (const Symbol& symbol : symbols) {
        if (symbol.type == SymbolType::TERMINAL) {
            firstSet.insert(symbol);
            break;
        } else {
            set<Symbol> firstOfSymbol = LRParserGenerator::firstOfSymbol(symbol);
            for (const Symbol& s : firstOfSymbol) {
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

set<LRParserGenerator::Symbol> LRParserGenerator::firstOfSymbol(const LRParserGenerator::Symbol &symbol) {
    set<Symbol> firstSet;
    if (symbol.type == SymbolType::TERMINAL) {
        firstSet.insert(symbol);
    } else {
        for (const Production& prod : productions) {
            if (prod.lhs.name == symbol.name) {
                set<Symbol> firstOfRhs = first(prod.rhs);
                firstSet.insert(firstOfRhs.begin(), firstOfRhs.end());
            }
        }
    }
    return firstSet;
}

// 计算符号的FOLLOW集合
set<LRParserGenerator::Symbol> LRParserGenerator::follow(const Symbol &symbol) {
    set<Symbol> followSet;
    if (symbol.name == startSymbol.name) {
        followSet.insert({"$", SymbolType::TERMINAL});
    }
    for (const Production& prod : productions) {
        for (size_t i = 0; i < prod.rhs.size(); ++i) {
            if (prod.rhs[i].name == symbol.name) {
                if (i + 1 < prod.rhs.size()) {
                    set<Symbol> firstOfNext = first({prod.rhs.begin() + i + 1, prod.rhs.end()});
                    followSet.insert(firstOfNext.begin(), firstOfNext.end());
                    followSet.erase({"epsilon", SymbolType::TERMINAL});
                }
                if (i + 1 == prod.rhs.size() || first({prod.rhs.begin() + i + 1, prod.rhs.end()}).count({"epsilon", SymbolType::TERMINAL})) {
                    set<Symbol> followOfLhs = follow(prod.lhs);
                    followSet.insert(followOfLhs.begin(), followOfLhs.end());
                }
            }
        }
    }
    return followSet;
}

LRParserGenerator::LRItem LRParserGenerator::shiftDot(const LRParserGenerator::LRItem &item) {
    LRItem newItem = item;
    newItem.dotIndex++;
    return newItem;
}

set<LRParserGenerator::Symbol> LRParserGenerator::getSymbolsAfterDot(const LRParserGenerator::LRItem &item) {
    set<Symbol> symbols;
    if (item.dotIndex < item.prod.rhs.size()) {
        symbols.insert(item.prod.rhs[item.dotIndex]);
    }
    return symbols;
}

LRParserGenerator::LRState LRParserGenerator::closure(const LRParserGenerator::LRState &state) {
    LRState closureState = state;
    stack<LRItem> itemStack;

    for (const LRItem& lrItem: closureState.items) {
        itemStack.push(lrItem);
    }

    while (!itemStack.empty()) {
        LRItem currentItem = itemStack.top();
        itemStack.pop();

        if (currentItem.dotIndex < currentItem.prod.rhs.size()) {
            Symbol symbolAfterDot = currentItem.prod.rhs[currentItem.dotIndex];
            if (symbolAfterDot.type == SymbolType::NON_TERMINAL) {
                for (const Production& prod : productions) {
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

LRParserGenerator::LRState LRParserGenerator::goTo(const LRParserGenerator::LRState &state, const LRParserGenerator::Symbol &symbol) {
    LRState nextState;
    for (const LRItem& item: state.items) {
//        if (item.dotIndex < item.prod.rhs.size() && item.prod.rhs[item.dotIndex].name == "$"){
//            break;
//        }
        if (item.dotIndex < item.prod.rhs.size() && item.prod.rhs[item.dotIndex].name == symbol.name) {
            LRItem shiftedItem = shiftDot(item);
            nextState.items.insert(shiftedItem);
        }
    }
    return closure(nextState);
}

void LRParserGenerator::buildLRItems() {
    LRItem initialItem = { productions[0], 0 };
    LRState initialState = { 0, {initialItem} };
    LRState startState = closure(initialState);
    states.push_back(startState);

    int stateId = 0;
    while (stateId < states.size()) {
        LRState currentState = states[stateId];
        currentState.stateId = stateId;
        for (const Symbol& symbol: terminals) {
            LRState nextState = goTo(currentState, symbol);
            if (!nextState.items.empty() && find(states.begin(), states.end(), nextState) == states.end()) {
                nextState.stateId = states.size();
                states.push_back(nextState);
            }
        }
        for (const Symbol& symbol : nonTerminals) {
            LRState nextState = goTo(currentState, symbol);
            if (!nextState.items.empty() && find(states.begin(), states.end(), nextState) == states.end()) {
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
        for (const LRItem& item : state.items) {
            if (item.dotIndex == item.prod.rhs.size()) {
                if (item.prod.lhs.name == startSymbol.name) {
                    actionTable[{i, {"$", SymbolType::TERMINAL}}] = {'Acc', 0};
                } else {
                    set<Symbol> followSet = follow(item.prod.lhs);
                    for (const Symbol& symbol : followSet) {
                        int prodIndex = find(productions.begin(), productions.end(), item.prod) - productions.begin();
                        actionTable[{i, symbol}] = {'R', prodIndex};
                    }
                }
            }
        }
        for (const Symbol& symbol : terminals) {
            LRState nextState = goTo(state, symbol);
            if (!nextState.items.empty()) {
                int nextStateId = find(states.begin(), states.end(), nextState) - states.begin();
                actionTable[{i, symbol}] = {'S', nextStateId};
            }
        }
    }
}

void LRParserGenerator::buildGotoTable() {
    for (int i = 0; i < states.size(); i++) {
        LRState state = states[i];
        for (const Symbol& symbol : nonTerminals) {
            LRState nextState = goTo(state, symbol);
            if (!nextState.items.empty()) {
                int nextStateId = find(states.begin(), states.end(), nextState) - states.begin();
                gotoTable[{i, symbol}] = nextStateId;
            }
        }
    }
}