#include <QDataStream>
#include <QDebug>
#include <algorithm>
#include <bitset>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

struct Comparator {
  std::size_t operator()(std::vector<int> const &vec) const {
    std::size_t seed = vec.size();
    for (auto &i : vec) {
      seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
};

#define psi pair<string, int>
#define msi map<string, int>
#define vi vector<int>
#define vc vector<char>
#define mvii unordered_map<vi, int, Comparator>
#define x first
#define y second
#define pb push_back
#define mp make_pair



using namespace std;
void writeData(vc bitstream, QDataStream &outs);
void tostream(vc &bitstream, bitset<8> &buff, int &buff_pos, int cnt,
              int &len_of_code, int x);
int initmap(int min_code_size, mvii &m);
void encode(int min_code_size, char *befor, int len_of_before,
            QDataStream &outs) {
  // qDebug()<<"min_code_size="<<min_code_size;
  // init
  vc bitstream;
  vi before;
  for (int i = 0; i < len_of_before; i++) {
    int a = (int)(unsigned char)befor[i];
    before.push_back(a);
  }
  mvii m;
  mvii::iterator it;
  bitstream.clear();
  bitset<8> stream_buff;
  int buff_pos = 0;
  for (it = m.begin(); it != m.end(); it++) m.erase(it->x);
  // cnt+=2;//clear_code eof_code;
  int cnt = initmap(min_code_size, m);
  int len_of_bit = min_code_size + 1;
  tostream(bitstream, stream_buff, buff_pos, cnt, len_of_bit,
           (1 << min_code_size));
  // encode
  for (int i = 0; i < len_of_before;) {
    vi buff;
    buff.pb(before[i]);
    vi match;

    int sp = 0;

    while (m.find(buff) != m.end() && i + sp + 1 < len_of_before) {
      match.pb(before[i + sp]);
      buff.pb(before[i + sp + 1]);
      sp++;
    }
    tostream(bitstream, stream_buff, buff_pos, cnt, len_of_bit, m[match]);
    if (cnt < 4095) {
      if (i + sp + 1 != len_of_before) {
        m.insert(mp(buff, cnt++));
      }
    } else {
      cnt = initmap(min_code_size, m);

      tostream(bitstream, stream_buff, buff_pos, cnt, len_of_bit,
               (1 << min_code_size));
      len_of_bit = min_code_size + 1;
    }
    i += sp;
    if (sp == 0) break;
  }
  tostream(bitstream, stream_buff, buff_pos, cnt, len_of_bit,
           (1 << min_code_size) + 1);

  for (int i = buff_pos; i < 8; i++) stream_buff.reset(i);
  char ch = (int)stream_buff.to_ulong();
  bitstream.pb(ch);

  writeData(bitstream, outs);
}
void writeData(vc bitstream, QDataStream &outs) {
  outs << (quint8)8;
  int imageDataSize = bitstream.size();
  int subBlockSize = 255;
  int blockNum = imageDataSize / subBlockSize;
  for (int i = 0; i < blockNum; i++) {
    outs << (quint8)subBlockSize;
    for (int j = 0; j < subBlockSize; j++) {
      outs << (quint8)bitstream[i * subBlockSize + j];
    }
  }
  int legacy = imageDataSize % subBlockSize;
  if (legacy > 0) {
    outs << (quint8)legacy;
    for (int i = 0; i < legacy; i++) {
      outs << (quint8)bitstream[subBlockSize * blockNum + i];
    }
  }
  outs << (quint8)0;
}
void tostream(vc &bitstream, bitset<8> &buff, int &buff_pos, int cnt,
              int &len_of_code, int x) {
  char temp[32];
  for (int i = 0; i < 32; i++) temp[i] = '0';

  int pos = 31;
  while (x) {
    temp[pos--] += x % 2;
    x /= 2;
  }
  /*  cout<<"temp";
  for(int i=0;i<len_of_code;i++)
      printf("%c",temp[31-i]);
  cout<<endl;*/
  pos = 31;
  for (int i = len_of_code; i > 0; i--) {
    if (temp[pos] == '1')
      buff.set(buff_pos);
    else
      buff.reset(buff_pos);
    buff_pos++;
    if (buff_pos == 8) {
      buff_pos = 0;

      char ch = (int)buff.to_ulong();
      bitstream.pb(ch);
      /*  cout<<buff<<" ";
      cout<<endl;*/
    }
    pos--;
  }
  if (cnt == 1 << len_of_code) len_of_code++;
}
int initmap(int min_code_size, mvii &m) {
  m.clear();
  int cnt = 0;
  for (cnt = 0; cnt < (1 << min_code_size); cnt++) {
    vi temp;
    temp.clear();
    temp.pb(cnt);
    m.insert(mp(temp, cnt));
  }
  cnt += 2;
  return cnt;
}
