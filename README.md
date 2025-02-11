==Description==

This program contains 3 different source files each do the same calculation but in a different manner i will explain how each one works down bellow:

## How it works:

#Ex3q1_

- All the other sorce files are built off this code.-
- Users can enter one matrix or up to two matrices. (row,column:values).
- The program will do the calculations without the usage of sahred memory or threads.
- User can choose between 3 matrices types being:- int's, float's and complex's.


#Ex3q2a/b

- Is built on Ex3q1 but this program uses shared memory to do the calculations using 2 different source files.
- First source file will only allow for input and copy it to shared memory and the second source file to extract that data in order to do calculations.


#Ex3q3a

- It's built on Ex3q1 but this program uses threads in order to do calculations it will allow the user to input any even number of matrices.


## How to run:

1. Open terminal in linux
2. Change directory to the ex3.zip file
3. Type ./run_me.sh_1/2/3 in the terminal to start the program desired.

## Commands:


ADD:This command will add the matrices values and then proceed to print them.
SUB:This command will subtract the matrices values and then proceed to print them.
MUL:This command will multiply the matrices values and then proceed to print them
TRANSPOSE:This command will change how values are arranged in the matrix row values become columns and vice-versa.
AND:This command will do the logical and function if values are equal to 1 the result will be 1 other will be 0.
OR:This command will do the logical or function if one of the values is equal to 1 then the result will be 1 other 0
NOT:This command will do the logical not function if values is equal to 1 then it will be 0 and vice-versa.

##NOTES: Inorder to enter a matrix it needs to be in the following way (row,column:values) example (2,2:1,2,3,4).
Ex3q3a only have 4 functions implemented which are the ADD, MUL, AND, OR
