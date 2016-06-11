#Bernardo Bacellos de Castro Cunha    9293380
#Eduardo Santos Carlos de Souza       9293481
#Fabricio Guedes Faria                9293522
#Piero Lima Capelo                    9293115

all:
	gcc arvB.c -O3 -Wno-unused-result -o arvB.out

run:
	./arvB.out

test:
	gcc arvB.c -O3 -Wall -Wextra -Wno-unused-result -o arvB.out -lm -g
	valgrind --leak-check=full --track-origins=yes ./arvB.out
	rm *.out *.hea *.avb
	zip T2 *.c *.h Makefile

git:
	cp *.c Git/
	cp *.h Git/
	cp *.txt Git/
	cp Makefile Git/
	git --git-dir Git/.git/ add * 
	git --git-dir Git/.git/ commit -m "Atualizacao"
	git --git-dir Git/.git/ push