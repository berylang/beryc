#pragma once
#include <string>
#include <unordered_map>
#include "diagnostic.h"

struct DiagnosticInfo {
    Severity severity;
    std::string messageTemplate;
    std::string hint;
};

static const std::unordered_map<std::string, DiagnosticInfo> DiagnosticRegistry = {

    // adding instructions - 
    // we have to add like this -
    //
    //
    //      {errorcode, {severity, msg, hint} }
    // 
    // hint should be basic, and targeted only for that specific error/warning.
    //
    //


    // @LEXER ERRORS and WARNINGS
    { "ERROR100", { Severity::ERROR, "Empty char literal", "put a character between the quotes" } },
    { "ERROR101", { Severity::ERROR, "Newline in char literal", "char literals cannot span multiple lines" } },
    { "ERROR102", { Severity::ERROR, "Incomplete escape sequence", "finish the escape sequence, e.g. '\\\\n'" } },
    { "ERROR103", { Severity::ERROR, "Invalid escape sequence '\\{}'", "use one of: \\n \\t \\r \\\\ \\0 \\\" \\'" } },
    { "ERROR104", { Severity::ERROR, "Multi-character char literal", "char literals must hold exactly one character" } },
    { "ERROR105", { Severity::ERROR, "Unclosed char literal", "add a closing '" } },
    { "ERROR106", { Severity::ERROR, "Invalid escape sequence '\\{}' in string", "use one of: \\n \\t \\r \\\\ \\0 \\\" \\'" } },
    { "ERROR107", { Severity::ERROR, "Unclosed string literal", "add a closing \"" } },
    { "ERROR108", { Severity::ERROR, "Unclosed comment", "add '!--' to close the comment block" } },
    { "ERROR110", { Severity::ERROR, "Hex literal '{}' has no digits after '0x'", "add at least one hex digit (0-9, a-f)" } },
    { "ERROR111", { Severity::ERROR, "Binary literal '{}' has no digits after '0b'", "add at least one binary digit (0 or 1)" } },
    { "ERROR112", { Severity::ERROR, "Octal literal '{}' has no digits after '0o'", "add at least one octal digit (0-7)" } },
    { "ERROR113", { Severity::ERROR, "Invalid digit '{}' in hex literal", "hex digits must be 0-9, a-f, or A-F" } },
    { "ERROR114", { Severity::ERROR, "Invalid digit '{}' in binary literal", "binary literals only allow 0 and 1" } },
    { "ERROR115", { Severity::ERROR, "Invalid digit '{}' in octal literal", "octal digits must be 0-7" } },
    { "ERROR116", { Severity::ERROR, "Malformed exponent '{}' in numeric literal", "add digits after 'e', e.g. '1e10' or '1e-3'" } },
    { "ERROR117", { Severity::ERROR, "Integer literal '{}' is out of range", "use a smaller value or a wider type" } },


    // @PARSER ERRORS and WARNINGS
    { "ERROR201", { Severity::ERROR, "Expected ';' after {}", "put ';' symbol right after {}" } },
    { "ERROR202", { Severity::ERROR, "Expected '}' after {}", "add '}' to close {}" } },
    { "ERROR203", { Severity::ERROR, "Expected 'case' or 'default'", "add a 'case' or 'default' label" } },
    { "ERROR204", { Severity::ERROR, "Expected ')' after {}", "add ')' after {}" } },
    { "ERROR205", { Severity::ERROR, "Expected '{' before {}", "add '{' before {}" } },
    { "ERROR206", { Severity::ERROR, "Expected '(' after {}", "add '(' after {}" } },
    { "ERROR207", { Severity::ERROR, "Expected '{' before switch body", "add '{' before the switch body" } },
    { "ERROR208", { Severity::ERROR, "Expected ':' after case", "add ':' after the case value" } },
    { "ERROR209", { Severity::ERROR, "Expected ':'", "add ':' here" } },
    { "ERROR210", { Severity::ERROR, "Expected '}' after switch body", "add '}' to close the switch body" } },
    { "ERROR211", { Severity::ERROR, "Invalid assignment target", "assign the value to a valid variable or target" } },
    { "ERROR212", { Severity::ERROR, "Expected ':' in ternary operator", "add ':' between the two expressions" } },
    { "ERROR213", { Severity::ERROR, "Expected ',' after lower bound", "add ',' after the lower bound" } },

    { "ERROR214", { Severity::ERROR, "Expected class name after 'new'", "add a class name after 'new'" } },
    { "ERROR215", { Severity::ERROR, "Expected '(' after class name", "add '(' after the class name" } },
    { "ERROR216", { Severity::ERROR, "Expected ')' after constructor arguments", "add ')' after the constructor arguments" } },
    { "ERROR217", { Severity::ERROR, "Expected identifier after '.'", "add an identifier after '.'" } },
    { "ERROR218", { Severity::ERROR, "Expected ']' after array index", "add ']' after the array index" } },

    //functions
    { "ERROR219", { Severity::ERROR, "Expected function name", "add a function name" } },
    { "ERROR220", { Severity::ERROR, "Expected '(' after function name", "add '(' after the function name" } },
    { "ERROR221", { Severity::ERROR, "Expected parameter name", "add a parameter name" } },
    { "ERROR222", { Severity::ERROR, "Expected ']' after '[' in array parameter type", "add ']' to close the array parameter type" } },
    { "ERROR223", { Severity::ERROR, "Expected ')' after parameters", "add ')' after the parameters" } },
    { "ERROR224", { Severity::ERROR, "Expected ']' after '[' in array return type", "add ']' to close the array return type" } },
    { "ERROR225", { Severity::ERROR, "Expected '{' before function body", "add '{' before the function body" } },
    { "ERROR226", { Severity::ERROR, "Expected '(' in function call", "add '(' after the function name" } },
    { "ERROR227", { Severity::ERROR, "Expected ')' after arguments", "add ')' after the arguments" } },
    { "ERROR228", { Severity::ERROR, "Expected ';' after return statement", "put ';' after the return statement" } },

    //loops
    { "ERROR229", { Severity::ERROR, "Expected '(' after 'while'", "add '(' after 'while'" } },
    { "ERROR230", { Severity::ERROR, "Expected '{' before while-body", "add '{' before the while body" } },
    { "ERROR231", { Severity::ERROR, "Expected '{' before do-while-body", "add '{' before the do-while body" } },
    { "ERROR232", { Severity::ERROR, "Expected 'while' condition after do-while-body", "add the while condition after the do-while body" } },
    { "ERROR233", { Severity::ERROR, "Expected ';' after condition-end", "put ';' after the condition" } },
    { "ERROR234", { Severity::ERROR, "Expected '(' after 'for'", "add '(' after 'for'" } },
    { "ERROR235", { Severity::ERROR, "Expected identifier", "add an identifier" } },
    { "ERROR236", { Severity::ERROR, "Expected ')' after for-in declaration", "add ')' after the for-in declaration" } },
    { "ERROR237", { Severity::ERROR, "Expected '{' before loop body", "add '{' before the loop body" } },
    { "ERROR238", { Severity::ERROR, "Expected ';' after loop condition", "put ';' after the loop condition" } },
    { "ERROR239", { Severity::ERROR, "Expected ')' after loop update", "add ')' after the loop update" } },
        
    //OOP
    { "ERROR240", { Severity::ERROR, "Destructor name '~{}' does not match class '{}'", "use the class name as the destructor name" } },
    { "ERROR241", { Severity::ERROR, "Expected class name", "add a class name" } },
    { "ERROR242", { Severity::ERROR, "Expected '{' after class name", "add '{' after the class name" } },
    { "ERROR243", { Severity::ERROR, "Expected '}' after class body", "add '}' to close the class body" } },
    { "ERROR244", { Severity::ERROR, "Expected 'attributes' section", "add the 'attributes' section" } },
    { "ERROR245", { Severity::ERROR, "Expected '[' after 'attributes'", "add '[' after 'attributes'" } },
    { "ERROR246", { Severity::ERROR, "Expected self-reference identifier", "add a self-reference identifier" } },
    { "ERROR247", { Severity::ERROR, "Expected ']' after self-reference", "add ']' after the self-reference" } },
    { "ERROR248", { Severity::ERROR, "Expected '::' after attributes section", "add '::' after the attributes section" } },
    { "ERROR249", { Severity::ERROR, "Expected 'methods' section", "add the 'methods' section" } },
    { "ERROR250", { Severity::ERROR, "Expected '::' after 'methods'", "add '::' after 'methods'" } },
    { "ERROR251", { Severity::ERROR, "Expected class name after '~'", "add the class name after '~'" } },
    { "ERROR252", { Severity::ERROR, "Expected '(' after destructor name", "add '(' after the destructor name" } },
    { "ERROR253", { Severity::ERROR, "Expected '{' before destructor body", "add '{' before the destructor body" } },
    { "ERROR254", { Severity::ERROR, "Expected '(' after constructor name", "add '(' after the constructor name" } },
    { "ERROR255", { Severity::ERROR, "Expected '{' before constructor body", "add '{' before the constructor body" } },

    //expressions/declarations
    { "ERROR256", { Severity::ERROR, "Expected valid expression or literal", "provide a valid expression or literal" } },
    { "ERROR257", { Severity::ERROR, "Arrays must be initialized with list inside '{}'", "initialize the array using a list inside '{}'" } },
    { "ERROR258", { Severity::ERROR, "Expected ']'", "add ']'" } },
    { "ERROR259", { Severity::ERROR, "Expected ';'", "add ';'" } },
    { "ERROR260", { Severity::ERROR, "Expected type", "add a valid type" } },
    { "ERROR261", { Severity::ERROR, "Expected '{'", "add '{'" } },
    { "ERROR262", { Severity::ERROR, "Expected '}'", "add '}'" } },

    //enum
    { "ERROR263", { Severity::ERROR, "Expected enum name", "add an enum name" } },
    { "ERROR264", { Severity::ERROR, "Expected '=' after enum name", "add '=' after the enum name" } },
    { "ERROR265", { Severity::ERROR, "Expected '{' to start enum values", "add '{' before enum values" } },
    { "ERROR266", { Severity::ERROR, "Expected enum value", "add an enum value" } },
    { "ERROR267", { Severity::ERROR, "Expected '}' after enum values", "add '}' after enum values" } },
    { "ERROR268", { Severity::ERROR, "Expected ';' after enum declaration", "put ';' after the enum declaration" } },

    //imports
    { "ERROR269", { Severity::ERROR, "Expected module name after 'import'", "add a module name after 'import'" } },
    { "ERROR270", { Severity::ERROR, "Expected 'func' after 'extern'", "add 'func' after 'extern'" } },
    { "ERROR271", { Severity::ERROR, "Expected function name after 'func'", "add a function name after 'func'" } },
    { "ERROR272", { Severity::ERROR, "Expected ')' after extern parameters", "add ')' after the extern parameters" } },
    { "ERROR273", { Severity::ERROR, "Expected ';' after extern declaration", "put ';' after the extern declaration" } },

    { "ERROR274", { Severity::ERROR, "Unexpected token", "remove or replace the unexpected token" } },
    { "ERROR275", { Severity::ERROR, "no run{} block found", "add a run{} block to the program" } },
    { "ERROR276", { Severity::ERROR, "Expected '{' after run", "add '{' after run" } },
    { "ERROR277", { Severity::ERROR, "Expected '}' after run block", "add '}' to close the run block" } },
    { "ERROR278", { Severity::ERROR, "Expected '}' after block", "add '}' to close the block" } },


    // @SEMA ERRORS and WARNINGS
    { "ERROR003", { Severity::ERROR, "Undefined variable '{}'", "declare '{}' before using it" } },
    { "ERROR004", { Severity::ERROR, "'{}' already declared in this scope", "rename this or remove the earlier declaration" } },
    { "ERROR005", { Severity::ERROR, "'{}' used outside of a loop or switch", "use {} only inside a loop/switch body" } },
    { "ERROR305", { Severity::ERROR, "function '{}' is already defined with same parameters.", "try using different parameters, or use different function name"}},
    { "ERROR306", { Severity::ERROR, "if condition must evaluate to 'bool'", "make sure the condition expression returns bool" } },
    { "ERROR307", { Severity::ERROR, "Invalid switch condition type '{}'. Expected int, bigint, or char.", "use an int, bigint, or char as the switch condition" } },
    { "ERROR308", { Severity::ERROR, "Case type '{}' does not match switch condition type '{}'", "match the case value's type to the switch condition's type" } },
    { "ERROR309", { Severity::ERROR, "'break' used outside of a loop or switch.", "only use 'break' inside a loop or switch body" } },
    { "ERROR310", { Severity::ERROR, "'continue' used outside of a loop.", "only use 'continue' inside a loop body" } },
    { "ERROR311", { Severity::ERROR, "'while' condition must evaluate to 'bool'", "make sure the condition expression returns bool" } },
    { "ERROR312", { Severity::ERROR, "Loop condition must evaluate to 'bool'", "make sure the loop condition returns bool" } },
    { "ERROR313", { Severity::ERROR, "Type '{}' is not iterable", "use an array or other iterable type in the for-in loop" } },
    { "ERROR314", { Severity::ERROR, "Type mismatched in for-in loop. Variable '{}' declared as '{}' but iterable has element type '{}'", "declare the loop variable with the iterable's element type" } },
    { "ERROR315", { Severity::ERROR, "Unknown type '{}'", "check the type name for typos or missing declarations" } },
    { "ERROR316", { Severity::ERROR, "'{}' already declared in this scope.", "rename this or remove the earlier declaration" } },
    { "ERROR317", { Severity::ERROR, "constant '{}' must be initialized.", "give the constant an initial value at declaration" } },
    { "ERROR318", { Severity::ERROR, "Cannot assign 'null' to non-reference type '{}'", "only reference types can be assigned 'null'" } },
    { "ERROR319", { Severity::ERROR, "Type mismatch for {}. Expected '{}', got '{}'", "make the expression's type match the declared type" } },
    { "ERROR320", { Severity::ERROR, "Unknown array element type '{}'", "check the array element type for typos" } },
    { "ERROR321", { Severity::ERROR, "'{}' already declared.", "rename this or remove the earlier declaration" } },
    { "ERROR322", { Severity::ERROR, "Type mismatch for '{}'. Expected '{}', got '{}'", "make the expression's type match the declared type" } },
    { "ERROR323", { Severity::ERROR, "Type mismatch in array initialization.", "make sure each initializer value matches the array's element type" } },
    { "ERROR324", { Severity::ERROR, "Only the first dimension can be omitted.", "specify sizes for all dimensions except the first" } },
    { "ERROR325", { Severity::ERROR, "Dimensions must be greater than 0.", "use a positive size for each array dimension" } },
    { "ERROR326", { Severity::ERROR, "Must have an initializer list if dimension is omitted.", "provide an initializer list so the size can be inferred" } },
    { "ERROR327", { Severity::ERROR, "Initializer list size does not match multi-dimensional bounds.", "match the initializer list shape to the declared dimensions" } },
    { "ERROR328", { Severity::ERROR, "initializer list count exceeds array size.", "reduce the initializer list or increase the array size" } },
    { "ERROR329", { Severity::ERROR, "Unknown parameter type '{}' in function '{}'", "check the parameter type for typos" } },
    { "ERROR330", { Severity::ERROR, "Unknown return type '{}' in function '{}'", "check the return type for typos" } },
    { "ERROR331", { Severity::ERROR, "'return' used outside of a function.", "only use 'return' inside a function body" } },
    { "ERROR332", { Severity::ERROR, "Expected return value of type '{}'", "return a value matching the function's declared return type" } },
    { "ERROR333", { Severity::ERROR, "Type mismatch in return. Expected '{}', got '{}'", "return a value matching the function's declared return type" } },
    { "ERROR334", { Severity::ERROR, "Enum value '{}' already declared.", "rename this enum value or remove the duplicate" } },
    { "ERROR335", { Severity::ERROR, "Class '{}' cannot inherit from itself", "remove the self-referential inheritance" } },
    { "ERROR336", { Severity::ERROR, "Unknown parent class '{}' for class '{}'", "check the parent class name for typos" } },
    { "ERROR337", { Severity::ERROR, "Circular inheritance detected involving class '{}'", "break the inheritance cycle between these classes" } },
    { "ERROR338", { Severity::ERROR, "Cannot assign 'null' to non-reference field '{}'", "only reference-type fields can be assigned 'null'" } },
    { "ERROR339", { Severity::ERROR, "Type mismatch for field '{}'. Expected '{}', got '{}'", "make the assigned value's type match the field's declared type" } },
    { "ERROR340", { Severity::ERROR, "Duplicate field '{}' in class '{}'", "rename this field or remove the duplicate" } },
    { "ERROR341", { Severity::ERROR, "Class '{}' already has a destructor, only one destructor is supported", "remove the extra destructor" } },
    { "ERROR342", { Severity::ERROR, "Destructor '~{}' cannot take parameters (it is invoked automatically)", "remove the parameters from the destructor" } },
    { "ERROR343", { Severity::ERROR, "Unknown parameter type '{}' in method '{}'", "check the parameter type for typos" } },
    { "ERROR344", { Severity::ERROR, "Unknown return type '{}' in method '{}'", "check the return type for typos" } },
    { "ERROR345", { Severity::ERROR, "Method '{}' overrides parent method with diff return type.", "match the overriding method's return type to the parent's" } },
    { "ERROR346", { Severity::ERROR, "Function '{}' is already defined with same parameter types.", "use different parameters, or a different function name" } },
    { "ERROR347", { Severity::ERROR, "internal compiler error: symbol '{}' not found in symbol table", "this indicates a compiler bug, please report it" } },
    { "ERROR348", { Severity::ERROR, "Invalid operand for string concatenation", "only strings (or compatible types) can be concatenated" } },
    { "ERROR349", { Severity::ERROR, "Logical operator '{}' cannot be used on type '{}' and '{}'", "use logical operators only with compatible boolean operands" } },
    { "ERROR350", { Severity::ERROR, "Type mismatch in binary expression '{}' and '{}'", "make both operands of the same or compatible type" } },
    { "ERROR351", { Severity::ERROR, "Relational operator '{}' cannot be used on type '{}' and '{}'", "use relational operators only with compatible operand types" } },
    { "ERROR352", { Severity::ERROR, "Right operand of shift must be an integer type", "use an integer type for the right shift operand" } },
    { "ERROR353", { Severity::ERROR, "Left operand of shift must be an integer type", "use an integer type for the left shift operand" } },
    { "ERROR354", { Severity::ERROR, "Bitwise operators require integer operands", "use integer types with bitwise operators" } },
    { "ERROR355", { Severity::ERROR, "Ternary condition must be 'bool', got '{}'", "make the ternary condition evaluate to bool" } },
    { "ERROR356", { Severity::ERROR, "Ternary branch type mismatch ('{}' vs '{}')", "make both ternary branches return the same type" } },
    { "ERROR357", { Severity::ERROR, "delete target is not an object", "only object instances can be deleted" } },
    { "ERROR358", { Severity::ERROR, "delete target member does not exist", "check the member name being deleted" } },
    { "ERROR359", { Severity::ERROR, "Cannot delete const object field", "remove 'const' from the field, or don't delete it" } },
    { "ERROR360", { Severity::ERROR, "Undefined object variable", "declare the variable before deleting it" } },
    { "ERROR361", { Severity::ERROR, "Cannot delete const object variable", "remove 'const' from the variable, or don't delete it" } },
    { "ERROR362", { Severity::ERROR, "delete requires an object target", "only object instances can be deleted" } },
    { "ERROR363", { Severity::ERROR, "delete supports objects only (arrays not allowed)", "delete individual elements instead of the whole array" } },
    { "ERROR364", { Severity::ERROR, "Identifier required as operand of increment or decrement operator", "use a variable identifier with '++' or '--'" } },
    { "ERROR365", { Severity::ERROR, "Between operator supports only int, bigint, float, double and char", "use a numeric or char type with the 'between' operator" } },
    { "ERROR369", { Severity::ERROR, "Class '{}' has no method '{}'", "check the method name for typos" } },
    { "ERROR370", { Severity::ERROR, "Unknown method '{}' on type '{}'", "check the method name and target type" } },
    { "ERROR371", { Severity::ERROR, "Undefined function '{}'", "declare the function before calling it" } },
    { "ERROR372", { Severity::ERROR, "Undefined array '{}'", "declare the array before using it" } },
    { "ERROR373", { Severity::ERROR, "String index must be an integer", "use an integer expression to index a string" } },
    { "ERROR374", { Severity::ERROR, "Variable '{}' is not subscriptable", "only arrays and strings can be indexed" } },
    { "ERROR375", { Severity::ERROR, "Too many indices for array '{}'", "match the number of indices to the array's dimensions" } },
    { "ERROR376", { Severity::ERROR, "Cannot use compound assignment '{}' on type '{}'", "use a compatible type with this compound assignment operator" } },
    { "ERROR377", { Severity::ERROR, "'{}' is not an object", "only object instances support member access" } },
    { "ERROR378", { Severity::ERROR, "Class '{}' has no member '{}'", "check the member name for typos" } },
    { "ERROR379", { Severity::ERROR, "cannot reassign constant variable '{}'", "constants cannot be reassigned after initialization" } },
    { "ERROR380", { Severity::ERROR, "Invalid assignment target", "assign only to variables, fields, or array elements" } },
    { "ERROR381", { Severity::ERROR, "Type mismatch for assignment to '{}'. Expected '{}', got '{}'", "make the assigned value's type match the target's type" } },
    { "ERROR382", { Severity::ERROR, "Invalid cast from '{}' to '{}'.", "only cast between compatible types" } },
    { "ERROR383", { Severity::ERROR, "Unknown literal type", "this indicates a compiler bug, please report it" } },
    { "ERROR384", { Severity::ERROR, "Unknown class '{}'", "check the class name for typos" } },
    { "ERROR385", { Severity::ERROR, "Class '{}' has no constructor accepting {} argument(s)", "check the number of arguments passed to 'new'" } },
    { "ERROR386", { Severity::ERROR, "'super' used outside of a subclass method", "only use 'super' inside a method of a class with a parent" } },
    { "ERROR387", { Severity::ERROR, "'super(...)' can only be called inside a constructor", "move the 'super(...)' call into the constructor body" } },
    { "ERROR388", { Severity::ERROR, "No constructor found in parent chain of '{}' for 'super(...)'", "add a matching constructor to a parent class" } },
    { "ERROR389", { Severity::ERROR, "No method '{}' found in parent chain of '{}'", "check the method name for typos" } },
    { "ERROR390", { Severity::ERROR, "Cannot access private method '{}' via 'super'", "only public/protected methods can be accessed via 'super'" } },
    { "ERROR391", { Severity::ERROR, "'ref' can only be used on a variable, field, or array element", "use 'ref' only on addressable expressions" } },
    { "ERROR392", { Severity::ERROR, "Ambiguous call to method '{}' with {} arguments", "disambiguate the call, e.g. by adjusting argument types" } },
    { "ERROR393", { Severity::ERROR, "No overloaded method '{}' accepts the {} arguments.", "check the argument count and types against the overloads" } },
    { "ERROR394", { Severity::ERROR, "No matching overload of method '{}' for this given argument types", "check the argument types against the available overloads" } },
    { "ERROR395", { Severity::ERROR, "Ambiguous call to function '{}' with {} arguments", "disambiguate the call, e.g. by adjusting argument types" } },
    { "ERROR396", { Severity::ERROR, "No overloaded function '{}' accepts the {} arguments.", "check the argument count and types against the overloads" } },
    { "ERROR397", { Severity::ERROR, "No matching overload of function '{}' for this given argument types", "check the argument types against the available overloads" } },
    { "ERROR398", { Severity::ERROR, "Cannot access {} {} '{}' of class '{}' from outside the class", "access this member only from within the class" } },
    { "ERROR399", { Severity::ERROR, "Undefined variable '{}'", "declare '{}' before using it" } },
    { "ERROR400", { Severity::ERROR, "'{}' is not an object, cannot access '.{}'", "only object instances support member access" } },
    { "ERROR401", { Severity::ERROR, "class '{}' is already defined.", "use a different class name, or remove the duplicate definition" } },

    // WARNINGS of Semantic Analyzer
    { "WARNING301", { Severity::WARNING, "Unreachable code: this statement never executes after '{}'.", "remove the dead code, or move it before the loop/function exits" } },


    // Importer errors 
    { "ERROR501", { Severity::ERROR, "Cannot find imported module named '{}'", "write {}.bry module before importing it" } },
    { "ERROR502", { Severity::ERROR, "Compilation halted due to syntax errors in imported module '{}'", "check for syntax errors in {} module" } },
};