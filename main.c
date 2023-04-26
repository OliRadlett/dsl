#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <typeinfo>
#include "main.h"
#include "ast.h"

extern Block *programBlock;
extern int yyparse();


extern "C" FILE *yyin;

typedef std::unordered_map<std::string, Node*> SymbolTable;
std::vector<SymbolTable> VariableTable;
SymbolTable FunctionTable;

Node* interpret(Node* node);

bool debug = false;

// Print debug information (hidden from the user unless run with -d or --debug)
template<typename T>
void print(const T& value)
{
    if (debug)
    {
        std::cout << value << std::endl;
    }
}

// Pretty print a List
std::string pprint_list(std::vector<Integer> vector)
{
    if (vector.empty()) {
        return "[]";
    }

    std::ostringstream oss;
    oss << "[";

    for (size_t i = 0; i < vector.size() - 1; i++) {
        oss << vector[i].value << ", ";
    }

    oss << vector.back().value << "]";

    return oss.str();
}

// Pretty print a list of function call parameters
std::string pprint_list(std::vector<Expression*> vector)
{
    std::ostringstream oss;

    for (size_t i = 0; i < vector.size(); i++) {
        Node* assignmentExpr = interpret(vector[i]);
        oss << "(" << assignmentExpr->getNodeType() << ") ";
    }

    return oss.str();
}

// Pretty print a list of variable definitions (for function parameter declarations)
std::string pprint_list(std::vector<VariableDefinition*> vector)
{

    std::ostringstream oss;

    for (size_t i = 0; i < vector.size(); i++) {
        oss << vector[i]->id.name << " ";
    }

    return oss.str();
}

// Print global variable table and function table
void print_debug_info()
{
    std::cout << "====================================" << std::endl;
    std::cout << "[Function Table]" << std::endl;
    for (auto it = FunctionTable.begin(); it != FunctionTable.end(); ++it)
    {
        std::cout << it->first << std::endl;
    }
    SymbolTable& currentScope = VariableTable.back();
    std::cout << "[Global Variable Table]" << std::endl;
    for (auto it = currentScope.begin(); it != currentScope.end(); ++it)
    {
        if (Integer* integer = dynamic_cast<Integer*>(it->second))
        {
            std::cout << it->first << ": " << integer->value << std::endl;
        }
        else if (List* list = dynamic_cast<List*>(it->second))
        {
            std::cout << it->first << ": " << pprint_list(list->values) << std::endl;
        }
        else if (String* string = dynamic_cast<String*>(it->second))
        {
            std::cout << it->first << ": " << string->value << std::endl;
        }
    }
}

// Check if an identifier can be declared
bool identifier_available(std::string name)
{
    SymbolTable scope = VariableTable.back();
    auto iter = scope.find(name);
    if (iter != scope.end())
    {
        return false;
    }

    // Check functions
    iter = FunctionTable.find(name);
    if (iter != FunctionTable.end())
    {
        return false;
    }

    return true;
}

// Evaluate a condition
bool evaluate(Node* lhs, Node* rhs, int op)
{
    switch (op)
    {
        case EQUALS:
        {
            if (Integer* lhs_ = dynamic_cast<Integer*>(lhs))
            {
                if (Integer* rhs_ = dynamic_cast<Integer*>(rhs))
                {
                   print("Evalutating " + std::to_string(lhs_->value) + " == " + std::to_string(rhs_->value));
                   if (lhs_->value == rhs_->value)
                   {
                    return true;
                   }
                   else
                   {
                    return false;
                   }
                }
            }
            else if (Boolean* lhs_ = dynamic_cast<Boolean*>(lhs))
            {
                if (Boolean* rhs_ = dynamic_cast<Boolean*>(rhs))
                {
                   print("Evalutating " + std::to_string(lhs_->value) + " == " + std::to_string(rhs_->value));
                   if (lhs_->value == rhs_->value)
                   {
                    return true;
                   }
                   else
                   {
                    return false;
                   }
                }
            }
        }
        break;
    case NOTEQUALS:
        {
            if (Integer* lhs_ = dynamic_cast<Integer*>(lhs))
            {
                if (Integer* rhs_ = dynamic_cast<Integer*>(rhs))
                {
                   print("Evalutating " + std::to_string(lhs_->value) + " != " + std::to_string(rhs_->value));
                   if (lhs_->value != rhs_->value)
                   {
                    return true;
                   }
                   else
                   {
                    return false;
                   }
                }
            }
            else if (Boolean* lhs_ = dynamic_cast<Boolean*>(lhs))
            {
                if (Boolean* rhs_ = dynamic_cast<Boolean*>(rhs))
                {
                   print("Evalutating " + std::to_string(lhs_->value) + " != " + std::to_string(rhs_->value));
                   if (lhs_->value != rhs_->value)
                   {
                    return true;
                   }
                   else
                   {
                    return false;
                   }
                }
            }
        }
        break;
    default:
        {
            throw std::runtime_error("Somehow there's an invalid operator. This should be impossible");
        }
        break;
    }
    throw std::runtime_error("Somehow condition is neither true or false. This should be impossible");
}

// Library print function - used to print a value to stdout. Library functions will be moved to another file
void library_print(ExpressionList& parameters)
{
    if (parameters.size() > 1)
    {
        throw std::runtime_error("Error: print only takes one parameter");
    }
    else
    {
        // Print only takes one parameter
        Node* toPrint = interpret(parameters[0]);

        if (Integer* value = dynamic_cast<Integer*>(toPrint))
        {
            print("Printing integer");
            std::cout << value->value << std::endl;
        }
        else if (List* value = dynamic_cast<List*>(toPrint))
        {
            print("Printing list");
            std::cout << pprint_list(value->values) << std::endl;
        }
        else if (String* value = dynamic_cast<String*>(toPrint))
        {
            print("Printing string");
            std::cout << value->value << std::endl;
        }
        else if (Boolean* value = dynamic_cast<Boolean*>(toPrint))
        {
            print("Printing boolean");
            // Print true/false not 1/0
            if (value->value)
            {
                std::cout << "True" << std::endl;
            }
            else
            {
                std::cout << "False" << std::endl;
            }
        }
        else
        {
            throw std::runtime_error("Invalid parameter");
        }
    }
}

// Library countif function - used to count occurences of a value in a list. Library functions will be moved to another file
Integer* library_countif(LambdaArgs* lambda)
{

    if (lambda->expression->identifier.name.compare(lambda->expression->condition->lhs.name) != 0)
    {
        throw std::runtime_error("Malformed lambda expression");
    }

    // Cast identifier and rhs to Node* so we can pass it through the interpreter to extract its value
    Node* variable = dynamic_cast<Node*>(&lambda->identifier);
    Node* rhs = dynamic_cast<Node*>(&lambda->expression->condition->rhs);
    variable = interpret(variable);
    rhs = interpret(rhs);
    int count = 0;
    if (List* list = dynamic_cast<List*>(variable))
    {

        for (size_t i = 0; i < list->values.size(); i++)
        {
            // This is fun because lists contain ints not Integers we have to do some type casting
            Node* lhs = new Integer(list->values[i]);
            if (evaluate(lhs, rhs, lambda->expression->condition->op))
            {
                count++;
            }
        }
        print(count);
        return new Integer(count);
    }
    else
    {
        throw std::runtime_error("countif only works on lists");
    }
    return 0; // This should never occur
}

// Library all function - uses to check of all the items in a list are a value. Library functions will be moved to another file
Boolean* library_all(LambdaArgs* lambda)
{

    if (lambda->expression->identifier.name.compare(lambda->expression->condition->lhs.name) != 0)
    {
        throw std::runtime_error("Malformed lambda expression");
    }

    // Cast identifier and rhs to Node* so we can pass it through the interpreter to extract its value
    Node* variable = dynamic_cast<Node*>(&lambda->identifier);
    Node* rhs = dynamic_cast<Node*>(&lambda->expression->condition->rhs);
    variable = interpret(variable);
    rhs = interpret(rhs);
    int count = 0;
    if (List* list = dynamic_cast<List*>(variable))
    {

        for (size_t i = 0; i < list->values.size(); i++)
        {
            // This is fun because lists contain ints not Integers we have to do some type casting
            Node* lhs = new Integer(list->values[i]);
            if (evaluate(lhs, rhs, lambda->expression->condition->op))
            {
                count++;
            }
        }

        if (count == list->values.size())
        {
            print("True");
            return new Boolean(true);
        }
        else
        {
            print("False");
            return new Boolean(false);
        }
    }
    else
    {
        throw std::runtime_error("all only works on lists");
    }
    return new Boolean(false); // This should never occur
}

// Library exists function - used to check if a value exists in a list. Library functions will be moved to another file
Boolean* library_exists(LambdaArgs* lambda)
{

    if (lambda->expression->identifier.name.compare(lambda->expression->condition->lhs.name) != 0)
    {
        throw std::runtime_error("Malformed lambda expression");
    }

    // Cast identifier and rhs to Node* so we can pass it through the interpreter to extract its value
    Node* variable = dynamic_cast<Node*>(&lambda->identifier);
    Node* rhs = dynamic_cast<Node*>(&lambda->expression->condition->rhs);
    variable = interpret(variable);
    rhs = interpret(rhs);
    if (List* list = dynamic_cast<List*>(variable))
    {

        for (size_t i = 0; i < list->values.size(); i++)
        {
            // This is fun because lists contain ints not Integers we have to do some type casting
            Node* lhs = new Integer(list->values[i]);
            if (evaluate(lhs, rhs, lambda->expression->condition->op))
            {
                print("True");
                return new Boolean(true);
            }
        }

        print("False");
        return new Boolean(false);
    }
    else
    {
        throw std::runtime_error("exists only works on lists");
    }
    return new Boolean(false); // Should never occur
}

// Recursive interpreter - main part of this file. Calls itself until it meets a left node then returns up
Node* interpret(Node* node)
{
    switch (node->getNodeType())
    {
    case NODE:
        print("Node");
        break;
    case EXPRESSION:
        print("Expression");
        break;
    case STATEMENT:
        print("Statement");
        break;
    case BLOCK:
        {
            print("Block");
            Block* block = dynamic_cast<Block*>(node);
            // Create a new symbol table for this block's scope
            SymbolTable blockScope;

            // Add paramaters to scope
            if (!block->parameter_names.empty())
            {
                for (size_t i = 0; i < block->parameter_names.size(); i++)
                {
                    print("Adding " + block->parameter_names[i] + " to function scope");
                    blockScope[block->parameter_names[i]] = interpret(block->parameters_expressions[i]);
                }
            }

            // Push the symbol table onto the stack
            VariableTable.push_back(blockScope);
            // Interpret each statement in the block
            for (auto stmt : block->statements) {
                if (Return* return_ = dynamic_cast<Return*>(stmt))
                {
                    // Evaluate the return expression and then remove the scope from the stack BEFORE returning
                    Node* x =  interpret(return_->expression);
                    VariableTable.pop_back();
                    return x;
                }
                else
                {
                    interpret(stmt);
                }
            }
            // Pop the symbol table off the stack when we leave the block
            VariableTable.pop_back();
        }
        break;
    case IDENTIFIER:
        {
            print("Identifier");
            Identifier* identifier = dynamic_cast<Identifier*>(node);

            // If our identifier is actually just a datatype we skip the lookup and just return the value
            if (Integer* value = dynamic_cast<Integer*>(identifier))
            {
                print("Found integer: " + std::to_string(value->value));
                return value;
            } else if (List* value = dynamic_cast<List*>(identifier))
            {
                print("Found list: " + pprint_list(value->values));
                return value;
            } else if (String* value = dynamic_cast<String*>(identifier))
            {
                print("Found string: " + value->value);
                return value;
            } else if (Boolean* value = dynamic_cast<Boolean*>(identifier))
            {
                print("Found boolean: " + value->value);
                return value;
            }

            // Check if variable is accessible from scope
            for (auto i = VariableTable.rbegin(); i != VariableTable.rend(); i++)
            {
                SymbolTable& symbolTable = *i;
                auto entryIter = symbolTable.find(identifier->name);
                if (entryIter != symbolTable.end())
                {
                    print("Variable " + identifier->name + " is accessible from current scope");
                    return symbolTable[identifier->name];
                }
            }

            throw std::runtime_error(identifier->name + " doesn't exist in current scope");
        }
        break;
    case VARIABLEDEFINITION:
        {
            print("Variable definition");
            VariableDefinition* variableDefinition = dynamic_cast<VariableDefinition*>(node);

            if (identifier_available(variableDefinition->id.name))
            {
                Node* assignmentExpr = nullptr;
                if (variableDefinition->assignmentExpression != nullptr)
                {
                    if (FunctionCall* value = dynamic_cast<FunctionCall*>(variableDefinition->assignmentExpression))
                    {
                        print("Assigning to function call");
                        Block* function = dynamic_cast<Block*>(FunctionTable[value->id.name]);
                        if (!value->parameters.empty())
                        {
                            print("Parameters: " + pprint_list(value->parameters));
                            function->parameters_expressions = value->parameters;
                        }
                        if (function != nullptr)
                        {
                            assignmentExpr = interpret(function);
                        }
                        
                    }
                    else
                    {
                        assignmentExpr = interpret(variableDefinition->assignmentExpression);
                    }

                    if (Integer* value = dynamic_cast<Integer*>(assignmentExpr))
                    {
                        print("Variable defined. Identifier: " + variableDefinition->id.name  + " Assignment Expression: " + std::to_string(value->value));
                    }
                    else if (List* value = dynamic_cast<List*>(assignmentExpr))
                    {
                        print("Variable defined. Identifier: " + variableDefinition->id.name  + " Assignment Expression: " + pprint_list(value->values));
                    }
                    else if (String* value = dynamic_cast<String*>(assignmentExpr))
                    {
                        print("Variable defined. Identifier: " + variableDefinition->id.name  + " Assignment Expression: " + value->value);
                    }
                    else if (Boolean* value = dynamic_cast<Boolean*>(assignmentExpr))
                    {
                        print("Variable defined. Identifier: " + variableDefinition->id.name  + " Assignment Expression: " + std::to_string(value->value));
                    }
                    else
                    {
                        throw std::runtime_error("Unimplemented datatype");
                    }
                }
                else
                {
                    print("Identifier: " + variableDefinition->id.name + " (no assignment expression)");
                }

                // Assign variable in current scope
                SymbolTable& currentScope = VariableTable.back();
                currentScope[variableDefinition->id.name] = assignmentExpr;

            }
            else 
            {
                throw std::runtime_error("Identifier exists in current scope: " + variableDefinition->id.name);
            }
        }
        break;
    case ASSIGNMENT:
        {
            print("ASSIGNMENT");
            Assignment* assignment = dynamic_cast<Assignment*>(node);
            Identifier* lhs = dynamic_cast<Identifier*>(&assignment->lhs);
            bool successful = false;

            // Search backwards (local -> global)
            for (auto i = VariableTable.rbegin(); i != VariableTable.rend(); i++)
            {
                SymbolTable& symbolTable = *i;
                auto entryIter = symbolTable.find(lhs->name);
                if (entryIter != symbolTable.end())
                {
                    // Update the value of the entry if found
                    print("Updating " + lhs->name);
                    entryIter->second = assignment->rhs;
                    successful = true;
                    break;
                }
            }

            if (!successful)
                throw std::runtime_error("Undefined variable: " + lhs->name);
        }
        break;
    case FUNCTIONCALL:
        {
            print("Function call");
            FunctionCall* call = dynamic_cast<FunctionCall*>(node);
            Block* function = dynamic_cast<Block*>(FunctionTable[call->id.name]);
            print("Identifier: " + call->id.name);

            if (function->parameter_names.size() > call->parameters.size())
            {
                throw std::runtime_error("Not enough parameters provided");
            }

            if (!call->parameters.empty())
            {
                print("Parameters: " + pprint_list(call->parameters));
                // Inject parameters
                function->parameters_expressions = call->parameters;
            }
            if (function != nullptr)
            {
                // Run function
                interpret(function);
            }
        }
        break;
    case EXPRESSIONSTATEMENT:
        {
            print("Expression statement");
            ExpressionStatement* e = dynamic_cast<ExpressionStatement*>(node);
            return interpret(&e->expression);
        }
        break;
    case FUNCTIONDEFINITION:
        {
            print("Function definition");
            FunctionDefinition* fd = dynamic_cast<FunctionDefinition*>(node);
            if (identifier_available(fd->id.name))
            {
                print("Identifier: " + fd->id.name);
                if (!fd->parameters.empty())
                {
                    print("Parameters: " + pprint_list(fd->parameters));

                    // Inject parameters
                    std::vector<std::string> parameter_names;

                    for (size_t i = 0; i < fd->parameters.size(); i++)
                    {
                        parameter_names.push_back(fd->parameters[i]->id.name);
                    }

                    fd->block.parameter_names = parameter_names;

                }
                FunctionTable[fd->id.name] = &fd->block;

            }
            else 
            {
                throw std::runtime_error("Identifier exists in current scope: " + fd->id.name);
            }
        }
        break;
    case IFSTATEMENT:
        {
            print("If statement");
            IfStatement* if_statement = dynamic_cast<IfStatement*>(node);
            Condition* condition = dynamic_cast<Condition*>(if_statement->condition);

            if (evaluate(interpret(&condition->lhs), interpret(&condition->rhs), condition->op))
            {
                Node* block = dynamic_cast<Node*>(&if_statement->block);
                interpret(block);
            }
        }
        break;
    case INTEGER:
        {
            print("Integer");
            Integer* integer = dynamic_cast<Integer*>(node);
            return integer;
        }
        break;
    case LIST:
        {
            print("List");
            List* list = dynamic_cast<List*>(node);
            return list;
        }
        break;
    case STRING:
        {
            print("String");
            String* string = dynamic_cast<String*>(node);
            return string;
        }
        break;
    case BOOLEAN:
        {
            print("Boolean");
            Boolean* boolean = dynamic_cast<Boolean*>(node);
            return boolean;
        }
        break;
    case LIBRARYFUNCTION:
        {
            print("Library function");
            LibraryFunction* function = dynamic_cast<LibraryFunction*>(node);
            switch (function->type)
            {
                case PRINT:
                    {
                        print("Print library function");
                        library_print(function->parameters);
                        break;
                    }
                case COUNTIF:
                    {
                        print("Countif library function");
                        return library_countif(function->lambda);
                        break;
                    }
                case ALL:
                    {
                        print("All library function");
                        return library_all(function->lambda);
                        break;
                    }
                case EXISTS:
                    {
                        print("Exists library function");
                        return library_exists(function->lambda);
                        break;
                    }
                default:
                    print("Unknown library function");
                    break;
            }
        }
        break;
    case RETURN:
        {
            print("Return");
            Expression* expression = dynamic_cast<Expression*>(node);
            return expression;
        }
        break;
    default:
        print("Unknown node type: " + node->getNodeType());
        break;
    }
    // Never occurs, just keeps the compiler happy
    return nullptr;
}

int main(int argc, char **argv)
{

    bool file = false;
    const char* sourcePath;

    // Process command line options
    for (int i = 1; i < argc; i++)
    {
        if (std::string(argv[i]) == "-d" || std::string(argv[i]) == "--debug")
        {
            debug = true;
        }
        else if (std::string(argv[i]) == "-f" || std::string(argv[i]) == "--file")
        {
            file = true;
            sourcePath = argv[i + 1];
            i++; // Skip next paramater
        }
    }

    if(file)
    {
        yyin = fopen(sourcePath, "r");
        print("Parsing from file");
    }
    else
    {
        yyin = stdin;
        print("Parsing from stdin");
    }

    if (debug)
    {
        print("Running in debug mode");
    }

    yyparse();

    // Create global scope
    SymbolTable globalScope;
    VariableTable.push_back(globalScope);

    if (programBlock == nullptr)
    {
            throw std::runtime_error("AST not formed");
    }

    // Have to print these seperately because there is no nice way to concaternate string and pointer address
    print("Root node address: ");
    print(programBlock);

    // Execute root node
    for (auto statement : programBlock->statements)
    {
        interpret(statement);
    }


    // Show variable table if run with debug flag
    if (debug)
    {
        print_debug_info();
    }

    return 0;
}