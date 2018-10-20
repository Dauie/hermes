#!/bin/bash

RED='\033[0;31m'
NC='\033[0m' # No Color

i="0"
while [ $i -lt 5 ]
do
	input=$(openssl rand -base64 16)
	printf "input is ${RED}%s${NC}\n" $input
	eval ./hermes $input
	echo
i=$[$i+1]
done

i="0"
while [ $i -lt 5 ] 
do
	input=$(jot -r 1 -20000000000 20000000000)
	printf "input is ${RED}%s${NC}\n" $input
    eval ./hermes $input
	echo
i=$[$i+1]
done

input="--worker 1514534251:1361,125.125.62.126:734,       666.666.666.666:666,\n\r   \t   127.0.0.1:8000 123.123.123.123/39"
printf "input is ${RED}%s${NC}\n" $input
eval ./hermes $input
echo

input="127.0.0.1/"
printf "input is ${RED}%s${NC}\n" $input
eval ./hermes $input
echo

input=""
printf "input is ${RED}%s${NC}\n" $input
eval ./hermes $input
echo

input="-----worker --1512515125"
printf "input is ${RED}%s${NC}\n" $input
eval ./hermes $input
echo



input="--worker $(exec ifconfig | grep -Eo 'inet (addr:)?([0-9]*\.){3}[0-9]*' | grep -Eo '([0-9]*\.){3}[0-9]*' | grep -v '127.0.0.1'):4242 --daemon 4242"
printf "input is ${RED}%s${NC}\n" $input
eval ./hermes $input
echo
