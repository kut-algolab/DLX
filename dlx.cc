#include <iostream>
#include <vector>
#include <unordered_set>

typedef unsigned long long ullng;
typedef long long llng;

struct node {
  llng top;
  llng ulink;
  llng dlink;

  node() : top(-100), ulink(-100), dlink(-100) {}
  node(llng t, llng up, llng down) : top(t), ulink(up), dlink(down) {}
};

struct item {
  std::string name;
  llng llink;
  llng rlink;

  item() : name("null"), llink(-100), rlink(-100) {}
  item(std::string n, ullng prev, ullng next)
    : name(n), llink(prev), rlink(next) {}
};

struct DLX {
  /*** data members ***/
  ullng N1; // # of primary items
  ullng N2; // # of secondary items
  ullng Z;  // last spacer address

  std::vector<item> items;
  std::vector<node> nodes;
  
  /*** member functions ***/
  void search(std::unordered_set<ullng> &R);
  DLX() {}
};

void DLX::search(std::unordered_set<ullng> &R) {
  ;
}


int main()
{
  return 0;
}
