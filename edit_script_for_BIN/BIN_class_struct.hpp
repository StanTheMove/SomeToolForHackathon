#pragma once
#include <fstream>
#include <string>

using namespace std;
// Клас для читання BIN файлу
class BIN_READER {
private:
    ifstream file;

public:
// Конструктор для відкриття файлу
    BIN_READER(const string& filename);
// Метод для перевірки чи файл відкритий
    bool isOpen();
// Метод для читання байтів з файлу
    bool readBytes(char* buffer, size_t size);
};