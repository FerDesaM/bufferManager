#ifndef PAGE_H
#define PAGE_H
#include <unordered_map>
#include <string>
#include <iostream>

struct PageEntry {
    int frameId;
    int pinCount;
    bool dirtyBit;
};

class PageTable {
public:
    void addEntry(const std::string& pageId, int frameId) {
        table[pageId] = {frameId, 0, false}; // Inicializamos pinCount a 0 y dirtyBit a false
    }

    void removeEntry(const std::string& pageId) {
        table.erase(pageId);
    }

    PageEntry* getEntry(const std::string& pageId) {
        if (table.find(pageId) != table.end()) {
            return &table[pageId];
        }
        return nullptr; // Indica que la página no está en la tabla
    }

    std::unordered_map<std::string, PageEntry>& getTable() {
        return table;
    }

private:
    std::unordered_map<std::string, PageEntry> table;
};
#endif 