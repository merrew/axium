#pragma once

#include "Lexer.h"
#include "Parser.h"
#include <optional>

enum class FuncType {Forward, Inverse};

class FuncParse
{
public:
    std::string expression;
    explicit FuncParse(const std::string& expr);
    void setExpr(const std::string& expr);

    void printLex();
    void printAST();

    std::optional<double> eval(const std::unordered_map<std::string, double>& vars);

    FuncType type;

    bool correct = false;
private:
    bool need_to_rebuild_AST = true;
    Parser parser;
    std::vector<LatexLexer::Token> tokens;
    std::unique_ptr<Node> ast;

    FuncType detectFunctionType(const std::string& expression);
};
