/**Authorship Claim**/
/**Name: Sai Vardhan Vemulapalli**/

#include<bits/stdc++.h>
#include <fstream>
using namespace std;
/************** Variables,Data types and Structures used********************/
int64_t A,B,SP,PC; // The four main registers//

bool Halt =false; //Boolean to indicate whether HALT operation has been encountered//
bool error =false;
int64_t Memory_Limit = 100000;
vector< int >Memory(Memory_Limit); // Main Memory array //


struct Instruct // A data structure to store the Instruction format //
{
    int64_t Op_Code;
    int64_t value;
};
map<int64_t,string >Op_Table; //Hashmap to indicate operation for each Op_code//
vector< Instruct>Instructions; //Vector to store all the Instruction in order//
/***************************************************************************/
char Hex(int y) // Return Hexadecimal digit for decimal remainder with 16//
{
    if(y<10)
    {
        return (char)((y)+'0');
    }
    return (char)('a'+(y-10));
}
string DectoHex(int64_t x) //Helper function to convert integer to string containing Hexadecimal form//
{  string h(8,'0');
   if(x<0)
     {
        for(int i=0;i<7;i++)
        {
            h[i]='f';
        }
        int64_t g = (1ULL<<32);
        x = x+g;
     }
    
    
   
    int cnt =7;
    
    while(x)
    {
        char d =Hex(x%16);
        x /=16;
        h[cnt]=d;
        cnt--;
    }
    return h;
}
int64_t Twocomplement(int64_t x) // helper Function to return any integer in its two's complement form//
{    

    int64_t g = (1<<23);
    int64_t f =(1<<24);
    if(x<g)
    {
        return x;
    }
    else
    {
         x  = x-f;
         return x;
    }
}

struct Instruct Process(unsigned int x) //Get the Op Code and Operand from each input line//
{
    int z = (x%16);
    x /=16;
    int y =(x%16);
    y = (y*16)+z;
    x /=16;
    Instruct I;
    I.Op_Code = y;
    I.value = Twocomplement(x);
    return I;
}
void Read_Object() //Reads Object code and translates them to corresponding op code anf operand//
{
    ifstream infile;
    string FileName;
    cout<<"Please Enter the File Name:"<<endl;
    cin>>FileName;
    infile.open(FileName,ios::binary|ios::in);
    unsigned int x;
    int64_t y=0;
    while(infile.read(( char*)&x, sizeof(unsigned int)))
    {
             
            
             Memory[y]=x;
             y++;
             struct Instruct I = Process(x);
             Instructions.push_back(I);
             
             
    }
    
}

/***************Instructions and their corresponfing helper functions to implement them******/
/********************Helper Functions*************************************/
void ldc(int64_t value)
{
    B=A;
    A =value;
}
void adc(int64_t value)
{
    A +=value;
}
void ldl(int64_t offset)
{
      
      B =A;
        A = Memory[SP+offset];
   
    
}
void stl(int64_t offset)
{  
    
    
      Memory[SP+offset] = A;
       
        A=B;        
       
   
}
void ldnl(int64_t offset)
{  
    
      
        A = Memory[A+offset];      
       
    
    
    
        
    
       
}
void stnl(int64_t offset)
{  
    
     
   
        Memory[A+offset] = B;         
       
   
    
}
void add()
{  
    A = B+A;    
}
void sub()
{  
    A  = B-A;    
}
void shl()
{  
    A  = B<<A;    
}
void shr()
{  
   A  = B>>A;    
}
void adj(int64_t value)
{  
   SP = SP+ value;    
}
void a2sp()
{
    SP = A;
    A = B;  
}
void sp2a()
{
    B=A;
    A = SP;  
}
void call(int64_t offset)
{  
   B=A;
   A=PC;
   PC +=offset;
}
void Return()
{
   PC =A;
   A =B;
   
}
void brz(int64_t offset)
{
   if(A==0)
   {
    PC = PC+ offset;
   }
   
}
void brlz(int64_t offset)
{
   if(A<0)
   {
    PC = PC+ offset;
   }
   
}
void br(int64_t offset)
{
   PC = PC+ offset;
   
}
void HALT()
{
    Halt =true;
}
void Switch_Function(int64_t x,int64_t value=0) //Switch case to implement the corresponding operation based on op code//
{     

    switch(x)
    {
        case 0:
             ldc(value);
             break;
        case 1:
             adc(value);
             break;
        case 2:
             ldl(value);
             break;
        case 3:
             stl(value);
             break;
        case 4:
             ldnl(value);
             break;
        case 5:
             stnl(value);
             break; 
        case 6:
             add();
             break;
        case 7:
             sub();
             break;
        case 8:
             shl();
             break; 
        case 9:
             shr();
             break;
        case 10:
             adj(value);
             break; 
        case 11:
             a2sp();
             break;
        case 12:
             sp2a();
             break;
        case 13:
             call(value);
             break;
        case 14:
             Return();
             break;
        case 15:
             brz(value);
             break;
        case 16:
             brlz(value);
             break;
        case 17:
             br(value);
             break;
        case 18:
             HALT();
             break;
        default:
             cout<<"No Operation Match"<<endl;
             break;
    }
    return ;


}

void ShowRegisters() //Helper Function to Show contents of the Registers in Hecadecimal form along with the instruction//
{
    cout<<"A: "<<DectoHex(A)<<" "<<"B: "<<DectoHex(B)<<" "<<"SP: "<<DectoHex(SP)<<" "<<"PC: "<<DectoHex(PC+1)<<" "<<DectoHex(Memory[PC])<< endl;
}
void ShowMemory() //Helper function to implement Memory Dump//
{   int64_t f =Instructions.size();
    for(int64_t i=0;i<f;i+=4)
    {
          cout<<DectoHex(i)<<" ";          
          int64_t y  =min(i+3,f-1); 
          for(int64_t j =i;j<=y;j++)
          {
            if(j!=y)
            {cout<<DectoHex(Memory[j])<<" ";}
            else
            {
                cout<<DectoHex(Memory[j])<<endl;
            }
          }
    }
}
void ShowInstructionSet() //Helper funtion to Show ISA of the assembly language//
{
    cout<<"Mnemonic   |    Opcode     |    Operand    "<<endl;
    cout<<"data                             value     "<<endl;
    cout<<"ldc               0              value     "<<endl;
    cout<<"adc               1              value     "<<endl;
    cout<<"ldl               2              offset    "<<endl;
    cout<<"stl               3              offset    "<<endl;
    cout<<"ldnl              4              offset    "<<endl;
    cout<<"stnl              5              offset    "<<endl;
    cout<<"add               6                        "<<endl;
    cout<<"sub               7                        "<<endl;
    cout<<"shl               8                        "<<endl;
    cout<<"shr               9                        "<<endl;
    cout<<"adj              10              value     "<<endl;
    cout<<"a2sp             11                        "<<endl;
    cout<<"sp2a             12                        "<<endl;
    cout<<"call             13                        "<<endl;
    cout<<"return           14                        "<<endl;
    cout<<"brz              15              offset    "<<endl;
    cout<<"brlz             16              offset    "<<endl;
    cout<<"br               17              offset    "<<endl;
    cout<<"HALT             18                        "<<endl;
    cout<<"SET                              value     "<<endl;
}

void Execute_Object_Code(int64_t Op,int64_t times=(1<<25)) // helper function to simulate/process the object codes. "times" ->Represent how many lines we can run//
{      int64_t f =Instructions.size();
      bool done= false;
      int64_t cnt=0;
      int64_t  Max_Cnt =100000000;
      
    while((cnt<Max_Cnt)&&(times)&&(PC<f))
    {   times--;
        cnt++;
        
        
        if((PC>=f))
        {
            cout<<"Segmentation Fault: Memory Out of Bounds"<<endl;
            return;
        }
        int64_t g = Instructions[PC].Op_Code;
        int64_t  value = Instructions[PC].value;
                   
        if(times==0)
        {
            ShowRegisters();
            
            done =true;
        }
        
            
        
        Switch_Function(g,value);
        
        
        if(error)
        {
            error =false;
            cout<<"Errors Detected: Invaid Operations"<<endl;
            return;
        }
       
        if(Halt)
        {
            cout<<"Read HALT: Exiting from program"<<endl;
            break;
        }
      
        PC++;
        

    }
    
    if((times)&&(!Halt)) //Loop has been going on for long time
    {
      cout<<"Error: Indefinite Result"<<endl;
      return;
    }
    if(!done) //If Registers Haven't been shown in above loop, then show them now//
    {
        ShowRegisters();
    }
    
    return;
}
void Functions() //Helper function to simulate the functions offered by the emulator//
{
    
     
    string Op;
    cin >> Op;
    if(Op == "-E")
    {
        cout<<"Exiting From Program"<<endl;
        cout<<"Thank You!!"<<endl;
        exit(0);
    }
    else if(Op == "-D")
    {
        ShowMemory();
    }
    else if(Op == "-R")
    {
        ShowRegisters();
    }
    else if(Op == "-T")
    {
        Execute_Object_Code(0, 1);
        
    }
    else if(Op == "-Run")
    {
        Execute_Object_Code(0);
        
    }
    else if(Op == "-Isa")
    {
        ShowInstructionSet();
    }  
    else
    {
        cout << "Enter correct instruction" << endl;
    }
    cout << "Enter command or -E to exit:" << endl;
    return;
}
int main()
{
    Read_Object();
    A=B=SP=PC=0;  
    SP = Memory_Limit-1;    // Initialize the registers and Stack Pointer//  
    cout << "Welcome!!" << endl;
    cout << "Please Enter a command from the following menu:" << endl;
    cout <<" Function        Instruction Command"<<endl;
    cout << "1. Memory Dump  : -D" << endl;
    cout << "2. Trace the program line by line: -T" << endl;
    cout << "3. Run the complete code: -Run" << endl;
    cout << "4. Show values of the registers and Stack Pointer: -R" << endl;
    cout << "5. Show instruction set used : -Isa" << endl;   
    cout << "Enter command or -E to exit:" << endl;
   while(1)
   { 
     Functions();
   }
   
    
}
