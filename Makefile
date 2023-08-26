a.out : dlx.cc
	g++ -std=c++17 -Wall -o a.out dlx.cc

clean :
	rm -rf a.out