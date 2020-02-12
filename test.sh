#!/bin/sh
# This is a comment

# rm *.txt
make clean
make

./sched in.txt sout_fcfs.txt FCFS
./sched in.txt sout_fcfs_100.txt FCFS 100
./sched in.txt sout_pp.txt PP
./sched in.txt sout_pp_100.txt PP 100

# if diff -w sout_fcfs.txt correct_fcfs.txt; then
#     echo Test 1 - Success--------------------FCFS--------------------------------Success
# else
#     echo Test 1 - Fail-----------------------FCFS--------------------------------Fail
# fi

# if diff -w sout_fcfs_100.txt correct_fcfs_100.txt; then
#     echo Test 2 - Success--------------------FCFS-100----------------------------Success
# else
#     echo Test 2 - Fail-----------------------FCFS-100----------------------------Fail
# fi

if diff -w sout_pp.txt correct_pp.txt; then
    echo Test 3 - Success--------------------PP-------------------------------Success
else
    echo Test 3 - Fail-----------------------PP-------------------------------Fail
fi

# if diff -w sout_pp_100.txt correct_pp_100.txt; then
#     echo Test 4 - Success--------------------PP-100---------------------------Success
# else
#     echo Test 4 - Fail-----------------------PP-100---------------------------Fail
# fi
