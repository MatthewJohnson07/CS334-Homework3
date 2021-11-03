# Homework 3: Shell

* Author: Matthew Johnson
* Class: CS452 Section 2
* Semester: Fall 2021

## Overview

Within this shell, common Linux commands such as cd, pwd, history and exit can be executed very quickly in
the foreground. Using the '&' sign after a command, the command will be executed in the background. In the 
sleep command example, if you use the ps command after executing a sleep to background command, you will see
the sleep command listed in the list of processes running. Pipelining and redirection are still a work in progress
unfortunately. I'm disappointed in that and decided to comment out most of my efforts in those areas as it would 
break other areas of important code.  



## Compiling and Using
Simply, run the following commands in order to execute this shell program. Once ran, utilize like you would a normal
Linux shell. 

```
Run the compiled class with the command:
$ make clean
# make
$ ./shell
```

## Discussion
This assignment was very difficult for me. A lot of ups and downs and unfortunately I wasn't able to get as far as I would
have liked. Redirection and pipelining didn't come easy for me and I didn't give enough time for trial and error as I needed
to work through those gaps with interpretation of error and conceptual mismanagements. I am happy that the basic background
and foreground commands work as expected and the builtin history command works very well which even has a clear (-c) argument
as well that can clear the history for you. All and all, a good learning experience despite the shortcomings. 
