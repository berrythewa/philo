#!/bin/bash

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Paths to executables
MANDATORY_EXEC=philo/philo
BONUS_EXEC=philo_bonus/philo_bonus

# Default executable is the mandatory part
EXEC=$MANDATORY_EXEC

# Allow user to specify the bonus executable
if [ "$1" == "bonus" ]; then
    EXEC=$BONUS_EXEC
fi

# Log file for storing test results
LOG_FILE="test_results.log"
echo "Philosophers Test Results - $(date)" > $LOG_FILE

# Track pass/fail counts
PASS_COUNT=0
FAIL_COUNT=0

# Test scenarios: Array of test cases
declare -A TEST_CASES=(
    ["Basic test"]="5 800 200 200:is eating:5:died"
    # ["One philosopher"]="1 800 200 200:has taken a fork:3:is eating"
    # ["Death case"]="4 310 200 100:died:5:"
    # ["Stop after meals"]="5 800 200 200 7:is sleeping:10:died"
    # ["Instant death"]="1 0 200 200:Error: 2:All time values must be greater than 0"
    # ["One meal only"]="5 800 200 200 1:is sleeping:5:died"
    # ["Zero values"]="5 800 0 200:Error:2:All time values must be greater than 0"
    # ["Large number of philosophers"]="200 800 200 200:is eating:10:died"
    # ["Long durations"]="5 2000 1000 1000:is thinking:15:died"
    # ["Very short durations"]="5 20 10 10:died:5:"
    # ["Odd number of philosophers"]="7 800 200 200:is eating:10:died"
    # ["Even number of philosophers"]="6 800 200 200:is eating:10:died"
    # ["Maximum int values"]="5 2147483647 2147483647 2147483647:is eating:10:died"
    # ["Negative values"]="-5 800 200 200:Error:2:"
    # ["Non-numeric input"]="five 800 200 200:Error:2:"
    # ["Too few arguments"]="5 800 200:Error:2:"
    # ["Too many arguments"]="5 800 200 200 7 100:Error:2:"
)

# Function to run a single test
run_test() {
    local test_name=$1
    local args=$2
    local expected_output=$3
    local max_runtime=$4
    local unexpected_output=$5

    echo -e "${YELLOW}Running test: $test_name${NC}"
    echo "=== Test: $test_name ===" >> $LOG_FILE
    echo "Args: $args" >> $LOG_FILE

    # Run the test case
    output=$(timeout $max_runtime $EXEC $args 2>&1)
    exit_code=$?

    # Log the output
    echo "Output:" >> $LOG_FILE
    echo "$output" >> $LOG_FILE
    echo "Exit code: $exit_code" >> $LOG_FILE

    # Handle timeout
    if [ $exit_code -eq 124 ]; then
        if [ "$expected_output" == "timeout" ]; then
            echo -e "${GREEN}Test passed (timeout as expected)${NC}"
            PASS_COUNT=$((PASS_COUNT + 1))
            return 0
        else
            echo -e "${RED}Test failed (unexpected timeout)${NC}"
            FAIL_COUNT=$((FAIL_COUNT + 1))
            return 1
        fi
    fi

    # Check for unexpected output
    if [ -n "$unexpected_output" ] && echo "$output" | grep -q "$unexpected_output"; then
        echo -e "${RED}Test failed: Found unexpected output${NC}"
        echo "Unexpected output: $unexpected_output"
        FAIL_COUNT=$((FAIL_COUNT + 1))
        return 1
    fi

    # Check for expected output
    if echo "$output" | grep -q "$expected_output"; then
        echo -e "${GREEN}Test passed${NC}"
        PASS_COUNT=$((PASS_COUNT + 1))
        return 0
    else
        echo -e "${RED}Test failed${NC}"
        echo "Expected output containing: $expected_output"
        FAIL_COUNT=$((FAIL_COUNT + 1))
        return 1
    fi
}

# Compile the program
make re

# Iterate over test scenarios
for test_name in "${!TEST_CASES[@]}"; do
    IFS=':' read -r args expected_output max_runtime unexpected_output <<< "${TEST_CASES[$test_name]}"
    run_test "$test_name" "$args" "$expected_output" "$max_runtime" "$unexpected_output"
done

# Memory leak test
if command -v valgrind &> /dev/null; then
    echo -e "${YELLOW}Running memory leak test${NC}"
    valgrind_output=$(valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 $EXEC 5 800 200 200 2>&1)
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}No memory leaks detected${NC}"
        echo "Memory Leak Test: Passed" >> $LOG_FILE
    else
        echo -e "${RED}Memory leaks detected${NC}"
        echo "$valgrind_output" >> $LOG_FILE
        echo "Memory Leak Test: Failed" >> $LOG_FILE
    fi
else
    echo -e "${YELLOW}Valgrind not found, skipping memory leak test${NC}"
fi

# Race condition test
if command -v valgrind &> /dev/null; then
    echo -e "${YELLOW}Running race condition test${NC}"
    helgrind_output=$(valgrind --tool=helgrind --error-exitcode=1 $EXEC 5 800 200 200 2>&1)
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}No race conditions detected${NC}"
        echo "Race Condition Test: Passed" >> $LOG_FILE
    else
        echo -e "${RED}Possible race conditions detected${NC}"
        echo "$helgrind_output" >> $LOG_FILE
        echo "Race Condition Test: Failed" >> $LOG_FILE
    fi
else
    echo -e "${YELLOW}Valgrind not found, skipping race condition test${NC}"
fi

# Clean up
make fclean

# Test Summary
echo -e "${GREEN}Tests passed: $PASS_COUNT${NC}"
echo -e "${RED}Tests failed: $FAIL_COUNT${NC}"
echo -e "${YELLOW}Test results logged in $LOG_FILE${NC}"
