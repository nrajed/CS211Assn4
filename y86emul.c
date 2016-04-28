#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


/**Size**/
unsigned int size; //This is given the size directive at the start of the input file.

/**Memory**/
char *memory;

/**program counter **/
int pc=0;

/**Text Directive**/
char textdirective[1000]="";
int length=0;

/**End-Of-File**/
int eof=EOF;


/**Registers**/
/*
 %eax 0
 %ecx 1
 %edx 2
 %ebx 3
 %esp 4
 %ebp 5
 %esi 6
 %edi 7
 */
int reg[8]; //All the registers

/**condition codes**/
int OF; //overflow
int ZF; // zero
int SF;// negative

/**STATUS Code**/
char *statuscode = "AOK"; //Can be AOK, HLT, ADR, INS
/*
 AOK = everything all good
 HLT = straight up halt
 ADR = address error
 INS = invalid instruction
 */

union putIntsInProperly
{
    int number;
    char bits[4];
} intMem;

int checkPC()
{
    if(pc>=size || pc<0) //We can't let PC overflow
    {
        statuscode="ADR"; //Status is an address error
        return 0;
    }
    return 1;
}


void printStatus()
{
    printf("Status: %s\n", statuscode);
}

void OutputErrorMessage(char *error)
{
    printStatus();
    fprintf(stderr,"ERROR: %s\n", error);
    exit(0);
}

void OutputHelpMessage()
{
    fprintf(stderr,"Usage: %s\n", "y86emul [-h] <y86 input file>");
    exit(0);
}

void setFlags(int of, int zf, int sf)
{
    OF=of;
    ZF=zf;
    SF=sf;
}

///STEP 1: READ FILE
/*
int findDirective(char *line) ///finds the directive
{
    if(strcmp(line,"size")==0)
    {
        return 0;
    }
    else if(strcmp(line,"string")==0)
    {
        return 1;
    }
    else if(strcmp(line,"long")==0)
    {
        return 2;
    }
    else if(strcmp(line,"bss")==0)
    {
        return 3;
    }
    else if(strcmp(line,"byte")==0)
    {
        return 4;
        
    }
    else if(strcmp(line,"text")==0)
    {
        return 5;
    }
    return -1;
}*/

int addressToInt(char *address, int base) //converts a hex address to a hex integer
{
    ///Need to take a string, make it a decimal integer
 //   printf("address b40: %s\n",address);
    
    int value=0;


    while(address[0]=='0')
    {
        address+=1;
    }
  //  printf("address: %s\n",address);
    
    while(address[0]!='\0')
    {
        char c = address[0];
        if(c==' ') {break;}
        //printf("char: \n%c\n",c);
        int digit=0;
        if(isdigit(c))
        {
            digit=c-'0';
        }
        else if(isxdigit(c))
        {
            if(c=='A' || c=='a') {digit=10;}
            if(c=='B' || c=='b') {digit=11;}
            if(c=='C' || c=='c') {digit=12;}
            if(c=='D' || c=='d') {digit=13;}
            if(c=='E' || c=='e') {digit=14;}
            if(c=='F' || c=='f') {digit=15;}
        }
        else
        {
            statuscode="INS";
            OutputErrorMessage("Invalid directive input!");
        }
        //printf("digit %d\n",digit);
        value=value*base+digit;
        //printf("value %d\n",value);
        address+=1;
        
        //printf("address: \n%s\n",address);
    }
    //printf("value %d\n",value);
    //printf("%s\n","");
    return value;
}


///STEP 2: PARSING INSTRUCTION

int state6(char code)
{
    switch(code)
    {
        case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        default: OutputErrorMessage("Invalid code!"); //Incorrect code
    }
    return -1;
}

int state7(char code)
{
    switch(code)
    {
        case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        default: OutputErrorMessage("Invalid instruction!"); //Incorrect code
    }
    return -1;
}

int stateC(char code)
{
    switch(code)
    {
        case '0': return 0;
        case '1': return 1;
        default: OutputErrorMessage("Invalid instruction!"); //Incorrect code
    }
    return -1;
}

int stateD(char code)
{
    switch(code)
    {
        case '0': return 0;
        case '1': return 1;
        default: OutputErrorMessage("Invalid instruction!"); //Incorrect code
    }
    return -1;
}

int findInstruction(char *opCode)
{
    char code = opCode[0];
    pc+=2;
    switch(code)
    {
        case '0': return 0; //nop
        case '1': return 10; //halt
        case '2': return 20; //rrmovl
        case '3': return 30; //irmovl
        case '4': return 40; //rmmovl
        case '5': return 50; //mrmovl
        case '6': return 60+state6(code); //state 6 will break down op
        case '7': return 70+state7(code); //state 7 will break down jmp
        case '8': return 80; //call
        case '9': return 90; //ret
        case 'A': return 100; //pushl
        case 'B': return 110; //popl
        case 'C': return 120+stateC(code); //will break down read
        case 'D': return 121+stateD(code); //will break down write
        default: OutputErrorMessage("Invalid instruction!");
    }
    return -1;
}

///Part 3: Execute
/**
 Execution Process:
 1. Get all commands in order
 2. Call command and move up program counter accordingly
 3.
 **/

/**All Methods**/

void nop() ///00
{
} //No operation
void halt() ///10 //HALT instruction
{
    statuscode="HLT";
}
void rrmovl(int rA, int rB)///20
{
    int val=reg[rA];
    reg[rB]=val;
    printf("Hello! you're working %s\n","");
}
void irmovl(int F, int rB) ///30 <- hex F represents unused register
{
    int val = F;
    reg[rB]=val;

}
void rmmovl(int rA, int rB, int D) ///40
{
    int val=reg[rA];
    memory[reg[rB]+D]=val;
}
void mrmovl(int rA, int rB, int D) ///50
{
    int val=memory[reg[rA]+D];
    memory[reg[rB]]=val;
}

///OP INSTRUCTIONS,
void addl(int rA, int rB) ///ADDS rA and rB and stores value in register B
{
    int sum=reg[rA]+reg[rB];
    reg[rB]=sum;
    setFlags(sum==0, (rA > 0 && rB > 0 && sum < 0) || (rA < 0 && rB < 0 && sum > 0), sum<0); //Sets all three flags
}

void subl(int rA, int rB)
{
    int diff=reg[rB]-reg[rA];
    reg[rB]=diff;
    setFlags(diff==0, (rA < 0 && rB > 0 && diff < 0) || (rA > 0 && rB < 0 && diff > 0), diff<0); //Sets three flags
}

void mull(int rA, int rB)
{
    int product = reg[rA]*reg[rB];
    reg[rB]=product;
    setFlags(product==0, (rA > 0 && rB > 0 && product < 0) || (rA < 0 && rB < 0 && product < 0) ||
             (rA < 0 && rB > 0 && product > 0) || (rA > 0 && rB < 0 && product > 0), product<0);
}

void xorl(int rA, int rB)
{
    int res = reg[rB]^reg[rA];
    reg[rB]=res;
    setFlags(res == 0,0,res<0);
}

///JUMP INSTRUCTIONS

int jmp() ///70
{
    return 1; //Always jumps.
}
int jle() ///71
{
    return (SF^OF)||ZF; //If true, returns 0, else returns 1.
}
int jl() ///72
{
    return SF ^ OF;
}
int je() ///73
{
    return ZF;
}
int jne() ///74
{
    return ZF==0;
}
int jge() ///75
{
    return ~(SF ^ OF);
}
int jg() ///76
{
    return ~(SF ^ OF) && ~ZF;
}
///END OF JUMP INSTRUCTIONS

///CALL, RET, PUSH AND POP

void call(int dest) ///80
{
    int esp=4;
    
    int valC=dest+pc+1; //destination
    int valP=pc+5; //return point
    int valB=reg[esp]; //stack val
    
    int valE=valB-4; //decrement stack pointer
    memory[valE]=valP;
    reg[esp]=valE;
    pc=valC; //PRETTY SURE THIS IS WHAT CALL DOES??
}

void ret()
{
    int esp=4;
    
    int valA = reg[esp];
    int valB = reg[esp];
    int valE = valB+4;
    int valM = memory[valA];
    reg[esp]=valE;
    pc=valM;
}

void pushl(int rA, int F)
{
    reg[4]-=4;
    memory[reg[4]]=rA; //NEED TO PROPERLY WRITE INTS INTO MEMORY
    int esp=4;
    
    int valP = pc+2; //next register byte
    int valA = reg[rA];
    int valB = reg[esp];
    int valE = valB-4;
    int valM = memory[valA];
    
    reg[esp]=valE;
    reg[esp]=valM;
    pc=valP;
}

void popl(int rA, int F)
{
    reg[rA]=memory[reg[4]];
    reg[4]+=4;
}

///Read and Write Instructions

void readb(int rA, int F, int D, int eof) //READS A SINGLE BYTE INTO MEMORY
{
    //if reached end of file, zf is 1, else 0.
    setFlags(eof,0,0);
}

void readl(int rA, int F,  int D,int eof)//READS FOUR BYTES INTO MEMORY
{
    setFlags(eof,0,0);
}

void writeb(int rA, int F, int D) //PRINTS TO THE TERMINAL
{
    
}

void writel(int rA, int F, int D) //PRINTS TO THE TERMINAL
{
    
}
///END OF READ&WRITE INSTRUCTIONS


int executeInstruction(int instruction) //Calls from Textdirective and is modified based on needs.
{
    ///Given an instruction, finds the necessary arguments and calls the method to execute.
    ///Returns 1 if successful, returns 0 if problem occurs
    
    ///In each method, veryify the input given is valid and then call or OutputError
    
    if(checkPC()==0)
    {
        OutputErrorMessage("Address Error!");
    }
    
    
    if(instruction==0) {nop(); pc++;}
    else if(instruction==10) {halt();}
    else if(instruction>=20 && instruction<=50) ///MOV instructions, 6 bytes
    {
        
        char num1[2];
        char num2[2];
        
        num1[0]=textdirective[pc]; num1[strlen(num1)]='\0'; //first register as a char
        pc+=1;
        
        num2[0]=textdirective[pc]; num2[strlen(num2)]='\0';//second register as a char
        pc+=1;
            
        int n1=addressToInt(num1, 16); //register 1
        
        int n2=addressToInt(num2, 16); //register 2
        

        
        if(instruction==20)
        {
            rrmovl(n1,n2);
            
        }
        else
        {
            char displacement[5];
            int k;
            for(k=0; k<4; k++)
            {
                displacement[k]=textdirective[pc];
                pc+=1;
            }

            displacement[4]='\0';

            int dis=addressToInt(displacement,16);

            switch(instruction)
            {
                case 30: irmovl(n1,n2);
                case 40: rmmovl(n1,n2,dis);
                case 50: mrmovl(n1,n2,dis);
            }
            
        }
    }
    else if(instruction>=60 && instruction<=70) //Op instructions, 4 bytes
    {
        char num1[2];
        char num2[2];
        
        num1[0]=textdirective[pc]; num1[strlen(num1)]='\0'; //first register as a char
        pc+=1;
        
        num2[0]=textdirective[pc]; num2[strlen(num2)]='\0';//second register as a char
        pc+=1;
        
        int n1=addressToInt(num1, 16); //register 1
        int n2=addressToInt(num2, 16); //register 2
        
        switch(instruction)
        {
            case 60: addl(n1,n2);
            case 61: subl(n1,n2);
            case 62: mull(n1,n2);
            case 63: xorl(n1,n2);
        }

    }
    else if(instruction>=70 && instruction<=80) //JMP instructions, 5 bytes
    {
        int ShouldIJump=0;
        switch(instruction)
        {
            case 70: ShouldIJump=jmp();
            case 71: ShouldIJump=jle();
            case 72: ShouldIJump=jl();
            case 73: ShouldIJump=je();
            case 74: ShouldIJump=jne();
            case 75: ShouldIJump=jge();
            case 76: ShouldIJump=jg();
        }
        if(ShouldIJump==1)
        {
            char dest[8];
            int k;
            for(k=0; k<8; k++)
            {
                dest[k]=textdirective[pc];
                pc+=1;
            }
            pc=addressToInt(dest,16);
        }
        else
        {
            pc+=5; //We increment 5*2=10 bytes
        }
    }
    else if(instruction==80) //call instruction, 5 bytes
    {
        char destination[8];
        int k;
        for(k=0; k<8; k++)
        {
            destination[k]=textdirective[pc];
            pc+=1;
            
        }
        destination[strlen(destination)]='\0';
        
        int dest=addressToInt(destination,16);
        ///Calls a function at an address
        //call
        call(dest);
        
    }
    else if(instruction==90) //return instruction, 2 bytes
    {
        ret();
    }
    else if(instruction==100 || instruction==110) //Push or pop instruction, 2 bytes
    {
        char num1[1]; //rA
        char num2[1]; //F
        
        num1[0]=textdirective[pc]; num1[strlen(num1)]='\0'; //first register as a char
        pc+=1;
        
        num2[0]=textdirective[pc]; num2[strlen(num2)]='\0';//second register as a char
        pc+=1;
    }
    else //READ OR WRITE INSTRUCTIONS, 6 bytes
    {
        char num1[1];
        char num2[1];
        
        num1[0]=textdirective[pc]; num1[strlen(num1)]='\0'; //first register as a char
        pc+=1;
        
        num2[0]=textdirective[pc]; num2[strlen(num2)]='\0';//second register as a char
        pc+=1;
        
        int n1=addressToInt(num1, 16); //register 1
        int n2=addressToInt(num2, 16); //register 2
        
        char displacement[8];
        int k;
        for(k=0; k<8; k++)
        {
            displacement[k]=textdirective[pc];
            pc+=1;
            
        }
        displacement[strlen(displacement)]='\0';
        
        int dis=addressToInt(displacement,16);
        
        switch(instruction)
        {
            case 120: readl(n1,n2,dis,eof);
            case 121: readl(n1,n2,dis,eof);
            case 130: writeb(n1,n2,dis);
            case 131: writel(n1,n2,dis);
        }
    }
    return 1;
}


int storeIntInMem(int addr)
{
    //Stores bit by bit in memory the integer
	intMem.bits[0]= memory[addr]; addr++;
	intMem.bits[1]= memory[addr]; addr++;
	intMem.bits[2]= memory[addr]; addr++;
	intMem.bits[3]= memory[addr];
	return intMem.number;
}

int factorial(int num)
{
    if(num==0) {return 1;}
    return num*factorial(num-1);
}

void recallIntfromMem(int addr, int value)
{
	intMem.number = value;
	memory[addr]= intMem.bits[0]; addr++;
    memory[addr]= intMem.bits[1]; addr++;
	memory[addr]= intMem.bits[2]; addr++;
	memory[addr]= intMem.bits[3];
}

int main(int argc, char**argv)
{
    if(argc!=2) {statuscode="INS"; OutputErrorMessage("Invalid number of arguments!");}
    if(strcmp(argv[1],"-h")==0) { OutputHelpMessage();}
    //CHECK FILE
    FILE *f;
    f=fopen(argv[1],"r");
    //Must check if file exists first
    if(f==NULL)
    {
        statuscode="INS";
        OutputErrorMessage("File not in directory!");
    }
    
    ///Want to allocate memory first
    char *directive=malloc(4*sizeof(unsigned char));
    unsigned int *sizeAsHex=malloc(4*sizeof(unsigned int));
    
    fscanf(f, "%s\t%x", directive, sizeAsHex); //<- so this here reads the value straight up as hex
   // printf("%X\n",sizeAsHex[0]);
    
    if(!strcmp(directive,".size")==0)
    {
        statuscode="INS";
        OutputErrorMessage("Invalid start to file! No size directive!");
    }
    
    
    size=*sizeAsHex;
    
    memory=malloc(sizeof(char)*size); ///we have allocated our size and will now scan the rest of the file
    
    //SCAN FILE
    unsigned int *address=malloc(4*sizeof(unsigned int));
    //address=0;
    unsigned int *value=malloc(4*sizeof(unsigned int));
    //value=0;
    
    //SCAN THE FILE
    while(fscanf(f, "%s", directive)!=EOF) //EOF indicates end of file
    {
        if(strcmp(".byte",directive)==0)
        {
            fscanf(f,"\t%x\t%x",address,value);
            memory[*address]=*value;

        }
        if(strcmp(".long",directive)==0)
        {
            fscanf(f,"\t%x\t%x",address,value);
            memory[*address]=*value;
        }
        if(strcmp(".string",directive)==0)
        {
            fscanf(f,"\t%x\t",address);
            char c=fgetc(f);
            //printf("%c", c);
            while(EOF!=(c=fgetc(f)) && c!='\n')
            {
                if(*address>size)
                {
                    statuscode="ADR";
                    OutputErrorMessage("Address too large");
                }
                memory[*address]=c;
                //printf("%c", c);
                *address+=1;

                
            }
          //  printf("%s\n","");
        }
        if(strcmp(".bss",directive)==0)
        {
            //DO NOTHING I GUESS??
        }
        if(strcmp(".text",directive)==0)
        {
            fscanf(f,"\t%x\t",address);
            
            char c=fgetc(f);
            //printf("%c", c);
            int i=0;
            while(c!=EOF && c!='\n')
            {
                textdirective[i]=c;
                i++;
                c=fgetc(f);
                //printf("%c", c);
            }
            textdirective[i++]=c;
            textdirective[strlen(textdirective)-1]='\0';
        }
        //printf("directive %s\n",directive);
        directive=realloc(directive, (4*sizeof(unsigned char)));
        address=realloc(address,(4*sizeof(unsigned int)));
        value=realloc(value,(4*sizeof(unsigned int)));
        ///NEED TO REALLOCATE THE MEMORY
        //free(directive);
        //free(address);
        //free(value);
    }
    length = strlen(textdirective); //printf("%d\n",length); //printf("%s\n",textdirective);
    //end of SCANNING PROCESS AND DOING THE BIDDING OF THE DIRECTIVES.
    
    //READING TEXT DIRECTIVE
    
    
    /*used in calculating num1,num2*/
    //textdirective is a bunch of chars
    /*int instr=findInstruction(textdirective);
    printf("%d\n",instr);
    printf("%c\n",textdirective[2]);
    
    executeInstruction(instr);
    printf("%d\n",reg[4]);*/
    //printf("%d\n",executeInstruction(instr));
    while(pc>length)
    {
        int instr=findInstruction(textdirective);
        executeInstruction(instr);
        printf("pc: %d\n", pc);
        printf("text: %s\n",textdirective+pc);
    }
    return 0;
}

