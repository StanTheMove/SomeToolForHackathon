#pragma once
#include <fstream>
#include <string>

using namespace std;

class BIN_READER {
private:
    ifstream file;

public:
    BIN_READER(const string& filename);

    bool isOpen();

    bool readBytes(char* buffer, size_t size);
};