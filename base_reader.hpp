#ifndef DB_READER
#define DB_READER

#include <vector>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <utility>

#include "utils.hpp"
#include "base_common.hpp"

namespace Base {
    class Reader {
        public:

            const int DEFINITION_READING_BIT = 0;
            const int FIELDS_READING_BIT = 1;
            unsigned int stream_controller = 0;
            std::string definition_path;
            std::string base_data_path;
            bool terminate = false;

            Info load();
            void process_line(const std::string& line);
            Info current_info;
            void mutate_stream(const std::string& markup_name, bool terminating);
            Reader() = default;
            Reader(std::string definition_path);
    };



}



#endif