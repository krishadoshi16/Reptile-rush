CXX ?= g++

.PHONY: test coverage clean

test: lab4/food_position_test
	./lab4/food_position_test

lab4/food_position_test: lab4/food_position_test.cpp food_position.cpp food_position.hpp
	$(CXX) -std=c++17 lab4/food_position_test.cpp food_position.cpp -o $@

coverage: lab4/food_position_test.cpp food_position.cpp food_position.hpp
	$(CXX) -std=c++17 --coverage -O0 -g lab4/food_position_test.cpp food_position.cpp -o lab4/food_position_test
	./lab4/food_position_test
	gcov -b lab4/food_position_test-food_position.gcno

clean:
	Remove-Item -Force -ErrorAction SilentlyContinue lab4/food_position_test, food_position.gcda, food_position.gcno, food_position.cpp.gcov
