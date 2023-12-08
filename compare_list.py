#!/opt/homebrew/bin/python3
'''
Program to compare the grep'ed output of list of all header files with pragma and
all files modified in the project files by the script, gotten by `git status -s -uno`.
Modified the status file by removing the ' M: ' prefix.
Modified the header list file by removing the '#pragma once' suffix.

File only used for verification.
'''

FILE1 = "status.txt"
FILE2 = "list-headers.txt"

list_of_files = set() 
with open(FILE1, 'r') as f:
    for line in f:
        list_of_files.add(line.strip())

counter = 0
print(len(list_of_files))

with open(FILE2, 'r') as f:
    for i, line in enumerate(f):
        if line.strip()[2:] in list_of_files:
            counter += 1
        else:
            print(line[2:])

print(counter)
