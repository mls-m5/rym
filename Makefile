
# This makefile can be used to build without using c++-modules

build/rym: src/*.cpp src/*.cppm src/*.h
	./non-module-build.sh ${CXX}

clean:
	rm -rf build/tmp
	rm -f build/rym
