#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

run_test() {
    local test_name=$1
    local args=$2
    local expected_output=$3
    local max_runtime=$4
    local unexpected_output=$5

    echo -e "${YELLOW}Running test: $test_name${NC}"
    output=$(timeout $max_runtime ./philo $args 2>&1)
    exit_code=$?

    if [ $exit_code -eq 124 ]; then
        if [ "$expected_output" == "timeout" ]; then
            echo -e "${GREEN}Test passed (ran for full duration)${NC}"
            return 0
        fi
    fi

    if [ -n "$unexpected_output" ] && echo "$output" | grep -q "$unexpected_output"; then
        echo -e "${RED}Test failed: Found unexpected output${NC}"
        echo "Unexpected output: $unexpected_output"
        echo "Actual output: $output"
        return 1
    fi

    if echo "$output" | grep -q "$expected_output"; then
        echo -e "${GREEN}Test passed${NC}"
        return 0
    else
        echo -e "${RED}Test failed${NC}"
        echo "Expected output containing: $expected_output"
        echo "Actual output: $output"
        return 1
    fi
}

# Compile the program
make re

# Run test cases
run_test "Basic test (5 800 200 200)" "5 800 200 200" "is eating" 5 "died"
run_test "One philosopher (1 800 200 200)" "1 800 200 200" "has taken a fork" 3 "is eating"
run_test "Death case (4 310 200 100)" "4 310 200 100" "died" 5
run_test "Stop after meals (5 800 200 200 7)" "5 800 200 200 7" "is sleeping" 10 "died"
run_test "Instant death (1 0 200 200)" "1 0 200 200" "Error: All time values must be greater than 0" 2
run_test "One meal only (5 800 200 200 1)" "5 800 200 200 1" "is sleeping" 5 "died"
run_test "Zero values (5 800 0 200)" "5 800 0 200" "Error: All time values must be greater than 0" 2

# Additional test cases
run_test "Large number of philosophers (200 800 200 200)" "200 800 200 200" "is eating" 10 "died"
run_test "Long durations (5 2000 1000 1000)" "5 2000 1000 1000" "is thinking" 15 "died"
run_test "Very short durations (5 20 10 10)" "5 20 10 10" "died" 5
run_test "Odd number of philosophers (7 800 200 200)" "7 800 200 200" "is eating" 10 "died"
run_test "Even number of philosophers (6 800 200 200)" "6 800 200 200" "is eating" 10 "died"
run_test "Maximum int values (5 2147483647 2147483647 2147483647)" "5 2147483647 2147483647 2147483647" "is eating" 10 "died"
run_test "Negative values (-5 800 200 200)" "-5 800 200 200" "Error" 2
run_test "Non-numeric input (five 800 200 200)" "five 800 200 200" "Error" 2
run_test "Too few arguments (5 800 200)" "5 800 200" "Error" 2
run_test "Too many arguments (5 800 200 200 7 100)" "5 800 200 200 7 100" "Error" 2

# Memory leak test (requires valgrind)
if command -v valgrind &> /dev/null; then
    echo -e "${YELLOW}Running memory leak test${NC}"
    valgrind_output=$(valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./philo 5 800 200 200 2>&1)
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}No memory leaks detected${NC}"
    else
        echo -e "${RED}Memory leaks detected${NC}"
        echo "$valgrind_output"
    fi
else
    echo -e "${YELLOW}Valgrind not found, skipping memory leak test${NC}"
fi

# Race condition test (requires helgrind)
if command -v valgrind &> /dev/null; then
    echo -e "${YELLOW}Running race condition test${NC}"
    helgrind_output=$(valgrind --tool=helgrind --error-exitcode=1 ./philo 5 800 200 200 2>&1)
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}No race conditions detected${NC}"
    else
        echo -e "${RED}Possible race conditions detected${NC}"
        echo "$helgrind_output"
    fi
else
    echo -e "${YELLOW}Valgrind not found, skipping race condition test${NC}"
fi

# Clean up
make fclean