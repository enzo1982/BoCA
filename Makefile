########## BoCA directory makefile ##########

all:
	cd runtime && make -j 2
	cd components && make -j 2

clean:
	cd runtime && make clean
	cd components && make clean
