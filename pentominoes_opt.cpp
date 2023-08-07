#include <iostream>
#include <vector>
#include <unordered_set>


void set_opt(std::unordered_set<std::string>* opt, std::vector<std::vector<bool>> o, std::string s) {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 20; j++) {
      // 3-i行以内に収まっているか
      if (o.size() <= 3-i) {
	// 20列以内に収まる場合
	if (o[0].size() <= 20-j) {
	  std::string tmp;
	  // 該当箇所を出力
	  tmp += s;
	  tmp += " ";
	  for (int k = 0; k < o.size(); k++) {
	    for (int l = 0; l < o[k].size(); l++) {
	      if (o[k][l]) {
	        tmp += std::to_string((i*20)+j+(20*k)+l);
		tmp += " ";
	      }
	    }
	  }
	  (*opt).insert(tmp);
	}
      }
    }
  }
}

std::vector<std::vector<bool>> rot90(std::vector<std::vector<bool>> original) {
  std::vector<std::vector<bool>> rot(original[0].size(), std::vector<bool>(original.size(), false));
  for (int i = 0; i < original.size(); i++) {
    for (int j = 0; j < original[i].size(); j++) {
      rot[j][i] = original[i][j];
    }
  }
  return rot;
}

std::vector<std::vector<bool>> rot180(std::vector<std::vector<bool>> original) {
  return rot90(rot90(original));
}

std::vector<std::vector<bool>> rot270(std::vector<std::vector<bool>> original) {
  return rot90(rot180(original));
}

std::vector<std::vector<bool>> rev(std::vector<std::vector<bool>> original) {
  std::vector<std::vector<bool>> vv(original.size(), std::vector<bool>(original[0].size(), false));
  for (int i = 0; i < original.size(); i++) {
    for (int j = 0; j < original[i].size(); j++) {
      vv[i][j] = original[original.size()-1-i][j];
    }
  }
  return vv;
}

int main() {
  std::vector<std::vector<bool>> O = {
    {1, 1, 1, 1, 1}
  };
  std::vector<std::vector<bool>> P = {
    {1, 1},
    {1, 1},
    {1, 0}
  };
  std::vector<std::vector<bool>> Q = {
    {1, 1, 1, 1},
    {0, 0, 0, 1}
  };
  std::vector<std::vector<bool>> R = {
    {0, 1, 1},
    {1, 1, 0},
    {0, 1, 0}
  };
  std::vector<std::vector<bool>> S = {
    {0, 0, 1, 1},
    {1, 1, 1, 0}
  };
  std::vector<std::vector<bool>> T = {
    {1, 1, 1},
    {0, 1, 0},
    {0, 1, 0}
  };
  std::vector<std::vector<bool>> U = {
    {1, 0, 1},
    {1, 1, 1}
  };
  std::vector<std::vector<bool>> V = {
    {1, 0, 0},
    {1, 0, 0},
    {1, 1, 1}
  };
  std::vector<std::vector<bool>> W = {
    {1, 0, 0},
    {1, 1, 0},
    {0, 1, 1}
  };
  std::vector<std::vector<bool>> X = {
    {0, 1, 0},
    {1, 1, 1},
    {0, 1, 0}
  };
  std::vector<std::vector<bool>> Y = {
    {0, 0, 1, 0},
    {1, 1, 1, 1}
  };
  std::vector<std::vector<bool>> Z = {
    {1, 1, 0},
    {0, 1, 0},
    {0, 1, 1}
  };

  std::vector<std::vector<std::vector<bool>>> opt = {O, P, Q, R, S, T, U, V, W, X, Y, Z};
  std::vector<std::string> opt_str = {"O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};

  // print primary
  for (int i = 0; i < 60; i++) {
    std::cout << i << " ";
  }
  
  for (int i = 0; i < opt_str.size(); i++) {
    std::cout << opt_str[i] << " ";
  }
  std::cout << std::endl;

  std::unordered_set<std::string> us;
  // make options
  for (int i = 0; i < opt.size(); i++) {
    set_opt(&us, opt[i], opt_str[i]);
    set_opt(&us, rot90(opt[i]), opt_str[i]);
    set_opt(&us, rot180(opt[i]), opt_str[i]);
    set_opt(&us, rot270(opt[i]), opt_str[i]);
    set_opt(&us, rev(opt[i]), opt_str[i]);
    set_opt(&us, rev(rot90(opt[i])), opt_str[i]);
    set_opt(&us, rev(rot180(opt[i])), opt_str[i]);
    set_opt(&us, rev(rot270(opt[i])), opt_str[i]);
  }

  // print options
  for (auto itr = us.begin(); itr != us.end(); itr++) {
    std::cout << *itr << std::endl;
  }
  
}
