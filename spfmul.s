*-----------------------------------------------------------
* Title      : spf_mult.s
* Written by : J. Lovrinic
* Date       : 20241030format compatable
*              format compatable but not compliant 
*              multiplication routine for CP/M-68K 
*-----------------------------------------------------------
**********************************************************/
* Copyright 2025 John J Lovrinic                         */
* Permission is hereby granted, free of charge,          */     
* to any person obtaining a copy of this software        */
* and associated documentation files (the “Software”),   */
* to deal in the Software without restriction,           */
* including without limitation the rights to use,        */
* copy, modify, merge, publish, distribute, sublicense,  */
* and/or sell copies of the Software, and to permit      */
* persons to whom the Software is furnished to do so,    */
* subject to the following conditions:                   */
*                                                        */
* The above copyright notice and this permission notice  */
* shall be included in all copies or substantial         */
* portions of the Software.                              */
*                                                        */
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY     */
* OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT     */
* LIMITED TO THE WARRANTIES OF MERCHANTABILITY,          */
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  */
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS     */
* BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,   */
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,   */
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE         */
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */
**********************************************************/
    .globl      _spfmul
    
_spfmul:
* Version2: Shift and Add Based Solution for IEEE-754 Multiplication with 25th-bit rounding

*   load ops from stack(A7), starting at +4 and then +8 (0 must not be touched!)
    MOVE.L  4(A7),D0
    MOVE.L  8(A7),D1
*   save other registers
    MOVEM.L D2-D7/A0-A5,-(A7)
* check for zero operand
    CMP.L   #0,D0
    BEQ     mfin
    CMP.L   #0,D1
    BNE     mcalc
    MOVE.L  D1,D0
    BRA     mfin
mcalc:
* Step 1: Clear result register pair (D6:D7)
    CLR.L   D6
    CLR.L   D7

* Step 2: Prepare operands and loop counter
    MOVE.L  D0, D2
    MOVE.L  D1, D3
    AND.L   #$007FFFFF, D2
    AND.L   #$007FFFFF, D3
    OR.L    #$00800000, D2
    OR.L    #$00800000, D3
    MOVE.L  #23, D4
    MOVE.L  #0, D5          ; D5 will hold bias

* Step 3: Shift-and-Add Loop
shift_add_loop:
    ASL.L   #1, D7
    ROXL.L  #1, D6
    BTST    #23, D3
    BEQ     no_addition
    ADD.L   D2, D7
    ADDX.L  D5, D6
no_addition:
    ASL.L   #1, D3
    DBRA    D4, shift_add_loop

* Step 4: Shift result left by 8 bits (bit-by-bit, corrected)
    MOVEQ   #7, D4          ; Loop exactly 8 times
shift_left_loop:
    ASL.L   #1, D7
    ROXL.L  #1, D6
    DBRA    D4, shift_left_loop

* Step 5: Normalize based on bit 23 of D6 (optimized)
    MOVE.L  #1, D5          ; Assume bias = 1
    BTST    #23, D6
    BNE     round           ; If bit 23 is set, skip shift

    ASL.L   #1, D7
    ROXL.L  #1, D6
    MOVE.L  #0, D5          ; Correct bias to 0

* Step 6: Rounding (round-to-nearest-even)
round:
    BTST    #31, D7
    BEQ     round_done
    BTST    #0, D6
    BNE     round_done
    ADDQ.L  #1, D6
round_done:

* Step 7: Exponent Calculation
continue_exponent:
    MOVE.L  D0, D2
    MOVE.L  D1, D3
    AND.L   #$7F800000, D2
    AND.L   #$7F800000, D3
    MOVE.L  #23, D4
    LSR.L   D4, D2
    LSR.L   D4, D3
    SUB.L   #127, D2
    SUB.L   #127, D3
    ADD.L   D2, D3
    ADD.L   D5, D3
    ADD.L   #127, D3

    MOVE.L  #23, D4
    LSL.L   D4, D3
    MOVE.L  D3, D4

* Step 8: Sign Calculation
    MOVE.L  D0, D2
    MOVE.L  D1, D3
    EOR.L   D2, D3
    AND.L   #$80000000, D3
    MOVE.L  D3, D5

* Step 9: Assemble final result
    AND.L   #$007FFFFF, D6
    OR.L    D4, D6
    OR.L    D5, D6
    MOVE.L  D6, D0

mfin:
    MOVEM.L (A7)+,D2-D7/A0-A5
    RTS

