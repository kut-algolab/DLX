#include "SAPPOROBDD/include/ZBDD.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <map>

#define DUMMY (1U << 31) - 1
#define SIGNBIT   (1ULL << 63)
#define LOG_HASHSIZE 30
#define HASHSIZE  (1 << LOG_HASHSIZE)
#define CACHESIZE (1 << LOG_HASHSIZE)
#define HASHMASK ((1 << LOG_HASHSIZE) -1)

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
    color = 0;
  }
      
  node(int t, int up, int down, int color) :
    top(t), ulink(up), dlink(down), color(color) {}
};

struct item {
  std::string name;
  int llink;
  int rlink;
  int sig;

  item(char c) { // For item 0 and N1+N2+1
    name = "-";
    llink = 0;
    rlink = 0;
    sig = DUMMY;
  }

  item(std::string s) {
    name = s;
  }
  
  item(std::string n, int prev, int next)
    : name(n), llink(prev), rlink(next) { sig = DUMMY; }
};

struct inx {
  int hash;
  int wd;
  short shift;
  short code;
  int orig;

  inx(int h, int w, short s, short c, int o)
    : hash(h), wd(w), shift(s), code(c), orig(o) {};
  //  inx(int h, short c, short s, int o)
  //: hash(h), code(c), shift(s), orig(o) {};
};

struct DLZ {
  /*** data members ***/
  int N1 = 0; // # of primary items
  int N2 = 0; // # of secondary items
  int Z = 0;  // last spacer address

  std::vector<item> items;
  std::vector<node> nodes;
  std::map<std::string, int> names;
  std::vector<std::string> colors;
  //std::vector<int> opt_number = {1};
  std::vector<int> opt_number;
  std::vector<inx> siginx;
  ullng cacheptr = 0;
  unsigned sigsiz = 0;
  std::vector<ZBDD> ZDD;
  ullng *cache;
  int *hash;
  
  /*** member functions ***/
  void init();
  void read_instance();
  void add_primary_to_header(std::string);
  void init_secondary_header();
  void add_secondary_to_header(std::string);
  void add_header_for_secondary();
  void init_nodes();
  void prepare_signature();

  void cover(const int);
  void hide(const int);
  void commit(const int, const int);
  void purify(const int);
  
  void uncover(const int);
  void unhide(const int);
  void uncommit(const int, const int);
  void unpurify(const int);

  unsigned compute_signature();
  std::pair<bool, int> hash_lookup(const unsigned);
  
  ZBDD search();
  int select_item();
  std::vector<std::vector<int> > collect_options(const int);
  
  void print_table();
  void debug();
  DLZ() {}
};

void DLZ::init() {
  item itm('0');
  items.push_back(itm);

  node node('0');
  nodes.push_back(node);

  colors.push_back("-");

  opt_number.push_back(1);

  cache = (ullng*)malloc(CACHESIZE * sizeof(ullng));
  if (NULL == cache) exit(1);
  hash = (int*)malloc(HASHSIZE * sizeof(int));
  if (NULL == cache) exit(1);
}


// // Primary items are must inserted before secondary items.
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

void DLZ::add_header_for_secondary() {
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

void DLZ::init_nodes() {
  for (int i = 1; i <= N1+N2; ++i) {
    node node(0, i, i, 0);
    nodes.push_back(node);
    Z += 1;
  }

  // First spacer node
  node node(0, DUMMY, DUMMY, DUMMY);
  nodes.push_back(node);
  Z += 1;
}

void DLZ::print_table() {
  //std::cout << "print table" << std::endl;
  std::cout << "N1 = " << N1 << ", N2 = " << N2 << std::endl;

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
    }
    add_header_for_secondary();
    break;
  }
  
  init_nodes();
  printf("%d + %d items successfully read\n", N1, N2);

  // read options
  int ptr_spacer = Z;
  while (std::getline(std::cin, line)) {
    // std::cout << Z << ": " << line << std::endl;
    if ('|' == line[0]) {
      continue;
    }
    if (++opt_number.back() >= BDD_MaxVar) {
      std::cerr << "Option limit is " << BDD_MaxVar << std::endl;
      exit(1);
    }
    std::istringstream iss(line);
    std::string s;
    while (iss >> s) {
      int c = 0;
      std::string namae = "";
      std::string color = "";
      int pos;
      // if (std::string::npos != (pos = std::string(s).find(':'))) { // with color
      if (-1 != (pos = std::string(s).find(':'))) { // with color
	for (int i = 0; i < pos; ++i) namae += s[i];
	for (unsigned i = pos+1; i < s.size(); ++i) color += s[i];
	unsigned i = 0;
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
int DLZ::select_item() {
  // int ptr = items[0].rlink;
  // int i = items[0].rlink;
  // while (0 != ptr) {
  //   if (nodes[ptr].top < nodes[i].top) i = ptr;
  //   ptr = items[ptr].rlink;
  // }
  // if (0 == nodes[i].top) return -1;
  // return i;
  if (0 == items[0].rlink) return -1;
  return items[0].rlink;
}

std::vector<std::vector<int> > DLZ::collect_options(const int i) {
  std::vector<std::vector<int> > O;
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

void DLZ::cover(const int i) {
  for (int p = nodes[i].dlink; i != p; p = nodes[p].dlink) hide(p);
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
      if (0 <= nodes[q].color) {
	nodes[u].dlink = d;
	nodes[d].ulink = u;
      }
      nodes[x].top -= 1;
      /* FIXME */
      if (0 == nodes[x].top && N1 < x) {
	int l = items[x].llink;
	int r = items[x].rlink;
	items[l].rlink = r;
	items[r].llink = l;
      }
      q += 1;
    }
  }
}

void DLZ::commit(const int p, const int j) {
  if (0 == nodes[p].color) cover(j);
  else if (0 < nodes[p].color) purify(p);
}

void DLZ::purify(const int p) {
  const int i = nodes[p].top;
  const int c = nodes[p].color;
  int ll = nodes[i].top;
  nodes[i].color = c;
  for (int q = nodes[i].dlink; i != q; q = nodes[q].dlink) {
    if (c == nodes[q].color) nodes[q].color = -1;
    else {
      ll -= 1;
      hide(q);
    }
  }
  /* FIXME */
  if (ll > 0) nodes[i].top = ll;
  else {
    int l = items[i].llink;
    int r = items[i].rlink;
    items[l].rlink = r;
    items[r].llink = l;
    nodes[i].top = -1; // SIGNAL for unpurification
  }
}

void DLZ::uncover(const int i) {
  int l = items[i].llink, r = items[i].rlink;
  items[l].rlink = i;
  items[r].llink = i;
  for (int p = nodes[i].ulink; i != p; p = nodes[p].ulink) unhide(p);
}

void DLZ::unhide(const int p) {
  for (int q = p-1; p != q; ) {
    int x = nodes[q].top;
    int u = nodes[q].ulink;
    int d = nodes[q].dlink;

    if (x <= 0) q = d;
    else {
      if (0 <= nodes[q].color) {
	nodes[d].ulink = q;
	nodes[u].dlink = q;
      }
      nodes[x].top += 1;
      /* FIXME */
      if (1 == nodes[x].top && N1 < x) {
	int l = items[x].llink;
	int r = items[x].rlink;
	items[l].rlink = x;
	items[r].llink = x;
      }
      q -= 1;
    }
  }
}

void DLZ::uncommit(const int p, const int j) {
  if (0 == nodes[p].color) uncover(j);
  else if (0 < nodes[p].color) unpurify(p);
}

void DLZ::unpurify(const int p) {
  const int i = nodes[p].top;
  const int c = nodes[p].color;
  nodes[i].color = 0;
  int ll = nodes[i].top;
  /* FIXME */
  if (-1 == ll) {
    ll = 0;
    int l = items[i].llink;
    int r = items[i].rlink;
    items[l].rlink = i;
    items[r].llink = i;
  }
  for (int q = nodes[i].ulink; i != q; q = nodes[q].ulink) {
    if (nodes[q].color < 0) nodes[q].color = c;
    else {
      unhide(q);
      ll += 1;
    }
  }
  nodes[i].top = ll;
}

void DLZ::prepare_signature() {
  int q = 1, r = 0, sigptr = 0;
  std::srand(time(NULL));
  for (int k = N1+N2; 0 != k; --k) {
    if (k <= N1) { // primary item
      if (63 == r) ++q, r = 0;
      inx tmp(rand(), q, r, 1, names[colors[k]]);
      siginx.push_back(tmp);
      items[k].sig = sigptr;
      ++sigptr;
      ++r;
    } else { // secondary item
      if (k == nodes[k].dlink) { // this secondary items does not appear the instance
	int l, r;
	l = items[k].llink, r = items[k].rlink;
	items[l].rlink = r, items[r].llink = l;
	continue;
      }
      nodes[k].color = 0;
      unsigned cc = 1;
      {
	std::unordered_set<llng> usedcolor;
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
	inx tmp(rand(), q, r, 1+i, names[colors[i]]);
	siginx.push_back(tmp);
      }
      items[k].sig = sigptr;
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
  for (int k = items[N1+N2+1].llink; k != N1+N2+1; k = items[k].llink) {
    if (0 == nodes[k].top) continue;
    sig = items[k].sig;
    //printf("k = %d, sig = %d, offset = %d\n", k, sig, offset);
    offset = siginx[sig].wd;
    while (off < offset) {
      cache[cacheptr+off] = sigacc | SIGNBIT;
      // printf("(S) cacheptr = %u, off = %d, sigacc | SIGNBIT = %llu\n", cacheptr, off, sigacc | SIGNBIT);
      ++off;
      sigacc = 0;
    }
    sig += nodes[k].color;
    sighash += siginx[sig].hash;
    sigacc += (long long)siginx[sig].code << siginx[sig].shift;
  }
  for (int k = items[0].llink; k != 0 ; k = items[k].llink) {
    sig = items[k].sig;
    offset = siginx[items[k].sig].wd;
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
    for (unsigned l = 0; l < sigsiz-1; ++l) {
      if (cache[s+l] != cache[cacheptr+l+1]) break;
      if (0 != (cache[s+l] & SIGNBIT)) continue;
      //return {true, h};
      return std::make_pair(true, h);
    }
    h = (h + hh) & HASHMASK;
  }
  hash[h] = cacheptr + 1;
  cacheptr += sigsiz;
  //return {false, h};
  return std::make_pair(false, h);
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
    return ZDD[cache[hash[t.second]-1]]*ZBDD(1).Change(opt_number.back());
  }
  
  ZBDD z = ZBDD(0);
  // collect the set of remaining options having i
  cover(i);
  std::vector<std::vector<int> > O = collect_options(i);
  for (auto X : O) {
    opt_number.push_back(std::abs(nodes[X[0]-1].top)+1);
    for (int p = X[0]+1; X[0] != p; ) {
      const int j = nodes[p].top;
      if (j <= 0) {
	p = nodes[p].ulink;
	continue;
      }
      if (j <= N1 || 0 != nodes[j].top)
	commit(p, j);
      p += 1;
    }

    z += search();

    opt_number.pop_back();
    for (int p = X[0]-1; X[0] != p; ) {
      const int j = nodes[p].top;
      if (j <= 0) {
	p = nodes[p].dlink;
	continue;
      }
      if (j <= N1 || 0 != nodes[j].top)
	uncommit(p, j);
      p -= 1;
    }
  }
  uncover(i);
  
  cache[hash[t.second]-1] = ZDD.size();
  ZDD.push_back(z);

  return z * ZBDD(1).Change(opt_number.back());
}

void DLZ::debug() {
  for (unsigned i = 0; i < items.size(); ++i) {
    std::cout << "i " << i << " " << items[i].name <<
      " " << items[i].llink << " " << items[i].rlink
	      << " " << items[i].sig << std::endl;
  }
}

int main()
{
  DLZ d;
  d.init();
  d.read_instance();
  // d.print_table();
  BDD_Init(1024, 1024 * 1024 * 1024);  
  for (int i = 0; i < d.opt_number.back(); ++i) BDD_NewVar();
  d.prepare_signature();
  // d.debug();
  
  ZBDD z = d.search();
  std::cout << z.Card() << std::endl;

  return 0;
}
