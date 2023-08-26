dlx : dlx.cc
	clang++ -std=c++17 -Wall -o dlx dlx.cc

dlc : dlc.cc
	clang++ -std=c++17 -Wall -o dlc dlc.cc

clean :
	rm -rf a.out
