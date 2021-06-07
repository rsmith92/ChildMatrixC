# ChildMatrixC
-This project uses two programs. The goal of this project is to use child processes to more efficiently multiply matrices.<br />
-The first program is ChildMatrixMultiplyDriver, which takes in the name of the second program and the number of child processes you want to use at command line. <br />
-Next, ChildMatrixMultiplyDriver creates the number of child processes specified at the command line.<br />
-It uses execv to then run the other program (also specified at the command line) with the number of child procceses and an argument array that tells which child is running the program.<br />
<br />
-ChildMatrixMultiply then takes over to actually do the matrix multiplication.<br />
-The first thing we do in main is initialize name shared memory so that all of the different children can access the arrays.<br />
-Next, we have the first child randomize the arrays with a value from 0.0 - 1.0.<br />
-Finally we have a function that synchronizes the children before and after each multiplication so that no children will run ahead of the other.<br />
-My main currently multiplies arrC * arrA and store in arrB and then multiplies arrA * arrB and store in arrC and then lastly multiplies arrB * arrA and store in arrC and print the values in arrC.<br />
-We use a print function that I wrote for this. It is pretty ugly as you might expect with a large array but it does work.<br />
-Finally we run through and free up all of the memory that we used.<br />
<br />
-This program takes in arguments of 1, 2, 4, 8, or 16 children.<br />
-To run this program I used: ./ChildMatrixMultiplyDriver ./ChildMatrixMultiply 1<br />
<br />
-The 1 indicates how many child processes should be used.<br />
