
# Falling chars

## Description

This is a reprogramming of the payload 
([visualization](https://de.wikipedia.org/wiki/Cascade_(Computervirus)#/media/Datei:Cascade_Computervirus_Payload.gif)) 
of the MS-DOS 
[Cascade computer virus](https://en.wikipedia.org/wiki/Cascade_(computer_virus)) 
for Unix systems in C using the ncurses library. It makes all the letters of the 
current view of your Unix console "fall down" one by one.


## Usage

To run the program, you must install ncurses on your system. Probably, you can 
do that using your package manager (e.g. "apt" on Debian and derivatives). 
After that, compile and link the source file with
`g++ falling_chars.c -o falling_chars -l ncurses`


## Current limitations

I started to write this in C and then added C++ code where necessary due to 
limitations of C (no vector data type, complicated string concatenation, etc.). 
Therefore, the program's source file is now a mixture of C and C++ code. Maybe, 
I will translate the whole program into pure C++ in the future.

Currently, there is a bug leaving a single character on the screen at the end, 
which also may prevent other characters above it from falling down.

Also, the program behaves strange e.g. when starting programs like vim or ssh 
doing more complicated things than just printing some output to the console.
