//MIT License
//
//Copyright(c) 2016 - 2018 Terik23
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files(the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions :
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.
  
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