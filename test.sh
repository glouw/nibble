#!/usr/bin/env bash

GRN='\033[0;32m'
RED='\033[0;31m'
NRM='\033[0m'

gcc main.c -o lang

sources=(
    'i32 main() { ret 11 + 22 * 2 + 2 * 7 / 33 - 4; }'
)

outputs=(
    51
)

for i in "${!sources[@]}";
do

    src="${sources[$i]}"
    out="${outputs[$i]}"

    ./lang "$src" > main.ll
    clang main.ll -o prog
    ./prog
    status=$?
    echo "$src @ $status"

    if [[ $status -ne $out ]];
    then
        echo -e "${RED}FAIL${NRM}"
    else
        echo -e "${GRN}OKAY${NRM}"
    fi

done
