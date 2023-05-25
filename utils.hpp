#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <vector>

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
int bytes_to_int(char* buffer);
char* int_to_bytes(int some);


#endif