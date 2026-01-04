#pragma once
#define _USE_MATH_DEFINES
#include <cmath>

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "Lexer.h"
enum class NodeType
{
    Const,      // pi, e
    Number,     // 1, 2.3...
    Variable,   // x
    UnaryOp,    // ^, sqrt...
    BinaryOp,   // +, -, *...
    Function,   // \\sin, \\lg...
    Index
};

struct Node
{
    NodeType type;
    virtual double evaluate(const std::unordered_map<std::string, double>& vars) = 0;
    virtual ~Node() = default;
};

struct NumberNode : Node
{
    double value;
    NumberNode(double num);
    double evaluate(const std::unordered_map<std::string, double>& vars) override;

};

struct VariableNode : Node
{
    std::string name;
    VariableNode(const std::string& str);
    double evaluate(const std::unordered_map<std::string, double>& vars) override;
};

struct ConstNode : Node
{
    std::string constant;
    ConstNode(const std::string& c);
    double evaluate(const std::unordered_map<std::string, double>& vars) override;
};

struct IndexNode : Node
{
    std::string name_ind;
    std::unique_ptr<Node> base, ind;
    IndexNode(const std::string& name, std::unique_ptr<Node> b, std::unique_ptr<Node> i);
    double evaluate(const std::unordered_map<std::string, double>& vars) override;
};

struct UnaryNode : Node
{
    std::string name;
    std::unique_ptr<Node> op;
    UnaryNode(const std::string& n, std::unique_ptr<Node> oper);
    double evaluate(const std::unordered_map<std::string, double>& vars) override;

};

struct BinaryOpNode : Node
{
    std::string op;
    std::unique_ptr<Node> left, right;
    BinaryOpNode(const std::string& oper, std::unique_ptr<Node> left, std::unique_ptr<Node> right);
    double evaluate(const std::unordered_map<std::string, double>& vars) override;
};

struct FunctionNode : Node
{
    std::string f;
    std::vector<std::unique_ptr<Node>> args;
    FunctionNode(std::string func, std::vector<std::unique_ptr<Node>> args_vec);
    double evaluate(const std::unordered_map<std::string, double>& vars) override;
};

// Парсер
class Parser
{
public:
    Parser();
    explicit Parser(std::vector<LatexLexer::Token> toks);
    std::unique_ptr<Node> parse();

    void print_ast(const Node* node, int indent = 0);

private:
    std::vector<LatexLexer::Token> tokens;
    int pos = 0;

    LatexLexer::Token& look();
    LatexLexer::Token& get();
    bool match_token(LatexLexer::TokenType type);
    bool match_op(const std::string& op);

    std::unique_ptr<Node> parse_expression();
    std::unique_ptr<Node> parse_term();
    std::unique_ptr<Node> parse_factor();
    std::unique_ptr<Node> parse_primary();
    std::unique_ptr<Node> parse_function(LatexLexer::Token& t);
};

