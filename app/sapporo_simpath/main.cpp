#include "SAPPOROBDD/include/ZBDD.h"
#include "SAPPOROBDD/include/BDDCT.h"
#include "sbdd_helper/SBDD_helper.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <stack>
#include <unordered_set>
#include <unordered_map>
//#include <gmp.h>

typedef unsigned long long ullng;
typedef long long llng;

struct inx {
  int hash;
  short code;
  short shift;
  std::string orig;

  inx(int h, short c, short s, std::string o)
    : hash(h), code(c), shift(s), orig(o) {};
  friend std::ostream& operator<<(std::ostream& ss, const inx& inx) {
    ss << inx.hash << " " << inx.code << " " << inx.shift << " " << inx.orig;
    return ss;
  }
};

struct node {
  llng top;
  llng ulink;
  llng dlink;
  llng color;

  node(int i) : ulink(-100), dlink(-100), color(-100) {
    if (-1 == i) { // spacer node
      top = 0;
      color = 0;
    } else {
      top = -100; // header node
    }
  }
  node(llng t, llng up, llng down, llng color) : top(t), ulink(up), dlink(down), color(color) {}

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
  ullng llink;
  ullng rlink;
  unsigned sig;
  unsigned wd;

  item(unsigned u = 0) : name("-"), llink(0), rlink(0) {} // header of items
  item(std::string n) : name(n) {}
  item(std::string n, ullng prev, ullng next) : name(n), llink(prev), rlink(next) {}
  void set_llink(ullng prev) { llink = prev; }
  void set_rlink(ullng next) { rlink = next; }

  friend std::ostream& operator<<(std::ostream& ss, const item& itm) {
    ss << itm.name << " " << itm.llink << " " << itm.rlink;
    return ss;
  };
};

struct DLZ {
  /*** data members ***/
  ullng N1 = 0; // # of primary items
  ullng N2 = 0; // # of secondary items
  ullng Z = 0;  // last spacer address

  std::vector<item> items;
  std::vector<node> nodes;
  std::unordered_map<std::string, ullng> names;
  std::vector<std::string> colors;
  std::vector<int> opt_number = {1};

  unsigned sigsiz = 0; // size of offset
  std::vector<inx> siginx;
  std::vector<unsigned> secondary_count;
  std::unordered_map<unsigned, ullng> hash;
  std::vector<ZBDD> cache;
    
  /*** member functions ***/
  void read_instance();
  void add_primary_to_header(std::string);
  void add_secondary_to_header(std::string);
  void set_header_items();
  
  void cover(const ullng);
  void hide(const ullng);
  void commit(const ullng, const ullng);
  void purify(const ullng);
  
  void uncover(const ullng);
  void unhide(const ullng);
  void uncommit(const ullng, const ullng);
  void unpurify(const ullng);

  ZBDD search();
  llng select_item();
  std::vector<std::vector<ullng>> collect_options(const ullng);
  
  void prepare_signature();
  unsigned compute_signature();
  std::pair<bool, int> hash_lookup(const unsigned);

  DLZ() {
    item itm(0);
    items.push_back(itm);

    node node(0);
    nodes.push_back(node);

    colors.push_back("-");
  }
};

// Primary items are must inserted before secondary items.
void DLZ::add_primary_to_header(std::string name) {
  const llng i = N1 + 1;
  item itm(name, i-1, 0);
  items.push_back(itm);
  items[i-1].rlink = i;
  items[0].llink = i;
  names[name] = i;
  ++N1;
}

// N = N1 + N2
void DLZ::add_secondary_to_header(std::string name) {
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

void DLZ::set_header_items() {
  for (ullng i = 1; i <= N1 + N2; ++i) {
    node x(0, i, i, -100);
    nodes.push_back(x);
    ++Z;
  }
}

// FIXME; we must check the syntax of the first line
void DLZ::read_instance() {
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
      secondary_count.push_back(0);
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
    if (++opt_number.back() >= BDD_MaxNode) {
      std::cerr << "Option limit is " << BDD_MaxNode << std::endl;
      exit(1);
    }
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
      if (std::string::npos != std::string(s).find(':')) { // with color
        unsigned pos = std::string(s).find(':');
        for (unsigned i = 0; i < pos; ++i) name += s[i];
        for (unsigned i = pos+1; i < s.size(); ++i) color += s[i];
	
        unsigned i = 0;
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

void DLZ::cover(const ullng i) {
  // std::cout << "cover " << i << " : " << std::endl;
  for (ullng p = nodes[i].dlink; i != p; p = nodes[p].dlink) {
    // std::cout << "p: " << p << std::endl;
    hide(p);
  }
  ullng l = items[i].llink, r = items[i].rlink;
  items[l].rlink = r;
  items[r].llink = l;
}

void DLZ::hide(const ullng p) {
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

void DLZ::commit(const ullng p, const ullng j) {
  if (0 == nodes[p].color) cover(j);
  if (0 < nodes[p].color) purify(p);
}

void DLZ::purify(const ullng p) {
  // std::cout << "  purify: " << p << std::endl;
  const llng c = nodes[p].color;
  const llng i = nodes[p].top;
  nodes[i].color = c;
  for (llng q = nodes[i].dlink; i != q; q = nodes[q].dlink) {
    if (c == nodes[q].color) nodes[q].color = -1;
    else hide(q);
  }
}

void DLZ::uncover(const ullng i) {
  // std::cout << "uncover " << i << std::endl;
  ullng l = items[i].llink, r = items[i].rlink;
  items[l].rlink = i;
  items[r].llink = i;
  for (ullng p = nodes[i].ulink; i != p; p = nodes[p].ulink) unhide(p);
}

void DLZ::unhide(const ullng p) {
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

void DLZ::uncommit(const ullng p, const ullng j) {
  if (0 == nodes[p].color) uncover(j);
  if (0 < nodes[p].color) unpurify(p);
}

void DLZ::unpurify(const ullng p) {
  // std::cout << "  unpurify: " << p << std::endl;
  const llng c = nodes[p].color;
  const llng i = nodes[p].top;
  for (llng q = nodes[i].ulink; i != q; q = nodes[q].ulink) {
    if (nodes[q].color < 0) nodes[q].color = c;
    else unhide(q);
  }
}

// In order to obtain an ordered ZDD, select the left most item
// if there is item i to be covered and its len is 0, then return -1
llng DLZ::select_item() {
  ullng i = items[0].rlink;
  if (0 >= nodes[i].top) return -1;
  return i;
}

std::vector<std::vector<ullng>> DLZ::collect_options(const ullng i) {
  std::vector<std::vector<ullng>> O;
  ullng p = nodes[i].dlink;
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

void DLZ::prepare_signature() {
  int q = 1, r = 0, sigptr = 0;
  std::srand(time(NULL));
  for (ullng k = N1+N2; 0 != k; --k) {
    if (k <= N1) { // primary item
      if (63 == r) ++q, r = 0;
      inx tmp(rand(), 1, r, "-");
      siginx.push_back(tmp);
      items[k].sig = sigptr;
      ++sigptr;
      items[k].wd = q;
      ++r;
    } else { // secondary item
      if ((llng)k == nodes[k].dlink) { // this secondary items does not appear the instance
	ullng l, r;
	l = items[k].llink, r = items[k].rlink;
	items[l].rlink = r, items[r].llink = l;
	continue;
      }
      unsigned cc = 1;
      {
	std::unordered_set<llng> usedcolor;
	for (ullng p = nodes[k].dlink; k != p; p = nodes[p].dlink) {
	  llng i = nodes[p].color;
	  if (0 != i) {
	    if (!usedcolor.count(i)) {
	      usedcolor.insert(i);
	      ++cc;
	    }
	  }
	}
      }
      unsigned t = 1;
      for ( ; cc >= (1U << t); ++t);

      if (r + t >= 63) ++q, r = 0;

      for (unsigned i = 0; i < cc; ++i) {
	inx tmp(rand(), 1+i, r, colors[i]);
	siginx.push_back(tmp);
	items[k].sig = sigptr, items[k].wd = q;
      }
      sigptr += cc;
      r += t;
    }
  }
  sigsiz = q + 1;
}

unsigned DLZ::compute_signature() {
  unsigned sighash = 0;
  int off = 1, sig, offset;
  for (ullng k = N1+N2; k > N1; --k) {
    if (0 == nodes[k].top) continue;
    /**********************/
    if (2 == secondary_count[k-N1-1]) continue; // FIXME
    /**********************/
    sig = items[k].sig;
    offset = items[k].wd;
    while (off < offset) ++off;
    sig += nodes[k].color;
    sighash += siginx[sig].hash;
  }
  for (ullng k = items[0].llink; k != 0; k = items[k].llink) {
    sig = items[k].sig;
    offset = items[k].wd;
    while (off < offset) ++off;
    sighash += siginx[sig].hash;
  }
  return sighash;
}

std::pair<bool, int> DLZ::hash_lookup(unsigned sighash) {
  auto itr = hash.find(sighash);
  if (itr != hash.end()) {
    return {true, itr->second};
  }
  return {false, 0};
}

ZBDD DLZ::search() {
  unsigned s = compute_signature();
  std::pair<bool, int> t = hash_lookup(s);
  if (t.first) {
    // std::cout << "cache hit!" << std::endl;
    return cache[t.second]*ZBDD(1).Change(opt_number.back());
  }
  
  if (0 == items[0].rlink) {
    // std::cout << "find answer" << std::endl;
    return ZBDD(1).Change(opt_number.back());
  }

  // select item i
  const llng i = select_item();
  if (-1 == i) {
    return ZBDD(0);
  }

  ZBDD z = ZBDD(0);
  // collect the set of remaining options having i
  std::vector<std::vector<ullng>> O = collect_options(i);
  for (auto X : O) {
    opt_number.push_back((int)std::abs(nodes[X[0]-1].top)+1);
    for (auto p : X) {
      // std::cout << "commit: " << p << ", " << nodes[p].top << std::endl;
      if (nodes[p].top > (llng)N1) ++secondary_count[nodes[p].top-N1-1];
      commit(p, nodes[p].top);
    }
    
    z += search();
    
    opt_number.pop_back();
    for (auto p = X.rbegin(); p != X.rend(); ++p) {
      if (nodes[*p].top > (llng)N1) --secondary_count[nodes[*p].top-N1-1];
      uncommit(*p, nodes[*p].top);
    }
  }
  hash.insert({s, cache.size()}); // key:signature, value:index
  cache.push_back(z);
  // std::cout <<  "signature = " << s << " : cache[" << cache.size()-1 << "] = " << sbddh::ZStr(cache.back()) << std::endl;
  return z * ZBDD(1).Change(opt_number.back());
}

int main() {
  BDD_Init(1024, 1024 * 1024 * 1024);
  DLZ d;
  d.read_instance();
  for (int i = 0; i < d.opt_number.back(); ++i) BDD_NewVar();

  d.prepare_signature();
  ZBDD z = d.search();
  std::cout << z.Card() << std::endl;
  // std::cout << sbddh::ZStr(z.OnSet0(z.Top())) << std::endl;
  
  return 0;
}
