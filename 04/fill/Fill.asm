// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, 
// the screen should be cleared.


// screensize = 0x2000
@8192
D=A
@screensize
M=D
// infinite loop
(LOOP)
@KBD
D=M
// if RAM[KBD] == 0 goto DRAW_IN_WHITE
@DRAW_IN_WHITE
D;JEQ
// else goto DRAW_IN_BLACK
@DRAW_IN_BLACK
D;JMP
(DRAW_IN_WHITE)
// k = 0
@k
M=0
(DRAWING_IN_WHITE_LOOP)
// if k == screensize goto LOOP 
@k
D=M
@screensize
D=D-M
@LOOP
D;JEQ
// RAM[SCREEN+k] = 0b00..0
@SCREEN
D=A
@k
A=D+M
M=0
// k++
@k
M=M+1
// goto DRAWING_IN_WHITE_LOOP
@DRAWING_IN_WHITE_LOOP
0;JMP
(DRAW_IN_BLACK)
// k = 0
@k
M=0
(DRAWING_IN_BLACK_LOOP)
// if k == screensize goto LOOP 
@k
D=M
@screensize
D=D-M
@LOOP
D;JEQ
// RAM[SCREEN+k] = 0b11..1
@SCREEN
D=A
@k
A=D+M
M=-1
// k++
@k
M=M+1
// goto DRAWING_IN_BLACK_LOOP
@DRAWING_IN_BLACK_LOOP
0;JMP
(END)
@END
0;JMP