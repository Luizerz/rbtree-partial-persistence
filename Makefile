CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Isrc
TARGET   = rbn-tree

SRCS := $(shell find src -name '*.cpp')
OBJS := $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS) saida.txt

run: $(TARGET)
	./$(TARGET)
