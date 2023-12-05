#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <map>

#define DUMMY (1U << 31) - 1

typedef unsigned long long ullng;
typedef long long llng;

struct node {
  int top;
  int ulink;
  int dlink;
  int color;

  node(char c) { // For node 0
    top = DUMMY;
    ulink = DUMMY;
    dlink = DUMMY;
    color = DUMMY;
  }
  
  node(int t) : top(t) { // For spacer node
  }
      
  node(int t, int up, int down, int color) :
    top(t), ulink(up), dlink(down), color(color) {}
};

struct item {
  std::string name;
  int llink;
  int rlink;

  item(char c) { // For item 0 and N1+N2+1
    name = "-";
    llink = 0;
    rlink = 0;
  }

  item(std::string s) {
    name = s;
  }
  
  item(std::string n, int prev, int next)
    : name(n), llink(prev), rlink(next) {}
};

struct DLC {
  /*** data members ***/
  int N1; // # of primary items
  int N2; // # of secondary items
  int Z;  // last spacer address

  std::vector<item> items;
  std::vector<node> nodes;
  std::map<std::string, int> names;
  std::vector<std::string> colors;

  ullng sols = 0; // # of solutions
  bool print_flag = false;
  
  /*** member functions ***/
  void init();
  void read_instance();
  void add_primary_to_header(std::string);
  void init_secondary_header();
  void add_secondary_to_header(std::string);
  void add_header_for_secondary();
  void init_nodes();

  void cover(const int);
  void hide(const int);
  void commit(const int, const int);
  void purify(const int);
  
  void uncover(const int);
  void unhide(const int);
  void uncommit(const int, const int);
  void unpurify(const int);

  void search(std::vector<int> &);
  int select_item();
  std::vector<std::vector<int>> collect_options(const int);

  void print_table();
  void print_option(int);
  void print_options(std::vector<int>&);
  void print_all_solutions();
  ullng get_num_of_solutions() { return sols; };
  
  DLC() {}
};

void DLC::init() {
  item itm('0');
  items.push_back(itm);

  node node('0');
  nodes.push_back(node);

  colors.push_back("-");
}


// // Primary items are must inserted before secondary items.
void DLC::add_primary_to_header(std::string name) {
  const int i = N1 + 1;
  item itm(name, i-1, 0);
  items.push_back(itm);
  items[i-1].rlink = i;
  items[0].llink = i;
  names[name] = i;
  ++N1;
}

// N = N1 + N2
void DLC::add_secondary_to_header(std::string name) {
  const int i = N1 + N2 + 1;
  item itm(name);
  names[name] = i;
  items.push_back(itm);
  if (0 == N2) {
    items[i].llink = i;
    items[i].rlink = i;
  } else {
    items[i].llink = i-1;
    items[i].rlink = N1+1;
    items[N1+i].llink = i;
    items[i-1].rlink = i;
  }
  ++N2;
}

void DLC::add_header_for_secondary() {
  if (0 != N2) {
    item itm('-');
    items.push_back(itm);

    // First secondary item
    items[N1+1].llink = N1+N2+1; 

    // Last secondary item
    items[N1+N2].rlink = N1+N2+1;

    // Header for secondary items
    items[N1+N2+1].llink = N1+N2; 
    items[N1+N2+1].rlink = N1+1;
  }
}

void DLC::init_nodes() {
  for (int i = 1; i <= N1+N2; ++i) {
    node node(0, i, i, DUMMY);
    nodes.push_back(node);
    Z += 1;
  }

  // First spacer node
  node node(0, DUMMY, DUMMY, DUMMY);
  nodes.push_back(node);
  Z += 1;
}

void DLC::print_table() {
  std::cout << "print table" << std::endl;

  // print items
  std::string number, name, ll, rl;
  number += "i:\t\t0\t";
  name += "NAME(i):\t-\t";
  ll += "LLINK(i):\t" + std::to_string(items[0].llink) + "\t";
  rl += "RLINK(i):\t" + std::to_string(items[0].rlink) + "\t";
  for (int i = items[0].rlink; i != 0; i = items[i].rlink) {
    number += std::to_string(i) + "\t";
    name += items[i].name + "\t";
    ll += std::to_string(items[i].llink) + "\t";
    rl += std::to_string(items[i].rlink) + "\t";
  }
  for (int i = items[N1+N2+1].rlink; i != N1+N2+1; i = items[i].rlink) {
    number += std::to_string(i) + "\t";
    name += items[i].name + "\t";
    ll += std::to_string(items[i].llink) + "\t";
    rl += std::to_string(items[i].rlink) + "\t";
  }
  number += std::to_string(N1+N2+1);
  name += items[N1+N2+1].name;
  ll += std::to_string(items[N1+N2+1].llink);
  rl += std::to_string(items[N1+N2+1].rlink);
  
  std::cout << number << std::endl << name << std::endl << ll << std::endl << rl << std::endl << std::endl;;

  // print options
  number.clear();
  std::string top, ul, dl, co;
  number += "i:\t\t";
  top += "LEN(i):\t\t";
  ul += "ULINK(i):\t";
  dl += "DLINK(i):\t";
  co += "COLOR(i):\t";
  for (int i = 0; i <= N1+N2+1; ++i) {
    number += std::to_string(i) + "\t";
    top += std::to_string(nodes[i].top) + "\t";
    ul += std::to_string(nodes[i].ulink) + "\t";
    dl += std::to_string(nodes[i].dlink) + "\t";
    co += std::to_string(nodes[i].color) + "\t";
  }
  std::cout << number << std::endl << top << std::endl << ul << std::endl << dl << std::endl << co << std::endl << std::endl;
  
  number.clear();
  top.clear();
  ul.clear();
  dl.clear();
  co.clear();
  number += "i:\t\t";
  top += "TOP(i):\t\t";
  ul += "ULINK(i):\t";
  dl += "DLINK(i):\t";
  co += "COLOR(i):\t";
  for (int i = N1+N2+2, col = 0; i <= Z; ++i, ++col) {
    number += std::to_string(i) + "\t";
    top += std::to_string(nodes[i].top) + "\t";
    ul += std::to_string(nodes[i].ulink) + "\t";
    dl += std::to_string(nodes[i].dlink) + "\t";
    co += std::to_string(nodes[i].color) + "\t";
    if (col == 15) {
      std::cout << number << std::endl << top << std::endl << ul << std::endl << dl << std::endl << co << std::endl << std::endl;
      number.clear();
      top.clear();
      ul.clear();
      dl.clear();
      co.clear();
      number += "i:\t\t";
      top += "TOP(i):\t\t";
      ul += "ULINK(i):\t";
      dl += "DLINK(i):\t";
      co += "COLOR(i):\t";
      col = -1;
    }
  }
  if (number != "i:\t\t") {
    std::cout << number << std::endl << top << std::endl << ul << std::endl
	      << dl << std::endl << co << std::endl << std::endl;
  }
}

// FIXME; we must check the syntax of the first line
void DLC::read_instance() {
  N1 = N2 = Z = 0;
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
    add_header_for_secondary();
    break;
  }
  
  init_nodes();

  // read options
  int ptr_spacer = Z;
  while (std::getline(std::cin, line)) {
    // std::cout << Z << ": " << line << std::endl;
    if ('|' == line[0]) {
      continue;
    }
    std::istringstream iss(line);
    std::string s;
    while (iss >> s) {
      int c = 0;
      std::string namae = "";
      std::string color = "";
      int pos;
      if (std::string::npos != (pos = std::string(s).find(':'))) { // with color
	for (int i = 0; i < pos; ++i) namae += s[i];
	for (int i = pos+1; i < s.size(); ++i) color += s[i];
	int i = 0;
	for ( ; i < colors.size(); ++i)
	  if (color == colors[i]) break;
	if (colors.size() == i) colors.push_back(color);
	c = i;
      } else {
	namae = s;
      }

      int t = names[namae]; // the id of the item whose name is name
      int u = nodes[t].ulink;
      node tmp(t, u, t, c);
      const int x = Z + 1;
      
      nodes.push_back(tmp);
      nodes[u].dlink = x;
      nodes[t].ulink = x;
      nodes[t].top += 1;
      ++Z;
    }
    
    // add spacer node
    node tmp(nodes[ptr_spacer].top-1, ptr_spacer+1, DUMMY, 0);
    nodes[ptr_spacer].dlink = Z;
    nodes.push_back(tmp);
    ++Z;
    ptr_spacer = Z;
  }
}



// select item i using the MRV heuristic
// if there is item i to be covered and its len is 0, then return -1
int DLC::select_item() {
  int ptr = items[0].rlink;
  int i = items[0].rlink;
  while (0 != ptr) {
    if (nodes[ptr].top < nodes[i].top) i = ptr;
    ptr = items[ptr].rlink;
  }
  if (0 == nodes[i].top) return -1;
  return i;
}

std::vector<std::vector<int>> DLC::collect_options(const int i) {
  std::vector<std::vector<int>> O;
  int p = nodes[i].dlink;
  while (i != p) {
    std::vector<int> o;
    o.push_back(p);
    int q = p+1;
    while (p != q) {
      if (nodes[q].top <= 0) {
	q = nodes[q].ulink;
	continue;
      }
      o.push_back(q);
      q += 1;
    }
    O.push_back(o);
    p = nodes[p].dlink;
  }
  return O;
}

void DLC::cover(const int i) {
  for (int p = nodes[i].dlink; i != p; p = nodes[p].dlink) hide(p);
  int l = items[i].llink, r = items[i].rlink;
  items[l].rlink = r;
  items[r].llink = l;
}

void DLC::hide(const int p) {
  for (int q = p+1; p != q; ) {
    int x = nodes[q].top;
    int u = nodes[q].ulink;
    int d = nodes[q].dlink;
    
    if (nodes[q].color < 0) {
      q += 1;
      continue;
    }
    
    if (x <= 0) q = u;
    else {
      nodes[u].dlink = d;
      nodes[d].ulink = u;
      nodes[x].top -= 1;
      q += 1;
    }
  }
}

void DLC::commit(const int p, const int j) {
  if (0 == nodes[p].color) cover(j);
  if (0 < nodes[p].color) purify(p);
}

void DLC::purify(const int p) {
  const int c = nodes[p].color;
  const int i = nodes[p].top;
  nodes[i].color = c;
  for (int q = nodes[i].dlink; i != q; q = nodes[q].dlink) {
    if (c == nodes[q].color) nodes[q].color = -1;
    else hide(q);
  }
}

void DLC::uncover(const int i) {
  int l = items[i].llink, r = items[i].rlink;
  items[l].rlink = i;
  items[r].llink = i;
  for (int p = nodes[i].ulink; i != p; p = nodes[p].ulink) unhide(p);
}

void DLC::unhide(const int p) {
  for (int q = p-1; p != q; ) {
    int x = nodes[q].top;
    int u = nodes[q].ulink;
    int d = nodes[q].dlink;

    if (nodes[q].color < 0) {
      q -= 1;
      continue;
    }
    
    if (x <= 0) q = d;
    else {
      nodes[d].ulink = q;
      nodes[u].dlink = q;
      nodes[x].top += 1;
      q -= 1;
    }
  }
}

void DLC::uncommit(const int p, const int j) {
  if (0 == nodes[p].color) uncover(j);
  if (0 < nodes[p].color) unpurify(p);
}

void DLC::unpurify(const int p) {
  const int c = nodes[p].color;
  const int i = nodes[p].top;
  for (int q = nodes[i].ulink; i != q; q = nodes[q].ulink) {
    if (nodes[q].color < 0) nodes[q].color = c;
    else unhide(q);
  }
}

void DLC::search(std::vector<int> &R) {
  // if no remaining items in items then output R and return
  if (0 == items[0].rlink) {
    if (print_flag) print_options(R);
    sols += 1;
    return;
  }

  // select item i
  const llng i = select_item();
  if (-1 == i) return;

  // std::cout << "select item " << i << std::endl;

  // collect the set of remaining options having i
  std::vector<std::vector<int>> O = collect_options(i);

  cover(i);
  for (auto X : O) {
    R.push_back(X[0]);

    for (int p = X[0]+1; X[0] != p; ) {
      if (nodes[p].top <= 0) {
	p = nodes[p].ulink;
	continue;
      }
      commit(p, nodes[p].top);
      ++p;
    }
    
    search(R);

    for (int p = X[0]-1; X[0] != p; ) {
      if (nodes[p].top <= 0) {
	p = nodes[p].dlink;
	continue;
      }
      uncommit(p, nodes[p].top);
      --p;
    }
    
    R.pop_back();
  }
  uncover(i);
}

// void DLC::print_items() {
//   printf("###### Print header #####\n\"(i) name llink rlink\"\n");
//   for (ullng i = 0; i <= N1 + N2; ++i) {
//     std::cout << "(" << i << ") " << items[i] << std::endl;
//   }
// }

// void DLC::print_nodes() {
//   printf("###### Print node #####\n");
//   printf("(i) top ulink dlink color\n");
//   printf("################# #####\n");

//   for (ullng i = 0; i <= N1 + N2; ++i) {
//     std::string c = "-";
//     if (-100 != nodes[i].color) c = colors[nodes[i].color];
//     std::cout << "(" << i << " " << nodes[i] << " " << c << ") ";
//   }
//   std::cout << std::endl;
  
//   for (ullng i = N1+N2+1; i <= Z; ++i) {
//     std::string c = "-";
//     if (-100 != nodes[i].color) c = colors[nodes[i].color];
//     std::cout << "(" << i << " " << nodes[i] << " " << c << ") ";
//     if (nodes[i].top <= 0) std::cout << std::endl;
//   }
// }

void DLC::print_option(int p) {
  std::cout << "{" << items[nodes[p].top].name;
  if (0 != nodes[p].color) {
    std::cout << ":" << colors[nodes[nodes[p].top].color];
  }
  for (int q = p+1; p != q; ) {
    if (nodes[q].top <= 0) q = nodes[q].ulink;
    else {
      std::cout << ", " << items[nodes[q].top].name;
      if (0 != nodes[q].color) {
	std::cout << ":" << colors[nodes[nodes[q].top].color];
      }
      q += 1;
    }
  }
  std::cout << "}";
}

void DLC::print_options(std::vector<int> &R) {
  std::cout << "{";
  print_option(R[0]);
  for (int i = 1; i < R.size(); ++i) {
    std::cout << ", ";
    print_option(R[i]);
  }
  std::cout << "}" << std::endl;
}

void DLC::print_all_solutions() {
  std::vector<int> R;
  print_flag = true;
  search(R);
}

int main()
{
  DLC d;
  d.init();
  printf("hoge\n");
  d.read_instance();
  // d.print_table();
  // d.print_all_solutions();
  std::vector<int> R;
  d.search(R);
  std::cout << d.get_num_of_solutions() << std::endl;
  
  return 0;
}
