
.PHONY: rbruno.com quark former quark-clean former-clean


all : quark former rbruno.com

clean : quark-clean former-clean

quark-clean : 
	cd quark/ && make clean

former-clean : 
	cd former/ && make clean

quark : 
	cd quark/ && make
	cp quark/quark ../bin/

former : 
	cd former/ && make
	cp former/former ../bin/

rbruno.com : 
	cd rbruno.com/ && make
