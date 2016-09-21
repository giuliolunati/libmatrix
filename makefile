libmatrix.so: libmatrix.c matrix.h
	gcc -shared -o $@ $< -lm

test.bin: test.c matrix.h libmatrix.so
	gcc -o $@ $< ./libmatrix.so -lm

.PHONY: test
test: test.bin test.mat
	./$< < test.mat

.PHONY: clean
clean:
	rm -f *.o
	rm -f *.so
	rm -f test

.PHONY: backup
backup:
	rsync ./ user@giuliolunati.no-ip.org:B/Prog/libmatrix -Favun
