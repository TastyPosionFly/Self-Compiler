#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include <string>
#include <vector>
#include <map>
#include "LRParserGenerator.h"

using namespace std;

class parser {
private:
    // 符号的类型
    enum class SymbolType{
        TERMINAL,
        NON_TERMINAL
    };

    std::vector<LRParserGenerator::Production> productions;
    std::map<pair<int, LRParserGenerator::Symbol>, pair<char, int>> actionTable;
    std::map<std::pair<int, LRParserGenerator::Symbol>, int> gotoTable;

public:
    parser(const std::vector<LRParserGenerator::Production>& productions,
           const std::map<std::pair<int, LRParserGenerator::Symbol>, pair<char, int>>& actionTable,
           const std::map<std::pair<int, LRParserGenerator::Symbol>, int>& gotoTable)
            : productions(productions), actionTable(actionTable), gotoTable(gotoTable) {}

    bool parse(const std::vector<LRParserGenerator::Symbol>& input);

};


#endif //COMPILER_PARSER_H
