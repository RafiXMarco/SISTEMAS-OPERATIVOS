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
#include <sys/wait.h>


#include "scope.hpp"
#include "shell.h"


int ultimo_comandoERROR;

namespace shell {
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

std::error_code write(int fd, std::vector<uint8_t>& buffer) {
  ssize_t bytes_written = write(fd, buffer.data(), buffer.size());
  if (bytes_written < 0) {
    return std::error_code(errno, std::system_category());
  }
  return std::error_code(0, std::system_category());
}

std::vector<uint8_t> pending_input; 
/**
 * @brief Lee una linea de la terminal y la guarda en line(uno de los parametros)
 * 
 * @param fd 
 * @param line 
 * @return std::error_code 
 */
std::error_code read_line(int fd, std::string& line) { 

    while (true) { 
        //std::cout << "SIZE DE PENDING: "  << pending_input.size() << "\n";
        for (unsigned i = 0; i < pending_input.size(); i++) { 
            if (pending_input[i] == '\n') {
                line.clear();
                i++;
                for (unsigned j = 0; j < i; j++) {
                    line += pending_input[j];
                }
                std::vector<uint8_t> new_pending;
                for (unsigned j = i; j < pending_input.size(); j++) {
                    new_pending.push_back(pending_input[j]);
                }
                pending_input = new_pending;
                return std::error_code(0, std::system_category()); // return cuando esta vacio
            } 
        } 
        std::vector<uint8_t> buffer(1024); 
        std::error_code error = read(fd, buffer); 
        if (error) {
            //std::cout << "ERROR al leer. \n";
            return error; 
        }
        if (buffer.empty()) { 
            //std::cout << "BUFFER-> empty \n";
            if (!pending_input.empty()) { 
                line = std::string(pending_input.begin(), pending_input.end()); 
                line.push_back('\n'); 
                pending_input.clear(); 
            }
            return std::error_code(0, std::system_category()); // return cuando esta vacio
        } else { 
            //std::cout << "BUFFER-> !empty \n";
            pending_input.insert(pending_input.end(), buffer.begin(), buffer.end()); 
        } 
    } 
}

/**
 * @brief Imprime un string con write()
 * 
 * @param str 
 * @return std::error_code 
 */
std::error_code print(const std::string& str) {
    int bytes_written = write(STDOUT_FILENO, str.c_str(), str.size());
    if (bytes_written == -1) {
        std::error_code(errno, std::system_category());
    }
  return std::error_code(0, std::system_category());
}

/**
 * @brief Imprime el prompt 
 * 
 * @param last_command_status 
 */
void print_promt(int last_command_status) {
    std::string prompt_line;
    char hostname[256];
    char current_DIR[256];
    /*
    
    std::array <char, USER_NAME_MAX> uname;

    if (getlogin_r(uname.data(),uname.size()<0)) {
        struct passwd *pwd = getpwd(getuid());
        pwd -> pw_name
    }
    */

    prompt_line += getlogin(); // nombre usuario

    prompt_line += "@";

    if (gethostname(hostname, sizeof hostname) != 0) { // si no es 0 es error
        std::cout << "hay un error al extraer el hostname";
    }
    prompt_line += hostname; // nombre maquina

    if (getcwd(current_DIR, sizeof(current_DIR)) == NULL) { // en vez de 0 seria NULL
        std::cout << "hay un error al extraer el cuurent DIR";
    }
    
    prompt_line += current_DIR;



    if(!isatty(STDIN_FILENO)) {
        // std::cout << "error no esta conectado a un terminal";
        return;
    }

    if (last_command_status == 0) {
        prompt_line += "$> ";
    } else {
        prompt_line += "$< ";
    }
    print(prompt_line);
}


/**
 * @brief divide la linea en varios comandos
 * 
 * @param line 
 * @return std::vector<shell::command> 
 */
std::vector<shell::command> parse_line(const std::string& line) {
    std::istringstream iss(line);
    std::vector<shell::command> resultado_final;
    shell::command comando_entero;
    
    while(!iss.eof()) {
        std::string word;
        iss >> word;
        if (word[0]  == '#') { break;}
        if (word == ";"|| word == "&" || word == "|") {
            // Hemos terminado de leer comando
            comando_entero.push_back(word);
            resultado_final.push_back(comando_entero);
            comando_entero.clear();
            continue;
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
            if (word != "")
            comando_entero.push_back(word);
        }
        
    }
    if(!comando_entero.empty()) {
        resultado_final.push_back(comando_entero);
    }

    return resultado_final;
}

/**
 * @brief imprimer una linea
 * 
 * @param line 
 */
void imprimir (std::string line) {
    std::cout <<"EXTRAIDO:" << line;
}

/**
 * @brief Imprime un vector de vectores de string entero
 * 
 * @param comandos_separados 
 */
void imprimir (std::vector<shell::command> comandos_separados) {
    for(unsigned i = 0; i < comandos_separados.size(); i++) {
        std::cout << i + 1 << "º COMANDO: ";
        for (unsigned j = 0; j < comandos_separados[i].size(); j++) {
            std::cout << "->" << comandos_separados[i][j] << "<- ";
        }
        std::cout << "\n";
    }
}


/**
 * @brief ejecuta un comando externo
 * 
 * @param parametros 
 * @param has_wait 
 * @return int 
 */
int execute(const std::vector<std::string>& parametros, bool has_wait = true) {
    std::vector<const char*> vector;
    for (unsigned i = 0; i < parametros.size(); i++) {
        vector.push_back(parametros[i].c_str());
    }
    // for (auto& param : parametros) vector.push_back(param.c_str());
    vector.push_back(nullptr);
    const char* primero = vector[0];
    int resultado = execvp(primero, const_cast <char* const*>(vector.data()));
    if (errno == ENOENT ) {
        std::cout << "ERROR comando no encontrado\n";
        ultimo_comandoERROR = 1;
    }
    return resultado;
}

/**
 * @brief ejecuta un comando interno
 * 
 * @param args 
 * @return int 
 */
int foo_command(std::vector<std::string> args) {
    if (args[0] == "echo") {
        std::string linea_imprimir;
        for(unsigned i = 1; i < args.size(); i++) {
            linea_imprimir += args[i] + " ";
        }
        linea_imprimir += "\n";
        print(linea_imprimir);
        return 0;
    }
    if (args[0] == "cd") {
        if (args.size() > 3) {
            std::cerr <<  args.size()  << " demasiados argumentos\n";
            return 1;
        }
        if (chdir(args[1].c_str()) != 0) {
            std::cerr << "Error change directory\n";
            return 1;
        }

        return 0;
    }
    if (args[0] == "cp") {
        if (copy_file(args[1], args[2], true)) {
            //std::cout << "si";
        } else {

            //std::cout << "no";
        }

        return 0;
    }
    if (args[0] == "mv") {
        move_file(args[1], args[2], true);

        return 0;
    }
    return 1;
}

/**
 * @brief ejecuta un un comando entero
 * 
 * @param command 
 * @param has_wait 
 * @return int 
 */
int execute_program(const std::vector<std::string>& command, bool has_wait=true) {

    pid_t child = fork();
    int status;


    if(child == 0) {
        // hijo

        // execvp (command[0].c_str(), argvs);
        int resultado = execute(command, has_wait);
        //---error -> tenemos q terminar el proceso hijo, por ejemplo con exit.
        if (resultado != 0) exit(1);

    } else if (child > 0) {
        // padre
        if (has_wait) {
            waitpid(child, &status, 0);
            if (WEXITSTATUS(status) != 0) {
                // std::cout << "la ejecucion fallo, poniendo1 \n";
                ultimo_comandoERROR = 1;
            }
        } else {
            

            

            int res = waitpid(child, &status, WNOHANG);
            if (res!=0){
                // proceso hijo ha terminado
                std::cout << "proceso hijo ha terminado \n";
            } else {
                // proceso hio NO ha terminado
                std::cout << "proceso hijo NO ha terminado \n";
            }

            
            return child;
        }
    }else {
        //error
        return -1;
    }
    return 0;
}

/**
 * @brief ejecuta todos los comandos del vector
 * 
 * @param commands 
 * @return shell::command_result 
 */
shell::command_result execute_commands( std::vector<shell::command>& commands) {
    bool has_wait= true;
    static std::vector<pid_t> children;
    int return_value = 0;
    for (unsigned i = 0; i < commands.size(); i++) {
        std::string last_argument = commands[i].back();
        if (last_argument == ";" || last_argument == "&" || last_argument == "|" ) {
            if (last_argument == "&") {
                has_wait = false;
            }
            commands[i].pop_back();
        }

        if (commands[i][0] == "exit") {
            return shell::command_result::quit(return_value);
        } else if (commands[i][0] == "echo" || commands[i][0] == "cd" || commands[i][0] == "cp" || commands[i][0] == "mv") {
            if (foo_command(commands[i]) == 1) {
                ultimo_comandoERROR = 1;
            }
            continue;
        }

        pid_t child = execute_program(commands[i], has_wait);
        
        if (!has_wait) {
            children.push_back(child);
        }

        int status;
        unsigned numero_children = children.size();
        // std::cout << numero_children;
        for (unsigned i = 0; i < numero_children; i++) {
            pid_t child_pid = waitpid(children[i], &status, WNOHANG);
            if (child_pid != 0) {
                if (WIFEXITED(status)) {

                    auto exit_status = WEXITSTATUS(status);
                //-- sacar pid de la lista de procesis en seundo plano

                    std::cout << "[pid]: " << children[i] << " terminado [codigo de salida]: " << exit_status ;
                }
            }
        }

        // return_value = execute(commands[i], true);
    }
    return 0;
}



int main(int argc, char* argv[]) {
    ultimo_comandoERROR = 0;
    int last_command_status = 0;
    while (true) {
        //PROMT
        print_promt(last_command_status);

        //READ LINE
        std::string line;
        read_line(isatty(STDIN_FILENO), line);
        if (line == "") {
            return 0;
        }
        //imprimir(line);

        // PARSE (DIVIDIR COMANDO GRANDE EN VECTOR DE VECTORES)
        std::vector<shell::command> comandos_separados = parse_line(line);
        //imprimir(comandos_separados);
        

        shell::command_result resultado = execute_commands(comandos_separados);

        if (resultado.is_quit_requested) {
            return resultado.return_value;
        }
        
        last_command_status = ultimo_comandoERROR;
        ultimo_comandoERROR = 0;
        //std::cout << last_command_status << "->LAST COMAND STATUS" << std::flush ;
    }


    return 0;
}
