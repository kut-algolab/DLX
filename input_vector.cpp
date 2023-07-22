#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

void input(std::vector<std::string>* primary, std::vector<std::string>* secondary, std::vector<std::vector<std::string>>* options) {
  // input items
  std::string input_items;
  std::string input_options;
  std::string tmp;
  std::getline(std::cin, input_items);
  // std::cout << s << std::endl;

  // store primary items and seconday items
  std::istringstream iss(input_items);
  bool is_primary = true;
  while (iss >> tmp) {
    // std::cout << tmp << std::endl;
    if (std::find((*primary).begin(), (*primary).end(), tmp) != (*primary).end() || std::find((*secondary).begin(), (*secondary).end(), tmp) != (*secondary).end()) {
      std::cerr << "item " << tmp << " is already exist!" << std::endl;
      exit(1);
    } else if (tmp == "|") {
      if ((*primary).size() == 0 || !is_primary) {
	std::cerr << "wrong input format" << std::endl;
	exit(1);
      }
      // std::cout << "find \"|\"" << std::endl;
      is_primary = false;
      continue;
    } else if (is_primary) {
      (*primary).push_back(tmp);
    } else {
      (*secondary).push_back(tmp);
    }
  }

  // store options
  int i = 0;
  while (std::getline(std::cin, input_options)) {
    // std::cout << input_options << std::endl;
    std::vector<std::string> vtmp;
    (*options).push_back(vtmp);
    std::istringstream iss(input_options);
    while (iss >> tmp) {
      if (std::find((*primary).begin(), (*primary).end(), tmp) == (*primary).end() && std::find((*secondary).begin(), (*secondary).end(), tmp) == (*secondary).end()) {
	std::cerr << "item \"" << tmp << "\" is not found!" << std::endl;
	exit(1);
      } else {
	(*options)[i].push_back(tmp);
      }
    }
    i++;
  }
}

void print_items(std::vector<std::string> p, std::vector<std::string> s) {
  std::cout << "print primary items" << std::endl;
  for (int i = 0; i < p.size(); i++) {
    std::cout << p[i] << " ";
  }
  std::cout << std::endl;
  std::cout << "print secondary items" << std::endl;
  for (int i = 0; i < s.size(); i++) {
    std::cout << s[i] << " ";
  }
  std::cout << std::endl;
}

void print_options(std::vector<std::vector<std::string>> options) {
  std::cout << "print options" << std::endl;
  for (int i = 0; i < options.size(); i++) {
    for (int j = 0; j < options[i].size(); j++) {
      std::cout << options[i][j] << " ";
    }
    std::cout << std::endl;
  }
}

int main() {
  std::vector<std::string> primary_item;
  std::vector<std::string> secondary_item;
  std::vector<std::vector<std::string>> options;

  /*
    input format
    A B C D E | F G    (primary items) | (secondary items)
    A C D              (options)
    D E
    B F
    A E
   */
  
  input(&primary_item, &secondary_item, &options);
  
  print_items(primary_item, secondary_item);
  print_options(options);
  

  
  return 0;
}
