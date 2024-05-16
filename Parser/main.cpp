#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include "LRParserGenerator.h"

using namespace std;

int main() {
    vector<LRParserGenerator::Production> productions;
    vector<LRParserGenerator::Symbol> terminals;
    vector<LRParserGenerator::Symbol> nonTerminals;

    cout << "Enter the filename containing the grammar:" << endl;
    string filename;
    cin >> filename;

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return 1;
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

        LRParserGenerator::Symbol lhsSymbol = { lhs, LRParserGenerator::SymbolType::NON_TERMINAL };

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
                LRParserGenerator::Symbol symbol = { symbolName, type };
                rhsSymbols.push_back(symbol);
                if (type == LRParserGenerator::SymbolType::TERMINAL &&
                    find(terminals.begin(), terminals.end(), symbol) == terminals.end()) {
                    terminals.push_back(symbol);
                } else if (type == LRParserGenerator::SymbolType::NON_TERMINAL &&
                           find(nonTerminals.begin(), nonTerminals.end(), symbol) == nonTerminals.end()) {
                    nonTerminals.push_back(symbol);
                }
            }
            LRParserGenerator::Production production = { lhsSymbol, rhsSymbols };
            productions.push_back(production);
        }
    }

    file.close();

    // 扩展文法，和添加终止符
    LRParserGenerator::Symbol startSymbol = { "S'", LRParserGenerator::SymbolType::NON_TERMINAL };
    LRParserGenerator::Production startProduction = { startSymbol, { productions[0].lhs} };
    productions.insert(productions.begin(), startProduction);

    LRParserGenerator generator(productions, terminals, nonTerminals, startSymbol);
    generator.generateParser();
    generator.printProductions();
    generator.printStates();
    generator.printTables();

    return 0;
}