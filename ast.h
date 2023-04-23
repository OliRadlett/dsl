#include <iostream>
#include <string>
#include <vector>

class Statement;
class Expression;
class VariableDefinition;

typedef std::vector<Statement*> StatementList;
typedef std::vector<Expression*> ExpressionList;
typedef std::vector<VariableDefinition*> VariableList;
typedef enum {NODE, EXPRESSION, STATEMENT, BLOCK, INTEGER, STRING, LIST, BOOLEAN, IDENTIFIER, VARIABLEDEFINITION, FUNCTIONDEFINITION, SPECIALFUNCTIONDEFINITION, LIBRARYFUNCTION, FUNCTIONCALL, EXPRESSIONSTATEMENT, IFSTATEMENT, CONDITION, ASSIGNMENT, LAMBDAEXPRESSION, LAMBDAARGS, RETURN} type;
typedef enum {EQUALS, NOTEQUALS} operator_type;
typedef enum {SETUP, LEAD, FOLLOW, SCORE} special_function;
typedef enum {PRINT, COUNTIF, ALL, EXISTS} library_function;


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
	std::vector<std::string> parameter_names;
	ExpressionList parameters_expressions;
	StatementList statements;
	Block() {};
};

class Integer : public Expression {
public:
	const int getNodeType() const override { return INTEGER; }
	int value;
	Integer(int value) : value(value) {}
};

class String : public Expression {
public:
	const int getNodeType() const override { return STRING; }
	std::string value;
	String(std::string value) { this->value = value.substr(1, value.length() - 2);} // Nice easy way to remove the speech marks from a string
};

class Boolean : public Expression {
public:
	const int getNodeType() const override { return BOOLEAN; }
	bool value;
	Boolean(bool value) : value(value) {}
};

class Vector {
public:
	std::vector<Integer> values;
	Vector(Integer value)
	{
		Push(value);
	};
	void Push(Integer value)
	{
		this->values.push_back(value);
	}
};

class List : public Expression {
public:
	const int getNodeType() const override { return LIST; }
	std::vector<Integer> values;
	List() {}
	List(Vector* values)
	{
		this->values = values->values;
	}
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
	ExpressionList parameters;
	FunctionCall(Identifier& id) : id(id) {};
	FunctionCall(Identifier& id, ExpressionList& parameters) : id(id), parameters(parameters) {};
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

class LambdaExpression : public Expression {
public:
	const int getNodeType() const override { return LAMBDAEXPRESSION; }
	Identifier& identifier;
	Condition *condition;
	LambdaExpression(Identifier& identifier, Condition *condition) : identifier(identifier), condition(condition) {};
};

class LambdaArgs : public Expression {
public:
	const int getNodeType() const override { return LAMBDAARGS; }
	Identifier& identifier;
	LambdaExpression *expression;
	LambdaArgs(Identifier& identifier, LambdaExpression *expression) : identifier(identifier), expression(expression) {};
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
	VariableList parameters;
	Block& block;
	FunctionDefinition(/*const Identifier& type, */ Identifier& id, Block &block): id(id), block(block) {}
	FunctionDefinition(/*const Identifier& type, */ Identifier& id, VariableList& parameters, Block &block): id(id), parameters(parameters), block(block) {}

};

class SpecialFunctionDefinition : public FunctionDefinition {
public:
	const int getNodeType() const override { return SPECIALFUNCTIONDEFINITION; }
	int type;
};

class LibraryFunction : public Statement {
public:
	const int getNodeType() const override { return LIBRARYFUNCTION; }
	int type;
	ExpressionList parameters;
	LambdaArgs *lambda;
	LibraryFunction(int type, ExpressionList parameters) : type(type), parameters(parameters) {}
	LibraryFunction(int type, LambdaArgs *lambda) : type(type), lambda(lambda) {}

};

class Return: public Statement {
public:
	const int getNodeType() const override { return RETURN; }
	Expression *expression;
	Return(Expression *expression) : expression(expression) {}
};

class IfStatement : public Statement {
public:
	const int getNodeType() const override { return IFSTATEMENT; }
	Condition *condition;
	Block& block;
	IfStatement(Condition *condition, Block& block) : condition(condition), block(block) {}
};