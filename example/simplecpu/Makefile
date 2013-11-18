
all: simple-cpu-pmda

simple-cpu-pmda: simplecpu.cpp ../../include/pcp-cpp/*.hpp
	g++ -I../../include simplecpu.cpp -lpcp -o simple-cpu-pmda

clean:
	rm -f simple-cpu-pmda
