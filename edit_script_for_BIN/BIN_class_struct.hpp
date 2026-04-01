#pragma once
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

//оголошення класу BIN_READER

class BIN_READER {
private:
fstream file;

public:
// конструктор для відкриття файлу
BIN_READER (const string& filename);

bool isOpen ();

bool hasData();

void readBytes (char* buffer,size_t size);

};