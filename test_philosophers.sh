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
    echo "Output:" >> $LOG_FILE

    # Run the test case based on its type
    if [[ $test_name == *"no death test"* ]] || [[ $test_name == *"Stress test"* ]]; then
        # For no-death tests and stress tests, run for specified duration
        output=$(timeout $max_runtime $EXEC $args 2>&1)
        exit_code=$?
        
        # Check if anyone died during the run
        if echo "$output" | grep -q "died"; then
            echo -e "${RED}Test failed: A philosopher died${NC}"
            echo "$output" >> $LOG_FILE
            FAIL_COUNT=$((FAIL_COUNT + 1))
            return 1
        else
            # For no-death tests, timeout is expected and OK
            echo -e "${GREEN}Test passed: No deaths occurred during ${max_runtime}s run${NC}"
            PASS_COUNT=$((PASS_COUNT + 1))
            return 0
        fi
    elif [[ $test_name == *"Stop after meals"* ]] || [[ $test_name == *"Meals"* ]]; then
        # For meal limit tests, should exit cleanly within timeout
        output=$(timeout $max_runtime $EXEC $args 2>&1)
        exit_code=$?
        
        if [ $exit_code -eq 124 ]; then
            echo -e "${RED}Test failed: Did not complete meals within ${max_runtime}s${NC}"
            FAIL_COUNT=$((FAIL_COUNT + 1))
            return 1
        elif ! echo "$output" | grep -q "died"; then
            echo -e "${GREEN}Test passed: Completed all meals${NC}"
            PASS_COUNT=$((PASS_COUNT + 1))
            return 0
        else
            echo -e "${RED}Test failed: A philosopher died${NC}"
            FAIL_COUNT=$((FAIL_COUNT + 1))
            return 1
        fi
    elif [[ $test_name == *"Death test"* ]] || [ "$test_name" == "One philosopher" ]; then
        # For death tests, should detect and report death within timeout
        output=$(timeout $max_runtime $EXEC $args 2>&1)
        exit_code=$?
        
        if echo "$output" | grep -q "died"; then
            echo -e "${GREEN}Test passed: Death detected${NC}"
            PASS_COUNT=$((PASS_COUNT + 1))
            return 0
        else
            echo -e "${RED}Test failed: Death not detected within ${max_runtime}s${NC}"
            FAIL_COUNT=$((FAIL_COUNT + 1))
            return 1
        fi
    else
        # For error cases, check for error message and quick exit
        output=$($EXEC $args 2>&1)
        exit_code=$?
        
        if echo "$output" | grep -qi "error\|invalid\|usage"; then
            echo -e "${GREEN}Test passed: Error detected${NC}"
            PASS_COUNT=$((PASS_COUNT + 1))
            return 0
        else
            echo -e "${RED}Test failed: Error not handled${NC}"
            FAIL_COUNT=$((FAIL_COUNT + 1))
            return 1
        fi
    fi
}

# Compile the program
make re

# Test scenarios: Array of test cases
declare -A TEST_CASES=(
    ## No death cases - program should keep running until all meals are eaten or interrupted
    ["Basic no death test"]="5 800 200 200:is eating:10:died"                    # no one should die, run for 10s
    ["Fast no death test"]="5 600 150 150:is eating:5:died"                     # no one should die, faster timing, 5s
    ["Meals no death test"]="4 410 200 200 10:is eating:10:died"               # no one should die, stops after 10 meals
    ["Stress test 100"]="100 800 200 200:is eating:20:died"                    # no one should die, 20s test
    ["Stress test 105"]="105 800 200 200:is eating:20:died"                    # no one should die, 20s test
    ["Stress test 200"]="200 800 200 200:is eating:30:died"                    # no one should die, 30s test

    ## Death cases - program should detect and report death
    ["One philosopher"]="1 800 200 200:died:3:"                               # one philosopher should die
    ["Death test 310"]="4 310 200 100:died:1:"                               # death should occur around 310ms
    ["Death test 205"]="4 200 205 200:died:1:"                               # death should occur due to long eating time
    ["Death test 200"]="4 200 200 200:died:1:"                               # death should occur (eat+sleep > time_to_die)
    ["Death test 60"]="2 60 60 60:died:1:"                                   # quick death scenario

    ## Stop after meals cases - program should exit cleanly after all meals
    ["Stop after meals"]="5 800 200 200 7:is sleeping:15:"                    # should stop after 7 meals each
    ["Stop after meals 2"]="4 410 200 200 10:is sleeping:15:"                 # should stop after 10 meals each

    ## Error cases - program should exit immediately with error
    ["Zero philosophers"]="0 400 200 200:Error:1:"                           # error: invalid number of philosophers
    ["Zero time to die"]="4 0 200 200:Error:1:"                             # error: invalid time to die
    ["Zero time to eat"]="4 400 0 200:Error:1:"                             # error: invalid time to eat
    ["Zero time to sleep"]="4 400 200 0:Error:1:"                           # error: invalid time to sleep
    ["Negative time"]="4 -400 200 200:Error:1:"                             # error: negative numbers
    ["Too few arguments"]="4 400:Error:1:"                                   # error: too few arguments
    ["Too many arguments"]="4 400 200 200 10 10:Error:1:"                   # error: too many arguments
)

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
