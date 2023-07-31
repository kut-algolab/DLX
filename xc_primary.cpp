// case inputs are primary

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unistd.h>

struct HNODE {
  std::string name;
  int llink;
  int rlink;
};
typedef struct HNODE hnode;

struct TNODE {
  int top;  // if primary item, this mean len(x)
  int ulink;
  int dlink;
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
  int link = 0; // link number
  while (iss >> in) {
    hnode tmp;
    if ((*tb).header.size() == 0) {
      tmp.name = "-";
      tmp.rlink = 1;
      (*tb).header.push_back(tmp);
    }
    tmp.name = in;
    tmp.llink = link;
    tmp.rlink = ++link + 1;
    (*tb).header.push_back(tmp);
    exists.push_back(in);
  }
  (*tb).header[link].rlink = 0;
  (*tb).header[0].llink = link;

  // input options
  while (std::getline(std::cin, input)) {
    tnode tmp;
    // add primary to node
    while ((*tb).node.size() <= (*tb).header.size()) {
      tmp.top = 0; // this means len
      tmp.ulink = -100; // init
      tmp.dlink = -100;
      (*tb).node.push_back(tmp);
    }
    
    std::istringstream iss(input);
    // add option to node
    while (iss >> in) {
      int opt_num = std::distance(exists.begin(), std::find(exists.begin(), exists.end(), in))+1;
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
    int s_num = (*tb).node.size()-1;
    while ((*tb).node[s_num].top > 0) {
      s_num--;
    }
    tmp.top = (*tb).node[s_num].top-1;
    tmp.ulink = s_num+1;
    (*tb).node.push_back(tmp);
    (*tb).node[s_num].dlink = (*tb).node.size()-2;
  }
  // set dlink and ulink (post-processing)
  for (int i = 1; i < (*tb).header.size(); i++) {
    int x = i;
    while ((*tb).node[x].dlink > 0) {
      x = (*tb).node[x].dlink;
    }
    (*tb).node[x].dlink = (*tb).node[x].top;
    (*tb).node[i].ulink = x;
  }
}

void print_header(std::vector<hnode> header) {
  printf("print header \"(i) name llink rlink\"\n");
  for (int i = 0; i < header.size(); i++) {
    printf("(%d) %s %d %d\n", i, header[i].name.c_str(), header[i].llink, header[i].rlink);
  }
  std::cout << std::endl;
}

void print_node(std::vector<tnode> node) {
  printf("print node\n");
  for (int i = 0; i < node.size(); i++) {
    printf("%4d", i);
  }
  std::cout << std::endl;
  for (int i = 0; i < node.size(); i++) {
    printf("%4d", node[i].top);
  }
  std::cout << std::endl;
  for (int i = 0; i < node.size(); i++) {
    printf("%4d", node[i].ulink);
  }
  std::cout << std::endl;
  for (int i = 0; i < node.size(); i++) {
    printf("%4d", node[i].dlink);
  }
  std::cout << std::endl;
}

void print_table(table tb) {
  print_header(tb.header);
  print_node(tb.node);
}

void hide(table* tb, int p) {
  int q = p+1;
  while (q != p) {
    int x = (*tb).node[q].top;
    int u = (*tb).node[q].ulink;
    int d = (*tb).node[q].dlink;
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

void cover(table* tb, int i) {
  int p = (*tb).node[i].dlink;
  while (p != i) {
    hide(tb, p);
    p = (*tb).node[p].dlink;
  }
  int l = (*tb).header[i].llink;
  int r = (*tb).header[i].rlink;
  (*tb).header[l].rlink = r;
  (*tb).header[r].llink = l;
}

void unhide(table* tb, int p) {
  int q = p-1;
  while (q != p) {
    int x = (*tb).node[q].top;
    int u = (*tb).node[q].ulink;
    int d = (*tb).node[q].dlink;
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

void uncover(table* tb, int i) {
  int l = (*tb).header[i].llink;
  int r = (*tb).header[i].rlink;
  (*tb).header[l].rlink = i;
  (*tb).header[r].llink = i;
  int p = (*tb).node[i].ulink;
  while(p != i) {
    unhide(tb, p);
    p = (*tb).node[p].ulink;
  }
}

void print_solution(table* tb, std::vector<int> x, int l) {
  std::cout << "====solution====" << std::endl;
  for (int i = 0; i < l; i++) {
    int buf = x[i];
    while (0 <= (*tb).node[buf].top) {
      int opt = (*tb).node[buf].top;
      std::cout << (*tb).header[opt].name << " ";
      buf++;
    }
    std::cout << std::endl;
  }
}


void algo_x(table tb) {
  int l, i, p, ptr;
  std::vector<int> x(tb.header.size(), 0);
  int c = 1; // case selecter

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
	  int j = tb.node[p].top;
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
	int j = tb.node[p].top;
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


int main() {
  table tb;
  input(&tb);
  // print_table(tb);
  algo_x(tb);
}
