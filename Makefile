.PHONY = all clean compile install

all:clean compile

clean:
	cd ./lib_dir && make clean
	cd ./exec && make clean

compile:
	cd ./lib_dir && make
	cd ./exec && make

install:
	cd ./lib_dir && make
	cd ./exec && make install
