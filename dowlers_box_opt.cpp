#include <iostream>
#include <vector>
#include <unordered_set>


void set_opt(std::unordered_set<std::string>* opt, std::vector<std::vector<std::vector<bool>>> o, std::string s) {
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 7; j++) {
      for (int k = 0; k < 7; k++) {
	if (o.size() <= 5-i && o[0].size() <= 7-j && o[0][0].size() <= 7-k) {
	  bool flag = true; // dowlers_box?
	  std::string tmp;
	  tmp += s;
	  tmp += " ";
	  for (int l = 0; l < o.size(); l++) {
	    for (int m = 0; m < o[l].size(); m++) {
	      for (int n = 0; n < o[l][m].size(); n++) {
		if (o[l][m][n]) {
		  if (!(i+l >= 1 && 1 <= j+m && j+m <= 5 && 1 <= k+n && k+n <= 5)) {
		    tmp += std::to_string((i*49)+(j*7)+k+(l*49)+(m*7)+n);
		    tmp += " ";
		  } else {
		    flag = false;
		  }
		}
	      }
	    }
	  }
	  if (flag) {
	    (*opt).insert(tmp);
	  }
	}
      }
    }
  }
}

std::vector<std::vector<std::vector<bool>>> zrot90(std::vector<std::vector<std::vector<bool>>> original) {
  std::vector<std::vector<std::vector<bool>>> rot(original.size(), std::vector<std::vector<bool>>(original[0][0].size(), std::vector<bool>(original[0].size(), false)));
  for (int i = 0; i < original.size(); i++) {
    for (int j = 0; j < original[i].size(); j++) {
      for (int k = 0; k < original[i][j].size(); k++) {
	rot[i][k][j] = original[i][j][k];
      }
    }
  }
  return rot;  
}
std::vector<std::vector<std::vector<bool>>> zrot180(std::vector<std::vector<std::vector<bool>>> original) {
  return zrot90(zrot90(original));
}
std::vector<std::vector<std::vector<bool>>> zrot270(std::vector<std::vector<std::vector<bool>>> original) {
  return zrot90(zrot180(original));
}

std::vector<std::vector<std::vector<bool>>> yrot90(std::vector<std::vector<std::vector<bool>>> original) {
  std::vector<std::vector<std::vector<bool>>> rot(original[0][0].size(), std::vector<std::vector<bool>>(original[0].size(), std::vector<bool>(original.size(), false)));
  for (int i = 0; i < original.size(); i++) {
    for (int j = 0; j < original[i].size(); j++) {
      for (int k = 0; k < original[i][j].size(); k++) {
	rot[k][j][i] = original[i][j][k];
      }
    }
  }
  return rot;
}
std::vector<std::vector<std::vector<bool>>> yrot180(std::vector<std::vector<std::vector<bool>>> original) {
  return yrot90(yrot90(original));
}
std::vector<std::vector<std::vector<bool>>> yrot270(std::vector<std::vector<std::vector<bool>>> original) {
  return yrot90(yrot180(original));
}

std::vector<std::vector<std::vector<bool>>> xrot90(std::vector<std::vector<std::vector<bool>>> original) {
  std::vector<std::vector<std::vector<bool>>> rot(original[0].size(), std::vector<std::vector<bool>>(original.size(), std::vector<bool>(original[0][0].size(), false)));
  for (int i = 0; i < original.size(); i++) {
    for (int j = 0; j < original[i].size(); j++) {
      for (int k = 0; k < original[i][j].size(); k++) {
	rot[j][i][k] = original[i][j][k];
      }
    }
  }
  return rot;
}
std::vector<std::vector<std::vector<bool>>> xrot180(std::vector<std::vector<std::vector<bool>>> original) {
  return xrot90(xrot90(original));
}
std::vector<std::vector<std::vector<bool>>> xrot270(std::vector<std::vector<std::vector<bool>>> original) {
  return xrot90(xrot180(original));
}

int main() {
  std::vector<std::vector<std::vector<bool>>> a = {
    {
      {0, 0, 1},
      {1, 1, 1}
    },
    {
      {0, 0, 0},
      {1, 0, 0}
    }
  };
  std::vector<std::vector<std::vector<bool>>> b = {
    {
      {0, 0, 1},
      {1, 1, 1}
    },
    {
      {0, 0, 0},
      {0, 1, 0}
    }
  };
  std::vector<std::vector<std::vector<bool>>> c = {
    {
      {0, 0, 1},
      {1, 1, 1}
    },
    {
      {0, 0, 1},
      {0, 0, 0}
    }
  };
  std::vector<std::vector<std::vector<bool>>> d = {
    {
      {0, 1, 1},
      {1, 1, 0}
    },
    {
      {0, 0, 1},
      {0, 0, 0}
    }
  };
  std::vector<std::vector<std::vector<bool>>> e = {
    {
      {0, 1, 1},
      {1, 1, 0}
    },
    {
      {0, 1, 0},
      {0, 0, 0}
    }
  };
  std::vector<std::vector<std::vector<bool>>> f = {
    {
      {0, 1, 0},
      {1, 1, 0}
    },
    {
      {0, 1, 1},
      {0, 0, 0}
    }
  };
  std::vector<std::vector<std::vector<bool>>> A = {
    {
      {1, 0, 0},
      {1, 1, 1}
    },
    {
      {0, 0, 0},
      {0, 0, 1}
    } 
  };
  std::vector<std::vector<std::vector<bool>>> B = {
    {
      {1, 0, 0},
      {1, 1, 1}
    },
    {
      {0, 0, 0},
      {0, 1, 0}
    }
  };
  std::vector<std::vector<std::vector<bool>>> C = {
    {
      {1, 0, 0},
      {1, 1, 1}
    },
    {
      {1, 0, 0},
      {0, 0, 0}
    }
  };
  std::vector<std::vector<std::vector<bool>>> D = {
    {
      {1, 1, 0},
      {0, 1, 1}
    },
    {
      {1, 0, 0},
      {0, 0, 0}
    }
  };
  std::vector<std::vector<std::vector<bool>>> E = {
    {
      {1, 1, 0},
      {0, 1, 1}
    },
    {
      {0, 1, 0},
      {0, 0, 0}
    }
  };
  std::vector<std::vector<std::vector<bool>>> F = {
    {
      {1, 1, 0},
      {0, 1, 0}
    },
    {
      {0, 0, 0},
      {0, 1, 1}
    }
  };
  std::vector<std::vector<std::vector<bool>>> j = {
    {
      {1, 0},
      {1, 1}
    },
    {
      {1, 0},
      {0, 1}
    }
  };
  std::vector<std::vector<std::vector<bool>>> k = {
    {
      {1, 1},
      {1, 1}
    },
    {
      {0, 1},
      {0, 0}
    }
  };
  std::vector<std::vector<std::vector<bool>>> l = {
    {
      {0, 0, 1},
      {1, 1, 1}
    },
    {
      {0, 0, 0},
      {0, 0, 1}
    }
  };
  std::vector<std::vector<std::vector<bool>>> m = {
    {
      {1, 1, 1},
      {0, 1, 0}
    },
    {
      {0, 1, 0},
      {0, 0, 0}
    }
  };
  std::vector<std::vector<std::vector<bool>>> n = {
    {
      {1, 1, 1},
      {0, 1, 0}
    },
    {
      {0, 0, 0},
      {0, 1, 0}
    }
  };
  std::vector<std::vector<std::vector<bool>>> O = {
    {
      {1, 1, 1, 1, 1}
    }
  };
  std::vector<std::vector<std::vector<bool>>> P = {
    {
      {1, 1},
      {1, 1},
      {1, 0}
    }
  };
  std::vector<std::vector<std::vector<bool>>> Q = {
    {
      {1, 1, 1, 1},
      {0, 0, 0, 1}
    }
  };
  std::vector<std::vector<std::vector<bool>>> R = {
    {
      {0, 1, 1},
      {1, 1, 0},
      {0, 1, 0}
    }
  };
  std::vector<std::vector<std::vector<bool>>> S = {
    {
      {0, 0, 1, 1},
      {1, 1, 1, 0}
    }
  };
  std::vector<std::vector<std::vector<bool>>> T = {
    {
      {1, 1, 1},
      {0, 1, 0},
      {0, 1, 0}
    }
  };
  std::vector<std::vector<std::vector<bool>>> U = {
    {
      {1, 0, 1},
      {1, 1, 1}
    }
  };
  std::vector<std::vector<std::vector<bool>>> V = {
    {
      {1, 0, 0},
      {1, 0, 0},
      {1, 1, 1}
    }
  };
  std::vector<std::vector<std::vector<bool>>> W = {
    {
      {1, 0, 0},
      {1, 1, 0},
      {0, 1, 1}
    }
  };
  std::vector<std::vector<std::vector<bool>>> X = {
    {
      {0, 1, 0},
      {1, 1, 1},
      {0, 1, 0}
    }
  };
  std::vector<std::vector<std::vector<bool>>> Y = {
    {
      {0, 0, 1, 0},
      {1, 1, 1, 1}
    }
  };
  std::vector<std::vector<std::vector<bool>>> Z = {
    {
      {1, 1, 0},
      {0, 1, 0},
      {0, 1, 1}
    }
  };

  

  std::vector<std::vector<std::vector<std::vector<bool>>>> opt = {a, b, c, d, e, f, A, B, C, D, E, F, j, k, l, m, n, O, P, Q, R, S, T, U, V, W, X, Y, Z};
  std::vector<std::string> opt_str = {"a", "b", "c", "d", "e", "f", "A", "B", "C", "D", "E", "F", "j", "k", "l", "m", "n", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};

  // print primary
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 7; j++) {
      for (int k = 0; k < 7; k++) {
	if (!(i >= 1 && 1 <= j && j <= 5 && 1 <= k && k <= 5)) {
	  std::cout << 49*i+7*j+k << " ";
	}
      }
    }
  }
  
  for (int i = 0; i < opt_str.size(); i++) {
    std::cout << opt_str[i] << " ";
  }
  std::cout << std::endl;

  std::unordered_set<std::string> us;

  
  // make options
  for (int i = 0; i < opt.size(); i++) {
    set_opt(&us, opt[i], opt_str[i]);
    set_opt(&us, zrot90(opt[i]), opt_str[i]);
    set_opt(&us, zrot180(opt[i]), opt_str[i]);
    set_opt(&us, zrot270(opt[i]), opt_str[i]);
    set_opt(&us, yrot90(opt[i]), opt_str[i]);
    set_opt(&us, yrot180(opt[i]), opt_str[i]);
    set_opt(&us, yrot270(opt[i]), opt_str[i]);
    set_opt(&us, xrot90(opt[i]), opt_str[i]);
    set_opt(&us, xrot180(opt[i]), opt_str[i]);
    set_opt(&us, xrot270(opt[i]), opt_str[i]);

    
    set_opt(&us, zrot90(yrot90(opt[i])), opt_str[i]);
    set_opt(&us, zrot90(yrot180(opt[i])), opt_str[i]);
    set_opt(&us, zrot90(yrot270(opt[i])), opt_str[i]);
    set_opt(&us, zrot90(xrot90(opt[i])), opt_str[i]);
    set_opt(&us, zrot90(xrot180(opt[i])), opt_str[i]);
    set_opt(&us, zrot90(xrot270(opt[i])), opt_str[i]);
    set_opt(&us, zrot180(yrot90(opt[i])), opt_str[i]);
    set_opt(&us, zrot180(yrot180(opt[i])), opt_str[i]);
    set_opt(&us, zrot180(yrot270(opt[i])), opt_str[i]);
    set_opt(&us, zrot180(xrot90(opt[i])), opt_str[i]);
    set_opt(&us, zrot180(xrot180(opt[i])), opt_str[i]);
    set_opt(&us, zrot180(xrot270(opt[i])), opt_str[i]);
    set_opt(&us, zrot270(yrot90(opt[i])), opt_str[i]);
    set_opt(&us, zrot270(yrot180(opt[i])), opt_str[i]);
    set_opt(&us, zrot270(yrot270(opt[i])), opt_str[i]);
    set_opt(&us, zrot270(xrot90(opt[i])), opt_str[i]);
    set_opt(&us, zrot270(xrot180(opt[i])), opt_str[i]);
    set_opt(&us, zrot270(xrot270(opt[i])), opt_str[i]);

    set_opt(&us, yrot90(zrot90(opt[i])), opt_str[i]);
    set_opt(&us, yrot90(zrot180(opt[i])), opt_str[i]);
    set_opt(&us, yrot90(zrot270(opt[i])), opt_str[i]);
    set_opt(&us, yrot90(xrot90(opt[i])), opt_str[i]);
    set_opt(&us, yrot90(xrot180(opt[i])), opt_str[i]);
    set_opt(&us, yrot90(xrot270(opt[i])), opt_str[i]);
    set_opt(&us, yrot180(zrot90(opt[i])), opt_str[i]);
    set_opt(&us, yrot180(zrot180(opt[i])), opt_str[i]);
    set_opt(&us, yrot180(zrot270(opt[i])), opt_str[i]);
    set_opt(&us, yrot180(xrot90(opt[i])), opt_str[i]);
    set_opt(&us, yrot180(xrot180(opt[i])), opt_str[i]);
    set_opt(&us, yrot180(xrot270(opt[i])), opt_str[i]);
    set_opt(&us, yrot270(zrot90(opt[i])), opt_str[i]);
    set_opt(&us, yrot270(zrot180(opt[i])), opt_str[i]);
    set_opt(&us, yrot270(zrot270(opt[i])), opt_str[i]);
    set_opt(&us, yrot270(xrot90(opt[i])), opt_str[i]);
    set_opt(&us, yrot270(xrot180(opt[i])), opt_str[i]);
    set_opt(&us, yrot270(xrot270(opt[i])), opt_str[i]);

    set_opt(&us, xrot90(yrot90(opt[i])), opt_str[i]);
    set_opt(&us, xrot90(yrot180(opt[i])), opt_str[i]);
    set_opt(&us, xrot90(yrot270(opt[i])), opt_str[i]);
    set_opt(&us, xrot90(zrot90(opt[i])), opt_str[i]);
    set_opt(&us, xrot90(zrot180(opt[i])), opt_str[i]);
    set_opt(&us, xrot90(zrot270(opt[i])), opt_str[i]);
    set_opt(&us, xrot180(yrot90(opt[i])), opt_str[i]);
    set_opt(&us, xrot180(yrot180(opt[i])), opt_str[i]);
    set_opt(&us, xrot180(yrot270(opt[i])), opt_str[i]);
    set_opt(&us, xrot180(zrot90(opt[i])), opt_str[i]);
    set_opt(&us, xrot180(zrot180(opt[i])), opt_str[i]);
    set_opt(&us, xrot180(zrot270(opt[i])), opt_str[i]);
    set_opt(&us, xrot270(yrot90(opt[i])), opt_str[i]);
    set_opt(&us, xrot270(yrot180(opt[i])), opt_str[i]);
    set_opt(&us, xrot270(yrot270(opt[i])), opt_str[i]);
    set_opt(&us, xrot270(zrot90(opt[i])), opt_str[i]);
    set_opt(&us, xrot270(zrot180(opt[i])), opt_str[i]);
    set_opt(&us, xrot270(zrot270(opt[i])), opt_str[i]);
    

    set_opt(&us, zrot90(yrot90(xrot90(opt[i]))), opt_str[i]);
    set_opt(&us, zrot90(yrot90(xrot180(opt[i]))), opt_str[i]);
    set_opt(&us, zrot90(yrot90(xrot270(opt[i]))), opt_str[i]);
    set_opt(&us, zrot90(yrot180(xrot90(opt[i]))), opt_str[i]);
    set_opt(&us, zrot90(yrot180(xrot180(opt[i]))), opt_str[i]);
    set_opt(&us, zrot90(yrot180(xrot270(opt[i]))), opt_str[i]);
    set_opt(&us, zrot90(yrot270(xrot90(opt[i]))), opt_str[i]);
    set_opt(&us, zrot90(yrot270(xrot180(opt[i]))), opt_str[i]);
    set_opt(&us, zrot90(yrot270(xrot270(opt[i]))), opt_str[i]);
    set_opt(&us, zrot180(yrot90(xrot90(opt[i]))), opt_str[i]);
    set_opt(&us, zrot180(yrot90(xrot180(opt[i]))), opt_str[i]);
    set_opt(&us, zrot180(yrot90(xrot270(opt[i]))), opt_str[i]);
    set_opt(&us, zrot180(yrot180(xrot90(opt[i]))), opt_str[i]);
    set_opt(&us, zrot180(yrot180(xrot180(opt[i]))), opt_str[i]);
    set_opt(&us, zrot180(yrot180(xrot270(opt[i]))), opt_str[i]);
    set_opt(&us, zrot180(yrot270(xrot90(opt[i]))), opt_str[i]);
    set_opt(&us, zrot180(yrot270(xrot180(opt[i]))), opt_str[i]);
    set_opt(&us, zrot180(yrot270(xrot270(opt[i]))), opt_str[i]);
    set_opt(&us, zrot270(yrot90(xrot90(opt[i]))), opt_str[i]);
    set_opt(&us, zrot270(yrot90(xrot180(opt[i]))), opt_str[i]);
    set_opt(&us, zrot270(yrot90(xrot270(opt[i]))), opt_str[i]);
    set_opt(&us, zrot270(yrot180(xrot90(opt[i]))), opt_str[i]);
    set_opt(&us, zrot270(yrot180(xrot180(opt[i]))), opt_str[i]);
    set_opt(&us, zrot270(yrot180(xrot270(opt[i]))), opt_str[i]);
    set_opt(&us, zrot270(yrot270(xrot90(opt[i]))), opt_str[i]);
    set_opt(&us, zrot270(yrot270(xrot180(opt[i]))), opt_str[i]);
    set_opt(&us, zrot270(yrot270(xrot270(opt[i]))), opt_str[i]);
    set_opt(&us, zrot90(xrot90(yrot90(opt[i]))), opt_str[i]);
    set_opt(&us, zrot90(xrot90(yrot180(opt[i]))), opt_str[i]);
    set_opt(&us, zrot90(xrot90(yrot270(opt[i]))), opt_str[i]);
    set_opt(&us, zrot90(xrot180(yrot90(opt[i]))), opt_str[i]);
    set_opt(&us, zrot90(xrot180(yrot180(opt[i]))), opt_str[i]);
    set_opt(&us, zrot90(xrot180(yrot270(opt[i]))), opt_str[i]);
    set_opt(&us, zrot90(xrot270(yrot90(opt[i]))), opt_str[i]);
    set_opt(&us, zrot90(xrot270(yrot180(opt[i]))), opt_str[i]);
    set_opt(&us, zrot90(xrot270(yrot270(opt[i]))), opt_str[i]);
    set_opt(&us, zrot180(xrot90(yrot90(opt[i]))), opt_str[i]);
    set_opt(&us, zrot180(xrot90(yrot180(opt[i]))), opt_str[i]);
    set_opt(&us, zrot180(xrot90(yrot270(opt[i]))), opt_str[i]);
    set_opt(&us, zrot180(xrot180(yrot90(opt[i]))), opt_str[i]);
    set_opt(&us, zrot180(xrot180(yrot180(opt[i]))), opt_str[i]);
    set_opt(&us, zrot180(xrot180(yrot270(opt[i]))), opt_str[i]);
    set_opt(&us, zrot180(xrot270(yrot90(opt[i]))), opt_str[i]);
    set_opt(&us, zrot180(xrot270(yrot180(opt[i]))), opt_str[i]);
    set_opt(&us, zrot180(xrot270(yrot270(opt[i]))), opt_str[i]);
    set_opt(&us, zrot270(xrot90(yrot90(opt[i]))), opt_str[i]);
    set_opt(&us, zrot270(xrot90(yrot180(opt[i]))), opt_str[i]);
    set_opt(&us, zrot270(xrot90(yrot270(opt[i]))), opt_str[i]);
    set_opt(&us, zrot270(xrot180(yrot90(opt[i]))), opt_str[i]);
    set_opt(&us, zrot270(xrot180(yrot180(opt[i]))), opt_str[i]);
    set_opt(&us, zrot270(xrot180(yrot270(opt[i]))), opt_str[i]);
    set_opt(&us, zrot270(xrot270(yrot90(opt[i]))), opt_str[i]);
    set_opt(&us, zrot270(xrot270(yrot180(opt[i]))), opt_str[i]);
    set_opt(&us, zrot270(xrot270(yrot270(opt[i]))), opt_str[i]);

    set_opt(&us, yrot90(zrot90(xrot90(opt[i]))), opt_str[i]);
    set_opt(&us, yrot90(zrot90(xrot180(opt[i]))), opt_str[i]);
    set_opt(&us, yrot90(zrot90(xrot270(opt[i]))), opt_str[i]);
    set_opt(&us, yrot90(zrot180(xrot90(opt[i]))), opt_str[i]);
    set_opt(&us, yrot90(zrot180(xrot180(opt[i]))), opt_str[i]);
    set_opt(&us, yrot90(zrot180(xrot270(opt[i]))), opt_str[i]);
    set_opt(&us, yrot90(zrot270(xrot90(opt[i]))), opt_str[i]);
    set_opt(&us, yrot90(zrot270(xrot180(opt[i]))), opt_str[i]);
    set_opt(&us, yrot90(zrot270(xrot270(opt[i]))), opt_str[i]);
    set_opt(&us, yrot180(zrot90(xrot90(opt[i]))), opt_str[i]);
    set_opt(&us, yrot180(zrot90(xrot180(opt[i]))), opt_str[i]);
    set_opt(&us, yrot180(zrot90(xrot270(opt[i]))), opt_str[i]);
    set_opt(&us, yrot180(zrot180(xrot90(opt[i]))), opt_str[i]);
    set_opt(&us, yrot180(zrot180(xrot180(opt[i]))), opt_str[i]);
    set_opt(&us, yrot180(zrot180(xrot270(opt[i]))), opt_str[i]);
    set_opt(&us, yrot180(zrot270(xrot90(opt[i]))), opt_str[i]);
    set_opt(&us, yrot180(zrot270(xrot180(opt[i]))), opt_str[i]);
    set_opt(&us, yrot180(zrot270(xrot270(opt[i]))), opt_str[i]);
    set_opt(&us, yrot270(zrot90(xrot90(opt[i]))), opt_str[i]);
    set_opt(&us, yrot270(zrot90(xrot180(opt[i]))), opt_str[i]);
    set_opt(&us, yrot270(zrot90(xrot270(opt[i]))), opt_str[i]);
    set_opt(&us, yrot270(zrot180(xrot90(opt[i]))), opt_str[i]);
    set_opt(&us, yrot270(zrot180(xrot180(opt[i]))), opt_str[i]);
    set_opt(&us, yrot270(zrot180(xrot270(opt[i]))), opt_str[i]);
    set_opt(&us, yrot270(zrot270(xrot90(opt[i]))), opt_str[i]);
    set_opt(&us, yrot270(zrot270(xrot180(opt[i]))), opt_str[i]);
    set_opt(&us, yrot270(zrot270(xrot270(opt[i]))), opt_str[i]);
    set_opt(&us, yrot90(xrot90(zrot90(opt[i]))), opt_str[i]);
    set_opt(&us, yrot90(xrot90(zrot180(opt[i]))), opt_str[i]);
    set_opt(&us, yrot90(xrot90(zrot270(opt[i]))), opt_str[i]);
    set_opt(&us, yrot90(xrot180(zrot90(opt[i]))), opt_str[i]);
    set_opt(&us, yrot90(xrot180(zrot180(opt[i]))), opt_str[i]);
    set_opt(&us, yrot90(xrot180(zrot270(opt[i]))), opt_str[i]);
    set_opt(&us, yrot90(xrot270(zrot90(opt[i]))), opt_str[i]);
    set_opt(&us, yrot90(xrot270(zrot180(opt[i]))), opt_str[i]);
    set_opt(&us, yrot90(xrot270(zrot270(opt[i]))), opt_str[i]);
    set_opt(&us, yrot180(xrot90(zrot90(opt[i]))), opt_str[i]);
    set_opt(&us, yrot180(xrot90(zrot180(opt[i]))), opt_str[i]);
    set_opt(&us, yrot180(xrot90(zrot270(opt[i]))), opt_str[i]);
    set_opt(&us, yrot180(xrot180(zrot90(opt[i]))), opt_str[i]);
    set_opt(&us, yrot180(xrot180(zrot180(opt[i]))), opt_str[i]);
    set_opt(&us, yrot180(xrot180(zrot270(opt[i]))), opt_str[i]);
    set_opt(&us, yrot180(xrot270(zrot90(opt[i]))), opt_str[i]);
    set_opt(&us, yrot180(xrot270(zrot180(opt[i]))), opt_str[i]);
    set_opt(&us, yrot180(xrot270(zrot270(opt[i]))), opt_str[i]);
    set_opt(&us, yrot270(xrot90(zrot90(opt[i]))), opt_str[i]);
    set_opt(&us, yrot270(xrot90(zrot180(opt[i]))), opt_str[i]);
    set_opt(&us, yrot270(xrot90(zrot270(opt[i]))), opt_str[i]);
    set_opt(&us, yrot270(xrot180(zrot90(opt[i]))), opt_str[i]);
    set_opt(&us, yrot270(xrot180(zrot180(opt[i]))), opt_str[i]);
    set_opt(&us, yrot270(xrot180(zrot270(opt[i]))), opt_str[i]);
    set_opt(&us, yrot270(xrot270(zrot90(opt[i]))), opt_str[i]);
    set_opt(&us, yrot270(xrot270(zrot180(opt[i]))), opt_str[i]);
    set_opt(&us, yrot270(xrot270(zrot270(opt[i]))), opt_str[i]);

    set_opt(&us, xrot90(yrot90(zrot90(opt[i]))), opt_str[i]);
    set_opt(&us, xrot90(yrot90(zrot180(opt[i]))), opt_str[i]);
    set_opt(&us, xrot90(yrot90(zrot270(opt[i]))), opt_str[i]);
    set_opt(&us, xrot90(yrot180(zrot90(opt[i]))), opt_str[i]);
    set_opt(&us, xrot90(yrot180(zrot180(opt[i]))), opt_str[i]);
    set_opt(&us, xrot90(yrot180(zrot270(opt[i]))), opt_str[i]);
    set_opt(&us, xrot90(yrot270(zrot90(opt[i]))), opt_str[i]);
    set_opt(&us, xrot90(yrot270(zrot180(opt[i]))), opt_str[i]);
    set_opt(&us, xrot90(yrot270(zrot270(opt[i]))), opt_str[i]);
    set_opt(&us, xrot180(yrot90(zrot90(opt[i]))), opt_str[i]);
    set_opt(&us, xrot180(yrot90(zrot180(opt[i]))), opt_str[i]);
    set_opt(&us, xrot180(yrot90(zrot270(opt[i]))), opt_str[i]);
    set_opt(&us, xrot180(yrot180(zrot90(opt[i]))), opt_str[i]);
    set_opt(&us, xrot180(yrot180(zrot180(opt[i]))), opt_str[i]);
    set_opt(&us, xrot180(yrot180(zrot270(opt[i]))), opt_str[i]);
    set_opt(&us, xrot180(yrot270(zrot90(opt[i]))), opt_str[i]);
    set_opt(&us, xrot180(yrot270(zrot180(opt[i]))), opt_str[i]);
    set_opt(&us, xrot180(yrot270(zrot270(opt[i]))), opt_str[i]);
    set_opt(&us, xrot270(yrot90(zrot90(opt[i]))), opt_str[i]);
    set_opt(&us, xrot270(yrot90(zrot180(opt[i]))), opt_str[i]);
    set_opt(&us, xrot270(yrot90(zrot270(opt[i]))), opt_str[i]);
    set_opt(&us, xrot270(yrot180(zrot90(opt[i]))), opt_str[i]);
    set_opt(&us, xrot270(yrot180(zrot180(opt[i]))), opt_str[i]);
    set_opt(&us, xrot270(yrot180(zrot270(opt[i]))), opt_str[i]);
    set_opt(&us, xrot270(yrot270(zrot90(opt[i]))), opt_str[i]);
    set_opt(&us, xrot270(yrot270(zrot180(opt[i]))), opt_str[i]);
    set_opt(&us, xrot270(yrot270(zrot270(opt[i]))), opt_str[i]);
    set_opt(&us, xrot90(zrot90(yrot90(opt[i]))), opt_str[i]);
    set_opt(&us, xrot90(zrot90(yrot180(opt[i]))), opt_str[i]);
    set_opt(&us, xrot90(zrot90(yrot270(opt[i]))), opt_str[i]);
    set_opt(&us, xrot90(zrot180(yrot90(opt[i]))), opt_str[i]);
    set_opt(&us, xrot90(zrot180(yrot180(opt[i]))), opt_str[i]);
    set_opt(&us, xrot90(zrot180(yrot270(opt[i]))), opt_str[i]);
    set_opt(&us, xrot90(zrot270(yrot90(opt[i]))), opt_str[i]);
    set_opt(&us, xrot90(zrot270(yrot180(opt[i]))), opt_str[i]);
    set_opt(&us, xrot90(zrot270(yrot270(opt[i]))), opt_str[i]);
    set_opt(&us, xrot180(zrot90(yrot90(opt[i]))), opt_str[i]);
    set_opt(&us, xrot180(zrot90(yrot180(opt[i]))), opt_str[i]);
    set_opt(&us, xrot180(zrot90(yrot270(opt[i]))), opt_str[i]);
    set_opt(&us, xrot180(zrot180(yrot90(opt[i]))), opt_str[i]);
    set_opt(&us, xrot180(zrot180(yrot180(opt[i]))), opt_str[i]);
    set_opt(&us, xrot180(zrot180(yrot270(opt[i]))), opt_str[i]);
    set_opt(&us, xrot180(zrot270(yrot90(opt[i]))), opt_str[i]);
    set_opt(&us, xrot180(zrot270(yrot180(opt[i]))), opt_str[i]);
    set_opt(&us, xrot180(zrot270(yrot270(opt[i]))), opt_str[i]);
    set_opt(&us, xrot270(zrot90(yrot90(opt[i]))), opt_str[i]);
    set_opt(&us, xrot270(zrot90(yrot180(opt[i]))), opt_str[i]);
    set_opt(&us, xrot270(zrot90(yrot270(opt[i]))), opt_str[i]);
    set_opt(&us, xrot270(zrot180(yrot90(opt[i]))), opt_str[i]);
    set_opt(&us, xrot270(zrot180(yrot180(opt[i]))), opt_str[i]);
    set_opt(&us, xrot270(zrot180(yrot270(opt[i]))), opt_str[i]);
    set_opt(&us, xrot270(zrot270(yrot90(opt[i]))), opt_str[i]);
    set_opt(&us, xrot270(zrot270(yrot180(opt[i]))), opt_str[i]);
    set_opt(&us, xrot270(zrot270(yrot270(opt[i]))), opt_str[i]);
  }
  
  // print options
  for (auto itr = us.begin(); itr != us.end(); itr++) {
    std::cout << *itr << std::endl;
  }
  
}
