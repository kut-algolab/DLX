OPT = -O3 -DB_64 -I. -ISAPPOROBDD/include -ITdZdd/include

main: main.cpp SAPPOROBDD/lib/BDD64.a
	g++ $(OPT) main.cpp SAPPOROBDD/lib/BDD64.a -o main

# If you want to use GMP, use this.
main-gmp: main.cpp SAPPOROBDD/lib/BDD64.a
	g++ $(OPT) -DUSE_GMP main.cpp SAPPOROBDD/lib/BDD64.a -lgmp -lgmpxx -o main-gmp

template: template.cpp SAPPOROBDD/lib/BDD64.a
	g++ $(OPT) template.cpp SAPPOROBDD/lib/BDD64.a -o template

# If you want to use GMP, use this.
template-gmp: template.cpp SAPPOROBDD/lib/BDD64.a
	g++ $(OPT) -DUSE_GMP template.cpp SAPPOROBDD/lib/BDD64.a -lgmp -lgmpxx -o template-gmp

clean:
	rm -rf *.o *.core main template
