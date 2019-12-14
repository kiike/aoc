#!/bin/sh

run() {
	printf "$1" | ./intcode "$2"
}

testeq() {
    printf "\t"
    output=$(run "$1" "$2")
    test "$output" -eq "$3" && echo "PASS " || echo "FAIL "
}

testneq() {
    printf "\t"
    output=$(run "$1" "$2")
    test "$output" -ne "$3" && echo "PASS " || echo "FAIL "
}


echo "Testing equality opcode... "
testeq "3,9,8,9,10,9,4,9,99,-1,8" 8 1
testeq "3,3,1108,-1,8,3,4,3,99" 8 1

echo "Testing less-than opcode... " # prints 1 if less than 8, else print 8
testeq "3,9,7,9,10,9,4,9,99,-1,8" 7 1
testeq "3,3,1107,-1,8,3,4,3,99 " 7 1
testeq "3,9,7,9,10,9,4,9,99,-1,8" 8 0
testeq "3,3,1107,-1,8,3,4,3,99 " 8 0

echo "Testing jump operators... "
testeq "3,12,6,12,15,1,13,14,13,4,13,99,-1,0,1,9" 0 0
testeq "3,12,6,12,15,1,13,14,13,4,13,99,-1,0,1,9" 1 1
testeq "3,3,1105,-1,9,1101,0,0,12,4,12,99,1" 0 0
testeq "3,3,1105,-1,9,1101,0,0,12,4,12,99,1" 1 1

echo "Testing input and printing operators... "
testeq "3,0,4,0,99" 5 5
testneq "3,0,4,0,99" 1 0

echo "Testing addition... "
testeq "1,2,2,0,4,0,99" 4 4
testeq "1101,2,-1,0,4,0,99" 1 1
testneq "1,2,2,0,4,0,99" 4 5

echo "Testing multiplication... "
testeq "1002,0,3,0,4,0,99" 0 3006 
testeq "1102,3,3,0,4,0,99" 9 9 
testneq "1102,3,3,0,4,0,99" 9 10

echo
