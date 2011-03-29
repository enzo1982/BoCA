########## BoCA directory makefile ##########

all:
	cd runtime && $(MAKE) -j 4
	cd components && $(MAKE) -j 4

clean:
	cd runtime && $(MAKE) clean
	cd components && $(MAKE) clean
