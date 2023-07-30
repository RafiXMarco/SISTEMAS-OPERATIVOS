#include <iostream>
#include <cstdlib> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <system_error>
#include <vector>
#include <cassert>
#include <unistd.h>
#include <utime.h>
#include <sstream>

#include "scope.hpp"
namespace shell
{

    using command = std::vector<std::string>;
    struct command_result {
        int return_value;
        bool is_quit_requested;
        command_result(int return_value, bool request_quit=false) : return_value{return_value}, is_quit_requested{request_quit} {}
        static command_result quit(int return_value=0) {
            return command_result{return_value, true};
        }
    };

} 

std::error_code read(int fd, std::vector<uint8_t>& buffer) {
  ssize_t bytes_read = read(fd, buffer.data(), buffer.size());
  if (bytes_read < 0) {
    return std::error_code(errno, std::system_category());
  }
  buffer.resize(bytes_read);
  return std::error_code(0, std::system_category());
}

std::vector<uint8_t> pending_input; 
std::error_code read_line(int fd, std::string& line) { 
    while (true) { 
        for (auto it = pending_input.begin(); it != pending_input.end(); ++it) { 
            if (*it == '\n') { 
                line = std::string(pending_input.begin(), it); 
                pending_input.erase(pending_input.begin(), it + 1); 
                return std::error_code(0, std::system_category()); // return cuando esta vacio
            } 
        } 
        std::vector<uint8_t> buffer(16ul * 1024 * 1024); 
        std::error_code error = read(fd, buffer); 
        if (error) return error; 
        if (buffer.empty()) { 
            if (!pending_input.empty()) { 
                line = std::string(pending_input.begin(), pending_input.end()); 
                line.push_back('\n'); 
                pending_input.clear(); 
            }
            return std::error_code(0, std::system_category()); // return cuando esta vacio
        } else { 
            pending_input.insert(pending_input.end(), buffer.begin(), buffer.end()); 
        } 
    } 
}





std::vector<shell::command> parse_line(const std::string& line) {
    std::istringstream iss(line);
    std::vector<shell::command> resultado_final;
    shell::command comando_entero;
    
    while(! iss.eof()) {
        std::string word;
        iss >> word;
        if (word[0]  == '#') { break;}
        if (word == ";"|| word == "&" || word == "|") {
            // Hemos terminado de leer comando
            comando_entero.push_back(word);
            resultado_final.push_back(comando_entero);
            comando_entero.clear();
        }
        char ultimo = word[word.length() - 1];
        if (ultimo == ';'|| ultimo == '&' || ultimo == '|') {
            word.pop_back();
            comando_entero.push_back(word);

            std::string ultimo_string;
            ultimo_string.push_back(ultimo);
            comando_entero.push_back(std::string(ultimo_string));

            resultado_final.push_back(comando_entero);
            comando_entero.clear();
        } else {
            comando_entero.push_back(word);
        }
    }

    return resultado_final;
}

int execute(const std::vector<std::string>& parametros, bool has_wait = true) {
    std::vector<const char*> vector;
    for (auto& param : parametros) vector.push_back(param.c_str());
    vector.push_back(nullptr);
    const char* primero = vector[0];
    int resultado = execvp(primero, const_cast <char* const*>(vector.data()));
    return resultado;
}


shell::command_result execute_commands(const std::vector<shell::command>& commands) {
    int return_value = 0;
    for (unsigned i = 0; i < commands.size(); i++) {
        if (commands[i][0] == "exit") {
            return shell::command_result::quit(return_value);
        }
        std:: cout << commands[i][1];
        return_value = execute (commands[i], true);
    }
    return shell::command_result::quit(return_value);
}


int main(int argc, char* argv[]) {

    while (true) {
        std::cout << "& ";
        std::string line;
        (getline(std::cin,line));

        read_line(isatty(STDIN_FILENO), line);

        std::vector<shell::command> comandos_separados = parse_line(line);

        for (auto& a : comandos_separados) {

            a.pop_back();
        }
        
        shell::command_result resultado = execute_commands(comandos_separados);
        if (resultado.is_quit_requested) {
            break;
        }
        
    }


    return 0;
}
