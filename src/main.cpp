#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>
#include <limits.h>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <tchar.h>
#include <windows.h>

using namespace std;

typedef void (*func_void_void)();
typedef void (*func_void_const_char_p)(const char* a);
typedef void (*func_void_bool)(bool a);

typedef bool (*func_bool_void)();
typedef bool (*func_bool_const_char_p)(const char* a);
typedef bool (*func_bool_bool)(bool a);

typedef const char* (*func_const_char_p_void)();
typedef const char* (*func_const_char_p_const_char_p)(const char* a);
typedef const char* (*func_const_char_p_bool)(bool a);

vector <string> pre_var;
vector <string> pre_func;

map <string, string> variables;
map <string, string> chached_variables;

map <string, string> functions;
map <string, string> function_arguments;

string get_raw_name(string fullname) {
    return fullname.substr(0, fullname.find_last_of(".")); 
}

string get_name(string path) {
    return path.substr(path.find_last_of("/\\") + 1);
}

map <string, HINSTANCE> loaded_dlls;

void load_dll(string dll_name) {
    loaded_dlls[get_raw_name(get_name(dll_name))] = LoadLibrary((TCHAR*) dll_name.c_str());
}

func_void_void load_func_void_void(string dll_name, string func_name) {
    return (func_void_void) GetProcAddress(loaded_dlls[dll_name], func_name.c_str());
}

func_void_const_char_p load_func_void_const_char_p(string dll_name, string func_name) {
    return (func_void_const_char_p) GetProcAddress(loaded_dlls[dll_name], func_name.c_str());
}

func_void_bool load_func_void_bool(string dll_name, string func_name) {
    return (func_void_bool) GetProcAddress(loaded_dlls[dll_name], func_name.c_str());
}

func_bool_void load_func_bool_void(string dll_name, string func_name) {
    return (func_bool_void) GetProcAddress(loaded_dlls[dll_name], func_name.c_str());
}

func_bool_const_char_p load_func_bool_const_char_p(string dll_name, string func_name) {
    return (func_bool_const_char_p) GetProcAddress(loaded_dlls[dll_name], func_name.c_str());
}

func_bool_bool load_func_bool_bool(string dll_name, string func_name) {
    return (func_bool_bool) GetProcAddress(loaded_dlls[dll_name], func_name.c_str());
}

func_const_char_p_void load_func_const_char_p(string dll_name, string func_name) {
    return (func_const_char_p_void) GetProcAddress(loaded_dlls[dll_name], func_name.c_str());
}

func_const_char_p_const_char_p load_func_const_char_p_const_char_p(string dll_name, string func_name) {
    return (func_const_char_p_const_char_p) GetProcAddress(loaded_dlls[dll_name], func_name.c_str());
}

func_const_char_p_bool load_func_const_char_p_bool(string dll_name, string func_name) {
    return (func_const_char_p_bool) GetProcAddress(loaded_dlls[dll_name], func_name.c_str());
}

string execute(string command) {
    array <char, 128> buffer;
    string result;

    FILE* pipe = _popen(command.c_str(), "r");

    if (!pipe) {
        cout << "\033[31m" << "RuntimeError: Couldn't start command." << "\033[0m" <<  endl;
        return 0;
    }

    while (fgets(buffer.data(), 128, pipe) != NULL) {
        result = result + buffer.data();
    }

    _pclose(pipe);

    return result;
}

string open_file(string filename, string ext) {
    string code = "";
    string codeline = "";
    ifstream codefile(filename);

    if (filename.substr(filename.find_last_of(".") + 1) == ext) {
        while (getline(codefile, codeline)) {
            code = code + codeline + "\n";
        }

        code = code + "<EOF>";
    }

    else {
        cout << "\033[31m" << "Invalid file extension, proccess terminated." << "\033[0m" << endl;
        return "";
    }

    return code;
}

bool is_file_exists(const string& filename) {
    ifstream ifile;
    ifile.open(filename);

    if(ifile) {
        return true;
    }

    return false;
}

bool is_dir_exists(const char *path) {
    struct stat info;

    if(stat(path, &info) != 0) {
        return false;
    }

    else if(info.st_mode & S_IFDIR) {
        return true;
    }

    else {
        return false;
    }
}

string get_directory(string path) {
    size_t found = path.find_last_of("/\\");
    
    return (path.substr(0, found));
}

string lib_reader_code(string str) {
    string code;

    if (is_dir_exists(str.c_str())) {
        code = open_file(str + "\\init.nhpp", "nhpp");
    }

    else {
        if (is_file_exists(str)) {
            code = open_file(str, "nhpp");
        }
        
        else {
            if (is_dir_exists(("C:\\AeNCPP\\lib\\" + str).c_str())) {
                code = open_file("C:\\AeNCPP\\lib\\" + str + "\\init.nhpp", "nhpp");
            }

            else {
                if (is_file_exists("C:\\AeNCPP\\lib\\" + str)) {
                    code = open_file("C:\\AeNCPP\\lib\\" + str, "nhpp");
                }
            }
        }
    }

    return code;
}

string lib_reader_lib_name(string str) {
    string lib_name;

    if (is_dir_exists(str.c_str())) {
        lib_name = get_raw_name(get_name(str));
    }

    else {
        if (is_file_exists(str)) {
            lib_name = get_raw_name(get_name(str));
        }
        
        else {
            if (is_dir_exists(("C:\\AeNCPP\\lib\\" + str).c_str())) {
                lib_name = get_raw_name(get_name(str));
            }

            else {
                if (is_file_exists("C:\\AeNCPP\\lib\\" + str)) {
                    lib_name = get_raw_name(get_name(str));
                }
            }
        }
    }

    return lib_name;
}

vector <string> expression_lexer(string expression) {
    vector <string> tokens;
    string token = "";
    string var_name = "";
    string str = "";
    string condition;
    bool variable_state = false;
    bool string_state = false;
    bool condition_state = false;

    expression = expression + "<EOF>";

    for(char& chr : expression) {
        token = token + chr;

        if (token == " ") {
            if (string_state == true || condition_state == true) {
                str = str + " ";
            }
        }

        if (token == " " || token == "<EOF>") {
            if (var_name != "") {
                tokens.push_back(var_name);
                variable_state = false;
                var_name = "";
            }

            token = "";
        }

        else if (token == ")") {
            tokens.push_back("<CEX>");
            tokens.push_back(condition);
            condition_state = false;
            condition = "";
            token = "";
        }

        else if (condition_state == true) {
            condition = condition + token;
            token = "";
        }

        else if (token == "(") {
            condition_state = true;
            token = "";
        }

        else if (token == "\"" || token == " \"" || token == "\\\"") {
            if (token == " \"") {
                str = str + " ";
            }

            token = "";

            if (string_state == false) {
                string_state = true;
            }

            else if (string_state == true) {
                tokens.push_back(str);
                string_state = false;
                str = "";
            }
        }

        else if (string_state == true) {
            if (token == "\\") {

            }

            else if (token == "\\q") {
                str = str + "\"";
                token = "";
            }

            else if (token == "\\n") {
                str = str + "\n";
                token = "";
            }

            else if (token == "\\\\") {
                str = str + "\\";
                token = "";
            }

            else {
                str = str + token;
                token = "";
            }
        }

        else if (token == "<") {
            if (var_name != "") {
                tokens.push_back(var_name);
                variable_state = false;
                var_name = "";
            }
        }

        else if (token == "+") {
            if (var_name != "") {
                tokens.push_back(var_name);
                variable_state = false;
                var_name = "";
            }

            tokens.push_back("PLUS");
            token = "";
        }

        else if (token == "var") {
            tokens.push_back("VAR");
            variable_state = true;
            token = "";
        }

        else if (variable_state == true) {
            var_name = var_name + token;
            token = "";
        }

        else if (token == "system") {
            tokens.push_back("SYSTEM");
            token = "";
        }

        else {
            for (auto const& i : functions) {
                if (token == i.first) {
                    tokens.push_back("START");
                    tokens.push_back(i.first);
                    token = "";
                }
            }

            for (auto const& i : variables) {
                if (token == i.first) {
                    tokens.push_back("VAR");
                    tokens.push_back(i.first);
                    token = "";
                }
            }

            for (string i : pre_var) {
                if (token == i) {
                    tokens.push_back("VAR");
                    tokens.push_back(i);
                    token = "";
                }
            }
        }
    }

    return tokens;
}

string expression_parser(vector <string> tokens) {
    vector <string> new_tokens;
    vector <string> expression_tokens;
    string result;
    string chached_variable;
    string input;
    int pos = 0;

    while (true) {
        if (tokens.size() <= pos) {
            break;
        }

        if (tokens[pos] == "VAR") {
            new_tokens.push_back(variables[tokens[pos + 1]]);
            pos = pos + 2;
        }

        else {
            new_tokens.push_back(tokens[pos]);
            pos = pos + 1;
        }
    }

    pos = 0;
    tokens = new_tokens;
    new_tokens.clear();

    while (true) {
        if (tokens.size() <= pos) {
            break;
        }

        else if (tokens[pos] == "<CEX>") {
            expression_tokens = expression_lexer(tokens[pos + 1]);
            new_tokens.push_back(expression_parser(expression_tokens));
            pos = pos + 2;
        }

        else {
            new_tokens.push_back(tokens[pos]);
            pos = pos + 1;
        }
    }

    pos = 0;
    tokens = new_tokens;
    new_tokens.clear();

    result = tokens[pos];

    while (true) {
        if (tokens.size() <= pos) {
            break;
        }

        else if (tokens[pos] == "PLUS") {
            result = result + tokens[pos + 1];
            pos = pos + 2;
        }

        else {
            pos = pos + 1;
        }
    }

    return result;
}

vector <string> condition_lexer(string condition) {
    vector <string> tokens;
    string token = "";
    string var_name = "";
    string str = "";
    bool variable_state = false;
    bool string_state = false;
    bool condition_state = false;

    condition = condition + "<EOF>";

    for(char& chr : condition) {
        token = token + chr;

        if (token == " " || token == "<" || token == "!" || token == "=") {
            if (var_name != "") {
                tokens.push_back(var_name);
                variable_state = false;
                var_name = "";
            }
        }

        if (token == " " || token == "<EOF>") {
            if (string_state == false) {
                token = "";
            }
        }

        else if (variable_state == true) {
            var_name = var_name + token;
            token = "";
        }

        else if (token == "\"" || token == " \"" || token == "\\\"") {
            if (token == " \"") {
                str = str + " ";
            }

            token = "";

            if (string_state == false) {
                string_state = true;
            }

            else if (string_state == true) {
                tokens.push_back(str);
                string_state = false;
                str = "";
            }
        }

        else if (string_state == true) {
            if (token == "\\") {

            }

            else if (token == "\\q") {
                str = str + "\"";
                token = "";
            }

            else if (token == "\\n") {
                str = str + "\n";
                token = "";
            }

            else if (token == "\\\\") {
                str = str + "\\";
                token = "";
            }

            else {
                str = str + token;
                token = "";
            }
        }

        else if (token == "true") {
            tokens.push_back("TRUE");
            token = "";
        }

        else if (token == "false") {
            tokens.push_back("FALSE");
            token = "";
        }

        else if (token == "in") {
            tokens.push_back("IN");
            token = "";
        }

        else if (token == "==") {
            tokens.push_back("EQEQ");
            token = "";
        }

        else if (token == "!=") {
            tokens.push_back("UNEQ");
            token = "";
        }

        else if (token == "var") {
            tokens.push_back("VAR");
            variable_state = true;
            token = "";
        }

        else if (token == "and") {
            tokens.push_back("AND");
            token = "";
        }

        else if (token == "or") {
            tokens.push_back("OR");
            token = "";
        }

        else if (token == "not") {
            tokens.push_back("NOT");
            token = "";
        }

        else {
            for (auto const& i : functions) {
                if (token == i.first) {
                    tokens.push_back("START");
                    tokens.push_back(i.first);
                    token = "";
                }
            }

            for (auto const& i : variables) {
                if (token == i.first) {
                    tokens.push_back("VAR");
                    tokens.push_back(i.first);
                    token = "";
                }
            }
        }
    }

    return tokens;
}

bool condition_parser(vector <string> tokens) {
    vector <string> new_tokens;
    int pos = 0;

    while (true) {
        if (tokens.size() <= pos) {
            break;
        }

        if (tokens[pos] == "VAR") {
            new_tokens.push_back(variables[tokens[pos + 1]]);
            pos = pos + 2;
        }

        else {
            new_tokens.push_back(tokens[pos]);
            pos = pos + 1;
        }
    }

    pos = 0;
    tokens = new_tokens;
    new_tokens.clear();

    while (true) {
        if (tokens.size() <= pos) {
            break;
        }

        if (tokens[pos] == "NOT") {
            if (tokens[pos + 1] == "TRUE") {
                new_tokens.push_back("FALSE");
            }

            else if (tokens[pos + 1] == "FALSE") {
                new_tokens.push_back("TRUE");
            }

            else {
                cout << "\033[31m" << "ValueError: Expected bool after not operator." << "\033[0m" << endl;
            }
            
            pos = pos + 2;
        }

        else {
            new_tokens.push_back(tokens[pos]);
            pos = pos + 1;
        }
    }

    pos = 0;
    tokens = new_tokens;
    new_tokens.clear();

    while (true) {
        if (tokens.size() <= pos) {
            break;
        }

        else if (tokens[pos] == "EQEQ") {
            if (tokens[pos - 1] == tokens[pos + 1]) {
                new_tokens.push_back("TRUE");
            }

            else {
                new_tokens.push_back("FALSE");
            }

            pos = pos + 2;
        }

        else if (tokens[pos] == "UNEQ") {
            if (tokens[pos - 1] != tokens[pos + 1]) {
                new_tokens.push_back("TRUE");
            }

            else {
                new_tokens.push_back("FALSE");
            }

            pos = pos + 2;
        }
        
        else if (tokens[pos] == "AND") {
            new_tokens.push_back("AND");
            pos = pos + 1;
        }

        else if (tokens[pos] == "OR") {
            new_tokens.push_back("OR");
            pos = pos + 1;
        }

        else if (tokens[pos] == "TRUE") {
            if (tokens.size() - 1 != pos) {
                if (tokens[pos + 1] != "EQEQ" && tokens[pos + 1] != "UNEQ") {
                    new_tokens.push_back("TRUE");
                }
            }

            else {
                new_tokens.push_back("TRUE");
            }

            pos = pos + 1;
        }

        else if (tokens[pos] == "FALSE" && tokens[pos + 1] != "EQEQ" && tokens[pos + 1] != "UNEQ") {
            if (tokens.size() - 1 != pos) {
                if (tokens[pos + 1] != "EQEQ" && tokens[pos + 1] != "UNEQ") {
                    new_tokens.push_back("FALSE");
                }
            }

            else {
                new_tokens.push_back("FALSE");
            }

            pos = pos + 1;
        }

        else {
            pos = pos + 1;
        }
    }

    pos = 0;
    tokens = new_tokens;
    new_tokens.clear();

    while (true) {
        if (tokens.size() <= pos) {
            break;
        }

        else if (tokens[pos] == "OR") {
            if (tokens[pos - 1] == "TRUE" || tokens[pos + 1] == "TRUE") {
                tokens.erase(tokens.begin() + 0);
                tokens.erase(tokens.begin() + 0);
                tokens.erase(tokens.begin() + 0);
                tokens.insert(tokens.begin() + 0, {"TRUE"});
            }

            else {
                tokens.erase(tokens.begin() + 0);
                tokens.erase(tokens.begin() + 0);
                tokens.erase(tokens.begin() + 0);
                tokens.insert(tokens.begin() + 0, {"FALSE"});
            }
        }

        else if (tokens[pos] == "AND") {
            if (tokens[pos - 1] == "TRUE" && tokens[pos + 1] == "TRUE") {
                tokens.erase(tokens.begin() + 0);
                tokens.erase(tokens.begin() + 0);
                tokens.erase(tokens.begin() + 0);
                tokens.insert(tokens.begin() + 0, {"TRUE"});
            }

            else {
                tokens.erase(tokens.begin() + 0);
                tokens.erase(tokens.begin() + 0);
                tokens.erase(tokens.begin() + 0);
                tokens.insert(tokens.begin() + 0, {"FALSE"});
            }
        }

        else {
            pos = pos + 1;
        }
    }

    if (tokens.size() == 1) {
        if (tokens[0] == "TRUE") {
            return true;
        }

        else {
            return false;
        }
    }

    else {
        return false;
    }
}

vector <string> argument_lexer(string condition) {
    vector <string> tokens;
    string token = "";
    string var_name = "";
    string str = "";
    string condition_2 = "";
    bool variable_state = false;
    bool string_state = false;
    bool condition_state = false;

    condition = condition + "<EOF>";

    for (char& chr : condition) {
        token = token + chr;

        if (token == " " || token == "\n") {
            if (token == " ") {
                if (string_state == false && condition_state == false) {
                    token = "";
                }
            }

            else {
                token = "";
            }
        }

        else if (token == "<") {
            if (var_name != "") {
                tokens.push_back(var_name);
                var_name = "";
                variable_state = false;
            }

            token = "";
        }

        else if (token == ")") {
            tokens.push_back("<CEX>");
            tokens.push_back(condition_2);
            condition_state = false;
            condition_2 = "";
            token = "";
        }

        else if (condition_state == true) {
            condition_2 = condition_2 + token;
            token = "";
        }

        else if (token == "(") {
            condition_state = true;
            token = "";
        }

        else if (token == "\"" || token == " \"" || token == "\\\"") {
            if (token == " \"") {
                str = str + " ";
            }

            if (string_state == true) {
                tokens.push_back(str);
                str = "";
                string_state = false;
            }

            else {
                string_state = true;
            }

            token = "";
        }

        else if (string_state == true) {
            if (token == "\\") {

            }

            else if (token == "\\q") {
                str = str + "\"";
                token = "";
            }

            else if (token == "\\n") {
                str = str + "\n";
                token = "";
            }

            else if (token == "\\\\") {
                str = str + "\\";
                token = "";
            }

            else {
                str = str + token;
                token = "";
            }
        }

        else if (token == ",") {
            if (var_name != "") {
                tokens.push_back(var_name);
                var_name = "";
                variable_state = false;
            }

            tokens.push_back("COMMA");
            token = "";
        }

        else if (token == "var") {
            tokens.push_back("VAR");
            variable_state = true;
            var_name = "";
            token = "";
        }

        else if (variable_state == true) {
            var_name = var_name + token;
            token = "";
        }

        else if (token == "system") {
            tokens.push_back("SYSTEM");
            token = "";
        }

        else {
            for (auto const& i : functions) {
                if (token == i.first) {
                    tokens.push_back("START");
                    tokens.push_back(i.first);
                    token = "";
                }
            }

            for (auto const& i : variables) {
                if (token == i.first) {
                    tokens.push_back("VAR");
                    tokens.push_back(i.first);
                    token = "";
                }
            }
        }
    }

    return tokens;
}

map <string, string> argument_parser(vector <string> tokens, vector <string> tokens_2) {
    map <string, string> temp_variables;
    vector <string> expression_tokens;
    int pos = 0;
    int pos_2 = 0;

    while (true) {
        if (tokens.size() <= pos || tokens_2.size() <= pos_2) {
            break;
        }

        else if (tokens[pos] == "VAR") {
            if (tokens_2[pos_2] == "VAR") {
                temp_variables[tokens[pos + 1]] = variables[tokens_2[pos_2 + 1]];
                pos = pos + 3;
                pos_2 = pos_2 + 3;
            }

            else if (tokens_2[pos_2] == "<CEX>") {
                expression_tokens = expression_lexer(tokens_2[pos_2 + 1]);
                temp_variables[tokens[pos + 1]] = expression_parser(expression_tokens);
                pos = pos + 3;
                pos_2 = pos_2 + 3;
            }

            else {
                temp_variables[tokens[pos + 1]] = tokens_2[pos_2];
                pos = pos + 3;
                pos_2 = pos_2 + 2;
            }
        }

        else {
            cout << "\033[31m" << "Warning: skipped token" << "\033[0m" << endl;
        }
    }

    return temp_variables;
}

vector <string> argument_parser_2(vector <string> tokens) {
    vector <string> temp_variables;
    int pos = 0;

    while (true) {
        if (tokens.size() <= pos) {
            break;
        }

        else if (tokens[pos] == "VAR") {
            temp_variables.push_back(tokens[pos + 1]);
            pos = pos + 2;
        }

        else if (tokens[pos] == "COMMA") {
            pos = pos + 1;
        }

        else {
            cout << "\033[31m" << "Warning: skipped token" << "\033[0m" << endl;
        }
    }

    return temp_variables;
}

vector <string> argument_parser_3(vector <string> tokens) {
    vector <string> temp_variables;
    vector <string> expression_tokens;
    int pos = 0;

    while (true) {
        if (tokens.size() <= pos) {
            break;
        }

        else if (tokens[pos] == "VAR") {
            temp_variables.push_back(variables[tokens[pos + 1]]);
            pos = pos + 2;
        }

        else if (tokens[pos] == "<CEX>") {
            expression_tokens = expression_lexer(tokens[pos + 1]);
            temp_variables.push_back(expression_parser(expression_tokens));
            pos = pos + 2;
        }

        else if (tokens[pos] == "COMMA") {
            pos = pos + 1;
        }

        else {
            temp_variables.push_back(tokens[pos]);
            pos = pos + 1;
        }
    }

    return temp_variables;
}

int argument_parser_4(vector <string> tokens) {
    int count = 0;
    int pos = 0;

    while (true) {
        if (tokens.size() <= pos) {
            break;
        }

        else if (tokens[pos] == "VAR") {
            count = count + 1;
            pos = pos + 2;
        }

        else if (tokens[pos] == "<CEX>") {
            count = count + 1;
            pos = pos + 2;
        }

        else if (tokens[pos] == "COMMA") {
            pos = pos + 1;
        }

        else {
            count = count + 1;
            pos = pos + 1;
        }
    }

    return count;
}

vector <string> lexer(string code, string lib_name = "") {
    vector <string> tokens;
    vector <string> new_tokens;
    vector <string> temp_var;
    string token = "";
    string str = "";
    string var_name = "";
    string func_name = "";
    string task = "";
    string condition = "";
    string new_code = "";
    string new_lib_name = "";
    string dll_name = "";
    string dll_func_name = "";
    int ignore = 0;
    int ignore_2 = 0;
    bool string_state = false;
    bool task_state = false;
    bool condition_state = false;
    bool variable_state = false;
    bool func_state = false;
    bool lib_state = false;
    bool call_state = false;
    bool dll_state = false;
    bool dll_func_state = false;

    for(char& chr : code) {
        if (chr == '\n') {
            if (token != "" || variable_state || func_state || dll_state || dll_func_state || call_state) {
                cout << "\033[31m" << "SyntaxError: Invalid syntax." << "\033[0m" << endl;
                tokens.clear();
                return tokens;
            }
        }

        token = token + chr;

        if (token == "}" && string_state == false) {
            if (ignore > 0) {
                task = task + "}";
                ignore = ignore - 1;
            }

            else {
                tokens.push_back(task);
                task_state = false;
                task = "";
            }

            token = "";
        }

        else if (token == "{" && string_state == false) {
            if (task_state == true) {
                task = task + token;
                ignore = ignore + 1;
            }

            task_state = true;
            token = "";
        }

        else if (task_state == true) {
            task = task + token;
            token = "";
        }

        else if (token == ")" && string_state == false) {
            if (ignore_2 > 0) {
                condition = condition + ")";
                ignore_2 = ignore_2 - 1;
            }

            else {
                tokens.push_back("<CEX>");
                tokens.push_back(condition);
                condition_state = false;
                condition = "";
            }

            token = "";
        }

        else if (token == "(" && string_state == false) {
            if (dll_func_state == true) {
                tokens.push_back(dll_func_name);
                dll_func_name = "";
                dll_func_state = false;
                call_state = false;
            }

            if (func_name != "") {
                if (lib_name != "") {
                    tokens.push_back(lib_name + "." + func_name);
                    pre_func.push_back(lib_name + "." + func_name);
                }

                else {
                    tokens.push_back(func_name);
                    pre_func.push_back(func_name);
                }

                func_state = false;
                func_name = "";
            }

            if (condition_state == true) {
                ignore_2 = ignore_2 + 1;
                condition = condition + "(";
            }

            condition_state = true;

            token = "";
        }

        else if (condition_state == true) {
            condition = condition + token;
            token = "";
        }

        else if (token == " " && string_state == false) {
            token = "";
        }

        else if (token == "\n" && string_state == false || token == "<EOF>" && string_state == false) {
            token = "";
        }

        else if (token == "\"" || token == " \"" || token == "\\\"") {
            if (token == " \"") {
                str = str + " ";
            }

            if (string_state == false) {
                string_state = true;
                token = "";
            }

            else if (string_state == true) {
                if (token == "\\\"") {
                    str = str + "\\";
                }

                if (lib_state == true) {
                    if (str.substr(str.find_last_of(".") + 1) == "dll") {
                        load_dll(str.c_str());
                    }

                    else {
                        new_code = lib_reader_code(str);
                        new_lib_name = lib_reader_lib_name(str);

                        if (lib_name != "") {
                            new_lib_name = lib_name + "." + new_lib_name;
                        }

                        new_tokens = lexer(new_code, new_lib_name);

                        int pos = 0;

                        while (true) {
                            if (new_tokens.size() <= pos) {
                                break;
                            }

                            else if (new_tokens[pos] == "FUNC") {
                                pre_func.push_back(new_tokens[pos + 1]);
                                functions[new_tokens[pos + 1]] = new_tokens[pos + 4];
                                function_arguments[new_tokens[pos + 1]] = new_tokens[pos + 3];
                                temp_var.push_back(new_tokens[pos + 1]);
                                pos = pos + 5;
                            }
                            
                            else {
                                pos = pos + 1;
                            }
                        }
                    }

                    lib_state = false;
                }

                else {
                    tokens.push_back(str);
                }

                string_state = false;
                token = "";
                str = "";
            }
        }

        else if (token == "*" && string_state == false) {
            tokens.push_back("POINTER");
            token = "";
        }

        else if (call_state == true) {
            if (token == ".") {
                tokens.push_back(dll_name);
                dll_name = "";
                dll_state = false;
                dll_func_state = true;
            }

            else {
                if (dll_state == true) {
                    dll_name = dll_name + token;
                }

                else if (dll_func_state == true) {
                    dll_func_name = dll_func_name + token;
                }
            }

            token = "";
        }

        else if (string_state == true) {
            if (token == "\\") {

            }

            else if (token == "\\q") {
                str = str + "\"";
                token = "";
            }

            else if (token == "\\n") {
                str = str + "\n";
                token = "";
            }

            else if (token == "\\\\") {
                str = str + "\\";
                token = "";
            }

            else {
                str = str + token;
                token = "";
            }
        }

        else if (func_state == true) {
            func_name = func_name + token;
            token = "";
        }

        else if (token == "=") {
            if (var_name != "") {
                if (lib_name != "") {
                    tokens.push_back(lib_name + "." + var_name);
                    pre_var.push_back(lib_name + "." + var_name);
                }

                else {
                    tokens.push_back(var_name);
                    pre_var.push_back(var_name);
                }

                variable_state = false;
                var_name = "";
            }

            token = "";
        }

        else if (variable_state == true) {
            var_name = var_name + token;
            token = "";
        }

        else if (token == "include") {
            lib_state = true;
            token = "";
        }

        else if (token == "true") {
            tokens.push_back("TRUE");
            token = "";
        }

        else if (token == "false") {
            tokens.push_back("FALSE");
            token = "";
        }

        else if (token == "return") {
            tokens.push_back("RETURN");
            token = "";
        }

        else if (token == "func") {
            tokens.push_back("FUNC");
            token = "";
            func_state = 1;
        }

        else if (token == "system") {
            tokens.push_back("SYSTEM");
            token = "";
        }

        else if (token == "var") {
            tokens.push_back("VAR");
            token = "";
            variable_state = true;
        }

        else if (token == "void") {
            tokens.push_back("VOID");
            token = "";
            call_state = true;
            dll_state = true;
        }

        else if (token == "bool") {
            tokens.push_back("BOOL");
            token = "";
            call_state = true;
            dll_state = true;
        }

        else if (token == "const") {
            tokens.push_back("CONST");
            token = "";
        }

        else if (token == "char") {
            tokens.push_back("CHAR");
            token = "";
            call_state = true;
            dll_state = true;
        }

        else if (token == "while") {
            tokens.push_back("WHILE");
            token = "";
        }

        else if (token == "for") {
            tokens.push_back("FOR");
            token = "";
        }

        else if (token == "if") {
            tokens.push_back("IF");
            token = "";
        }

        else if (token == "else") {
            tokens.push_back("ELSE");
            token = "";
        }

        else {
            for (string i : pre_func) {
                if (token == i) {
                    tokens.push_back("START");
                    tokens.push_back(i);
                    token = "";
                }
            }

            for (string i : pre_var) {
                if (token == i) {
                    tokens.push_back("VAR");
                    tokens.push_back(i);
                    token = "";
                }
            }

            for (string i : temp_var) {
                if (token == i) {
                    tokens.push_back("VAR");
                    tokens.push_back(i);
                    token = "";
                }
            }
        }
    }

    if (token != "" || variable_state || func_state || dll_state || dll_func_state || call_state) {
        cout << "\033[31m" << "SyntaxError: Invalid syntax." << "\033[0m" << endl;
        tokens.clear();
        return tokens;
    }

    if (ignore != 0 || ignore_2 != 0 || string_state || task_state || condition_state || variable_state || func_state || lib_state) {
        cout << "\033[31m" << "SyntaxError: Invalid syntax." << "\033[0m" << endl;
        tokens.clear();
        return tokens;
    }

    return tokens;
}

string parser(vector <string> tokens) {
    map <string, string> new_variables;
    vector <string> new_tokens;
    vector <string> task_tokens;
    vector <string> condition_tokens;
    vector <string> expression_tokens;
    string code = "";
    string lib_name = "";
    string result = "";
    string chached_variable = "";
    bool condition = false;
    bool last_condition = false;
    int pos = 0;

    while (true) {
        if (tokens.size() <= pos) {
            break;
        }

        /*
         * cout << tokens[pos] << endl;
         *
         * pos = pos + 1;
         */

        else if (tokens[pos] == "FUNC") {
            functions[tokens[pos + 1]] = tokens[pos + 4];
            function_arguments[tokens[pos + 1]] = tokens[pos + 3];
            pos = pos + 5;

            if (tokens[pos - 4] == "main") {
                tokens.insert(tokens.begin() + pos, "START");
                tokens.insert(tokens.begin() + pos + 1, "main");
                tokens.insert(tokens.begin() + pos + 2, "<CEX>");
                tokens.insert(tokens.begin() + pos + 3, "");
            }
        }

        else if (tokens[pos] == "VOID") {
            vector <string> argument_tokens = argument_lexer(tokens[pos + 4]);
            vector <string> temp_variables = argument_parser_3(argument_tokens);
            int count = argument_parser_4(argument_tokens);

            if (count == 0) {
                load_func_void_void(tokens[pos + 1], tokens[pos + 2])();
            }

            else if (count == 1) {
                if (argument_tokens[0] == "TRUE") {
                    load_func_void_bool(tokens[pos + 1], tokens[pos + 2])(true);
                }

                else if (argument_tokens[0] == "FALSE") {
                    load_func_void_bool(tokens[pos + 1], tokens[pos + 2])(false);
                }

                else {
                    load_func_void_const_char_p(tokens[pos + 1], tokens[pos + 2])(temp_variables[0].c_str());
                }
            }

            else {
                cout << "\033[31m" << "SyntaxError: Invalid syntax.." << "\033[0m" << endl;
                break;
            }

            pos = pos + 5;
        }

        else if (tokens[pos] == "BOOL") {
            vector <string> argument_tokens = argument_lexer(tokens[pos + 4]);
            vector <string> temp_variables = argument_parser_3(argument_tokens);
            int count = argument_parser_4(argument_tokens);

            if (count == 0) {
                load_func_void_void(tokens[pos + 1], tokens[pos + 2])();
            }

            else if (count == 1) {
                if (argument_tokens[0] == "TRUE") {
                    load_func_bool_bool(tokens[pos + 1], tokens[pos + 2])(true);
                }

                else if (argument_tokens[0] == "FALSE") {
                    load_func_bool_bool(tokens[pos + 1], tokens[pos + 2])(false);
                }

                else {
                    load_func_bool_const_char_p(tokens[pos + 1], tokens[pos + 2])(temp_variables[0].c_str());
                }
            }

            else {
                cout << "\033[31m" << "SyntaxError: Invalid syntax.." << "\033[0m" << endl;
                break;
            }

            pos = pos + 5;
        }

        else if (tokens[pos] == "CONST") {
            if (tokens[pos + 1] == "CHAR") {
                if (tokens[pos + 2] == "POINTER") {
                    vector <string> argument_tokens = argument_lexer(tokens[pos + 6]);
                    vector <string> temp_variables = argument_parser_3(argument_tokens);
                    int count = argument_parser_4(argument_tokens);

                    if (count == 0) {
                        load_func_const_char_p(tokens[pos + 3], tokens[pos + 4])();
                    }

                    else if (count == 1) {
                        if (argument_tokens[0] == "TRUE") {
                            load_func_const_char_p_bool(tokens[pos + 3], tokens[pos + 4])(true);
                        }

                        else if (argument_tokens[0] == "FALSE") {
                            load_func_const_char_p_bool(tokens[pos + 3], tokens[pos + 4])(false);
                        }

                        else {
                            load_func_const_char_p_const_char_p(tokens[pos + 3], tokens[pos + 4])(temp_variables[0].c_str());
                        }
                    }

                    else {
                        cout << "\033[31m" << "SyntaxError: Invalid syntax.." << "\033[0m" << endl;
                        break;
                    }

                    pos = pos + 7;
                }
            }
        }

        else if (tokens[pos] == "RETURN") {
            if (tokens[pos + 1] == "VAR") {
                result = variables[tokens[pos + 2]];
                pos = pos + 3;
            }

            else if (tokens[pos + 1] == "BOOL") {
                vector <string> argument_tokens = argument_lexer(tokens[pos + 7]);
                vector <string> temp_variables = argument_parser_3(argument_tokens);
                int count = argument_parser_4(argument_tokens);

                if (count == 0) {
                    if (load_func_bool_void(tokens[pos + 4], tokens[pos + 5])() == true) {
                        result = "TRUE";
                    } else {
                        result = "FALSE";
                    }
                }

                else if (count == 1) {
                    if (argument_tokens[0] == "TRUE") {
                        if (load_func_bool_bool(tokens[pos + 4], tokens[pos + 5])(true) == true) {
                            result = "TRUE";
                        } else {
                            result = "FALSE";
                        }
                    }

                    else if (argument_tokens[0] == "FALSE") {
                        if (load_func_bool_bool(tokens[pos + 4], tokens[pos + 5])(false) == true) {
                            result = "TRUE";
                        } else {
                            result = "FALSE";
                        }
                    }

                    else {
                        if (load_func_bool_const_char_p(tokens[pos + 4], tokens[pos + 5])(temp_variables[0].c_str()) == true) {
                            result = "TRUE";
                        } else {
                            result = "FALSE";
                        }
                    }
                }

                else {
                    cout << "\033[31m" << "SyntaxError: Invalid syntax.." << "\033[0m" << endl;
                    break;
                }

                pos = pos + 8;
            }

            else if (tokens[pos + 1] == "CONST") {
                if (tokens[pos + 2] == "CHAR") {
                    if (tokens[pos + 3] == "POINTER") {
                        vector <string> argument_tokens = argument_lexer(tokens[pos + 7]);
                        vector <string> temp_variables = argument_parser_3(argument_tokens);
                        int count = argument_parser_4(argument_tokens);

                        if (count == 0) {
                            result = load_func_const_char_p(tokens[pos + 4], tokens[pos + 5])();
                        }

                        else if (count == 1) {
                            if (argument_tokens[0] == "TRUE") {
                                result = load_func_const_char_p_bool(tokens[pos + 4], tokens[pos + 5])(true);
                            }

                            else if (argument_tokens[0] == "FALSE") {
                                result = load_func_const_char_p_bool(tokens[pos + 4], tokens[pos + 5])(false);
                            }

                            else {
                                result = load_func_const_char_p_const_char_p(tokens[pos + 4], tokens[pos + 5])(temp_variables[0].c_str());
                            }
                        }

                        else {
                            cout << "\033[31m" << "SyntaxError: Invalid syntax.." << "\033[0m" << endl;
                            break;
                        }

                        pos = pos + 8;
                    }
                }
            }

            else if (tokens[pos + 1] == "SYSTEM") {
                if (tokens[pos + 2] == "<CEX>") {
                    expression_tokens = expression_lexer(tokens[pos + 3]);
                    result = execute(expression_parser(expression_tokens));
                    pos = pos + 4;
                }
            }

            else if (tokens[pos + 1] == "START") {
                if (tokens[pos + 3] == "<CEX>") {
                    chached_variables = variables;
                    variables = argument_parser(argument_lexer(function_arguments[tokens[pos + 2]]), argument_lexer(tokens[pos + 4]));
                    new_tokens = lexer(functions[tokens[pos + 2]]);
                    chached_variable = parser(new_tokens);
                    variables = chached_variables;
                    result = chached_variable;
                }

                pos = pos + 5;
            }

            else if (tokens[pos + 1] == "<CEX>") {
                expression_tokens = expression_lexer(tokens[pos + 2]);
                result = expression_parser(expression_tokens);
                pos = pos + 3;
            }

            else {
                result = tokens[pos + 1];
                pos = pos + 2;
            }

            break;
        }

        else if (tokens[pos] == "START") {
            if (tokens[pos + 2] == "<CEX>") {
                chached_variables = variables;
                variables = argument_parser(argument_lexer(function_arguments[tokens[pos + 1]]), argument_lexer(tokens[pos + 3]));
                new_tokens = lexer(functions[tokens[pos + 1]]);
                parser(new_tokens);
                variables = chached_variables;
            }

            pos = pos + 4;
        }

        else if (tokens[pos] == "SYSTEM") {
            if (tokens[pos + 1] == "<CEX>") {
                expression_tokens = expression_lexer(tokens[pos + 2]);
                system(expression_parser(expression_tokens).c_str());
                pos = pos + 3;
            }
        }

        else if (tokens[pos] == "WHILE") {
            condition_tokens.clear();
            condition = false;

            condition_tokens = condition_lexer(tokens[pos + 2]);
            condition = condition_parser(condition_tokens);

            if (condition == true) {
                new_tokens = lexer(tokens[pos + 3]);
                parser(new_tokens);
            }

            else {
                pos = pos + 4;
            }
        }

        else if (tokens[pos] == "FOR") {
            string text;

            new_tokens.clear();
            new_tokens = condition_lexer(tokens[pos + 2]);

            if (new_tokens[3] == "VAR") {
                text = variables[new_tokens[4]];
            }

            else {
                text = new_tokens[3];
            }

            for (char& chr : text) {
                variables[new_tokens[1]] = chr;
                task_tokens = lexer(tokens[pos + 3]);
                parser(task_tokens);
            }

            pos = pos + 4;
        }

        else if (tokens[pos] == "IF") {
            if (tokens[pos + 1] != "<CEX>") {
                cout << "\033[31m" << "SyntaxError: Invalid syntax." << "\033[0m" << endl;
                break;
            }

            condition_tokens = condition_lexer(tokens[pos + 2]);
            condition = condition_parser(condition_tokens);

            if (condition == true) {
                new_tokens = lexer(tokens[pos + 3]);
                parser(new_tokens);
                last_condition = true;
            }

            pos = pos + 4;
        }

        else if (tokens[pos] == "ELSE") {
            if (tokens[pos + 1] == "IF") {
                if (tokens[pos - 4] == "IF") {
                    if (last_condition == false) {
                        condition_tokens = condition_lexer(tokens[pos + 3]);
                        condition = condition_parser(condition_tokens);

                        if (condition == true) {
                            new_tokens = lexer(tokens[pos + 4]);
                            parser(new_tokens);
                            last_condition = true;
                        }

                        else {
                            last_condition = false;
                        }
                    }

                    else {
                        last_condition = true;
                    }

                    pos = pos + 5;
                }
            }

            else {
                if (last_condition == false) {
                    new_tokens = lexer(tokens[pos + 1]);
                    parser(new_tokens);
                    last_condition = true;
                }

                pos = pos + 4;
            }
        }

        else if (tokens[pos] == "VAR") {
            if (tokens[pos + 2] == "VAR") {
                variables[tokens[pos + 1]] = variables[tokens[pos + 3]];
                pos = pos + 4;
            }

            else if (tokens[pos + 2] == "BOOL") {
                vector <string> argument_tokens = argument_lexer(tokens[pos + 8]);
                vector <string> temp_variables = argument_parser_3(argument_tokens);
                int count = argument_parser_4(argument_tokens);

                if (count == 0) {
                    if (load_func_bool_void(tokens[pos + 5], tokens[pos + 6])() == true) {
                        variables[tokens[pos + 1]] = "TRUE";
                    } else {
                        variables[tokens[pos + 1]] = "FALSE";
                    }
                }

                else if (count == 1) {
                    if (argument_tokens[0] == "TRUE") {
                        if (load_func_bool_bool(tokens[pos + 5], tokens[pos + 6])(true) == true) {
                            variables[tokens[pos + 1]] = "TRUE";
                        } else {
                            variables[tokens[pos + 1]] = "FALSE";
                        }
                    }

                    else if (argument_tokens[0] == "FALSE") {
                        if (load_func_bool_bool(tokens[pos + 5], tokens[pos + 6])(false) == true) {
                            variables[tokens[pos + 1]] = "TRUE";
                        } else {
                            variables[tokens[pos + 1]] = "FALSE";
                        }
                    }

                    else {
                        if (load_func_bool_const_char_p(tokens[pos + 5], tokens[pos + 6])(temp_variables[0].c_str()) == true) {
                            variables[tokens[pos + 1]] = "TRUE";
                        } else {
                            variables[tokens[pos + 1]] = "FALSE";
                        }
                    }
                }

                else {
                    cout << "\033[31m" << "SyntaxError: Invalid syntax.." << "\033[0m" << endl;
                    break;
                }

                pos = pos + 9;
            }

            else if (tokens[pos + 2] == "CONST") {
                if (tokens[pos + 3] == "CHAR") {
                    if (tokens[pos + 4] == "POINTER") {
                        vector <string> argument_tokens = argument_lexer(tokens[pos + 8]);
                        vector <string> temp_variables = argument_parser_3(argument_tokens);
                        int count = argument_parser_4(argument_tokens);

                        if (count == 0) {
                            variables[tokens[pos + 1]] = load_func_const_char_p(tokens[pos + 5], tokens[pos + 6])();
                        }

                        else if (count == 1) {
                            if (argument_tokens[0] == "TRUE") {
                                variables[tokens[pos + 1]] = load_func_const_char_p_bool(tokens[pos + 5], tokens[pos + 6])(true);
                            }

                            else if (argument_tokens[0] == "FALSE") {
                                variables[tokens[pos + 1]] = load_func_const_char_p_bool(tokens[pos + 5], tokens[pos + 6])(false);
                            }

                            else {
                                variables[tokens[pos + 1]] = load_func_const_char_p_const_char_p(tokens[pos + 5], tokens[pos + 6])(temp_variables[0].c_str());
                            }
                        }

                        else {
                            cout << "\033[31m" << "SyntaxError: Invalid syntax.." << "\033[0m" << endl;
                            break;
                        }

                        pos = pos + 9;
                    }
                }
            }

            else if (tokens[pos + 2] == "SYSTEM") {
                if (tokens[pos + 3] == "<CEX>") {
                    expression_tokens = expression_lexer(tokens[pos + 4]);
                    variables[tokens[pos + 1]] = execute(expression_parser(expression_tokens));
                    pos = pos + 5;
                }
            }

            else if (tokens[pos + 2] == "START") {
                if (tokens[pos + 4] == "<CEX>") {
                    chached_variables = variables;
                    variables = argument_parser(argument_lexer(function_arguments[tokens[pos + 3]]), argument_lexer(tokens[pos + 5]));
                    new_tokens = lexer(functions[tokens[pos + 3]]);
                    chached_variable = parser(new_tokens);
                    variables = chached_variables;
                    variables[tokens[pos + 1]] = chached_variable;
                }

                pos = pos + 6;
            }

            else if (tokens[pos + 2] == "<CEX>") {
                expression_tokens = expression_lexer(tokens[pos + 3]);
                variables[tokens[pos + 1]] = expression_parser(expression_tokens);
                pos = pos + 4;
            }

            else {
                variables[tokens[pos + 1]] = tokens[pos + 2];
                pos = pos + 3;
            }
        }

        else {
            cout << "\033[31m" << "Warning: skipped token" << "\033[0m" << endl;
            pos = pos + 1;
        }
    }

    return result;
}

int main(int argc, char** argv) {
    vector <string> tokens;
    string filename = "";
    string code = "";
    string arg1 = "";

    if (argc > 1) {
        arg1 = argv[1];
    }

    else {
        cout << "\033[31m" << "No input files, proccess terminated." << "\033[0m" << endl;

        return 0;
    }

    if (arg1 == "--version") {
        cout << "AeNCPP 0.0.1" << endl;
        return 0;
    }

    code = open_file(arg1, "ncpp");
    tokens = lexer(code);
    parser(tokens);

    return 0;
}