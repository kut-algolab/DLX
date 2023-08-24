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

  item(unsigned u = 0) : name("-"), llink(0), rlink(0) {} // header of items
  item(std::string n) :  name(n) {}
  item(std::string n, ullng prev, ullng next)
    : name(n), llink(prev), rlink(next) {}
  void set_llink(llng prev) { llink = prev; }
  void set_rlink(llng next) { rlink = next; }

  friend std::ostream& operator<<(std::ostream& ss, const item& itm) {
    ss << itm.name << " " << itm.llink << " " << itm.rlink;
    return ss;
  };

  
};

struct DLX {
  /*** data members ***/
  ullng N1 = 0; // # of primary items
  ullng N2 = 0; // # of secondary items
  ullng Z;  // last spacer address

  std::vector<item> items;
  std::vector<node> nodes;
  
  /*** member functions ***/
  void read_instance();
  void add_primary(std::string);
  // void add_primary(item);
  void add_secondary(std::string);
  void search(std::unordered_set<ullng> &);

  void print_items();
  DLX() {
    item itm(0);
    items.push_back(itm);
  }
};

// Primary items are must inserted before secondary items.
void DLX::add_primary(std::string name) {
  const llng i = N1 + 1;
  item itm(name, i-1, 0);
  items.push_back(itm);
  items[i-1].rlink = i;
  items[0].llink = i;
  ++N1;
}

// N = N1 + N2
void DLX::add_secondary(std::string name) {
  const llng i = N1 + N2 + 1;
  item itm(name);
  if (0 == N2) {
    itm.set_llink(i);
    itm.set_rlink(i);
  } else {
    itm.set_llink(i-1);
    itm.set_rlink(N1+1);
    items[i-1].rlink = i;
  }
  items.push_back(itm);
  items[N1+1].llink = i;
  ++N2;
}


// FIXME; we must check the syntax of the first line
void DLX::read_instance() {
  N1 = N2 = 0;
  std::string line;

  // read the first line (primary and secondary items)
  while (std::getline(std::cin, line)) {
    // FIXME; we must remove leading white spaces
    if ('|' == line[0]) continue;

    std::string s;
    std::unordered_set<std::string> items;
    std::istringstream iss(line);
    // read primary items
    while (iss >> s) {
      if (items.count(s)) {
	std::cerr << "The item \"" << s << "\" already exists!\n";
	exit(1);
      }
      if ("|" == s) {
	break;
      }
      items.insert(s);
      add_primary(s);
    }

    // read secondary items
    while (iss >> s) {
      if (items.count(s)) {
	std::cerr << "The item \"" << s << "\" already exists!\n";
	exit(1);
      }
      items.insert(s);
      add_secondary(s);
    }
    break;
  }
}

void DLX::print_items() {
  printf("print header \"(i) name llink rlink\"\n");
  for (ullng i = 0; i <= N1 + N2; ++i) {
    std::cout << "(" << i << ") " << items[i] << std::endl;
  }
}

void DLX::search(std::unordered_set<ullng> &R) {
  ;
}


int main()
{
  DLX d;
  d.read_instance();
  d.print_items();
  
  return 0;
}
