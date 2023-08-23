#include <iostream>
#include <sstream>
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
  void read_instance();
  void search(std::unordered_set<ullng> &R);
  DLX() {}
};

void DLX::read_instance() {
  std::string line;

  // read primary items
  while (std::getline(std::cin, line)) {
    // FIXME; we must remove leading white spaces
    if ('|' == line[0]) continue;

    std::string s;
    std::unordered_set<std::string> items;
    std::istringstream iss(line);
    while (iss >> s) {
      if (items.count(s)) {
	std::cerr << "The item \"" << s << "\" already exists!\n";
	exit(1);
      }
      items.insert(s);
      // item itm(s, );
    }
    break;
  }
}

void DLX::search(std::unordered_set<ullng> &R) {
  ;
}


int main()
{
  DLX d;
  d.read_instance();
  
  return 0;
}
