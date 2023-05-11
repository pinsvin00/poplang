#include "utils.hpp"


const std::string WHITESPACE = " \n\r\t\f\v";
 
std::string ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}
 
std::string rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

void debug_log(const std::string & log) {
#ifdef DEBUG_LOGGING
    std::cout << "DEBUG LOG : " << log << std::endl;
#endif
}

std::string trim(const std::string &s) {
    return rtrim(ltrim(s));
}

bool prefix(const std::string &a, const std::string &b ) {
    return (b.compare(0, a.size(), a) == 0);
}


//TODO Implement solution for templates, although its fine as it is
std::vector<char> int_bytes(int param){
     std::vector<char> bytes(4);
     for (int i = 0; i < 4; i++)
         bytes[3 - i] = (param >> (i * 8));
     return bytes;
}

std::vector<char> long_int_bytes(long long int param) {
    std::vector<char> bytes(8);
    for (size_t i = 0; i < 8; i++){
        bytes[7 - i] = (param >> (i * 8));
    }
    return bytes;
    
}