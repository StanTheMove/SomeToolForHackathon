#pragma once
#include "BIN_class_struct.hpp"
#include "Data.hpp"

class Parser_for_BIN_class {
public:
// Метод для парсингу даних з BIN файлу
    static void parse(BIN_READER& reader, Data& storage);
};