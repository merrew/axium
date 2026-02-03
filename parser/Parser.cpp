#include "Parser.h"
#include <QDebug>

NumberNode::NumberNode(double num)
    : value(std::move(num))
{
    type = NodeType::Number;
}

VariableNode::VariableNode(const std::string& str)
    : name(std::move(str))
{
    type = NodeType::Variable;
}

ConstNode::ConstNode(const std::string& c)
    : constant(std::move(c))
{
    type = NodeType::Const;
}

IndexNode::IndexNode(const std::string& name, std::unique_ptr<Node> b, std::unique_ptr<Node> i)
    : name_ind(std::move(name)), base(std::move(b)), ind(std::move(i))
{
    type = NodeType::Index;
}

UnaryNode::UnaryNode(const std::string& n, std::unique_ptr<Node> oper)
    : name(std::move(n)), op(std::move(oper))
{
    type = NodeType::UnaryOp;
}

BinaryOpNode::BinaryOpNode(const std::string& oper, std::unique_ptr<Node> left, std::unique_ptr<Node> right)
    : op(std::move(oper)), left(std::move(left)), right(std::move(right))
{
    type = NodeType::BinaryOp;
}

FunctionNode::FunctionNode(std::string func, std::vector<std::unique_ptr<Node>> x)
    : args(std::move(x)), f(std::move(func))
{
    type = NodeType::Function;
}

// Парсер
Parser::Parser(){}
Parser::Parser(std::vector<LatexLexer::Token> toks) : tokens(std::move(toks)) {}

LatexLexer::Token& Parser::look() { return tokens[pos]; }

LatexLexer::Token& Parser::get() { return tokens[pos++]; }

bool Parser::match_token(LatexLexer::TokenType type)
{
    if (look().type == type)
    {
        get();
        return true;
    }
    return false;
}

bool Parser::match_op(const std::string& op)
{
    if (look().type == LatexLexer::TokenType::Operator && look().value == op)
    {
        get();
        return true;
    }
    return false;
}

// Функции
std::unique_ptr<Node> Parser::parse_function(LatexLexer::Token& t)
{
    std::vector<std::unique_ptr<Node>> args;

    auto parse_block = [&](LatexLexer::TokenType open, LatexLexer::TokenType close)
    {
        if (!match_token(open))
            throw std::invalid_argument("Parser Error: Expected '{' after " + t.value + "in pos " + std::to_string(pos));
        int open_pos = pos;
        auto res = parse_expression();
        if (!match_token(close))
            throw std::invalid_argument("Parser Error: Unclosed '{' in " + t.value + "in pos " + std::to_string(open_pos));
        return res;
    };

    auto parse_power = [&](std::unique_ptr<Node>& power_node)
    {
        if (match_op("^"))
        {
            if (!match_token(LatexLexer::TokenType::Left_Brace))
                throw std::invalid_argument("Parser Error: Expected '{' after ^ in function power in pos "+ std::to_string(pos));
            int open_pos = pos;
            power_node = parse_expression();
            if (!match_token(LatexLexer::TokenType::Right_Brace))
                throw std::invalid_argument("Parser Error: Unclosed '}' in function power in pos "+ std::to_string(pos));
        }
    };

    if (t.value == "\\frac")
    {
        // 2 аргумента
        for (int i = 0; i < 2; ++i)
        {
            args.emplace_back(parse_block(LatexLexer::TokenType::Left_Brace, LatexLexer::TokenType::Right_Brace));
        }
        return std::make_unique<BinaryOpNode>("\\frac", std::move(args[0]), std::move(args[1]));
    }
    else if (t.value == "\\sqrt")
    {
        std::unique_ptr<Node> ind = std::make_unique<NumberNode>(2);;
        if (look().type == LatexLexer::TokenType::Left_Sqr_Par)
        // Парсим степень корня
        {
            ind = parse_block(LatexLexer::TokenType::Left_Sqr_Par, LatexLexer::TokenType::Right_Sqr_Par);
        }
        // Парсим выражение под корнем
        args.emplace_back(parse_block(LatexLexer::TokenType::Left_Brace, LatexLexer::TokenType::Right_Brace));
        auto base = std::make_unique<FunctionNode>("\\sqrt", std::move(args));

        return std::make_unique<IndexNode>("[]", std::move(base), std::move(ind));
    }
    else if (t.value == "\\log_")
    {
        // Парсим основание логарифма
        auto base = parse_block(LatexLexer::TokenType::Left_Brace, LatexLexer::TokenType::Right_Brace);

        // Парсим степень (если есть)
        std::unique_ptr<Node> power_node = nullptr;
        parse_power(power_node);

        // Парсим логарифмическое выражение
        args.emplace_back(parse_block(LatexLexer::TokenType::Left_Par, LatexLexer::TokenType::Right_Par));

        // Если была степень
        auto func_node = std::make_unique<FunctionNode>("\\log", std::move(args));
        auto res = std::make_unique<IndexNode>("_", std::move(func_node), std::move(base));
        if (power_node)
        {
            return std::make_unique<BinaryOpNode>(
                "^",
                std::move(res),
                std::move(power_node)
                );
        }
        //return IndexNode;
        return res;

    }
    else if (t.value == "\\left")
    {
        // Парсим то, что под модулем
        if(get().value != "|") throw std::invalid_argument("Parser Error: Expected open '|' int abs in pos " + std::to_string(pos));
        int open_pos = pos;
        args.emplace_back(parse_block(LatexLexer::TokenType::Left_Par, LatexLexer::TokenType::Right_Par));
        if(get().value != "\\right") throw std::invalid_argument("Parser Error: Expected '\\right' int abs (\\left in pos " + std::to_string(pos) + ")");
        if(get().value != "|") throw std::invalid_argument("Parser Error: Expected close '|' in abs in pos "+ std::to_string(pos));

        return std::make_unique<FunctionNode>("\\Abs", std::move(args));
    }
    // Стандартная функция f()
    else
    {
        // Парсим степень (если есть)
        std::unique_ptr<Node> power_node = nullptr;
        parse_power(power_node);

        args.emplace_back(parse_block(LatexLexer::TokenType::Left_Par, LatexLexer::TokenType::Right_Par));

        // Если была степень
        auto func_node = std::make_unique<FunctionNode>(t.value, std::move(args));
        if (power_node)
        {
            return std::make_unique<BinaryOpNode>(
                "^",
                std::move(func_node),
                std::move(power_node)
                );
        }
        return func_node;
    }
}

// Числа, переменные, функции, скобки
std::unique_ptr<Node> Parser::parse_primary()
{
    LatexLexer::Token t = look();
    if(match_token(LatexLexer::TokenType::Number))
        return std::make_unique<NumberNode>(std::stod(t.value));
    if(match_token(LatexLexer::TokenType::Const))
        return std::make_unique<ConstNode>(t.value);
    else if(match_token(LatexLexer::TokenType::Variable))
        return std::make_unique<VariableNode>(t.value);
    else if (match_token(LatexLexer::TokenType::Function))
    {
        return parse_function(t);
    }
    else if (match_token(LatexLexer::TokenType::Left_Par)) {
        int open_pos = pos;
        auto node = parse_expression();
        if (!match_token(LatexLexer::TokenType::Right_Par))
            throw std::invalid_argument("Parser Error: Unclosed parenthesis in pos " + std::to_string(open_pos));
        return node;
    }
    else {
        throw std::invalid_argument("Parser Error: Unexpected LatexLexer::Token: " + t.value + " in pos " + std::to_string(pos));
    }
}

// Унарные операции, степени
std::unique_ptr<Node> Parser::parse_factor()
{
    if (match_op("+"))
        return std::make_unique<UnaryNode>("+", parse_factor());
    else if (match_op("-"))
        return std::make_unique<UnaryNode>("-", parse_factor());

    std::unique_ptr<Node> base = parse_primary();
    while (match_op("^"))
    {
        std::string op = "^";

        if (!match_token(LatexLexer::TokenType::Left_Brace))
            throw std::invalid_argument("Parser Error: Expected '{' after ^ in pos "+ std::to_string(pos));
        int open_pos = pos;
        std::unique_ptr<Node> power = parse_expression();
        if (!match_token(LatexLexer::TokenType::Right_Brace))
            throw std::invalid_argument("Parser Error: Unclosed '}' in ^ in pos " + std::to_string(open_pos));
        base = std::make_unique<BinaryOpNode>(op, std::move(base), std::move(power));
    }
    return base;
}

// Умножение, деление
std::unique_ptr<Node> Parser::parse_term()
{
    std::unique_ptr<Node> left = parse_factor();
    while (true)
    {
        if (match_op("*"))
        {
            std::unique_ptr<Node> right = parse_factor();
            left = std::make_unique<BinaryOpNode>("*", std::move(left), std::move(right));
        }
        else break;
    }
    return left;
}

std::unique_ptr<Node> Parser::parse_expression()
{
    std::unique_ptr<Node> left = parse_term();

    while (true)
    {
        if (match_op("+"))
        {
            std::unique_ptr<Node> right = parse_term();
            left = std::make_unique<BinaryOpNode>("+", std::move(left), std::move(right));
        }
        else if (match_op("-"))
        {
            std::unique_ptr<Node> right = parse_term();
            left = std::make_unique<BinaryOpNode>("-", std::move(left), std::move(right));
        }
        else
            break;
    }
    return left;
}

std::unique_ptr<Node> Parser::parse()
{
    pos = 0;
    std::unique_ptr<Node> root = parse_expression();
    if (look().type != LatexLexer::TokenType::End)
        throw std::invalid_argument("Parser Error: Unexpected LatexLexer::Token after end of expression " + look().value);
    return root;
}

void Parser::print_ast(const Node* node, int indent)
{
    if (!node) return;

    std::string pad(indent, ' '); // отступ

    switch (node->type)
    {
    case NodeType::Number:
    {
        auto n = static_cast<const NumberNode*>(node);
        qDebug().noquote() << pad << "Number(" << n->value << ")\n";
        break;
    }

    case NodeType::Variable:
    {
        auto v = static_cast<const VariableNode*>(node);
        qDebug().noquote() << pad << "Variable(" << v->name << ")\n";
        break;
    }

    case NodeType::Index:
    {
        auto i = static_cast<const IndexNode*>(node);
        qDebug().noquote() << pad << "Index(" << i->name_ind << ")\n";
        print_ast(i->base.get(), indent + 4);
        print_ast(i->ind.get(), indent + 4);
        break;
    }

    case NodeType::UnaryOp:
    {
        auto i = static_cast<const UnaryNode*>(node);
        qDebug().noquote() << pad << "UnaryOp(" << i->name << ")\n";
        print_ast(i->op.get(), indent + 4);
        break;
    }

    case NodeType::BinaryOp:
    {
        auto b = static_cast<const BinaryOpNode*>(node);
        qDebug().noquote() << pad << "BinaryOp(" << b->op << ")\n";
        print_ast(b->left.get(), indent + 4);
        print_ast(b->right.get(), indent + 4);
        break;
    }

    case NodeType::Const:
    {
        auto v = static_cast<const ConstNode*>(node);
        qDebug().noquote() << pad << "Const(" << v->constant << ")\n";
        break;
    }

    case NodeType::Function:
    {
        auto f = static_cast<const FunctionNode*>(node);
        qDebug().noquote() << pad << "Function(" << f->f << ")\n";
        for (const auto& arg : f->args)
            print_ast(arg.get(), indent + 4);
        break;
    }

    default:
        qDebug().noquote() << pad << "Unknown node\n";
        break;
    }
}

// ВЫЧИСЛЕНИЕ ЗНАЧЕНИЙ
double NumberNode::evaluate(const std::unordered_map<std::string, double>& vars)
{
    return value;
}

double VariableNode::evaluate(const std::unordered_map<std::string, double>& vars)
{
    auto it = vars.find(name);
    if (it == vars.end())
        throw std::invalid_argument("Parser Error: Unknown variable: " + name);
    return it->second;
}

double ConstNode::evaluate(const std::unordered_map<std::string, double>& vars)
{
    if (constant == "\\pi") return M_PI;
    else if (constant == "\\e") return M_E;
    throw std::invalid_argument("Unknown constant: " + constant);
}

double UnaryNode::evaluate(const std::unordered_map<std::string, double>& vars)
{
    if (name == "+") return op->evaluate(vars);
    else if (name == "-") return -op->evaluate(vars);

    throw std::runtime_error("Parser Error: Unknown unary operator: " + name);
}


double BinaryOpNode::evaluate(const std::unordered_map<std::string, double>& vars)
{
    double l = left->evaluate(vars);
    double r = right->evaluate(vars);

    if(op == "+") return l + r;
    else if (op == "-") return l - r;
    else if (op == "^") return std::pow(l, r);
    else if (op == "*") return l * r;
    else if (op == "\\frac")
    {
        //if (r == 0.0) return 0.0;
        return (double)l / r;
    }
    throw std::invalid_argument("Parser Error: Unknown operator: " + op);
}

double IndexNode::evaluate(const std::unordered_map<std::string, double>& vars)
{
    double base_val = base->evaluate(vars);
    double ind_val = ind->evaluate(vars);

    // log
    if (name_ind == "_")
    {
        auto* func = dynamic_cast<FunctionNode*>(base.get());
        if (!func) throw std::invalid_argument("Index '_' used with non-log function");

        if (func->f == "\\log")
        {
            if (func->args.size() != 1)
                throw std::invalid_argument("Parser Error: Log takes exactly 1 argument");
            double x = func->args[0]->evaluate(vars);
            return std::log(x) / std::log(ind_val);
        }
    }

    // sqrt
    if (name_ind == "[]")
    {
        auto* func = dynamic_cast<FunctionNode*>(base.get());
        if (!func)
            throw std::invalid_argument("Parser Error: Index '[]' used with non-sqrt function");

        if (func->args.size() != 1)
            throw std::invalid_argument("Parser Error: Sqrt takes exactly 1 argument");

        double x = func->args[0]->evaluate(vars);
        return std::pow(x, 1.0 / ind_val);
    }

    throw std::invalid_argument("Parser Error: Unknown IndexNode type: " + name_ind);
}


double FunctionNode::evaluate(const std::unordered_map<std::string, double>& vars)
{
    if (f == "\\sin")
        return std::sin(args[0]->evaluate(vars));
    if (f == "\\cos")
        return std::cos(args[0]->evaluate(vars));
    if (f == "\\tan")
        return std::tan(args[0]->evaluate(vars));
    if (f == "\\cot")
        return 1.0 / std::tan(args[0]->evaluate(vars));

    if (f == "\\arcsin")
        return std::asin(args[0]->evaluate(vars));
    if (f == "\\arccos")
        return std::acos(args[0]->evaluate(vars));
    if (f == "\\arctan")
        return std::atan(args[0]->evaluate(vars));
    if (f == "\\textrm{arccot}")
        return std::atan2(1.0, args[0]->evaluate(vars));

    if (f == "\\sinh")
        return std::sinh(args[0]->evaluate(vars));
    if (f == "\\cosh")
        return std::cosh(args[0]->evaluate(vars));
    if (f == "\\tanh")
        return std::tanh(args[0]->evaluate(vars));
    if (f == "\\coth")
        return 1.0 / std::tanh(args[0]->evaluate(vars));

    if (f == "\\textrm{argsinh}")
        return std::asinh(args[0]->evaluate(vars));
    if (f == "\\textrm{argcosh}")
        return std::acosh(args[0]->evaluate(vars));
    if (f == "\\textrm{argtanh}")
        return std::atanh(args[0]->evaluate(vars));

    if (f == "\\exp")
        return std::exp(args[0]->evaluate(vars));

    if (f == "\\sqrt")
        return std::sqrt(args[0]->evaluate(vars));

    if (f == "\\log")
        return std::log(args[0]->evaluate(vars));
    if (f == "\\ln")
        return std::log(args[0]->evaluate(vars));
    if (f == "\\lg")
        return std::log10(args[0]->evaluate(vars));

    if (f == "\\Abs")
        return std::abs(args[0]->evaluate(vars));

    return 0;
}





