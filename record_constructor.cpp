#include "record_constructor.hpp"

Base::Record Base::RecordConstructor::construct(Base::RecordDefinition record_definition, 
                                                std::vector<Token> arguments) {
    std::vector<Base::RecordField> fields = record_definition.fields;
    Base::Record record;

    if(fields.size() != arguments.size()) {
        throw std::invalid_argument("RecordConstructor : ERROR! too few arguments for the record...");
    }

    std::vector<char> record_byte_arr(record_definition.size);

    int record_pointer = 0;
    for (size_t i = 0; i < arguments.size(); i++){
        Token element = arguments[i];
        Base::RecordField rf = fields[i];


        std::vector<char> bytes = this->field_to_bytes(rf, element);

        int to_full = rf.size - bytes.size(); 
        if(to_full < 0 ) {
            //more specific error
            std::cout << "RecordConstructor : ERROR too long record!! ";
        }

        for (size_t i = 0; i < bytes.size(); i++){
            record_byte_arr[record_pointer] = bytes[i];
            record_pointer++;
        }
        for (size_t i = 0; i < to_full; i++){
            record_byte_arr[record_pointer] = 0;
            record_pointer++;
        }
        
    }


    record.data = record_byte_arr;
    record.definition = record_definition;

    return record;
}


std::vector<char> Base::RecordConstructor::field_to_bytes(RecordField &field, Token & frag ) {
    if (field.data_type == INT) {
        int num = atoi(frag.value.c_str());
        return int_bytes(num);
    }
    else if (field.data_type == CHARFIELD) {
        std::vector<char> bytes(frag.value.begin(), frag.value.end());
        return bytes;
    }
    else {
        std::cout << "Unrecognized field type." << std::endl;
        return {};
    }

}