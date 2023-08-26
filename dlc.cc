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
  llng color;

  node(int i) : ulink(-100), dlink(-100), color(-100) {
    if (-1 == i) { // spacer node
      top = 0;
      color = 0;
    }
    else top = -100; // header node
  }
  node(llng t, llng up, llng down, llng color) : top(t), ulink(up), dlink(down), color(color) {}
  
  friend std::ostream& operator<<(std::ostream& ss, const node& node) {
    if (-100 == node.top) ss << "-";
    else ss << node.top;
    if (-100 == node.ulink) ss << " -";
    else ss << " " << node.ulink;
    if (-100 == node.dlink) ss << " -";
    else ss << " " << node.dlink;
    // if (-100 == node.color) ss << " -";
    // else ss << " " << colors[node.color];
    return ss;
  };
};

struct item {
  std::string name;
  ullng llink;
  ullng rlink;

  item(unsigned u = 0) : name("-"), llink(0), rlink(0) {} // header of items
  item(std::string n) :  name(n) {}
  item(std::string n, ullng prev, ullng next)
    : name(n), llink(prev), rlink(next) {}
  void set_llink(ullng prev) { llink = prev; }
  void set_rlink(ullng next) { rlink = next; }

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
  // std::unordered_map<std::string, llng> colors;
  std::vector<std::string> colors;
  
  /*** member functions ***/
  void read_instance();
  void add_primary_to_header(std::string);
  void add_secondary_to_header(std::string);
  void set_header_items();

  void cover(const ullng);
  void hide(const ullng);
  void uncover(const ullng);
  void unhide(const ullng);

  void search(std::vector<ullng> &);
  llng select_item();
  std::vector<std::vector<ullng>> collect_options(const ullng);

  void print_items();
  void print_nodes();
  void print_option(ullng);
  void print_options(std::vector<ullng> &);
  void print_all_solutions();
  
  DLX() {
    item itm(0);
    items.push_back(itm);

    node node(0);
    nodes.push_back(node);

    colors.push_back("0");
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
    node x(0, i, i, -100);
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
    // std::cout << Z << ": " << line << std::endl;
    if ('|' == line[0]) {
      continue;
    }
    std::istringstream iss(line);
    std::string s;
    while (iss >> s) {
      llng c = 0;
      
      std::string name = "";
      std::string color = "";
      int pos;
      if (std::string::npos != (pos = std::string(s).find(':'))) {       // with color
	for (int i = 0; i < pos; ++i) name += s[i];
	for (int i = pos+1; i < s.size(); ++i) color += s[i];
	// if (!colors.count(color)) {
	//   colors[color] = colors.size() + 1;
	// }
	// c = colors[color];
	int i = 0;
	for ( ; i < colors.size(); ++i)
	  if (color == colors[i]) break;
	if (colors.size() == i) colors.push_back(color);
	c = i;
      } else {
	name = s;
      }

      llng t = names[name];
      llng u = nodes[t].ulink;
      node tmp(t, u, t, c);
      const llng x = Z + 1;
      
      nodes.push_back(tmp);
      nodes[u].dlink = x;
      nodes[t].ulink = x;
      nodes[t].top += 1;
      ++Z;
    }
    
    // add spacer node
    node tmp(nodes[ptr_spacer].top-1, ptr_spacer+1, -100, 0);
    nodes[ptr_spacer].dlink = Z;
    nodes.push_back(tmp);
    ++Z;
    ptr_spacer = Z;
  }
}

// select item i using the MRV heuristic
// if there is item i to be covered and its len is 0, then return -1
llng DLX::select_item() {
  ullng ptr = items[0].rlink;
  ullng i = items[0].rlink;
  while (0 != ptr) {
    if (nodes[ptr].top < nodes[i].top) i = ptr;
    ptr = items[ptr].rlink;
  }
  if (0 == nodes[i].top) return -1;
  return i;
}

std::vector<std::vector<ullng>> DLX::collect_options(const ullng i) {
  std::vector<std::vector<ullng>> O;
  llng p = nodes[i].dlink;
  while (i != p) {
    std::vector<ullng> o;
    o.push_back(p);
    ullng q = p+1;
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

void DLX::cover(const ullng i) {
  // std::cout << "cover " << i << " : " << std::endl;
  for (llng p = nodes[i].dlink; i != p; p = nodes[p].dlink) {
    // std::cout << "p: " << p << std::endl;
    hide(p);
  }
  ullng l = items[i].llink, r = items[i].rlink;
  items[l].rlink = r;
  items[r].llink = l;
}

void DLX::hide(const ullng p) {
  for (ullng q = p+1; p != q; ) {
    llng x = nodes[q].top;
    ullng u = nodes[q].ulink;
    ullng d = nodes[q].dlink;
    if (x <= 0) q = u;
    else {
      nodes[u].dlink = d;
      nodes[d].ulink = u;
      nodes[x].top -= 1;
      q += 1;
    }
  }
}

void DLX::uncover(const ullng i) {
  // std::cout << "uncover " << i << std::endl;
  ullng l = items[i].llink, r = items[i].rlink;
  items[l].rlink = i;
  items[r].llink = i;
  for (llng p = nodes[i].ulink; i != p; p = nodes[p].ulink) unhide(p);
}

void DLX::unhide(const ullng p) {
  // std::cout << "  unhide: " << p << std::endl;
  for (ullng q = p-1; p != q; ) {
    llng x = nodes[q].top;
    ullng u = nodes[q].ulink;
    ullng d = nodes[q].dlink;

    if (x <= 0) q = d;
    else {
      nodes[d].ulink = q;
      nodes[u].dlink = q;
      nodes[x].top += 1;
      q -= 1;
    }
  }
}

void DLX::search(std::vector<ullng> &R) {
  // if no remaining items in items then output R and return
  if (0 == items[0].rlink) {
    print_options(R);
    return;
  }

  // select item i
  const llng i = select_item();
  if (-1 == i) return;

  // collect the set of remaining options having i
  std::vector<std::vector<ullng>> O = collect_options(i);
  for (auto X : O) {
    // Only add the address of the first item in option X to R
    R.push_back(X[0]);

    for (auto j : X) {
      // Delete column corresponding to item j;
      // Delete all options having j;
      cover(nodes[j].top);
    }
    // print_items();
    // print_nodes();
    
    search(R);

    for (auto j = X.rbegin(); j != X.rend(); ++j) {
    // for (auto j : X) {
      // Restore all options having j;
      // Restore column corresponding to item j;
      uncover(nodes[*j].top);
    }
    
    R.pop_back();
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
  printf("(i) top ulink dlink color\n");
  printf("################# #####\n");

  for (ullng i = 0; i <= N1 + N2; ++i) {
    std::string c = "-";
    if (-100 != nodes[i].color) c = colors[nodes[i].color];
    std::cout << "(" << i << " " << nodes[i] << " " << c << ") ";
  }
  std::cout << std::endl;
  
  for (ullng i = N1+N2+1; i <= Z; ++i) {
    std::string c = "-";
    if (-100 != nodes[i].color) c = colors[nodes[i].color];
    std::cout << "(" << i << " " << nodes[i] << " " << c << ") ";
    if (nodes[i].top <= 0) std::cout << std::endl;
  }
}

void DLX::print_option(ullng p) {
  std::cout << "{" << items[nodes[p].top].name;
  for (ullng q = p+1; p != q; ) {
    if (nodes[q].top <= 0) q = nodes[q].ulink;
    else {
      std::cout << ", " << items[nodes[q].top].name;
      q += 1;
    }
  }
  std::cout << "}";
}

void DLX::print_options(std::vector<ullng> &R) {
  std::cout << "{";
  print_option(R[0]);
  for (ullng i = 1; i < R.size(); ++i) {
    std::cout << ", ";
    print_option(R[i]);
  }
  std::cout << "}" << std::endl;
}

void DLX::print_all_solutions() {
  std::vector<ullng> R;
  search(R);
}

int main()
{
  DLX d;
  d.read_instance();
  // d.print_items();
  d.print_nodes();
  // d.print_all_solutions();
  
  return 0;
}
