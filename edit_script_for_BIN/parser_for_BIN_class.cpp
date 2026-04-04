#include "parser_for_BIN_class.hpp"
#include <map>
#include <string>
#include <iostream>

#define HEADER1 0xA3
#define HEADER2 0x95

using namespace std;

void Parser_for_BIN_class::parse(BIN_READER& reader, Data& storage) {

    map<int, string> msgTypes;
    map<int, int> msgLengths;

    unsigned char byte;

    while (reader.readBytes((char*)&byte, 1)) {

        if (byte != HEADER1) continue;

        if (!reader.readBytes((char*)&byte, 1)) break;
        if (byte != HEADER2) continue;

        unsigned char msg_id;
        if (!reader.readBytes((char*)&msg_id, 1)) break;

        if (msg_id == 0x80) {

            char buffer[64];

            if (!reader.readBytes(buffer, sizeof(buffer)))
                break;

            int id = (unsigned char)buffer[0];
            int length = (unsigned char)buffer[1];
            string name(buffer + 2, 4);

            msgTypes[id] = name;
            msgLengths[id] = length;

            cout << "FMT: ID=" << id << " NAME=" << name << " LEN=" << length << endl;
        }

        else {

            if (msgTypes.find(msg_id) == msgTypes.end()) {
            continue;
            }

            string type = msgTypes[msg_id];
            int len = msgLengths[msg_id];

            if (type == "GPS") {

                vector<char> buffer(len);

                if (!reader.readBytes(buffer.data(), len))
                    break;

                storage.gps.push_back({0,0});

                cout << "GPS packet found (len=" << len << ")\n";
            }

            else if (type == "IMU") {

                vector<char> buffer(len);

                if (!reader.readBytes(buffer.data(), len))
                    break;

                storage.imu.push_back({0,0,0});

                cout << "IMU packet found (len=" << len << ")\n";
            }

            else {
                vector<char> skip(len);
                reader.readBytes(skip.data(), len);
            }
        }
    }
}