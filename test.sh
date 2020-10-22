#!/bin/bash

try(){
    test_output="$1"
    input="$2"
    
    ./rcc "$input" > tmp.s
    gcc -o tmp tmp.s
    ./tmp 
    real_output="$?"
    
    if [ "$real_output" = "$test_output" ]; then
        echo "$input => $real_output"
    else
        echo "$input => $test_output expected, but get $real_output"
        exit 1
    fi
}

try 0 '0;'
try 42 '42;'
try 3 '13+5-15;'
try 3 '13 + 5 -15;'
try 47 '5+6*7;'
try 15 '5*(9-6);'
try 4  '(3+5)/2;'
try 10 '-10+20;'

try 0 '0==1;'
try 1 '42==42;'
try 1 '0!=1;'
try 0 '42!=42;'

try 1 '0<1;'
try 0 '1<1;'
try 0 '2<1;'
try 1 '0<=1;'
try 1 '1<=1;'
try 0 '2<=1;'

try 1 '1>0;'
try 0 '1>1;'
try 0 '1>2;'
try 1 '1>=0;'
try 1 '1>=1;'
try 0 '1>=2;'

try 2 'a=2;'
try 123 'z=123;'

try 123 'abc=123;'

echo OK
