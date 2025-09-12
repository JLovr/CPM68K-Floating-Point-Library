/*********************************************************
* clibadds.c - missing functions in CP/M-68k default     *
* C compiler clib                                        *
* Written:      J. Lovrinic                              *
* Date:         20250425                                 *
**********************************************************
/**********************************************************/
/* Copyright 2025 John J Lovrinic                         */
/* Permission is hereby granted, free of charge,          */     
/* to any person obtaining a copy of this software        */
/* and associated documentation files (the “Software”),   */
/* to deal in the Software without restriction,           */
/* including without limitation the rights to use,        */
/* copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit      */
/* persons to whom the Software is furnished to do so,    */
/* subject to the following conditions:                   */
/*                                                        */
/* The above copyright notice and this permission notice  */
/* shall be included in all copies or substantial         */
/* portions of the Software.                              */
/*                                                        */
/* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY     */
/* OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT     */
/* LIMITED TO THE WARRANTIES OF MERCHANTABILITY,          */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS     */
/* BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,   */
/* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,   */
/* ARISING FROM, OUT OF OR IN CONNECTION WITH THE         */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */
/**********************************************************/

#include <ctype.h>
#include <string.h>
#include <stdio.h>

//      Find first occurance of a character
char *strchr(s, c) 
char    *s;
char    c;
{
    while (*s) {
        if (*s == (char)c) return (char *)s;
        s++;
    }
    if (c == '\0') return (char *)s;
    return 0;
}

//      Find last occurance of a character
char *strrchr(s, c) 
char    *s;
char    c;
{
char *last = 0;
    while (*s) {
        if (*s == (char)c) last = s;
        s++;
    }
    if (c == '\0') return (char *)s;
    return (char *)last;
}

