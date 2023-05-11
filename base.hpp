#ifndef BASE_H
#define BASE_H

#include <iostream>
#include <utility>
#include <vector>
#include <memory>
#include <fstream>
#include <queue>
#include <map>
#include "base_common.hpp"
#include "record_constructor.hpp"

namespace Base {
    enum class EngineMode {
        MEMORY, 
        FILE,
    };
    class Engine {
        std::shared_ptr<Base::Info> info;
        EngineMode mode = EngineMode::MEMORY;

        std::string temp_file_path;
        std::string base_file_path;
        public:
        std::map<std::string, std::string> record_to_map(const Record& record);
        Engine() = default;
        void entry(FunctionDispatchEvent event);
        Engine(std::shared_ptr<Base::Info> info, EngineMode mode);
        std::fstream obtain_base_file_ptr();
        std::fstream obtain_temp_file_ptr();
        void swap_buffer_file();
        Record pop_record();
        Record pop_nth(size_t nth);
        Record select_nth(size_t nth);
        Record delete_nth(size_t nth);
        void   insert_begin(std::shared_ptr<Record> record);
        void   insert(std::shared_ptr<Record> record);
    };

    class InMemoryEngine : public Engine {

    };

    class DiskEngine : public Engine {

    };
}

#endif