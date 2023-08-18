// case inputs are primary

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unistd.h>

struct HNODE {
  std::string name;
  long long llink;
  long long rlink;
};
typedef struct HNODE hnode;

struct TNODE {
  long long top;  // if primary item, this mean len(x)
  long long ulink;
  long long dlink;
};
typedef struct TNODE tnode;

struct TABLE {
  std::vector<hnode> header;
  std::vector<tnode> node;
};
typedef struct TABLE table;


void input(table* tb) {
  std::string input;
  std::string in;
  std::vector<std::string> exists;

  // input primary items
  std::getline(std::cin, input);
  // std::cout << "input items : " << input << std::endl;
  
  std::istringstream iss(input);
  bool exists_input = false;
  long long link = 0; // link number
  while (iss >> in) {
    // if item already exist
    bool contains_item = (exists.end() != std::find(exists.begin(), exists.end(), in));
    if (contains_item) {
      std::cerr << "The item \"" << in << "\" already exists!" << std::endl;
      exit(1);
    }
    hnode tmp;
    // add spacer to header
    if ((*tb).header.size() == 0) {
      tmp.name = "-";
      tmp.rlink = 1;
      (*tb).header.push_back(tmp);
      exists_input = true;
    }
    // add primary to header
    tmp.name = in;
    tmp.llink = link;
    tmp.rlink = ++link + 1;
    (*tb).header.push_back(tmp);
    exists.push_back(in);
  }
  if (!exists_input) {
    std::cerr << "missing input" << std::endl;
    exit(1);
  }
  (*tb).header[link].rlink = 0;
  (*tb).header[0].llink = link;
  
  // add primary to node
  while ((*tb).node.size() <= (*tb).header.size()) {
    tnode tmp;
    tmp.top = 0; // this means len
    tmp.ulink = -100; // init
    tmp.dlink = -100;
    (*tb).node.push_back(tmp);
  }
  
  // input options
  while (std::getline(std::cin, input)) {
    tnode tmp;
    std::istringstream iss(input);
    // add option to node
    while (iss >> in) {
      long long opt_num = std::distance(exists.begin(), std::find(exists.begin(), exists.end(), in))+1;
      tmp.top = opt_num;
      (*tb).node[opt_num].top++;
      while ((*tb).node[opt_num].dlink > 0) {
	opt_num = (*tb).node[opt_num].dlink;
      }
      tmp.ulink = opt_num;
      tmp.dlink = -100;
      (*tb).node.push_back(tmp);
      (*tb).node[opt_num].dlink = (*tb).node.size()-1;
    }
    // add spacer to node
    long long s_num = (*tb).node.size()-1;
    while ((*tb).node[s_num].top > 0) {
      s_num--;
    }
    tmp.top = (*tb).node[s_num].top-1;
    tmp.ulink = s_num+1;
    (*tb).node.push_back(tmp);
    (*tb).node[s_num].dlink = (*tb).node.size()-2;
  }
  // set dlink and ulink (post-processing)

  // first(!=0) spacer node top = 0
  (*tb).node[(*tb).header.size()].top = 0;
  
  for (long long i = 1; i < (*tb).header.size(); i++) {
    long long x = i;
    while ((*tb).node[x].dlink > 0) {
      x = (*tb).node[x].dlink;
    }
    (*tb).node[x].dlink = (*tb).node[x].top;
    (*tb).node[i].ulink = x;
  }
  for (long long i = 1; i < (*tb).header.size(); i++) {
    if ((*tb).node[i].top == 0) {
      std::cerr << "the total number of options must be at least the number of primary items." << std::endl;
      exit(1);
    }
  }
}

void print_header(std::vector<hnode> header) {
  printf("print header \"(i) name llink rlink\"\n");
  for (long long i = 0; i < header.size(); i++) {
    printf("(%lld) %s %lld %lld\n", i, header[i].name.c_str(), header[i].llink, header[i].rlink);
  }
  std::cout << std::endl;
}

void print_node(std::vector<tnode> node) {
  printf("print node\n");
  long long print_size = 25; // number of prints per line
  for (long long i = 0; i < node.size()/print_size +1; i++) {
    for (long long j = print_size*i; j < print_size*(i+1) && j < node.size(); j++) {
      printf("%7lld", j);
    }
    std::cout << std::endl;
    for (long long j = print_size*i; j < print_size*(i+1) && j < node.size(); j++) {
      printf("%7lld", node[j].top);
    }
    std::cout << std::endl;
    for (long long j = print_size*i; j < print_size*(i+1) && j < node.size(); j++) {
      printf("%7lld", node[j].ulink);
    }
    std::cout << std::endl;
    for (long long j = print_size*i; j < print_size*(i+1) && j < node.size(); j++) {
      printf("%7lld", node[j].dlink);
    }
    std::cout << std::endl << std::endl;;
  }
}

void print_table(table tb) {
  print_header(tb.header);
  print_node(tb.node);
}

void hide(table* tb, long long p) {
  long long q = p+1;
  while (q != p) {
    long long x = (*tb).node[q].top;
    long long u = (*tb).node[q].ulink;
    long long d = (*tb).node[q].dlink;
    if (x <= 0) {
      q = u;
    } else {
      (*tb).node[u].dlink = d;
      (*tb).node[d].ulink = u;
      (*tb).node[x].top--; // len --;
      q++;
    }
  }
}

void cover(table* tb, long long i) {
  long long p = (*tb).node[i].dlink;
  while (p != i) {
    hide(tb, p);
    p = (*tb).node[p].dlink;
  }
  long long l = (*tb).header[i].llink;
  long long r = (*tb).header[i].rlink;
  (*tb).header[l].rlink = r;
  (*tb).header[r].llink = l;
}

void unhide(table* tb, long long p) {
  long long q = p-1;
  while (q != p) {
    long long x = (*tb).node[q].top;
    long long u = (*tb).node[q].ulink;
    long long d = (*tb).node[q].dlink;
    if (x <= 0) {
      q = d;
    } else {
      (*tb).node[u].dlink = q;
      (*tb).node[d].ulink = q;
      (*tb).node[x].top++;
      q--;
    }
  }
}

void uncover(table* tb, long long i) {
  long long l = (*tb).header[i].llink;
  long long r = (*tb).header[i].rlink;
  (*tb).header[l].rlink = i;
  (*tb).header[r].llink = i;
  long long p = (*tb).node[i].ulink;
  while(p != i) {
    unhide(tb, p);
    p = (*tb).node[p].ulink;
  }
}

void print_solution(table* tb, std::vector<long long> x, long long l) {
  std::cout << "====solution====" << std::endl;
  for (long long i = 0; i < l; i++) {
    long long buf = x[i];
    while ((*tb).node[--buf].top > 0);
    while ((*tb).node[++buf].top >= 0) {
      long long opt = (*tb).node[buf].top;
      std::cout << (*tb).header[opt].name << " ";
    }
    std::cout << std::endl;
  }
}


void algo_x(table tb) {
  long long l, i, p, ptr;
  std::vector<long long> x(tb.header.size(), 0);
  long long c = 1; // case selecter

  while (c != -1) {
    switch (c) {
    case 1: // initialize
      // std::cout << "initialize" << std::endl;
      l = 0;
      c++;
      break;
    case 2: // enter level "l"
      // std::cout << "enter level " << l << std::endl;
      if (tb.header[0].rlink == 0) {
	// solution
	print_solution(&tb, x, l);
	c = 8;
      } else {
	c++;
      }
      break;
    case 3: // choose i
      ptr = tb.header[0].rlink;
      i = ptr;
      while (ptr != 0) {
	if (tb.node[ptr].top < tb.node[i].top) {
	  i = ptr;
	}
	ptr = tb.header[ptr].rlink;
      }
      // std::cout << "choose " << i << std::endl;
      c++;
      break;
    case 4: // cover i
      // std::cout << "cover " << i << std::endl;
      cover(&tb, i);
      x[l] = tb.node[i].dlink;
      c++;
      break;
    case 5: // try x_l
      // std::cout << "try " << x[l] << std::endl;
      if (x[l] == i) {
	c = 7;
      } else {
	p = x[l]+1;
	while (p != x[l]) {
	  long long j = tb.node[p].top;
	  if (j <= 0) {
	    p = tb.node[p].ulink;
	  } else {
	    // std::cout << "  cover " << j << std::endl;
	    cover(&tb, j);
	    p++;
	  }
	}
	l++;
	c = 2;
      }
      break;
    case 6: // try again
      // std::cout << "try again" << std::endl;
      p = x[l]-1;
      while (p != x[l]) {
	long long j = tb.node[p].top;
	if (j <= 0) {
	  p = tb.node[p].dlink;
	} else {
	  // std::cout << "  uncover " << j << std::endl;
	  uncover(&tb, j);
	  p--;
	}
      }
      i = tb.node[x[l]].top;
      x[l] = tb.node[x[l]].dlink;
      c = 5;
      break;
    case 7: // backtrack
      // std::cout << "backtrack" << std::endl;
      // std::cout << "  uncover " << i << std::endl;
      uncover(&tb, i);
      c++;
      break;
    case 8: // leave level l
      // std::cout << "leave level " << l << std::endl;
      if (l == 0) {
	c = -1; // terminate
	// std::cout << "  terminate " << std::endl;
      } else {
	l = l-1;
	c = 6;
      }
      break;
    }
  }
}

void print_dot(table tb) {
  /*
  std::cout << "digraph sample_graph {" << std::endl;
  std::cout << "graph [charset = \"UTF-8\", rankdir = LR, margin = 0.2];" << std::endl;
  std::cout << "node [shape = box, fontsize = 12, style = \"solid, filled\", fillcolor = \"#EFEFEF\"];" << std::endl;
  
  // header
  long long r = tb.header[0].rlink;
  std::cout << 0;
  while(r != 0) {
    std::cout << " -> " << r;
    r = tb.header[r].rlink;
  }
  std::cout << " -> " << r << ";" << std::endl;
  long long l = tb.header[0].llink;
  std::cout << 0;
  while (l != 0) {
    std::cout << " -> " << l;
    l = tb.header[l].llink;
  }
  std::cout << " -> " << l << ";" << std::endl;

  // options
  for (long long i = 1; i < tb.header.size(); i++) {
    long long d = tb.node[i].dlink;
    std::cout << i;
    while (d != i) {
      std::cout << " -> " << d;
      d = tb.node[d].dlink;
    }
    std::cout << " -> " << d << ";" << std::endl;
  }
  for (long long i = 1; i < tb.header.size(); i++) {
    long long u = tb.node[i].ulink;
    std::cout << i;
    while (u != i) {
      std::cout << " -> " << u;
      u = tb.node[u].ulink;
    }
    std::cout << " -> " << u << ";" << std::endl;
  }

  // ranked
  for (long long i = 1; i < tb.header.size(); i++) {
    long long d = tb.node[i].dlink;
    std::cout << "{rank = same; " << i << "; ";
    while (d != i) {
      std::cout << d << "; ";
      d = tb.node[d].dlink;
    }
    std::cout << "}" << std::endl;
  }
  
  std::cout << "}" << std::endl;
  */

  std::cout << "digraph sample_graph {" << std::endl;
  std::cout << "graph [charset = \"UTF-8\", newrank = true];" << std::endl;
  std::cout << "node [shape = box, fontsize = 12, style = \"solid, filled\", fillcolor = \"#EFEFEF\"];" << std::endl;

  long long r = tb.header[0].rlink;
  std::cout << 0;
  while(r != 0) {
    std::cout << " -> " << r;
    r = tb.header[r].rlink;
  }
  std::cout << " -> " << r << ";" << std::endl;
  long long l = tb.header[0].llink;
  std::cout << 0;
  while (l != 0) {
    std::cout << " -> " << l;
    l = tb.header[l].llink;
  }
  std::cout << " -> " << l << ";" << std::endl;

  
  for (long long i = 1; i < tb.header.size(); i++) {
    std::cout << "subgraph cluster_" << i << " {" << std::endl;
    std::cout << "penwidth = 0;" << std::endl;
    long long d = tb.node[i].dlink;
    std::cout << i;
    while (d != i) {
      std::cout << " -> " << d;
      d = tb.node[d].dlink;
    }
    std::cout << " -> " << d << ";" << std::endl;
    long long u = tb.node[i].ulink;
    std::cout << i;
    while (u != i) {
      std::cout << " -> " << u;
      u = tb.node[u].ulink;
    }
    std::cout << " -> " << u << ";" << std::endl;
    std::cout << "}" << std::endl;
  }

  std::cout << "{rank = same; ";
  for (long long i = 0; i < tb.header.size(); i++) {
    std::cout << i << "; ";
  }
  std::cout << "}" << std::endl;

  long long tmp = tb.header.size()+1;
  int i = 1;
  while (tmp < tb.node.size()) {
    std::cout << "{rank = " << i << ";";
    for (;tb.node[tmp].top > 0; tmp++) {
      std::cout << tmp << "; ";
    }
    std::cout << "}" << std::endl;
    tmp++;
    i++;
  }
  
  
  std::cout << "}" << std::endl;
}

int main() {
  table tb;
  input(&tb);
  // print_dot(tb);
  // print_table(tb);
  algo_x(tb);
}
