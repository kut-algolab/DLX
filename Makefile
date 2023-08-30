dlx : dlx.cc
	clang++ -std=c++17 -Wall -o dlx dlx.cc

dlc : dlc.cc
	clang++ -std=c++17 -Wall -o dlc dlc.cc

dlz : dlz.cc
	clang++ -std=c++17 -Wall -o dlz dlz.cc

clean :
	rm -rf dlx dlc dlz
