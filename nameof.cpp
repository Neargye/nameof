///Used to obtain the string name of a variable.
#define nameof_variable(name) template_nameof_variable(name, #name)
template <typename T> const char* template_nameof_variable(const T& validate_type, const char* name) { return name; }

///Used to obtain the string name of a type.
#define nameof_type(name) template_nameof_type<name>(#name)
template <typename T> const char* template_nameof_type(const char* name) { return name; }

///Used to obtain the string name of a function.
#define nameof_function(name) template_nameof_function(#name, sizeof(#name)/sizeof(char) - 1); (name);
const char* template_nameof_function(const char* name) { return name; }

//example
int test = 0;
std::cout << nameof_variable(test) << " " << nameof_type(int) << std::endl;
// prints "test int”
std::string test_wrong;
std::cout << nameof_variable(static_cast<std::string>(test_wrong.c_str())) << " " << nameof_type(std::string) << std::endl;
// prints "static_cast<std::string>(test_wrong.c_str()) std::string"