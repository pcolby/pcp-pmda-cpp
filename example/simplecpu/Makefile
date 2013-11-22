# Temporary Makefile. This will be replaced with a more
# robust example sometime.

pmdasimplecpu: simplecpu.cpp ../../include/pcp-cpp/*.hpp  ../../include/pcp-cpp/*/*.hpp
	g++ -I../../include simplecpu.cpp -lpcp -o pmdasimplecpu

clean:
	rm -f pmdasimplecpu pmda_simplecpu.so
