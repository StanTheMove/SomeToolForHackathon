#pragma once
#include <iostream>
#include "BIN_class_struct.hpp"
#include "bin_reader.cpp"

class parser_for_BIN_class {
public:

    static void parse (BIN_READER& reader,Data& storage);

};
