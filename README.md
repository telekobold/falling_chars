
# Falling chars

This is a reprogramming of the payload 
([visualization](https://de.wikipedia.org/wiki/Cascade_(Computervirus)#/media/Datei:Cascade_Computervirus_Payload.gif)) 
of the MS-DOS 
[Cascade computer virus](https://en.wikipedia.org/wiki/Cascade_(computer_virus)) 
in C using the NCurses library. It makes all the letters of the current view of 
your Unix console "fall down" one by one.

To run the file, you must install ncurses on your system. After that, compile
and link the file with `gcc falling_chars.c -o falling_chars -l ncurses`
