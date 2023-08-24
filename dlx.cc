#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>

typedef unsigned long long ullng;
typedef long long llng;

struct node {
  llng top;
  llng ulink;
  llng dlink;

  node(int i) : ulink(-100), dlink(-100) {
    if (-1 == i) top = 0; // spacer node
    else top = -100; // header node
  }
  node(llng t, llng up, llng down) : top(t), ulink(up), dlink(down) {}
  
  friend std::ostream& operator<<(std::ostream& ss, const node& node) {
    if (-100 == node.top) ss << "-";
    else ss << node.top;
    if (-100 == node.ulink) ss << " -";
    else ss << " " << node.ulink;
    if (-100 == node.dlink) ss << " -";
    else ss << " " << node.dlink;
    return ss;
  };
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
  ullng Z = 0;  // last spacer address

  std::vector<item> items;
  std::vector<node> nodes;
  std::unordered_map<std::string, ullng> names;
  
  /*** member functions ***/
  void read_instance();
  void add_primary_to_header(std::string);
  void add_secondary_to_header(std::string);
  void set_header_items();
  void search(std::unordered_set<ullng> &);

  void print_items();
  void print_nodes();
  
  DLX() {
    item itm(0);
    items.push_back(itm);

    node node(0);
    nodes.push_back(node);
  }
};

// Primary items are must inserted before secondary items.
void DLX::add_primary_to_header(std::string name) {
  const llng i = N1 + 1;
  item itm(name, i-1, 0);
  items.push_back(itm);
  items[i-1].rlink = i;
  items[0].llink = i;
  names[name] = i;
  ++N1;
}

// N = N1 + N2
void DLX::add_secondary_to_header(std::string name) {
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
  names[name] = i;
  ++N2;
}

void DLX::set_header_items() {
  for (ullng i = 1; i <= N1 + N2; ++i) {
    node x(0, i, i);
    nodes.push_back(x);
    ++Z;
  }
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
      add_primary_to_header(s);
    }

    // read secondary items
    while (iss >> s) {
      if (items.count(s)) {
	std::cerr << "The item \"" << s << "\" already exists!\n";
	exit(1);
      }
      items.insert(s);
      add_secondary_to_header(s);
    }
    break;
  }
  set_header_items();
  
  // add spacer node
  {
    node tmp(-1);
    nodes.push_back(tmp);
    ++Z;
  }
    
  llng ptr_spacer = Z;
  
  // read options
  while (std::getline(std::cin, line)) {
    std::cout << Z << ": " << line << std::endl;
    if ('|' == line[0]) {
      continue;
    }
    std::istringstream iss(line);
    std::string s;
    while (iss >> s) {
      const llng x = Z + 1;
      llng t = names[s];
      llng u = nodes[t].ulink;
      node tmp(t, u, t);
      nodes.push_back(tmp);
      nodes[u].dlink = x;
      nodes[t].ulink = x;
      nodes[t].top += 1;
      ++Z;
    }
    
    // add spacer node
    node tmp(nodes[ptr_spacer].top-1, ptr_spacer+1, -100);
    nodes[ptr_spacer].dlink = Z+1;
    nodes.push_back(tmp);
    ++Z;
    ptr_spacer = Z;
  }
}

void DLX::print_items() {
  printf("###### Print header #####\n\"(i) name llink rlink\"\n");
  for (ullng i = 0; i <= N1 + N2; ++i) {
    std::cout << "(" << i << ") " << items[i] << std::endl;
  }
}

void DLX::print_nodes() {
  printf("###### Print node #####\n");
  for (ullng i = 0; i <= N1 + N2; ++i) 
    std::cout << "(" << i << " " << nodes[i] << ") ";
  std::cout << std::endl;
  
  for (ullng i = N1+N2+1; i <= Z; ++i) {
    std::cout << "(" << i << " " << nodes[i] << ") ";
    if (nodes[i].top <= 0) std::cout << std::endl;
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
  d.print_nodes();
  
  return 0;
}
