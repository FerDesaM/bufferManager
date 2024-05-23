#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
using namespace std;

void crearArchivo(const string &rutaArchivo) {
        ofstream archivo(rutaArchivo);
        cout << "Creando archivo: " << rutaArchivo << endl;
    }

void crearCarpeta(int num_bloques){
    string carpeta = "Bloques";
        mkdir(carpeta.c_str(), 0777);
        for (int sector = 0; sector < num_bloques; sector++) {
                        string archivobloque = carpeta + "/bloque_" + to_string(sector + 1) + ".txt";
                        crearArchivo(archivobloque);
                    }
}
void ingresar_datos(const std::string &archivoTxt, int num_bloques,int tamano_bloque){
    ifstream archivo(archivoTxt);
        ofstream bloques[100]; 
    string carpeta = "Bloques";
    for (int i = 0; i < num_bloques; ++i) {
        string archivoBloque = carpeta + "/bloque_" + to_string(i + 1) + ".txt";
        bloques[i].open(archivoBloque, ios::out | ios::app);
    }

    string linea;
    int bloque_actual = 0;
    size_t tamano_actual = 0;

    while (getline(archivo, linea)) {
        if (tamano_actual + linea.size() + 1 > tamano_bloque) {
            bloque_actual = (bloque_actual + 1) % num_bloques;
            tamano_actual = 0;
        }
        bloques[bloque_actual] << linea << "\n";
        tamano_actual += linea.size() + 1;
    }

    archivo.close();
    for (int i = 0; i < num_bloques; ++i) {
        bloques[i].close();
    }
        

}
int main(){
    crearCarpeta(12);
    ingresar_datos("titanic.csv", 16, 256);
    return 0;
}