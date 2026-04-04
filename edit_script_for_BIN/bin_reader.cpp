#include "BIN_class_struct.hpp"
// конструктор для відкриття файлу
BIN_READER::BIN_READER(const string& filename) {
    file.open(filename, ios::binary);
}
// метод для перевірки чи файл відкритий
bool BIN_READER::isOpen() {
    return file.is_open();
}
// метод для читання байтів з файлу
bool BIN_READER::readBytes(char* buffer, size_t size) {
    file.read(buffer, size);

    if (!file) {
        return false;
    }

    return true;