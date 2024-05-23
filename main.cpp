#include <iostream>
#include <fstream>
#include <string>
#include <deque>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

int menuConsultaID() {
    int id;
    std::cout << "Buscar ID:" << std::endl;
    std::cin >> id;
    return id;
}

void buffer(int nroFrame) {
    std::deque<std::string> frames;  // Usar deque para facilitar la eliminación del elemento más antiguo

    int consulta;
    do {
        std::cout << "Consulta? (1) Si (2) No " << std::endl;
        std::cin >> consulta;
        switch (consulta) {
        case 1: {
            int id = menuConsultaID();
            bool found = false;
            bool alreadyInBuffer = false;

            // Verificar si el ID ya está en el buffer
            for (const auto& frame : frames) {
                std::ifstream inFile("Paginas/" + frame);
                if (inFile.is_open()) {
                    std::string line;
                    while (std::getline(inFile, line)) {
                        size_t pos = line.find(',');
                        if (pos != std::string::npos) {
                            std::string firstWord = line.substr(0, pos);
                            if (std::to_string(id) == firstWord) {
                                std::cout << "ID encontrado en el buffer en el archivo: " << frame << std::endl;
                                std::cout << "Contenido de la linea: " << line << std::endl;
                                alreadyInBuffer = true;
                                break;
                            }
                        }
                    }
                    inFile.close();
                }
                if (alreadyInBuffer) break;
            }

            if (!alreadyInBuffer) {
                // Buscar el ID en los archivos
                DIR *dir;
                struct dirent *ent;
                if ((dir = opendir("Paginas")) != nullptr) {
                    while ((ent = readdir(dir)) != nullptr) {
                        std::string fileName = ent->d_name;
                        if (fileName == "." || fileName == "..") continue;

                        std::ifstream inFile("Paginas/" + fileName);
                        if (inFile.is_open()) {
                            std::string line;
                            while (std::getline(inFile, line)) {
                                size_t pos = line.find(',');
                                if (pos != std::string::npos) {
                                    std::string firstWord = line.substr(0, pos);
                                    if (std::to_string(id) == firstWord) {
                                        std::cout << "ID encontrado en el archivo: " << fileName << std::endl;
                                        std::cout << "Contenido de la linea: " << line << std::endl;
                                        if (frames.size() >= nroFrame) {
                                            frames.pop_front(); // Eliminar el frame más antiguo
                                        }
                                        frames.push_back(fileName);
                                        found = true;

                                        // Imprimir el buffer actualizado
                                        std::cout << "Buffer actualizado: ";
                                        for (const auto& frame : frames) {
                                            std::cout << frame << " ";
                                        }
                                        std::cout << std::endl;
                                        break;
                                    }
                                }
                            }
                            inFile.close();
                        }
                        if (found) break; // Exit the loop if ID is found
                    }
                    closedir(dir);
                }
            }

            if (!found && !alreadyInBuffer) {
                std::cout << "ID no encontrado en ningun archivo." << std::endl;
            }

            break;
        }
        case 2:
            break;
        default:
            std::cout << "Opcion no valida." << std::endl;
        }
    } while (consulta != 2);
}

int main() {
    int nroFrame = 3; // Por ejemplo, puedes cambiar este valor
    buffer(nroFrame);
    return 0;
}