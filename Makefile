all: src

.PHONY: src
src:
	cd src && make

.PHONY: test
test:
	cd test && make

.PHONY: clean
clean:
	cd test && make clean
	cd src && make clean

.PHONY: distclean
distclean:
	cd test && make distclean
	cd src && make distclean
