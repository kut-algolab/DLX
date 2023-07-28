// case inputs are primary

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

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
      tb->header.push_back(tmp);
    }
    tmp.name = in;
    tmp.llink = link;
    tmp.rlink = ++link + 1;
    tb->header.push_back(tmp);
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
      tb->node.push_back(tmp);
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
      tb->node.push_back(tmp);
      (*tb).node[opt_num].dlink = (*tb).node.size()-1;
    }
    // add spacer to node
    int s_num = (*tb).node.size()-1;
    while ((*tb).node[s_num].top > 0) {
      s_num--;
    }
    tmp.top = (*tb).node[s_num].top-1;
    tmp.ulink = s_num+1;
    tb->node.push_back(tmp);
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

int main() {
  table tb;
  input(&tb);
  print_table(tb);
}
