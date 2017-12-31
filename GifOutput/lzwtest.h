#ifndef LZWTEST_H
#define LZWTEST_H

#include <QDataStream>
#include <algorithm>
#include <bitset>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

void tostream(std::vector<char> &bitstream, std::bitset<8> &buff, int &buff_pos,
              int cnt, int &len_of_code, int x);
void encode(int min_code_size, char *before, int len_of_before,
            QDataStream &outs);
int initmap(int min_code_size, std::map<std::vector<int>, int> &m);
#endif  // LZWTEST_H
