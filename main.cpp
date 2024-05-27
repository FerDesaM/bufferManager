#include "BufferManager.h"
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
void crearDirectorio(const std::string& nombre) {
    if (mkdir(nombre.c_str(), 0777) == -1) {
        if (errno != EEXIST) {
            std::cerr << "Error creando el directorio " << nombre << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

int menuConsultaID() {
    int id;
    std::cout << "Buscar ID: ";
    std::cin >> id;
    return id;
}

char menuOperacion() {
    char op;
    std::cout << "Operación (L para Leer, W para Escribir): ";
    std::cin >> op;
    return op;
}

void menu() {
    int nroFrames;
    std::cout << "Ingrese la cantidad de frames: ";
    std::cin >> nroFrames;

    crearDirectorio("Memoria");

    BufferManager bufferManager(nroFrames);

    int opcionMenu;
    do {
        std::cout << "Menu:" << std::endl;
        std::cout << "1. Consultar ID" << std::endl;
        std::cout << "2. Imprimir Tabla" << std::endl;
        std::cout << "3. Imprimir Contenido del Buffer" << std::endl;
        std::cout << "4. Leer contenido de un frame" << std::endl;
        std::cout << "5. Escribir contenido en un frame" << std::endl;
        std::cout << "0. Salir" << std::endl;
        std::cin >> opcionMenu;

        switch(opcionMenu) {
            case 1: {
                int id = menuConsultaID();
                char operation = menuOperacion();
                bufferManager.consultarID(id, operation);
                break;
            }
            case 2:
                bufferManager.printTable();
                break;
            case 3:
                bufferManager.printBuffer();
                break;
            case 4: {
                int frameId;
                std::cout << "Ingrese el ID del frame a leer: ";
                std::cin >> frameId;
                std::string content = bufferManager.read(frameId);
                std::cout << "Contenido del frame " << frameId << ":\n" << content << std::endl;
                break;
            }
            case 5: {
                int frameId;
                std::string newContent;
                std::cout << "Ingrese el ID del frame a escribir: ";
                std::cin >> frameId;
                std::cin.ignore(); // Ignorar el newline restante
                std::cout << "Ingrese el nuevo contenido: ";
                std::getline(std::cin, newContent);
                bufferManager.write(frameId, newContent);
                break;
            }
            case 0:
                std::cout << "Saliendo del programa." << std::endl;
                break;
            default:
                std::cout << "Opción inválida. Intente de nuevo." << std::endl;
        }
    } while(opcionMenu != 0);
}

int main() {
    menu();
    return 0;
}