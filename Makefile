CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET   = rbn-tree

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp

clean:
	rm -f $(TARGET) saida.txt

run: $(TARGET)
	./$(TARGET)
