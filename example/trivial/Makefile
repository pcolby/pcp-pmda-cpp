# Temporary Makefile. This will be replaced with a more
# robust example sometime.

pmdatrivial: trivial.cpp ../../include/pcp-cpp/*.hpp
	g++ -I../../include trivial.cpp -lboost_program_options -lpcp -lpcp_pmda -o pmdatrivial

clean:
	rm -f pmdatrivial pmda_trivial.so
