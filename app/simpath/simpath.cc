#include <iostream>
#include <sstream>
#include <vector>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <gmp.h>

typedef unsigned long long ullng;
typedef long long llng;

#define SIGNBIT (1ULL << 63)
#define LOG_HASHSIZE 30
#define HASHSIZE  (1 << LOG_HASHSIZE)
#define CACHESIZE (1 << LOG_HASHSIZE)
#define HASHMASK ((1 << LOG_HASHSIZE) -1)

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

struct hashentry {
  int sig;
  ullng zddref;
};

hashentry *hash;
ullng *cache;


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
  unsigned sig;
  unsigned wd;

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

struct zddentry {
  ullng zero_edge;
  ullng one_edge;
  ullng opt_number;
  mpz_t solutions;
  zddentry() {
    // init value = -1
    mpz_init(solutions);
    mpz_sub_ui(solutions, solutions, 1);
  }
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
  std::vector<ullng> secondary_item_count;

  unsigned sigsiz = 0; // size of offset
  std::vector<inx> siginx;
  unsigned cacheptr = 0;
  std::vector<zddentry> zdd;
  std::stack<ullng> opt_number;
  std::stack<unsigned> zdd_edge;
  std::stack<int> zdd_sig;
  ullng hit_count = 0;
  
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

  void search();
  llng select_item();
  std::vector<std::vector<ullng>> collect_options(const ullng);
  void prepare_zdd();
  void prepare_signature();
  unsigned compute_signature();
  std::pair<bool, int> hash_lookup(const unsigned);
  void search_solutions(const ullng);
  std::string get_num_of_solutions();

  void print_ZDD();
  void print_items();
  void print_nodes();
  void print_option(ullng);
  void print_options(std::vector<ullng> &);
  void print_all_solutions();
  
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
      secondary_item_count.push_back(0);
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
      // if (std::string::npos != (pos = std::string(s).find(':'))) { // with color
      if (std::string::npos != std::string(s).find(':')) { // with color
        unsigned pos = std::string(s).find(':');
        for (unsigned i = 0; i < pos; ++i) name += s[i];
        for (unsigned i = pos+1; i < s.size(); ++i) color += s[i];
        // if (!colors.count(color)) {
        //   colors[color] = colors.size() + 1;
        // }
        // c = colors[color];
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

void DLZ::prepare_zdd() {
  zddentry edge0;
  edge0.zero_edge = 0;
  edge0.one_edge = 0;
  edge0.opt_number = 0;
  mpz_set_ui(edge0.solutions, 0);
  zddentry edge1;
  edge1.zero_edge = 1;
  edge1.one_edge = 1;
  edge1.opt_number = 1;
  mpz_set_ui(edge1.solutions, 1);
  zdd.push_back(edge0);
  zdd.push_back(edge1);
}

void DLZ::prepare_signature() {
  int q = 1, r = 0, sigptr = 0;
  std::srand(time(NULL));
  // std::vector<bool> usedcolor(colors.size(), false);
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
	    // bool flag = usedcolor[i];
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
  // for (ullng k = N1+N2; k != N1+1; k = items[k].llink) {
  for (ullng k = N1+N2; k > N1; --k) {
    //32
    if (nodes[k].top == 0) continue;
    if (secondary_item_count[k-N1-1] == 2) continue;
    sig = items[k].sig;
    offset = items[k].wd;
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
  for (ullng k = items[0].llink; k != 0; k = items[k].llink) {
    // 31
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

std::pair<bool, int>  DLZ::hash_lookup(unsigned sighash) {
  int h;
  int hh = (sighash >> (LOG_HASHSIZE - 1)) | 1;
  for (h = sighash & HASHMASK; ; h = (h + hh) & HASHMASK) {
    int s = hash[h].sig;
    if (0 == s) break;
    for (int l = 0; ; ++l) {
      if (cache[s+l] != cache[cacheptr+l+1]) break;
      if (0 != (cache[s+l] & SIGNBIT)) continue;
      return {true, h+1};
    }
  }
  hash[h].sig = cacheptr + 1;
  cacheptr += sigsiz;
  return {false, h};
}

void DLZ::search() {
  // Form a signature sigma
  unsigned s = compute_signature();
  // std::cout << "signature = " << s << std::endl;
  
  std::pair<bool, int> t = hash_lookup(s);
  if (t.first) {
    ++hit_count;
    // printf("cache hit : s = %u, t = %d, hash[%d-1].sig = %u\n", s, t.second, t.second, hash[t.second-1].sig);
    zdd_sig.push(hash[t.second-1].sig-1);
    zdd_edge.push(hash[t.second-1].zddref);
    return;
  }

  if (0 == items[0].rlink) {
    // std::cout << "find ans" << std::endl;
    hash[t.second].zddref = 1;
    cache[hash[t.second].sig-1] = 1;
    zdd_sig.push(hash[t.second].sig-1);
    zdd_edge.push(1);
    return;
  }
  
  // select item i
  const llng i = select_item();
  if (-1 == i) {
    cache[hash[t.second].sig-1] = 0;
    zdd_sig.push(hash[t.second].sig-1);
    zdd_edge.push(0);
    return;
  }
  // std::cout << "select item " << i << std::endl;
  

  // collect the set of remaining options having i
  std::vector<std::vector<ullng>> O = collect_options(i);
  
  for (auto X : O) {
    opt_number.push(std::abs(nodes[X[0]-1].top));
    // std::cout << "select option " << opt_number.top() << std::endl;
    for (auto p : X) {
      // std::cout << "commit: " << p << ", " << nodes[p].top << std::endl;
      if (nodes[p].top > (llng)N1) ++secondary_item_count[nodes[p].top-N1-1];
      commit(p, nodes[p].top);
    }

    search();

    // std::cout << "deselect option " << opt_number.top() << std::endl;
    for (auto p = X.rbegin(); p != X.rend(); ++p) {
      // std::cout << "uncommit: " << *p << ", " << nodes[*p].top << std::endl;
      if (nodes[*p].top > (llng)N1) --secondary_item_count[nodes[*p].top-N1-1];
      uncommit(*p, nodes[*p].top);
    }
    if (X == O.back()) {
      zdd_sig.push(-1);
      zdd_edge.push(0);
      for (auto _ : O) {
	unsigned edge0 = zdd_edge.top();
	zdd_edge.pop();
	zddentry tmp;
	tmp.zero_edge = edge0;
	tmp.one_edge = zdd_edge.top();
	tmp.opt_number = opt_number.top();

	zdd.push_back(tmp);
	zdd_edge.pop();
	opt_number.pop();
	zdd_edge.push(zdd.size()-1);
	// printf("I(%lu) = {%lld, I(%lld), I(%lld)}\n", zdd.size()-1, zdd.back().opt_number, zdd.back().zero_edge, zdd.back().one_edge);
	
	hash[t.second].zddref = zdd_edge.top();
	ullng count = 0;
	if (-1 != zdd_sig.top()) count = cache[zdd_sig.top()];
	// std::cout << "0-edge -> " << zdd_sig.top() << std::endl;
	zdd_sig.pop();
	// std::cout << "1-edge -> " << zdd_sig.top() << std::endl;
	count += cache[zdd_sig.top()];
	// std::cout << "count -> " << count << std::endl;
	zdd_sig.pop();
	cache[hash[t.second].sig-1] = count;
	zdd_sig.push(hash[t.second].sig-1);
      }
    }
  }
}

std::string DLZ::get_num_of_solutions() {
  search_solutions(zdd.size()-1);
  char* sols = mpz_get_str(NULL, 10, zdd.back().solutions);
  return sols;
}

void DLZ::search_solutions(ullng n) {
  // if top or bottom
  if (0 == n || 1 == n) return;
  // if already calculated
  if (0 != (mpz_cmp_si(zdd[n].solutions, -1))) return;
  
  search_solutions(zdd[n].zero_edge);
  search_solutions(zdd[n].one_edge);
  
  mpz_add(zdd[n].solutions, zdd[zdd[n].zero_edge].solutions, zdd[zdd[n].one_edge].solutions);
}


void DLZ::print_items() {
  printf("###### Print header #####\n\"(i) name llink rlink\"\n");
  for (ullng i = 0; i <= N1 + N2; ++i) {
    std::cout << "(" << i << ") " << items[i] << std::endl;
  }
}

void DLZ::print_nodes() {
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

void DLZ::print_option(ullng p) {
  std::cout << "{" << items[nodes[p].top].name;
  if (0 != nodes[p].color) {
    std::cout << ":" << colors[nodes[nodes[p].top].color];
  }
  for (ullng q = p+1; p != q; ) {
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

void DLZ::print_options(std::vector<ullng> &R) {
  std::cout << "{";
  print_option(R[0]);
  for (ullng i = 1; i < R.size(); ++i) {
    std::cout << ", ";
    print_option(R[i]);
  }
  std::cout << "}" << std::endl;
}

void DLZ::print_ZDD() {
  std::cout << "print ZDD" << std::endl;
  std::cout << "I(i) = { option number, 0-edge, 1-edge }" << std::endl;
  for (ullng i = 0; i < zdd.size(); ++i) {
    printf("I(%llu) = {%lld, I(%lld), I(%lld)}\n", i, zdd[i].opt_number, zdd[i].zero_edge, zdd[i].one_edge);
  }
}


int main()
{
  DLZ d;
  d.read_instance();
  d.prepare_zdd();
  d.prepare_signature();

  hash = (hashentry*)malloc(HASHSIZE * sizeof(hashentry));
  if (NULL == hash) exit(1);
  cache = (ullng*)malloc(CACHESIZE * sizeof(ullng));
  if (NULL == cache) exit(1);
  
  // d.print_items();
  // d.print_nodes();

  d.search();

  std::cout << "cache[0] = " << cache[0] << std::endl;
  // d.print_ZDD();
  std::cout << "# of hit = " << d.hit_count << std::endl;;
  std::cout << "# of sols = " << d.get_num_of_solutions() << std::endl;
  return 0;
}
