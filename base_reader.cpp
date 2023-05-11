#include "base_reader.hpp"


Base::Field::Field(std::string field_raw) {
    size_t i = 0;
    std::string field_name;

    for (; i < field_raw.size(); i++){
        if(field_raw[i] == ':'){
            i++;
            break;
        } 
        else field_name += field_raw[i];
    }
    this->name = field_name;
    
    std::string value;
    for (; i < field_raw.size(); i++){
        if(field_raw[i] == ' ') {
            this->parameters.push_back(value);
            value = "";
            continue;
        }
        else value += field_raw[i];
    }
    if(!value.empty()) {
        this->parameters.push_back(value);
    }

}

Base::Info Base::Reader::load() {
    std::cout << "Loading base on path: " << this->definition_path << std::endl;
    std::fstream fs;
    fs.open(this->definition_path.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
    this->current_info.record_definition.size = 0;
    if(!fs.is_open()) {
        std::cout << "ERR! Failed to open database!" << std::endl;
        return this->current_info;
    }
    std::string buffer;
    while( getline(fs, buffer) ) {
        if(this->terminate) break;
        this->process_line(trim(buffer));
    }

    fs.close();

    this->current_info.data_path = this->base_data_path;
    std::cout << "Successfully loaded base" << std::endl;
    return this->current_info;
}

void Base::Reader::process_line(const std::string& line) {
    bool is_markup = prefix(";;", line);
    bool is_terminating = prefix(";;END", line);

    if(is_markup) {
        std::string markup_name;
        if(is_terminating) {
            markup_name = line.substr(6, line.length() - 4);
        }
        else {
            markup_name = line.substr(2, line.length() - 4);
        }
        this->mutate_stream(markup_name, is_terminating);
    }
    else {
        if (IS_BIT_SET(this->stream_controller, this->FIELDS_READING_BIT)) {
            RecordField f = RecordField(line);
            f.nth = this->current_info.record_definition.fields.size();
            this->current_info.record_definition.fields.push_back(f);
            this->current_info.record_definition.size += f.size;
        }
        else if(IS_BIT_SET( this->stream_controller, this->DEFINITION_READING_BIT) ) {
            Field f = Field(line);
            this->current_info.options.push_back(f);
        }
    }
}

Base::Reader::Reader(std::string definition_path) {
    this->stream_controller = 0;
    this->definition_path = definition_path;
    auto last_dot = definition_path.find_last_of('.') + 1;
    this->base_data_path = definition_path.replace(definition_path.begin() + last_dot, definition_path.end(), "popdb");
}

void Base::Reader::mutate_stream(const std::string &markup_name, bool terminating) {
    int nth_bit = 0;
    if(markup_name == "DEFINITION"){
        nth_bit = DEFINITION_READING_BIT;
    }
    else if(markup_name == "FIELDS") {
        nth_bit = FIELDS_READING_BIT;
    }

    if(terminating) {
        if(nth_bit == DEFINITION_READING_BIT) {
            this->terminate = true;
        }
        CLR_BIT(this->stream_controller, nth_bit);
    }
    else {
        SET_BIT(this->stream_controller, nth_bit);
    }
}