#include <iostream>
#include <string>
#include <vector>

class Statement;
class Expression;

typedef std::vector<Statement*> StatementList;
typedef std::vector<Expression*> ExpressionList;
typedef enum {NODE, EXPRESSION, STATEMENT, BLOCK, INTEGER, IDENTIFIER, VARIABLEDEFINITION, FUNCTIONDEFINITION, SPECIALFUNCTIONDEFINITION, FUNCTIONCALL, EXPRESSIONSTATEMENT, IFSTATEMENT, CONDITION, ASSIGNMENT} type;
typedef enum {EQUALS, NOTEQUALS} operator_type;


class Node {
public:
	virtual const int getNodeType() const = 0;
	virtual ~Node() {}
};

class Expression : public Node {
};

class Statement : public Node {
};

class Block : public Expression {
public:
	const int getNodeType() const override { return BLOCK; }
	StatementList statements;
	Block() {};
};

class Integer : public Expression {
public:
	const int getNodeType() const override { return INTEGER; }
	int value;
	Integer(int value) : value(value) {}
};

class Identifier : public Expression {
public:
	const int getNodeType() const override { return IDENTIFIER; }
	std::string name;
	Identifier(const std::string& name) : name(name) {};
};

class FunctionCall : public Expression {
public:
	const int getNodeType() const override { return FUNCTIONCALL; }
	Identifier& id;
	// Arguments
	FunctionCall(Identifier& id) : id(id) {};
};

class Condition : public Expression {
public:
	const int getNodeType() const override { return CONDITION; }
	Identifier& lhs;
	Identifier& rhs;
	int op;
	Condition(Identifier& lhs, Identifier& rhs, int op) : lhs(lhs), rhs(rhs), op(op) {};
};

class Assignment : public Expression {
public:
	const int getNodeType() const override { return ASSIGNMENT; }
	Identifier& lhs;
	Expression *rhs;
	Assignment(Identifier& lhs, Expression *rhs) : lhs(lhs), rhs(rhs) {};
};

class ExpressionStatement : public Statement {
public:
	const int getNodeType() const override { return EXPRESSIONSTATEMENT; }
    Expression& expression;
    ExpressionStatement(Expression& expression) : expression(expression) { }
};

class VariableDefinition : public Statement {
public:
	const int getNodeType() const override { return VARIABLEDEFINITION; }
	// const Identifier& type;
	Identifier& id;
	Expression *assignmentExpression;
	VariableDefinition(/*const Identifier& type, */Identifier& id) : /*type(type), */id(id) {}
	VariableDefinition(/*const Identifier& type, */Identifier& id, Expression *assignmentExpression) : /*type(type),*/ id(id), assignmentExpression(assignmentExpression) {}
};

class FunctionDefinition : public Statement {
public:
	const int getNodeType() const override { return FUNCTIONDEFINITION; }
	// const Identifier& type;
	Identifier& id;
	// Params
	Block& block;
	FunctionDefinition(/*const Identifier& type, */ Identifier& id, Block &block): id(id), block(block) {}
};

class SpecialFunctionDefinition : public Statement {
public:
	const int getNodeType() const override { return SPECIALFUNCTIONDEFINITION; }
};

class IfStatement : public Statement {
public:
	const int getNodeType() const override { return IFSTATEMENT; }
	Condition *condition;
	Block& block;
	IfStatement(Condition *condition, Block& block) : condition(condition), block(block) {}
};