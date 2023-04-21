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

template<typename T>
void print(const T& value)
{
    if (debug)
    {
        std::cout << value << std::endl;
    }
}

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

std::string pprint_list(std::vector<Expression*> vector)
{
    std::ostringstream oss;

    for (size_t i = 0; i < vector.size(); i++) {
        Node* assignmentExpr = interpret(vector[i]);
        oss << "(" << assignmentExpr->getNodeType() << ") ";
    }

    return oss.str();
}

std::string pprint_list(std::vector<VariableDefinition*> vector)
{

    std::ostringstream oss;

    for (size_t i = 0; i < vector.size(); i++) {
        oss << vector[i]->id.name << " ";
    }

    return oss.str();
}

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
    }
}

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
        }
        break;
    default:
        {
            throw std::runtime_error("Somehow there's an invalid operator. This should be impossible");
            break;
        }
    }
}

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
    }
}

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

            // Also need to make sure parameters are at the top of the scope

            // // Need to map these to their expected names
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
                interpret(stmt);
            }
            // Pop the symbol table off the stack when we leave the block
            VariableTable.pop_back();
        }
        break;
    case INTEGER:
        {
            print("Integer");
            Integer* integer = dynamic_cast<Integer*>(node);
            return integer;
        }
        break;
    case IDENTIFIER:
        {
            // Here we want to lookup the name in the function and variable tables and if its present we return either a value or a function pointer
            print("Identifier");
            Identifier* identifier = dynamic_cast<Identifier*>(node);

            // If our identifier is actually an interger we skip the lookup and just return the value
            if (Integer* value = dynamic_cast<Integer*>(identifier))
            {
                print("Found integer: " + std::to_string(value->value));
                return value;
            }

            // This might need changing
            // At the moment functions don't return anything so we only look in variables
            // What we're doing here is not just checking if the identifier exists but we're returning the value from the correct scope
            // Need to check if this works in the right order (if a variable x is defined in gobal and function scope and then accessed in function scope does it return the correct value)
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

            // return nullptr;

            throw std::runtime_error(identifier->name + " doesn't exist in current scope");
        }
        break;
    case VARIABLEDEFINITION:
        {
            print("Variable definition");
            VariableDefinition* variableDefinition = dynamic_cast<VariableDefinition*>(node);

            SymbolTable& currentScope = VariableTable.back();

            if (identifier_available(variableDefinition->id.name))
            {
                Node* assignmentExpr = nullptr;
                if (variableDefinition->assignmentExpression != nullptr)
                {
                    // Functions will need lots of work here
                    // All other datatypes will need adding

                    assignmentExpr = interpret(variableDefinition->assignmentExpression);

                    if (Integer* value = dynamic_cast<Integer*>(assignmentExpr))
                    {
                        print("Variable defined. Identifier: " + variableDefinition->id.name  + " Assignment Expression: " + std::to_string(value->value));
                    }
                    else if (List* value = dynamic_cast<List*>(assignmentExpr))
                    {
                        print("Variable defined. Identifier: " + variableDefinition->id.name  + " Assignment Expression: " + pprint_list(value->values));;
                    }
                    else
                    {
                        print("Something shite happened: ");
                        print(assignmentExpr);
                    }
                }
                else
                {
                    print("Identifier: " + variableDefinition->id.name + " (no assignment expression)");
                }
                // Add the variable to the current scope's symbol table
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

            // Search backwards
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
            if (!call->parameters.empty())
            {
                print("Parameters: " + pprint_list(call->parameters));
                function->parameters_expressions = call->parameters;
            }
            std::cout << std::endl;

            if (function != nullptr)
            {
                interpret(function);
            }

            // TODO Retrieve function from FunctionTable and run this on it's Block
            // Block* block = &fd->block;
            // for (auto stmt : block->statements) {
            //     interpret(stmt);
            // }
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

                    std::vector<std::string> parameter_names;

                    for (size_t i = 0; i < fd->parameters.size(); i++)
                    {
                        // Need to make sure these are in the same order as the call arguements
                        parameter_names.push_back(fd->parameters[i]->id.name);
                    }

                    fd->block.parameter_names = parameter_names;

                }
                std::cout << std::endl;
                FunctionTable[fd->id.name] = &fd->block;

            }
            else 
            {
                // Throw proper error here
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
    case LIST:
        {
            print("List");
            List* list = dynamic_cast<List*>(node);
            return list;
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
                        break;
                    }
                default:
                    print("Unknown library function");
                    break;
            }
        }
        break;
    default:
        print("Unknown node type: " + node->getNodeType());
        break;
    }
}

int main(int argc, char **argv)
{

    bool file = false;
    const char* sourcePath;

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

    for (auto statement : programBlock->statements)
    {
        // std::cout << "Interpreting statement of type " << statement->getNodeType() << std::endl;
        interpret(statement);
    }

    if (debug)
    {
        print_debug_info();
    }

    return 0;
}