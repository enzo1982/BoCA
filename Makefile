########## BoCA directory makefile ##########

all:
	cd runtime && $(MAKE) -j 2
	cd components && $(MAKE) -j 2

clean:
	cd runtime && $(MAKE) clean
	cd components && $(MAKE) clean
