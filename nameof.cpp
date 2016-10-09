///Used to obtain the string name of a variable.

#define nameof(name) template_nameof((name), #name)

template <typename T>
const char* template_nameof(const T& validate_type, const char* name)
{
    return name;
}

// int test = 0;
// std::cout << nameof(test) << std::endl;
// prints "test”