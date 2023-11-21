#include "SAPPOROBDD/include/ZBDD.h"
// #include "SAPPOROBDD/include/BDDCT.h"
// #include "sbdd_helper/SBDD_helper.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <stack>
#include <unordered_set>
#include <unordered_map>
//#include <gmp.h>

typedef unsigned long long ullng;
typedef long long llng;

#define SIGNBIT   (1ULL << 63)
#define LOG_HASHSIZE 30
#define HASHSIZE  (1 << LOG_HASHSIZE)
#define CACHESIZE (1 << LOG_HASHSIZE)
#define HASHMASK ((1 << LOG_HASHSIZE) -1)

ullng *cache;
ullng *hash;

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
  int top;
  int ulink;
  int dlink;
  int color;

  node(int i) : ulink(-100), dlink(-100), color(-100) {
    if (-1 == i) { // spacer node
      top = 0;
      color = 0;
    } else {
      top = -100; // header node
    }
  }
  node(int t, int up, int down, int color) : top(t), ulink(up), dlink(down), color(color) {}

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
  int llink;
  int rlink;
  unsigned sig;
  unsigned wd;

  item(unsigned u = 0) : name("-"), llink(0), rlink(0) {} // header of items
  item(std::string n) : name(n) {}
  item(std::string n, int prev, int next) : name(n), llink(prev), rlink(next) {}
  void set_llink(int prev) { llink = prev; }
  void set_rlink(int next) { rlink = next; }

  friend std::ostream& operator<<(std::ostream& ss, const item& itm) {
    ss << itm.name << " " << itm.llink << " " << itm.rlink;
    return ss;
  };
};

struct DLZ {
  /*** data members ***/
  int N1 = 0; // # of primary items
  int N2 = 0; // # of secondary items
  int Z = 0;  // last spacer address

  std::vector<item> items;
  std::vector<node> nodes;
  std::unordered_map<std::string, int> names;
  std::vector<std::string> colors;
  std::vector<int> opt_number = {1};

  int cacheptr = 0;
  unsigned sigsiz = 0; // size of offset (subproblem signature <- 64bit * sigsiz)
  std::vector<inx> siginx;
  std::vector<unsigned> secondary_count;
  std::vector<ZBDD> ZDD;
    
  /*** member functions ***/
  void read_instance();
  void add_primary_to_header(std::string);
  void add_secondary_to_header(std::string);
  void set_header_items();
  
  void cover(const int);
  void hide(const int);
  void commit(const int, const int);
  void purify(const int);
  
  void uncover(const int);
  void unhide(const int);
  void uncommit(const int, const int);
  void unpurify(const int);

  ZBDD search();
  int select_item();
  std::vector<std::vector<int>> collect_options(const int);
  
  void prepare_signature();
  unsigned compute_signature();
  std::pair<bool, int> hash_lookup(const unsigned);

  void print_table();

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
  const int i = N1 + 1;
  item itm(name, i-1, 0);
  items.push_back(itm);
  items[i-1].rlink = i;
  items[0].llink = i;
  names[name] = i;
  ++N1;
}

// N = N1 + N2
void DLZ::add_secondary_to_header(std::string name) {
  const int i = N1 + N2 + 1;
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
  if (N2 > 0) {
    item itm("-", N1+N2, N1+1);
    items.push_back(itm);
    items[N1+1].llink = N1+N2+1;
    items[N1+N2].rlink = N1+N2+1;
  }
  for (int i = 1; i <= N1; ++i) {
    node x(0, i, i, -100);
    nodes.push_back(x);
    ++Z;
  }
  for (int i = N1+1; i<= N1+N2; ++i) {
    node x(0, i, i, 0);
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
    
  int ptr_spacer = Z;
  
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
      int c = 0;
      
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

      int t = names[name];
      int u = nodes[t].ulink;
      node tmp(t, u, t, c);
      const int x = Z + 1;
      
      nodes.push_back(tmp);
      nodes[u].dlink = x;
      nodes[t].ulink = x;
      ++nodes[t].top;
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

void DLZ::cover(const int i) {
  // std::cout << "cover " << i << " : " << std::endl;
  for (int p = nodes[i].dlink; i != p; p = nodes[p].dlink) {
    // std::cout << "p: " << p << std::endl;
    hide(p);
  }
  int l = items[i].llink, r = items[i].rlink;
  items[l].rlink = r;
  items[r].llink = l;
}

void DLZ::hide(const int p) {
  for (int q = p+1; p != q; ) {
    int x = nodes[q].top;
    int u = nodes[q].ulink;
    int d = nodes[q].dlink;
    
    if (x <= 0) q = u;
    else {
      nodes[u].dlink = d;
      nodes[d].ulink = u;
      --nodes[x].top;
      ++q;
    }
  }
}

void DLZ::commit(const int p, const int j) {
  if (0 == nodes[p].color) cover(j);
  if (0 < nodes[p].color) purify(p);
}

void DLZ::purify(const int p) {
  // std::cout << "  purify: " << p << std::endl;
  const int c = nodes[p].color;
  const int i = nodes[p].top;
  nodes[i].color = c;
  for (int q = nodes[i].dlink; i != q; q = nodes[q].dlink) {
    if (c == nodes[q].color) nodes[q].color = -1;
    else hide(q);
  }
}

void DLZ::uncover(const int i) {
  // std::cout << "uncover " << i << std::endl;
  int l = items[i].llink, r = items[i].rlink;
  items[l].rlink = i;
  items[r].llink = i;
  for (int p = nodes[i].ulink; i != p; p = nodes[p].ulink) unhide(p);
}

void DLZ::unhide(const int p) {
  // std::cout << "  unhide: " << p << std::endl;
  for (int q = p-1; p != q; ) {
    int x = nodes[q].top;
    int u = nodes[q].ulink;
    int d = nodes[q].dlink;

    if (x <= 0) q = d;
    else {
      nodes[d].ulink = q;
      nodes[u].dlink = q;
      ++nodes[x].top;
      --q;
    }
  }
}

void DLZ::uncommit(const int p, const int j) {
  if (0 == nodes[p].color) uncover(j);
  if (0 < nodes[p].color) unpurify(p);
}

void DLZ::unpurify(const int p) {
  // std::cout << "  unpurify: " << p << std::endl;
  const int c = nodes[p].color;
  const int i = nodes[p].top;
  for (int q = nodes[i].ulink; i != q; q = nodes[q].ulink) {
    if (nodes[q].color < 0) nodes[q].color = c;
    else unhide(q);
  }
}

// In order to obtain an ordered ZDD, select the left most item
// if there is item i to be covered and its len is 0, then return -1
int DLZ::select_item() {
  int i = items[0].rlink;
  if (0 >= nodes[i].top) return -1;
  return i;
}

std::vector<std::vector<int>> DLZ::collect_options(const int i) {
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
      ++q;
    }
    O.push_back(o);
    p = nodes[p].dlink;
  }
  return O;
}

void DLZ::prepare_signature() {
  int q = 1, r = 0, sigptr = 0;
  std::srand(time(NULL));
  for (int k = N1+N2; 0 != k; --k) {
    if (k <= N1) { // primary item
      if (63 == r) ++q, r = 0;
      inx tmp(rand(), 1, r, "-");
      siginx.push_back(tmp);
      items[k].sig = sigptr;
      ++sigptr;
      items[k].wd = q;
      ++r;
    } else { // secondary item
      if (k == nodes[k].dlink) { // this secondary items does not appear the instance
	int l, r;
	l = items[k].llink, r = items[k].rlink;
	items[l].rlink = r, items[r].llink = l;
	continue;
      }
      unsigned cc = 1;
      {
	std::unordered_set<int> usedcolor;
	for (int p = nodes[k].dlink; k != p; p = nodes[p].dlink) {
	  int i = nodes[p].color;
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
  ullng sigacc = 0;
  unsigned sighash = 0;
  int off = 1, sig, offset;

  if (cacheptr + sigsiz >= CACHESIZE) exit(-1);
  for (int k = N1+N2; k > N1; --k) {
    if (nodes[k].top == 0) continue;
    if (secondary_count[k-N1-1] == 2) continue;
    sig = items[k].sig;
    offset = items[k].wd;
    while (off < offset) {
      cache[cacheptr+off] = sigacc | SIGNBIT;
      // printf("(S) cacheptr = %u, off = %d, sigacc | SIGNBIT = %llu\n", cacheptr, off, sigacc | SIGNBIT);
      ++off;
      sigacc = 0;
    }
    
    /*
    if (secondary_count[k-N1-1] != 0 && nodes[k].color != -100)
       sig += nodes[k].color;
    */
    if (secondary_count[k-N1-1] != 0)
      sig += nodes[k].color;
    
    sighash += siginx[sig].hash;
    sigacc += (long long)siginx[sig].code << siginx[sig].shift;
  }
  for (int k = items[0].llink; k != 0; k = items[k].llink) {
    sig = items[k].sig;
    offset = items[k].wd;
    while (off < offset) {
      cache[cacheptr+off] = sigacc | SIGNBIT;
      // printf("(P) cacheptr = %u, off = %d, sigacc | SIGNBIT = %llu\n", cacheptr, off, sigacc | SIGNBIT);
      off++;
      sigacc = 0;
    }
    sighash += siginx[sig].hash;
    sigacc += 1LL << siginx[sig].shift;
  }
  cache[cacheptr+off] = sigacc;
  // printf("(#) cacheptr = %u, off = %d, sigacc = %llu\n", cacheptr, off, sigacc);
  
  return sighash;
}

std::pair<bool, int> DLZ::hash_lookup(unsigned sighash) {
  // double hash <h, hh>
  int h = sighash & HASHMASK;
  int hh = (sighash >> (LOG_HASHSIZE - 1)) | 1;
  while (0 != hash[h]) {
    int s = hash[h];
    for (int l = 0; l < sigsiz-1; ++l) {
      if (cache[s+l] != cache[cacheptr+l+1]) break;
      if (0 != (cache[s+l] & SIGNBIT)) continue;
      return {true, h};
    }
    h = (h + hh) & HASHMASK;
  }
  hash[h] = cacheptr + 1;
  cacheptr += sigsiz;
  return {false, h};
}

ZBDD DLZ::search() {
  if (0 == items[0].rlink) {
    // std::cout << "find answer" << std::endl;
    return ZBDD(1).Change(opt_number.back());
  }

  // select item i
  const int i = select_item();
  if (-1 == i) {
    return ZBDD(0);
  }

  unsigned s = compute_signature();
  std::pair<bool, int> t = hash_lookup(s);
  if (t.first) {
    // std::cout << "cache hit!" << std::endl;
    // std::cout << sbddh::ZStr(ZDD[cache[hash[t.second]-1]]*ZBDD(1).Change(opt_number.back())) << std::endl;
    return ZDD[cache[hash[t.second]-1]]*ZBDD(1).Change(opt_number.back());
  }
  
  ZBDD z = ZBDD(0);
  // collect the set of remaining options having i
  std::vector<std::vector<int>> O = collect_options(i);
  for (auto X : O) {
    opt_number.push_back(std::abs(nodes[X[0]-1].top)+1);
    for (auto p : X) {
      // std::cout << "commit: " << p << ", " << nodes[p].top << std::endl;
      if (nodes[p].top > N1) ++secondary_count[nodes[p].top-N1-1];
      commit(p, nodes[p].top);
    }
    
    z += search();

    opt_number.pop_back();
    for (auto p = X.rbegin(); p != X.rend(); ++p) {
      if (nodes[*p].top > N1) --secondary_count[nodes[*p].top-N1-1];
      uncommit(*p, nodes[*p].top);
    }
  }
  cache[hash[t.second]-1] = ZDD.size();
  ZDD.push_back(z);

  return z * ZBDD(1).Change(opt_number.back());
}

void DLZ::print_table() {
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
    std::cout << number << std::endl << top << std::endl << ul << std::endl << dl << std::endl << co << std::endl << std::endl;
  }
}

int main() {
  BDD_Init(1024, 1024 * 1024 * 1024);
  DLZ d;
  d.read_instance();
  for (int i = 0; i < d.opt_number.back(); ++i) BDD_NewVar();
  d.prepare_signature();
  
  cache = (ullng*)malloc(CACHESIZE * sizeof(ullng));
  if (NULL == cache) exit(1);
  hash = (ullng*)malloc(HASHSIZE * sizeof(ullng));
  if (NULL == cache) exit(1);
  
  ZBDD z = d.search();
  z = z.OnSet0(z.Top());
  if (z.Card() == bddnull) {
    std::cout << "0x" <<z.CardMP16(NULL) << std::endl;
  } else {
    std::cout << z.Card() << std::endl;
  }
  
  // std::cout << sbddh::ZStr(z) << std::endl;
  
  return 0;
}
