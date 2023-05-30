#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cstring>

#define SET_BIT(BF, N) BF |= 1UL << N;
#define CLR_BIT(BF, N) BF &= ~(1UL << N)
#define IS_BIT_SET(BF, N) ((BF >> N) & 0x1)


std::string ltrim(const std::string &s);
std::string rtrim(const std::string &s);
std::string trim(const std::string &s);
void debug_log(const std::string & log);

bool prefix(const std::string &a, const std::string &b );
std::vector<char> int_bytes(int param);
std::vector<char> long_int_bytes(long long int param);
int32_t bytes_to_int(char* buffer);
char* int_to_bytes(int32_t some);
std::vector<std::string> split(std::string text, char delim);


#endif