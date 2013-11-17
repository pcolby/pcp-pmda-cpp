
all: simple-cpu-pmda

simple-cpu-pmda: SimpleCPU.cpp ../../include/pcp-cpp/*.hpp
	g++ -I../../include SimpleCPU.cpp -lpcp -o simple-cpu-pmda

clean:
	rm -f simple-cpu-pmda
