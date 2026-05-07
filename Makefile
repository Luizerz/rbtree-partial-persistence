CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Isrc
TARGET   = rbn-tree
INPUT    = entrada.txt

SRCS := $(shell find src -name '*.cpp')
OBJS := $(SRCS:.cpp=.o)

build: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

run: $(TARGET)
	./$(TARGET) $(INPUT)

test: build
	@echo "Running 30 tests..."; \
	passed=0; failed=0; \
	for i in $$(seq -f "%02g" 1 30); do \
		./$(TARGET) tests/entrada$$i.txt > /dev/null; \
		if diff -q saida.txt tests/saida_esperada$$i.txt > /dev/null 2>&1; then \
			echo "  Test $$i: PASSED"; passed=$$((passed+1)); \
		else \
			echo "  Test $$i: FAILED"; failed=$$((failed+1)); \
			diff saida.txt tests/saida_esperada$$i.txt; \
		fi; \
	done; \
	echo ""; \
	echo "Results: $$passed passed, $$failed failed"

clean:
	rm -f $(TARGET) $(OBJS) saida.txt
