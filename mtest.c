// mtest.c - program to test floating point library functions
// Writte by:	J. Lovrinic
// Date:	20240202
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
#include <stdio.h>
#include "spflib.h"

//       declarations
#define form1 "val: %08LX result: %08LX\n"
#define form2 "val1: %08LX val2: %08LX result: %08LX\n"
#define ishexd(c) ( \
 ((c) >= '0' && (c) <= '9' ) || \
 ((c) >= 'A' && (c) <= 'F' ) || \
 ((c) >= 'a' && (c) <= 'f' ) )

//      Helper functions
long    getxval(buff,val)
char    *buff;
spf     *val;
{
LONG    j;
char    c;

        if(strlen(buff)==0) return(false);
        for(j=0L;j<8L;j++) {
                c=buff[j];
                if(!ishexd(c)) return(false);
        }
        sscanf(buff,"%08X",val);
        return(true);
}

long    get1op(test,val)
char    *test;
spf     *val;
{
char    buff[32];

        printf("%s test\nEnter value:",test);
        gets(buff);
        if(!getxval(buff,val)) return(false);
        return(true);
}

long    get2op(test,val1,val2)
char    *test;
spf     *val1;
spf     *val2;
{
char    buff[32];

        printf("%s test\nEnter first value:",test);
        gets(buff);
        if(!getxval(buff,val1)) return(false);
        printf("Enter second value:");
        gets(buff);
        if(!getxval(buff,val2)) return(false);
printf("val1: %08X val2: %08X\n",*val1,*val2);
        return(true);
}

//      Core functions test
void    multt()
{
spf     val1,val2;
spf     result;

        while (true) {
                if(!get2op("Multiply",&val1,&val2)) break;
                result=spfmul(val1,val2);
                printf(form2,val1,val2,result);
        }
}

void    addt()
{
spf     val1,val2;
spf     result;

        while(true) {
                if(!get2op("Add",&val1,&val2)) break;
                result=spfadd(val1,val2);
                printf(form2,val1,val2,result);
        }
}


void    subt()
{
spf     val1,val2;
spf     result;

        while(true) {
                if(!get2op("Subtract",&val1,&val2)) break;
                result=spfsub(val1,val2);
                printf(form2,val1,val2,result);
        }
}

void    divt()
{
spf     val1,val2;
spf     result;

        while(true) {
                if(!get2op("Divide",&val1,&val2)) break;
                result=spfdiv(val1,val2);
                printf(form2,val1,val2,result);
        }
}

void    modt()
{
spf     val1,val2;
spf     result;

        while(true) {
                if(!get2op("modulus",&val1,&val2)) break;
                result=spfmod(val1,val2);
                printf(form2,val1,val2,result);
        }
}

void    compt()
{
spf     result;
spf     val1,val2;

        while(true) {
                if(!get2op("Comparison",&val1,&val2)) break;
                result=spfcmp(val1,val2);
                printf(form2,val1,val2,result);
        }
}
//      Support function tests

void    abst()
{
spf     val,result;

        while(true) {
                if(!get1op("Float abs",&val)) break;
                result=spfabs(val);
                printf(form1,val,result);
        }
}
void    dtrt()
{
spf     val;
spf     result;

        while(true) {
                if(!get1op("Degrees to Radians:",&val)) break;
                result=spfdtr(val);
                printf(form1,val,result);
        }
}

void    rtdt()
{
spf     val;
spf     result;

        while(true) {
                if(!get1op("Radians to degrees",&val)) break;
                result=spfrtd(val);
                printf(form1,val,result);
        }
}

void    intt()
{
spf     val;
spf     result;

        while(true) {
                if(!get1op("integer component",&val)) break;
                result=spfint(val);
                printf(form1,val,result);
        }
}

void    flrt()
{
spf     val;
spf     result;
        while(true) {
                if(!get1op("floor",&val)) break;
                result=spfflr(val);
                printf(form1,val,result);
        }
}

void    rndt()
{
spf     val;
spf     result;
        while(true) {
                if(!get1op("round",&val)) break;
                result=spfrnd(val);
                printf(form1,val,result);
        }
}

void    clt()
{
spf     val;
spf     result;
        while(true) {
                if(!get1op("ceiling",&val)) break;
                result=spfcl(val);
                printf(form1,val,result);
        }
}

void  sr28t()
{
q4f     val;
q4f     result;

        while(true) {
                if(!get1op("square root",&val)) break;
                result=spfsr4(val);
                printf(form1,val,result);
        }
}

void    fract()
{
spf     val;
spf     result;

        while(true) {
                if(!get1op("Fractional Part",&val)) break;
                result=spfgfr(val);
                printf(form1,val,result);
        }
}

void    ltft()
{
spf     val;
spf     result;

        while(true) {
                if(!get1op("Long to Float",&val)) break;
                result=spfltf(val);
                printf(form1,val,result);
        }
}

void    ftlt()
{
spf     val;
long    result;

        while(true) {
                if(!get1op("Float to Long",&val)) break;
                result=spflng(val);
                printf(form1,val,result);
        }
}


void    powt()
{
spf     val1,val2,result;

        while(true) {
                if(!get2op("spf to spf power",&val1,&val2)) break;
                result=spfpow(val1,val2);
                printf(form2,val1,val2,result);
        }
}

// Conversions

void    fp_q2T()
{
spf     val;
q2f     result;

        while(true) {
                if(!get1op("IEEE to q2",&val)) break;
                result=fp_q2(val);
                printf(form1,val,result);
        }
}

void    q2_fpT()
{
q2f     val;
spf     result;

        while(true) {
                if(!get1op("q2_IEEE",&val)) break;
                result=q2_fp(val);
                printf(form1,val,result);
        }
}

void    fp_q4T()
{
spf     val;
q4f     result;

        while(true) {
                if(!get1op("IEEE_q4",&val)) break;
                result=fp_q4(val);
                printf(form1,val,result);
        }
}

void    q4_fpT()
{
q4f     val;
spf     result;

        while(true) {
                if(!get1op("q4_IEEE",&val)) break;
                result=q4_fp(val);
                printf(form1,val,result);
        }
}

void    sqrtt()
{
spf     val;
spf     result;

        while(true) {
                if(!get1op("Square Root",&val)) break;
                result=spfsqr(val);
                printf(form1,val,result);
        }
}

void    negt()
{
spf     val;
spf     result;

        while(true) {
                if(!get1op("Negate",&val)) break;
                result=spfneg(val);
                printf(form1,val,result);
        }
}

void    ipowt()
{
long    val1,val2;
long    result;

        while(true) {
                if(!get2op("Integer Power",&val1,&val2)) break;
                result=spfipow(val1,val2);
                printf(form2,val1,val2,result);
        }
}

void powit()
{
spf     val1;
long    val2;
long    result;

        while(true) {
                if(!get2op("spf to int power",&val1,&val2)) break;
                result=spfpowi(val1,val2);
                printf(form2,val1,val2,result);
        }
}

// Euler functions

void    Expt()
{
spf     val,result;
long    going;

        going=true;
        while(going) {
                if(!get1op("E^x",&val)) break;
                result=spfexp(val);
                printf(form1,val,result);
        }
}

void    lnt()
{
spf     val,result;
long going;
char c;

        going=true;
        while(going) {
                if(!get1op("Natural Log",&val)) break;
                result=spfln(val);
                printf(form1,val,result);
        }
}

void    log2t()
{
spf     val,result;
long    going;

        going=true;
        while(going) {
                if(!get1op("log base 2",&val)) break;
                result=spflg2(val);
                printf(form1,val,result);
        }
}

void    logt()
{
spf     val,result;
long    going;

        going=true;
        while(going) {
                if(!get1op("log base 10",&val)) break;
                result=spflog(val);
                printf(form1,val,result);
        }
}

// trig function tests

void    sint()
{
spf     val,result;
long    going;

        going=true;
        while(going) {
                if(!get1op("Sine",&val)) break;
                result=spfsin(val);
                printf(form1,val,result);
        }
}

void    cost()
{
spf     val,result;
long    going;

        going=true;
        while(going) {
                if(!get1op("Cosine",&val)) break;
                result=spfcos(val);
                printf(form1,val,result);
        }
}

void    tant()
{
spf     val,result;
long    going;

        going=true;
        while(going) {
                if(!get1op("Tangent",&val)) break;
                result=spftan(val);
                printf(form1,val,result);
        }
}

void    arctt()
{
spf     val,result;
long    going;

        going=true;
        while(going) {
                if(!get1op("Arctangent",&val)) break;
                result=spfatn(val);
                printf(form1,val,result);
        }
}

//      I/O tests
void    inputT()
{
long    j,k;
spf     result;
char    buff[32];
char    c;

        while(true) {
                printf("Input floating point test:\n");
                printf("Enter value:");
                gets(buff);
                k=strlen(buff);
                if(k==0L) return;
                for(j=0;j<k;j++) {
                        c=buff[j];
                        if(!isdigit(c) && c!='-' && c!='.' && c!='e' && c!='E') break;
                }
                if(j==k) {
                        result=atof(buff);
                        printf("input: %s       result: %08LX\n",buff,result);
                }
        }
}

void    outputT()
{
long    j,k;
spf     work;
char    buffi[32];
char    buffo1[32];
char    buffo2[32];
char    c;

        while(true) {
                printf("Output floating point test\n");
                printf("Input value:");
                gets(buffi);
                k=strlen(buffi);
                if(k==0L) return;
                for(j=0;j<k;j++) {
                        c=buffi[j];
                        if(!isdigit(c) && c!='-' && c!='.' && c!='e' && c!='E') break;
                }
                if(j==k) {
                        work=atof(buffi);
                        ftoa(work,buffo1,6L,'e');
                        ftoa(work,buffo2,6L,'f');
                        printf("output buffi: %s buffo1 %s buffo2 %s\n",buffi,buffo1,buffo2);
                }
        }
}

//      Section Selector Menus
void    CoreT()
{
char buff[32];
long going;
char c;

        going=true;
        while(going) {
                printf("Core Tests\n");
                printf("1. Multiply\n");
                printf("2. Add\n");
                printf("3. Subtract\n");
                printf("4. Divide\n");
                printf("Enter selection:");
                gets(buff);
                c=buff[0];
                switch(c) {
        case '1':
                multt();
                break;
        case '2':
                addt();
                break;
        case '3':
                subt();
                break;
        case '4':
                divt();
                break;
        case '5':
                modt();
                break;
        case '6':
                compt();
                break;
        default:
                going=false;
                break;
                }
        }
}


void    supbasic()
{
long    going;
char    buff[32];
char    c;

        going=true;
        while(going) {
                printf("Basic support\n");
                printf("1. Float abs\n");
                printf("2. Get integer component of spf\n");
                printf("3. Get fractional component of spf\n");
                printf("4. Get floor\n");
                printf("5. Get ceiling\n");
                printf("6. Round\n");
                printf("7. Negate\n");
                printf("Enter selection:");
                gets(buff);
                c=buff[0];
                switch (c) {
        case '1':
                abst();
                break;
        case '2':
                intt();
                break;
        case '3':
                fract();
                break;
        case '4':
                flrt();
                break;
        case '5':
                clt();
                break;
        case '6':
                rndt();
                break;
        case '7':
                negt();
                break;
        default:
                going=false;
                break;
                }
        }
}

void    supConv()
{
long    going;
char    buff[32];
char    c;

        going=true;
        while(going) {
                printf("Conversion\n");
                printf("1. Float to long\n");
                printf("2. Long to spf\n");
                printf("3. Float to q2.30\n");
                printf("4. q2.30 to Float\n");
                printf("5. Float to q4.28\n");
                printf("6. q4.28 to Float\n");
                printf("7. Degrees to Radians\n");
                printf("8. Radians to Degrees\n");
                printf("Enter selection:");
                gets(buff);
                c=buff[0];
                switch(c) {
        case '1':
                ftlt();
                break;
        case '2':
                ltft();
                break;
        case '3':
                fp_q2T();
                break;
        case '4':
                q2_fpT();
                break;
        case '5':
                fp_q4T();
                break;
        case '6':
                q4_fpT();
                break;
        case '7':
                dtrt();
                break;
        case '8':
                rtdt();
                break;
        default:
                going=false;
                break;
                }
        }
}
void    supoth()
{
long    going;
char    buff[32];
char    c;

        going=true;
        while(going) {
                printf("Other Support\n");
                printf("1. Integer power of Integer\n");
                printf("2. Integer power of Float\n");
                printf("3. Square Root\n");
                printf("Enter selection:");
                gets(buff);
                c=buff[0];
                switch(c) {
        case '1':
                ipowt();
                break;
        case '2':
                powit();
                break;
        case '3':
                sqrtt();
                break;
        default:
                going=false;
                break;
                }
        }
}



void    Supt()
{
long    going;
char    buff[32];
char    c;

        going=true;
        while(going) {
                printf("Support Functions\n");
                printf("1. Basic\n");
                printf("2. Conversion\n");
                printf("3. Other\n");
                printf("Enter selection:");
                gets(buff);
                c=buff[0];
                switch(c) {
        case '1':
                supbasic();
                break;
        case '2':
                supConv();
                break;
        case '3':
                supoth();
                break;
        default:
                going=false;
                break;
                }
        }
}

void    Trigt()
{
long    going;
char    buff[32];
char    c;

        going=true;
        while(going) {
                printf("Trig functions\n");
                printf("1. Sine\n");
                printf("2. Cosine\n");
                printf("3. Tangent\n");
                printf("4. ArcTangent\n");
                printf("Enter Selection:");
                gets(buff);
                c=buff[0];
                switch(c) {
        case '1':
                sint();
                break;
        case '2':
                cost();
                break;
        case '3':
                tant();
                break;
        case '4':
                arctt();
                break;
        default:
                going=false;
                break;
                }
        }
}

void    Eulert()
{
long    going;
char    buff[32];
char    c;

        while(going) {
                printf("Euler functions\n");
                printf("1. Exponentiation\n");
                printf("2. Natural Log\n");
                printf("3. Log base 10\n");
                printf("4. Log base 2\n");
                printf("5. Float val to Float power\n");
                printf("Enter selection:");
                gets(buff);
                c=buff[0];
                switch(c) {
        case '1':
                Expt();
                break;
        case '2':
                lnt();
                break;
        case '3':
                logt();
                break;
        case '4':
                log2t();
                break;
        case '5':
                powt();
                break;
        default:
                going=false;
                break;
                }
        }
}


void    IOT()
{
char    buff[32];
char    c;

        while(true) {
                printf("I/O tests\n");
                printf("1. Input test\n");
                printf("2. Output test \n");
                gets(buff);
                c=buff[0];
                switch(c) {
        case '1':
                inputT();
                break;
        case '2':
                outputT();
                break;
        default:
                return;
                break;
        }
}
}

void    main()
{
char    buff[32];
char    c;
printf("In IOT\n");
        while(true) {
                printf("mtest - Floating Point library test routine\n");
                printf("1. Core Functions\n");
                printf("2. Support Functions\n");
                printf("3. Exponential Functions\n");
                printf("4. Trig Functions\n");
                printf("5. I/O Functions\n");
                printf("Enter Selection:");
                gets(buff);
                c=buff[0];
printf("mtest c: %c\n");
                switch(c) {
        case '1':
                CoreT();
                break;
        case '2':
                Supt();
                break;
        case '3':
                Eulert();
                break;
        case '4':
                Trigt();
                break;
        case '5':
printf("calling IOT\n");
                IOT();
                break;
        default:
                return;
                break;
                }
        }
}

