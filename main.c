#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <typeinfo>
#include "main.h"
#include "ast.h"

extern Block *programBlock;
extern int yyparse();


extern "C" FILE *yyin;

typedef std::unordered_map<std::string, Node*> SymbolTable;
std::vector<SymbolTable> VariableTable;
SymbolTable FunctionTable;

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
                   std::cout << "Evalutating " << lhs_->value << " == " << rhs_->value << std::endl;
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
                   std::cout << "Evalutating " << lhs_->value << " != " << rhs_->value << std::endl;
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


Node* interpret(Node* node)
{
    switch (node->getNodeType())
    {
    case NODE:
        std::cout << "Node" << std::endl;
        break;
    case EXPRESSION:
        std::cout << "Expression" << std::endl;
        break;
    case STATEMENT:
        std::cout << "Statement" << std::endl;
        break;
    case BLOCK:
        {
            std::cout << "Block" << std::endl;
            Block* block = dynamic_cast<Block*>(node);
            // Create a new symbol table for this block's scope
            SymbolTable blockScope;
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
            std::cout << "Integer" << std::endl;
            Integer* integer = dynamic_cast<Integer*>(node);
            return integer;
        }
        break;
    case IDENTIFIER:
        {
            // Here we want to lookup the name in the function and variable tables and if its present we return either a value or a function pointer
            std::cout << "Identifier" << std::endl;
            Identifier* identifier = dynamic_cast<Identifier*>(node);

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
                    std::cout << "Variable " << identifier->name << " is accessible from current scope" << std::endl;
                    return symbolTable[identifier->name];
                }
            }

            // return nullptr;

            throw std::runtime_error(identifier->name + " doesn't exist in current scope");
        }
        break;
    case VARIABLEDEFINITION:
        {
            std::cout << "Variable definition" << std::endl;
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
                        std::cout << "Variable defined. Identifier:" << variableDefinition->id.name  << " Assignment Expression: " << value->value << std::endl;
                    }
                    else
                    {
                        std::cout << "Something shite happened: " << assignmentExpr << std::endl;
                    }
                }
                else
                {
                    std::cout << "Identifier: " << variableDefinition->id.name << " (no assignment expression)" << std::endl;
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
            std::cout << "ASSIGNMENT" << std::endl;
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
                    std::cout << "Updating " << lhs->name << std::endl;
                    entryIter->second = assignment->rhs;
                    successful = true;
                    break;
                }
            }

            if (!successful)
                throw std::runtime_error("Undefined variable: " + lhs->name);

            // std::stack<SymbolTable> stackCopy = VariableTable;
            // int depth = 0;

            // while (!stackCopy.empty())
            // {
            //     if (!identifier_available(lhs->name, stackCopy.top()))
            //     {
            //         // Are we type checking or not?
            //         std::cout << lhs->name << " is available for assignment at scope depth " << stackCopy.size() << std::endl;
            //         updateVariable(lhs->name, assignment->rhs, depth);

            //         // Do I like this?
            //         // currentScope[lhs->name] = assignment->rhs;
            //         // if (Integer* rhs = dynamic_cast<Integer*>(assignment->rhs))
            //         // {
            //         //     currentScope[lhs->name] = rhs;
            //         // }
            //     }
            //     depth++;
            //     stackCopy.pop();
            // }
        }
        break;
    case FUNCTIONCALL:
        {
            std::cout << "Function call" << std::endl;
            FunctionCall* call = dynamic_cast<FunctionCall*>(node);
            std::cout << "Identifier: " << call->id.name << std::endl;

            // if (FunctionTable.find(call->id.name) != functionTable.end())
            // {

            // }

            Node* function = FunctionTable[call->id.name];
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
        std::cout << "Expression statement" << std::endl;
        ExpressionStatement* e = dynamic_cast<ExpressionStatement*>(node);
        return interpret(&e->expression);
        }
        break;
    case FUNCTIONDEFINITION:
        {
            std::cout << "Function definition" << std::endl;
            FunctionDefinition* fd = dynamic_cast<FunctionDefinition*>(node);
            if (identifier_available(fd->id.name))
            {
                FunctionTable[fd->id.name] = &fd->block;
                std::cout << "Identifier: " << fd->id.name << std::endl;
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
            std::cout << "If statement" << std::endl;
            IfStatement* if_statement = dynamic_cast<IfStatement*>(node);
            Condition* condition = dynamic_cast<Condition*>(if_statement->condition);

            if (evaluate(interpret(&condition->lhs), interpret(&condition->rhs), condition->op))
            {
                Node* block = dynamic_cast<Node*>(&if_statement->block);
                interpret(block);
            }
        }
        break;
    default:
        std::cout << "Unknown node type: " << node->getNodeType() << std::endl;
        break;
    }
}

int main(int argc, char **argv)
{
    if(argc > 1){
      yyin = fopen(argv[1], "r");
      printf("Parsing from file\n");
    }
    else
    {
        
      yyin = stdin;
      printf("Parsing from stdin\n");
    }

    yyparse();

    // Create global scope
    SymbolTable globalScope;
    VariableTable.push_back(globalScope);

    std::cout << "Root node address: " << programBlock <<  std::endl;

    for (auto statement : programBlock->statements)
    {
        // std::cout << "Interpreting statement of type " << statement->getNodeType() << std::endl;
        interpret(statement);
    }

    print_debug_info();

    return 0;
}