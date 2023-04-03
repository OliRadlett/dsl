#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <typeinfo>
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


int interpret(Statement* statement)
{
    switch (statement->getNodeType())
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
        std::cout << "Block" << std::endl;
        break;
    case INTEGER:
        std::cout << "Integer" << std::endl;
        break;
    case IDENTIFIER:
        std::cout << "Identifier" << std::endl;
        break;
    case VARIABLEDEFINITION:
        std::cout << "Variable definition" << std::endl;
        break;
    case FUNCTIONDEFINITION:
        std::cout << "Function definition" << std::endl;
        FunctionDefinition* fd = dynamic_cast<FunctionDefinition*>(statement);
        Block* block = &fd->block;
        for (auto stmt : block->statements) {
            interpret(stmt);
        }
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

    std::cout << "Root node address: " << programBlock <<  std::endl;

    for (auto statement : programBlock->statements)
    {
        std::cout << "Interpreting statement of type " << statement->getNodeType() << std::endl;
        interpret(statement);
    }

    // print_var_table();

    return 0;
}