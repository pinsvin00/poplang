
// #ifndef BASE_COMMON_H
// #define BASE_COMMON_H

// #include <iostream>
// #include <vector>
// #include <map>
// #include <memory>
// #include "expression_common.hpp"
// #include "function_registry.hpp"
// namespace Base {

//     const std::string CHARFIELD = "CHARFIELD";
//     const std::string INT = "INT";

//     class Field {
//     public:
//         std::string raw_field;
//         std::string name;
//         std::vector<std::string> parameters;
//         Field(std::string field_raw);
//     };
//     class RecordField : public Field {
//         public:
//         unsigned int size;
//         unsigned int nth;
//         unsigned int offset;
//         bool can_be_null = true;

//         std::string data_type;

//         RecordField(std::string field_raw) : Field(field_raw) {
//             try {
//                 this->size = std::stoi(parameters[1]);
//                 this->data_type = this->parameters[0];
//             }
//             catch(const std::invalid_argument & e) {
//                 std::cout << e.what() << std::endl;
//             }

//             if(this->parameters.size() > 3) {
//                 this->can_be_null = (this->parameters[3] == "NULLABLE");
//             }

//         }
//     };

//     class RecordDefinition {
//         public:
//         std::vector<RecordField> fields;
//         size_t size = 0;
//     };


//     class Info {
//     public:
//         std::string name;
//         std::string definition_path;
//         std::string data_path;
//         Base::RecordDefinition record_definition;
//         std::vector<Field> options;
        
//         bool is_busy;
//         int record_size;
//         int definition_end_byte;
//         unsigned long long int record_count;

//         Info(std::string name, std::string definition_path, Base::RecordDefinition record_definition) {
//             this->name = name;
//             this->definition_path = definition_path;
//             this->record_definition = record_definition;
//         }
//         Info() = default;

//         void load();
//     };


    

//     class Record {
//         public:
//         //we should move record definition to the shared_ptr, definitely
//         RecordDefinition definition;
//         std::vector<char> data;
//     };

    


//     enum class EventType {
//         InvokeFunction,
//     };

//     class Event {
//         public:
//         std::vector<Token> arguments;
//         EventType type;
//         Event() = default;
//     };

//     class FunctionDispatchEvent : public Event {
//         public:
//         std::vector<Token> results;
//         NativeFunctionIdentifier function_identifier;
//         FunctionDispatchEvent(const std::string& function_name, std::vector<Token> args) {
//             std::map<std::string, NativeFunctionIdentifier> function_name_identifier = {
//                 {"INSERT_BEGIN",    Base::NativeFunctionIdentifier::INSERT_BEGIN},
//                 {"INSERT",          Base::NativeFunctionIdentifier::INSERT},
//                 {"SELECT_NTH",      Base::NativeFunctionIdentifier::SELECT_NTH},
//                 {"DELETE_NTH",      Base::NativeFunctionIdentifier::DELETE_NTH},
//                 {"POP",             Base::NativeFunctionIdentifier::POP},
//             };
//             this->arguments = args;
//             this->function_identifier = function_name_identifier[function_name];
//         }
//     };

// }

// #endif