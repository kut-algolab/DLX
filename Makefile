dlx : src/dlx.cc
	clang++ -std=c++17 -Wall -o dlx src/dlx.cc

dlc : src/dlc.cc
	clang++ -std=c++17 -Wall -o dlc src/dlc.cc

dlz : src/dlz.cc
	g++ -lgmp -std=c++17 -Wall -o dlz src/dlz.cc

clean :
	rm -rf dlx dlc dlz
