#ifndef BUFFER_H
#define BUFFER_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <deque>
#include <algorithm>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include "pagetable.h"
struct Frame {
    int frameId;
    std::string pageId;
    std::string content;
    bool pinned;
};

class BufferManager {
public:
    BufferManager(int numFrames) : numFrames(numFrames), miss(0), hits(0) {
        frames.resize(numFrames);
        for (int i = 0; i < numFrames; ++i) {
            frames[i].frameId = i;
            frames[i].pageId = "";
            frames[i].pinned = false;
        }
    }

    void consultarID(int id, char operation) {
        std::string idStr = std::to_string(id);
        bool found = false;

        // Verificar si el ID ya está en el buffer usando la PageTable
        PageEntry* entry = pageTable.getEntry(idStr);
        if (entry) {
            std::cout << "ID encontrado en el buffer en el frame: " << entry->frameId << std::endl;
            if (operation == 'L'||operation=='l') {
                entry->pinCount++;
                
                frames[entry->frameId].pinned = false;
            } else if (operation == 'W'||operation=='w') {
                entry->pinCount++;
                entry->dirtyBit = true;
                frames[entry->frameId].pinned = true;
            }
            updateLRU(entry->frameId);
            found = true;
            hits++;
        }

        if (!found) {
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
                        std::string content;
                        while (std::getline(inFile, line)) {
                            content += line + "\n";
                            size_t pos = line.find(',');
                            if (pos != std::string::npos) {
                                std::string firstWord = line.substr(0, pos);
                                if (idStr == firstWord) {
                                    std::cout << "ID encontrado en el archivo: " << fileName << std::endl;
                                    std::cout << "Contenido de la linea: " << line << std::endl;
                                    cargarEnBuffer(idStr, fileName, content, operation);
                                    found = true;
                                    break;
                                }
                            }
                        }
                        inFile.close();
                    }
                    if (found) break;
                }
                closedir(dir);
            }

            if (!found) {
                std::cout << "ID no encontrado en ningun archivo." << std::endl;
                miss++;
            }
        }
    }

    void cargarEnBuffer(const std::string& pageId, const std::string& fileName, const std::string& content, char operation) {
        int frameToReplace = encontrarReemplazarFrame();
        frames[frameToReplace].pageId = pageId;
        frames[frameToReplace].content = content;
        frames[frameToReplace].pinned = true;

        pageTable.addEntry(pageId, frameToReplace);
        PageEntry* entry = pageTable.getEntry(pageId);
        if (entry) {
            if (operation == 'L') {
                entry->pinCount++;
            } else if (operation == 'W') {
                entry->pinCount++;
                entry->dirtyBit = true;
            }
        }

        std::cout << "Page " << pageId << " loaded into frame " << frameToReplace << std::endl;
        updateLRU(frameToReplace);
        printBuffer();
    }

    int encontrarReemplazarFrame() {
        for (int i = 0; i < numFrames; ++i) {
            if (frames[i].pageId.empty() || !frames[i].pinned) {
                return i;
            }
        }
        int lruFrame = lruQueue.back();
        lruQueue.pop_back();
        pageTable.removeEntry(frames[lruFrame].pageId);
        return lruFrame;
    }

    void updateLRU(int frameId) {
        lruQueue.erase(std::remove(lruQueue.begin(), lruQueue.end(), frameId), lruQueue.end());
        lruQueue.push_front(frameId);
    }

    void printBuffer() {
        std::cout << "Buffer status:" << std::endl;
        for (const auto& frame : frames) {
            if (!frame.pageId.empty()) {
                std::cout << "Frame ID: " << frame.frameId << ", Page ID: " << frame.pageId 
                          << ", Pinned: " << frame.pinned << std::endl;
                std::cout << "Content:\n" << frame.content << std::endl;
            }
        }
        std::cout << "Hits: " << hits << ", Misses: " << miss << std::endl;
    }

    void printTable() {
        std::cout << "Frame ID\tPage ID\tDirty Bit\tPin Count" << std::endl;
        for (int frameId : lruQueue) {
            for (const auto& frame : frames) {
                if (frame.frameId == frameId && !frame.pageId.empty()) {
                    PageEntry* entry = pageTable.getEntry(frame.pageId);
                    if (entry) {
                        std::cout << frame.frameId << "\t" 
                                  << frame.pageId << "\t" 
                                  << entry->dirtyBit << "\t" 
                                  << entry->pinCount << std::endl;
                    }
                }
            }
        }
    }

    std::string read(int frameId) {
        if (frameId >= 0 && frameId < numFrames && !frames[frameId].pageId.empty()) {
            PageEntry* entry = pageTable.getEntry(frames[frameId].pageId);
            if (entry) {
                entry->pinCount++;
                if (entry->pinCount == 0) {
                    frames[frameId].pinned = false;
                }
                updateLRU(frameId);
                return frames[frameId].content;
            }
        }
        return "Frame vacío o inválido.";
    }

    void write(int frameId, const std::string& newContent) {
        if (frameId >= 0 && frameId < numFrames && !frames[frameId].pageId.empty()) {
            frames[frameId].content = newContent;
            frames[frameId].pinned = true;
            PageEntry* entry = pageTable.getEntry(frames[frameId].pageId);
            if (entry) {
                entry->pinCount++;
                entry->dirtyBit = true;
            }
            guardarEnArchivo(frames[frameId].pageId, newContent);
            updateLRU(frameId);
        } else {
            std::cout << "Frame vacío o inválido." << std::endl;
        }
    }

    void guardarEnArchivo(const std::string& pageId, const std::string& content) {
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
                            if (pageId == firstWord) {
                                inFile.close();
                                std::ofstream outFile("Paginas/" + fileName);
                                outFile << content;
                                outFile.close();
                                std::cout << "Contenido actualizado en el archivo: " << fileName << std::endl;
                                return;
                            }
                        }
                    }
                    inFile.close();
                }
            }
            closedir(dir);
        }
    }

private:
    int numFrames;
    std::vector<Frame> frames;
    std::deque<int> lruQueue;
    PageTable pageTable;
    int miss;
    int hits;
};

#endif