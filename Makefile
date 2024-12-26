all : bittester optimalfinder

bittester : bittester.c
	gcc -o $@ $^ -O2 -g

optimalfinder : optimalfinder.c
	gcc -o $@ $^ -Os -g

test : bittester
	./bittester

clean :
	rm -rf *.o *~ bittester

