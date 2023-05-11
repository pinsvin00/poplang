#include "base.hpp"


std::fstream Base::Engine::obtain_base_file_ptr() {
    std::fstream fs;
    fs.open(this->base_file_path, std::fstream::in | std::fstream::out | std::fstream::app);
    return fs;
}

std::fstream Base::Engine::obtain_temp_file_ptr() {
    std::fstream fs;
    fs.open(this->temp_file_path, std::fstream::in | std::fstream::out | std::fstream::app);
    return fs;
}


Base::Record Base::Engine::pop_record() {
    std::fstream temp_file = this->obtain_temp_file_ptr();
    std::fstream base_file = this->obtain_base_file_ptr();
    std::string popped_record;
    getline(base_file, popped_record);
    
    std::string line;
    while(getline(base_file, line)) {
        temp_file << line << std::endl;
    }

    Record record = Record();
    record.data = std::vector<char>(popped_record.begin(), popped_record.end());
    record.definition = this->info->record_definition;
    
    temp_file.close();
    base_file.close();

    this->swap_buffer_file();

    return record;
}

void Base::Engine::insert_begin(std::shared_ptr<Record> record) {
    std::fstream temp_file = this->obtain_temp_file_ptr();
    std::fstream base_file = this->obtain_base_file_ptr();

    for(const auto& element : record->data) {
        temp_file << (char)element;
    }
    temp_file << std::endl;

    std::string buffer;
    while(getline(base_file, buffer)) {
        temp_file << buffer << std::endl;
    }

    temp_file.close();
    base_file.close();

    this->swap_buffer_file();
}

void Base::Engine::insert(std::shared_ptr<Record> record) {
    std::fstream base_file = this->obtain_base_file_ptr();
    for(const auto& element : record->data) {
        base_file << (char)element;
    }
    
    base_file << std::endl;
    base_file.close();
}

void Base::Engine::entry(Base::FunctionDispatchEvent event) {
    Base::RecordConstructor constructor;
    std::shared_ptr<Record> record = std::make_shared<Record>(constructor.construct(this->info->record_definition, event.arguments));

    if(event.function_identifier == Base::NativeFunctionIdentifier::INSERT_BEGIN) {
        this->insert_begin(record);
    }
    else if(event.function_identifier == Base::NativeFunctionIdentifier::INSERT) {
        this->insert(record);
    }
    else if(event.function_identifier == Base::NativeFunctionIdentifier::SELECT_NTH) {
        int nth = std::stoi(event.arguments[0].value);
        Record selected = this->select_nth(nth);
    }
    else if(event.function_identifier == Base::NativeFunctionIdentifier::DELETE_NTH) {
        int nth = std::stoi(event.arguments[0].value);
        this->delete_nth(nth);
    }
    else if(event.function_identifier == Base::NativeFunctionIdentifier::POP_NTH) {
        int nth = std::stoi(event.arguments[0].value);
        Record popped = this->pop_nth(nth);
    }
    else if(event.function_identifier == Base::NativeFunctionIdentifier::POP) {
        Record popped = this->pop_record();
    }
    std::cout << "Success!" << std::endl;
}

void Base::Engine::swap_buffer_file() {
    remove(this->base_file_path.c_str());
    rename(this->temp_file_path.c_str(), this->base_file_path.c_str());
}

std::map<std::string, std::string> Base::Engine::record_to_map(const Base::Record &record)  {
    RecordDefinition definition =  record.definition;
    std::map<std::string, std::string> record_map;
    size_t record_ptr = 0;
    for(const auto & element : definition.fields) {
        std::string value;
        for (int i = 0; i < element.size; i++, record_ptr++) {
            if(record.data[record_ptr] == 0x0) {
                record_ptr += (element.size - i);
                break;
            }
            value += record.data[i];
        }
        record_map[element.name] = value;
        value.clear();
    }

    return record_map;
}


Base::Record Base::Engine::delete_nth(size_t nth) {
    return Base::Record();
}

Base::Record Base::Engine::select_nth(size_t nth) {
    return Base::Record();
}

Base::Record Base::Engine::pop_nth(size_t nth) {
    return Base::Record();
}


Base::Engine::Engine(std::shared_ptr<Base::Info> info, Base::EngineMode mode)  {
    this->info = std::move(info);
    this->mode = mode;
    this->base_file_path = this->info->data_path;
    this->temp_file_path = this->info->data_path + "temp";
}

