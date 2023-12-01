dlx : src/dlx.cc
	clang++ -std=c++17 -Wall -o dlx src/dlx.cc

dlc : src/dlc.cc
	clang++ -std=c++17 -Wall -o dlc src/dlc.cc

dlz : src/dlz.cc
	g++ -Wall -O3 -DB_64 -ISAPPOROBDD/include src/dlz.cc src/SAPPOROBDD/lib/BDD64.a -o dlz

clean :
	rm -rf dlx dlc dlz
