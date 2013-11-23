# Temporary Makefile. This will be replaced with a more
# robust example sometime.

pmdasimple: simple.cpp ../../include/pcp-cpp/*.hpp
	g++ -I../../include simple.cpp -lpcp -lpcp_pmda -o pmdasimple

clean:
	rm -f pmdasimple pmda_simple.so
