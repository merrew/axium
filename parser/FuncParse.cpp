#include "FuncParse.h"
#include <algorithm>
#include <optional>
#include "QDebug"


std::string tokenTypeToString(LatexLexer::TokenType type)
{
    switch (type) {
    case LatexLexer::TokenType::Number:   return "Number";
    case LatexLexer::TokenType::Variable: return "Variable";
    case LatexLexer::TokenType::Operator: return "Operator";
    case LatexLexer::TokenType::Function: return "Function";
    case LatexLexer::TokenType::Left_Par:   return "Left_Par";
    case LatexLexer::TokenType::Right_Par:   return "Right_Par";
    case LatexLexer::TokenType::Left_Brace:   return "Left_Brace";
    case LatexLexer::TokenType::Right_Brace:   return "Right_Brace";
    case LatexLexer::TokenType::Left_Sqr_Par:   return "Left_Sqr_Par";
    case LatexLexer::TokenType::Right_Sqr_Par:   return "Right_Sqr_Par";
    case LatexLexer::TokenType::Abs:   return "Abs";
    case LatexLexer::TokenType::Const:   return "Const";
    case LatexLexer::TokenType::End:      return "End";
    default:                  return "Unknown";
    }
}

FuncParse::FuncParse(const std::string& expr) : expression(expr)
{
    setExpr(expr);
}

void FuncParse::printLex()
{
    qDebug().noquote() << "Tokens:\n";
    for (auto& t : tokens)
        qDebug().noquote() << tokenTypeToString(t.type) << " : " << t.value << "\n";
}

void FuncParse::printAST()
{
    qDebug().noquote() << "AST:\n";
    parser.print_ast(ast.get());
}

FuncType FuncParse::detectFunctionType(const std::string& expression)
{
    auto pos_eq = expression.find("=");
    if (pos_eq == std::string::npos)
        throw std::runtime_error("No '=' in the expression");
    if (expression.find('=', pos_eq + 1) != std::string::npos)
        throw std::runtime_error("More than one '='");

    std::string left = expression.substr(0,pos_eq);
    std::string right = expression.substr(pos_eq+1);

    bool left_has_y  = left.find('y') != std::string::npos;
    bool left_has_x  = left.find('x') != std::string::npos;
    bool right_has_y = right.find('y') != std::string::npos;
    bool right_has_x = right.find('x') != std::string::npos;

    if ((left_has_y && !left_has_x) ) {
        return FuncType::Forward; // y = f(x)
    } else if ((left_has_x && !left_has_y)) {
        return FuncType::Inverse; // x = f(y)
    } else {
        throw std::runtime_error("Cannot determine function direction");
    }

}

void FuncParse::setExpr(const std::string& expr)
{
    try
    {
        auto expression = expr;
        expression.erase(
            std::remove_if(expression.begin(), expression.end(),
                           [](unsigned char c){ return std::isspace(c); }),
            expression.end());

        if (expression.empty())
            throw std::runtime_error("Expression is empty");

        type = detectFunctionType(expression);

        expression = expression.substr(2, expression.size() - 1);
        // Лексер
        LatexLexer::Lexer lexer(expression);
        tokens = lexer.get_tokens();
        if (tokens.empty())
            throw std::runtime_error("Lexer produced zero tokens");

        // Парсер
        parser = Parser(std::move(tokens));
        ast = parser.parse();

        need_to_rebuild_AST = false;
    }
    catch (const std::exception& e)
    {
        std::cerr << "FuncParse ERROR: " << e.what()
        << " — fallback to y = 0\n";

        // Если ошибка — строим AST = константа 0
        ast = std::make_unique<NumberNode>(0.0);

        need_to_rebuild_AST = false;
    }
}

std::optional<double> FuncParse::eval(
    const std::unordered_map<std::string, double>& vars)
{
    try {
        if (!ast)
            throw std::runtime_error("AST is null");

        return ast->evaluate(vars);
    }
    catch (const std::exception& e)
    {
        std::cerr << "FuncParse::eval ERROR: "
                  << e.what() << " — return 0\n";
        return 0.0;   // безопасный откат
    }
}


