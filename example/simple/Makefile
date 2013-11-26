# Temporary Makefile. This will be replaced with a more
# robust example sometime.

IAM = simple

CXXFLAGS = -I../../include -Wall -Werror
LDLIBS = -lpcp -lpcp_pmda

all: pmda$(IAM) pmda$(IAM)-noboost

pmda$(IAM): $(IAM).cpp ../../include/pcp-cpp/*.hpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(LDLIBS) -lboost_program_options $< -o $@

pmda$(IAM)-noboost: $(IAM).cpp ../../include/pcp-cpp/*.hpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(LDLIBS) -DPCP_CPP_NO_BOOST $< -o $@

clean:
	rm -f pmda$(IAM) pmda$(IAM)-noboost pmda_$(IAM).so
