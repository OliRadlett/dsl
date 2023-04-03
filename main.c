#include <string>
#include <iostream>
#include <unordered_map>
#include <stack>
#include "main.h"
#include "ast.h"

extern Block *programBlock;
extern int yyparse();
extern variable *variable_table;

// enum datatypes {
//     i_val,
//     il_val,
// };


extern "C" FILE *yyin;

typedef std::unordered_map<std::string, Node*> VariableTable;
std::stack<VariableTable> VariableTableStack;


// variable *variable_table = (variable *)0;

// // Memory may get funky here
// int integer_init(char* var_name, int value)
// {

//     variable *pointer = add_variable(var_name, i_val);
//     pointer->value.i_val = value;

//     return 0;

// }

// int list_init(char* var_name, int value[])
// {

//     variable *pointer = add_variable(var_name, il_val);
//     memcpy(pointer->value.il_val, value, sizeof pointer->value.il_val);

//     return 0;

// }

// variable * add_variable(char* name, int type)
// {
//     variable *pointer;
//     pointer = (variable *) malloc(sizeof(variable));
//     pointer->var_name = (char *) malloc(strlen(name) + 1);
//     strcpy(pointer->var_name, name);
//     pointer->type = type;
//     pointer->value.i_val = 0;
//     pointer->next = (struct variable *) variable_table;
//     variable_table = pointer;
//     return pointer;
// }

// variable * get_variable(char* name)
// {
//     variable *pointer;
//     for (pointer = variable_table; pointer != (variable *) 0; pointer = (variable *)pointer->next)
//         if (strcmp (pointer->var_name, name) == 0)
//           return pointer;
//   return 0;
// }

// void print_var_table()
// {
//     printf("[Variable table]\n");
//     variable *pointer;
//     for (pointer = variable_table; pointer != (variable *) 0; pointer = (variable *)pointer->next)
//     {
//         switch (pointer->type)
//         {
//             case i_val:
//                 printf("%s<integer> %d\n", pointer->var_name, pointer->value.i_val);
//                 break;

//             case il_val:
//                 printf("%s<list> [", pointer->var_name);
//                 for (int i = 0; i < sizeof pointer->value.il_val / sizeof pointer->value.il_val[0]; i++)
//                 {
//                     // At the moment lists only contain integers, this will need changing in future
//                     if (i == sizeof pointer->value.il_val / sizeof pointer->value.il_val[0] - 1)
//                     {
//                         printf("%d", pointer->value.il_val[i]);
//                     }
//                     else
//                     {
//                         printf("%d, ", pointer->value.il_val[i]);
//                     }
//                 }
//                 printf("]\n");
//                 break;

//             default:
//                 break;

//         }        
//     }
// }


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
            VariableTable blockScope;
            // Push the symbol table onto the stack
            VariableTableStack.push(blockScope);
            // Interpret each statement in the block
            for (auto stmt : block->statements) {
                interpret(stmt);
            }
            // Pop the symbol table off the stack when we leave the block
            VariableTableStack.pop();
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
            
            VariableTable& currentScope = VariableTableStack.top();
            auto iter = currentScope.find(identifier->name);
            if (iter == currentScope.end())
            {
                // If the identifier isn't found, throw an error
                throw std::runtime_error("Undefined variable: " + identifier->name);
            }
            // At some point we return the value here
            std::cout << "Variable " << identifier->name << " exists in current scope" << std::endl;
            return identifier;
        }
        break;
    case VARIABLEDEFINITION:
        {
            std::cout << "Variable definition" << std::endl;
            VariableDefinition* variableDefinition = dynamic_cast<VariableDefinition*>(node);

            Node* assignmentExpr = nullptr;
            if (variableDefinition->assignmentExpression != nullptr)
            {
                assignmentExpr = interpret(variableDefinition->assignmentExpression);
                // Functions will need lots of work here
                // All other datatypes will need adding
                if (dynamic_cast<Integer*>(assignmentExpr))
                {
                    Integer* value = dynamic_cast<Integer*>(assignmentExpr);
                    std::cout << "Identifier:" << variableDefinition->id.name  << " Assignment Expression: " << value->value << std::endl;

                }

            }
            else
            {
                std::cout << "Identifier: " << variableDefinition->id.name << " (no assignment expression)" << std::endl;
            }
            // Add the variable to the current scope's symbol table
            VariableTable& currentScope = VariableTableStack.top();
            currentScope[variableDefinition->id.name] = assignmentExpr;
        }
        break;
    case FUNCTIONCALL:
        {
            std::cout << "Function call" << std::endl;
            FunctionCall* call = dynamic_cast<FunctionCall*>(node);
            std::cout << "Identifier: " << call->id.name << std::endl;
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
            std::cout << "Identifier: " << fd->id.name << std::endl;
            Block* block = &fd->block;
            for (auto stmt : block->statements) {
                interpret(stmt);
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
    VariableTable globalScope;
    VariableTableStack.push(globalScope);

    std::cout << "Root node address: " << programBlock <<  std::endl;

    for (auto statement : programBlock->statements)
    {
        // std::cout << "Interpreting statement of type " << statement->getNodeType() << std::endl;
        interpret(statement);
    }

    // print_var_table();

    return 0;
}