struct variable {

	char* var_name;
	int type;
	union {
		int i_val;
		int il_val[100];
	} value;
	struct variable *next;

};

typedef struct variable variable;

// Currently everything is global scope


variable *add_variable(char* name, int type);
variable *get_variable(char* name);

int integer_init(char* var_name, int value);
int list_init(char* var_name, int value[]);