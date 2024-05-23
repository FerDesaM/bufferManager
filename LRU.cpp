#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <regex>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <cstdio>
#include <cstring>

// Helper function to check if a directory exists
bool directoryExists(const std::string &path) {
    struct stat info;
    return (stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR));
}

// Helper function to create a directory
bool createDirectory(const std::string &path) {
    return mkdir(path.c_str(), 0777) == 0;
}

std::vector<std::string> listFiles(const std::string &path) {
    std::vector<std::string> files;
    DIR *dir = opendir(path.c_str());
    if (dir == nullptr) {
        return files;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_REG) {
            files.push_back(entry->d_name);
        }
    }
    closedir(dir);
    return files;
}

class FileRenamer {
public:
    FileRenamer(const std::string &sourceDir, const std::string &destDir)
        : sourceDir(sourceDir), destDir(destDir) {}

    void renameFiles() {
        // Create the destination directory if it doesn't exist
        if (!directoryExists(destDir)) {
            createDirectory(destDir);
        }

        std::regex pattern(R"(bloque_(\d+)\.txt)");
        for (const auto &fileName : listFiles(sourceDir)) {
            std::smatch match;
            if (std::regex_match(fileName, match, pattern)) {
                std::string number = match[1].str();
                std::string newFileName = "pagina_" + number + ".txt";
                std::string sourceFilePath = sourceDir + "/" + fileName;
                std::string destFilePath = destDir + "/" + newFileName;
                std::ifstream src(sourceFilePath, std::ios::binary);
                std::ofstream dst(destFilePath, std::ios::binary);
                dst << src.rdbuf();
                src.close();
                dst.close();
            }
        }
    }

private:
    std::string sourceDir;
    std::string destDir;
};

class DirectoryCreator {
public:
    DirectoryCreator(const std::string &baseDir)
        : baseDir(baseDir) {}

    void createStructure() {
        std::string bufferDir = baseDir + "/Buffer";
        std::string tableFile = baseDir + "/Tabla.txt";

        if (!directoryExists(baseDir)) {
            createDirectory(baseDir);
        }
        if (!directoryExists(bufferDir)) {
            createDirectory(bufferDir);
        }
        std::ofstream file(tableFile); // This line creates the file
        file.close();
    }

private:
    std::string baseDir;
};

void menu() {
    std::string sourceDir = "Bloques";
    std::string destDir = "Paginas";
    std::string memoryDir = "Memoria";

    FileRenamer renamer(sourceDir, destDir);
    renamer.renameFiles();

    DirectoryCreator creator(memoryDir);
    creator.createStructure();

    std::cout << "Files have been renamed and copied successfully." << std::endl;
    std::cout << "Memory structure has been created successfully." << std::endl;

    int opcionMenu;
    do {
        std::cout << "Menu:" << std::endl;
        std::cout << "1. Buffer" << std::endl;
        std::cout << "0. Salir" << std::endl;
        std::cout << "Opcion: ";
        std::cin >> opcionMenu;
        switch (opcionMenu) {
        case 1:
            int nroF, capacidadF;
            std::cout << "Menu:" << std::endl;
            std::cout << "Ingrese la cantidad de frames: " << std::endl;
            std::cin >> nroF;
            std::cout << "Ingrese la capacidad de cada frame: " << std::endl;
            std::cin >> capacidadF;
            //BufferManager bufferManager(nroF, capacidadF);
            break;
        case 0:
            break;
        default:
            std::cout << "Opcion no valida." << std::endl;
        }
    } while (opcionMenu != 0);
    std::cout << "Programa terminado." << std::endl;
}

int main() {
    menu();
    return 0;
}