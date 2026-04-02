#include "BIN_class_struct.hpp"

BIN_READER::BIN_READER(const string& filename) {
    file.open(filename, ios::binary);
}

bool BIN_READER::isOpen() {
    return file.is_open();
}

bool BIN_READER::readBytes(char* buffer, size_t size) {
    file.read(buffer, size);

    if (!file) {
        return false;
    }

    return true;
}
    

