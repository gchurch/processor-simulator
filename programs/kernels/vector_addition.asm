ADDI R7 R0 8
ADDI R8 R0 2
MULT R8 R7 R8
ADDI R1 R0 0
ADD R2 R0 R7
ADDI R3 R0 2
LOOP1:
ADD R4 R1 R7
ADDI R5 R1 1
MULT R6 R5 R3
SWR R6 R1
SWR R6 R4
ADDI R1 R1 1
BNE R1 R2 LOOP1
ADDI R1 R0 0
ADDI R2 R0 0
ADDI R3 R0 0
ADDI R4 R0 0
ADDI R5 R0 0
ADDI R6 R0 0
ADDI R10 R0 0
ADD R11 R0 R7
LOOP2:
LWR R12 R10
ADD R13 R10 R7
LWR R13 R13
ADD R14 R12 R13
ADD R15 R10 R8
SWR R14 R15
ADDI R10 R10 1
BNE R10 R11 LOOP2
ADDI R9 R0 0
ADDI R10 R0 0
ADDI R11 R0 0
ADDI R12 R0 0
ADDI R13 R0 0
ADDI R14 R0 0
ADDI R15 R0 0
LW R1 16
LW R2 17
LW R3 18
LW R4 19
LW R5 20
LW R6 21
LW R7 22
LW R8 23
HALT