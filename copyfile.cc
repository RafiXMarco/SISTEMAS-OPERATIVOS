// MARCO NAPIERSKI //
// PARCIAL SEGUNDA PRÁCTICA //
#include <iostream>
#include <cerrno>
#include <system_error>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <libgen.h>
#include <vector>
#include <utime.h>

#include "scope.h"

std::error_code copy_file(const std::string& src_path, std::string& dst_path, bool preserve_all=false);
//void move_file(const std::string& src_path, const std::string& dst_path);


int main(int argc, char* argv[]){
    std::string kFileORIGEN;
    std::string kFileDESTINO;
    std::string extencion;
    bool extension_activa = 0;
    bool option_a = 0;
    bool option_m = 0;

    if (argc == 4) {
        extencion = argv[1];
        kFileORIGEN = argv[2];
        kFileDESTINO = argv[3];

        if(extencion == "-a") {
            option_a = 1;
            extension_activa = 1;
        }
        if(extencion == "-m") {
            option_m = 1;
            extension_activa = 1;
        }
    }
    if (argc == 3) {
        kFileORIGEN = argv[1];
        kFileDESTINO = argv[2];
    }

    if (!extension_activa) {
        copy_file(kFileORIGEN, kFileDESTINO);
    } else if (option_a) {
        copy_file(kFileORIGEN, kFileDESTINO, true);
    } 



    return EXIT_SUCCESS;
}

std::error_code read(int fd, std::vector<uint8_t>& buffer) {
    if (read(fd, buffer.data(), buffer.size()) < 0) return std::error_code(errno, std::system_category());
    return std::error_code(0, std::system_category());
}
std::error_code write(int fd, std::vector<uint8_t>& buffer) {
    if (write(fd, buffer.data(), buffer.size()) < 0) return std::error_code(errno, std::system_category());
    return std::error_code(0, std::system_category());
}




std::error_code copy_file(const std::string& src_path, std::string& dst_path, bool preserve_all) {
    const char* pathname_src = src_path.c_str();
    const char* pathname_dst = dst_path.c_str();
    int fd = open(pathname_src, O_RDONLY);
    close(fd);
    if (fd == -1)  {
        std::cerr << "no existe source\n";
        return std::error_code(errno, std::system_category());
    }
    struct stat informacion_archivo_src;    
    stat(pathname_src, &informacion_archivo_src);
    struct stat informacion_archivo_dst;    
    stat(pathname_dst, &informacion_archivo_dst);

    bool existe = 1; // si existe el archivo source
    int fd2 = open(pathname_dst, O_RDONLY);
    close(fd2);
    if (fd2 == -1) {
        std::cerr << "no existe destino\n";
        existe = 0;
    }
    if (existe) { // si existe el archivo source
    
        if (informacion_archivo_dst.st_dev == informacion_archivo_src.st_dev && informacion_archivo_dst.st_ino == informacion_archivo_src.st_ino
            && informacion_archivo_dst.st_mode == informacion_archivo_src.st_mode) {
                std::cout << "son el mismo archivo";
        }
        if (S_ISDIR(informacion_archivo_dst.st_mode)) {
            std::string nombre = src_path.substr(src_path.find_last_of("/\\") + 1);
            dst_path += "/" + nombre;
            std::cout << "creando nombre ya que destino es un directorio -SOURCE:" << src_path << " -DESTINO:" << dst_path << "\n";
            
        }
    }
    const char* pathname_dst_corregido = dst_path.c_str();

    int src_fd = open(pathname_src, O_RDONLY);
    if (src_fd < 0) return std::error_code(errno, std::system_category());
    auto guardSOURCE = scope::make_scope_exit([src_fd]{close(src_fd); });
    int dst_fd = open(pathname_dst_corregido, O_WRONLY | O_CREAT | O_TRUNC , 0666);
    if (dst_fd < 0) return std::error_code(errno, std::system_category());
    auto guardDESTINO = scope::make_scope_exit([dst_fd]{close(dst_fd); });




    size_t nbytes = 1024;
    std::vector<uint8_t> buffer(16ul * 1024 * 1024);
    while (true) {
        buffer.resize(nbytes);
        ssize_t byes_read=read(src_fd,buffer.data(),buffer.size());
        if (byes_read < 0) {
            return std::error_code(errno, std::system_category());
        }
        if (byes_read > 0) {
            buffer.resize(byes_read);
            write(dst_fd,buffer.data(),buffer.size());
        }
        if (byes_read  == 0) {
            break;
        }
    }
    close(src_fd);
    close(dst_fd);
    if (preserve_all) {
        int error_proof;
        // mode_t filetype = informacion_archivo_src.st_mode & S_IFMT;
        mode_t file_permissions = informacion_archivo_src.st_mode & ~S_IFMT;
        uid_t file_ID_USER = informacion_archivo_src.st_uid;
        gid_t file_ID_GRUOUP = informacion_archivo_src.st_gid;

        // chmod(pathname_dst, filetype);
        error_proof = chmod(pathname_dst, file_permissions);
        if (error_proof) return std::error_code(errno, std::system_category());
        error_proof = chown(pathname_dst, file_ID_USER, file_ID_GRUOUP);
        if (error_proof) return std::error_code(errno, std::system_category());

        struct utimbuf buf2 = { informacion_archivo_src.st_atime, informacion_archivo_src.st_mtime };
        error_proof = utime(pathname_dst, &buf2);
        if (error_proof) return std::error_code(errno, std::system_category());

    }

    return std::error_code(0, std::system_category());
}

