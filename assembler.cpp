/**Authorship Claim**/
/**Name: Sai Vardhan Vemulapalli**/
/**Roll No.: 2001CS58**/
/**Declaration of Authorship: I claim that the following code has been authored by me solely**/
#include<bits/stdc++.h>
#include <fstream>
using namespace std;

/************** Variables,Data types and Structures used********************/
vector< vector <string > >Instructions; // To stors the instruction lines read from file in form of vector of strings//
vector<  string  > Errors; // To store error messages in form of strings//

map<string,bool>LabelNotYetFound; // To keep track of Labels which still haven't been found in symbol table but are used as offset/value during First Pass//
map<string,string>LabeltoInstruction; //  A Hashmap to store the corresponding instructions where Label was found earlier than its initialisation, to print error messages if neccessary//
map<string,int64_t >Symbol_Table; // To Store the values of Labels found in code//
map<string,string>Op_Table; // Maps each valid Operation to its Op Code in hexadecimal format, can be used to check if an Operation is valid or not also//
map<string,int>Len_Op;//Maps each valid Operation to the required number of operands, to detect errors//
int64_t PC;// Program Counter//
string FileName;//Input File Name//


struct Listing_Data // A data structure to store the Listing list format
{
   string Addr;
   string ObjCode;
   string Instruct;
   bool Objpres ;   // boolean to show if ObjCode exists or not//
};
vector< Listing_Data > List; // The vector to hold the Listinf file input, later will inputed to file.//

/***************************************************************************/
/********************Helper Functions*************************************/
void Form_Op_Table() // Helper Function to populate Op_Table and Len_Op//
{
        
    Op_Table["ldc"] = "00";   
	Op_Table["adc"] = "01";
	Op_Table["ldl"] = "02";
	Op_Table["stl"] = "03";
	Op_Table["ldnl"] = "04";
	Op_Table["stnl"] = "05";
	Op_Table["add"] = "06";
	Op_Table["sub"] = "07";
	Op_Table["shl"] = "08";
	Op_Table["shr"] = "09";
	Op_Table["adj"] = "0a";
	Op_Table["a2sp"] = "0b";
	Op_Table["sp2a"] = "0c";
	Op_Table["call"] = "0d";
	Op_Table["return"] = "0e";
	Op_Table["brz"] = "0f";
	Op_Table["brlz"] = "10";
	Op_Table["br"] = "11";
	Op_Table["HALT"] = "12";
    Op_Table["data"] = "";
    Op_Table["SET"] ="";

	

    Len_Op["data"] = 1;    
    Len_Op["ldc"] = 1;   
	Len_Op["adc"] = 1;
	Len_Op["ldl"] = 1;
	Len_Op["stl"] = 1;
	Len_Op["ldnl"] = 1;
	Len_Op["stnl"] = 1;
	Len_Op["add"] = 0;
	Len_Op["sub"] = 0;
	Len_Op["shl"] = 0;
	Len_Op["shr"] = 0;
	Len_Op["adj"] = 1;
	Len_Op["a2sp"] = 0;
	Len_Op["sp2a"] = 0;
	Len_Op["call"] = 1;
	Len_Op["return"] = 0;
	Len_Op["brz"] = 1;
	Len_Op["brlz"] = 1;
	Len_Op["br"] = 1;
	Len_Op["HALT"] = 0;	
	Len_Op["SET"] = 1;

}
bool is_Branch(string g) // Helper function to chech if given operation involves branching//
{
    if((g=="br")||(g=="brz")||(g=="brlz")||(g=="call"))
    {
        return true;
    }
    return false;
}
char Hex(int y) // Return Hexadecimal digit for decimal remainder with 16//
{
    if(y<10)
    {
        return (char)((y)+'0');
    }
    return (char)('a'+(y-10));
}
string DecToHex(string a) // Helper Function to convert decimal integer to hexadecimal form//
{
    
     string h(8,'0'); 
     int64_t f =0;    
     for(auto k :a)
     {  if((k!='-')&&(k!='+'))
     {int y =(k-'0');
        f =(f*10)+y;
     }
     }
     if(a[0]=='-')
     {
        f = -f;
     }
     if(f<0)
     {
        for(int i=0;i<7;i++)
        {
            h[i]='f';
        }
        int64_t g = (1ULL<<32);;
        
        
        f = f+g;
     }
    
    
   
    int cnt =7;
    
    while(f)
    {
        char d =Hex(f%16);
        f /=16;
        h[cnt]=d;
        cnt--;
    }
    return h;
     
}
string OctToHex(string a)
{
    int64_t  F=0;
    int64_t n =a.size();
    for(int64_t i=1;i<n;i++)
    {
        int y = a[i]-'0';
        F  = (F*8)+y;
    }
    string h =DecToHex(to_string(F));
    return h;
}
string Process_Hex(string a) // Helper function to sign extend hexadecimal operands to 32 bits//
{
     string h(8,'0');
     int64_t d = a.size();     
     int cnt=7;
     for(int i=d-1;i>=2;i--)
     {
            h[cnt] = a[i];
            cnt--;
     }
     return h;
}
string Offset(int64_t  a,int64_t  b) // Helper function to find hexadecimal value of offset between a and b//
{    
    string h(8,'0'); 
     int64_t f =a-b;    
    
     if(f<0)
     {
        for(int i=0;i<7;i++)
        {
            h[i]='f';
        }
        int64_t g = (1<<24);
        f = f+g;
     }
    
    
   
    int cnt =7;
    
    while(f)
    {
        char d =Hex(f%16);
        f /=16;
        h[cnt]=d;
        cnt--;
    }
    return h;
}
string Encoding(string Opcode ,string Operand) // Given Operand value and Op Code, Helper function to encode the object code//
{   
    if(Opcode=="data")
    {   
        return Operand;
    }
    string h (8,'0'); // Last two bits are Op Code
    h[7] = Op_Table[Opcode][1];
    h[6] = Op_Table[Opcode][0];
    for(int i =5;i>=0;i--) // First 6 bits are that of Operand, in hexadecimal form//
    {
        h[i] =Operand[i+2];
    }
    return h;

}
int64_t  is_Num(string h) //Helper function to check if the input string represents a number or not, if yes, in which form :decimal, octal, hexadecimal//
{   int64_t n =h.size();
    bool there =true;
    if(n==1)
    {
        return (isdigit(h[0]));
    }
    if((h[0]=='+')||(h[0]=='-')) //Check if its decimal or not,as only that can start with + or -//
    {    
        for(int64_t i=1;i<n;i++)
        {
            if(!isdigit(h[i]))
            {
                return 0;
            }
        }
        return 1;
    }

    if(h[0]!='0')
    {   
        
        for(auto k :h)
        {
            if(!isdigit(k))
            {          return 0;
            }
        }
        return 1;
        
    }
    if(h[0]=='0')
    {
        if((h[1]=='x'))
        {
            if(n==2) //"Rules out the string "0x"//
            {
                return 0;
            }
                   for(int64_t i=2;i<n;i++)
            {
                if((!isdigit(h[i]))&&(h[i]<'a')&&(h[i]>'f'))
                {
                    return 0;
                }


            }
            return 3;
        }
        else
        {
            for(int64_t i=1;i<n;i++)
            {
                if((h[i]<'0')&&(h[i]>'7'))
                {
                    return 0;
                }


            }
            return 2;


        }
    }
    return 0;  
    
    
}
bool is_Symbol(string h) // Check if the string is recorded in the Symbol Table or not//
{
    return  (Symbol_Table.find(h)!=Symbol_Table.end());
}


bool is_Label(string h) //Check if its a valid label definition//
{
    return(h.back()==':');
    
}
bool valid_Label(string h) //Check if its a valid Label name i.e alphanumeric starting with letter//
{   
    if(isdigit(h[0]))
    {
        return false;
    }
    for(auto k :h)
    {
        if((!isdigit(k))&&(!isalpha(k)))
        {
            return false;
        }
    }
    return true;
}

void check_Mnemonic(string m,vector< string>S,string l) //Check if the Mnemonic used has the right amoun of operands and is syntactically correct and log neccessary error messages//
{   
    if(Op_Table.find(m)==Op_Table.end()) //If its not a valid Mnemonic//
    {
        string error = "Invalid Mnemonic Name Error in Instruction: \"";
        error +=l;
        error +="\"";
        Errors.push_back(error);
        return;
    }
   if(Len_Op[m]==0)
   {
    if(S.back()!=m)
    {
        string error = "Excess Operand Error in Instruction: \"";
        error +=l;
        error +="\"";
        Errors.push_back(error);
    }
    
   }
   if(Len_Op[m]==1)
   {
     if((S.back()==m))
     {
        string error = "Missing Operand Error in Instruction: \"";
        
        error +=l;
        error +="\"";
        Errors.push_back(error);
        return;
     }
     string hq = S.back();
     S.pop_back();
      if(S.back()!=m)
     {
        string error = "Excess Operand Error in Instruction: \"";
        error +=l;
        error +="\"";
        Errors.push_back(error);
        return;
     }
     int64_t u = is_Num(hq);
     if((u==0)&&(!valid_Label(hq))) // If its not a proper label or a value
     {
        string error = "Invalid Operand Error in Instruction: \"";
        error +=l;
        error +="\"";
        Errors.push_back(error);
        return;
     }
     
   }
   return;
}
int64_t StrtoDec(string x) // Convert the operand string to decimal value, irrespective of what form it is in, i.e decimal , hexadecimal,octal //
{
    int64_t u = is_Num(x);
    int64_t ans=0;
    if(u==1)
    {
        for(auto k :x)
        {
            int y =k-'0';
            ans = (ans*10)+y;
        }
    }
    else if(u==2)
    {
        for(int i =1;i<x.size();i++)
        {
            int y = x[i]-'0';
            ans = (ans*8)+y;
        }
        
    }
    else if(u==3)
    {
        for(int i=2;i<x.size();i++)
        {
            if((x[i]=='a'))
            {
                  ans = (ans*16)+10;
            }
           else if((x[i]=='b'))
            {
                  ans = (ans*16)+11;
            }
            else if((x[i]=='c'))
            {
                  ans = (ans*16)+12;
            }
            else if((x[i]=='d'))
            {
                  ans = (ans*16)+13;
            }
           else  if((x[i]=='e'))
            {
                  ans = (ans*16)+14;
            }
            else if((x[i]=='f'))
            {
                  ans = (ans*16)+15;
            }
            else
            {
                int y= x[i]-'0';
                ans = (ans*16)+y;
            }
        }
        
    }
    else
    {
        return 0;
    }
    return ans;
    
}
void ErrorCheck_Pass(vector< string > k,string f) // Helper Function to pass each line of instruction, process them i.e update symbol table and check for Errors simultaneously//
{    PC= PC+1;
     string h = k[0];        
     string l = f;
                 
                  
                
        if(is_Label(h))
        {   
            h.pop_back();
            if(LabelNotYetFound[h])
            {
                LabelNotYetFound[h]=false;
                LabeltoInstruction.erase(h);
            }
            if(!valid_Label(h)) //Check for valid Bogus Label Names
             {
                  string error = "Invalid Label Name in Instruction: \"";                  
                  error += l;
                  error +="\"";
                  Errors.push_back(error);
                  return;
                 
             }
        
            
            if((k.size()>1))
            {   
                
                if(Symbol_Table.find(h)==Symbol_Table.end())
               {
                  if(k[1]!="SET") //For Non SET operation store value of PC in symbol table
                  {
                    Symbol_Table[h] = PC;
                  }
                  else if((k[1]=="SET")&&(k.size()==3)) //For SET operation , store value specified in symbol table//
                  {
                    Symbol_Table[h]= StrtoDec(k[2]);
                  }
                  
                  
               } 
               else
               {
                    string error = "Duplicate Label Error in Instruction: \"";
                    error += l;
                    error += "\"";
                    Errors.push_back(error);
                    return;
               }
               check_Mnemonic(k[1],k,l); 
               if((k.size()>=3)&&(valid_Label(k[2]))) //Checks if there is a label that has not been yet recorded in symbol table, but has been called//
               {
                  if(Symbol_Table.find(k[2])==Symbol_Table.end())
                 {
                   LabelNotYetFound[k[2]]  = true;
                   LabeltoInstruction[k[2]] = l;
                 
                 } 
               }
             
           }
           else
           {
                if(Symbol_Table.find(h)==Symbol_Table.end())
               {
                  Symbol_Table[h] = PC;
                  PC = PC-1;

               } 
                else
               {
                    string error = "Duplicate Label Error in Instruction: \"";
                    error += l;
                    error += "\"";
                    Errors.push_back(error);
                    return;
               }

           }
        }
        else
        {
            check_Mnemonic(k[0],k,l);
            if(k[0]=="SET")
            {
                string error = "Missing Label Error in Instruction: \"";
                error += l;
                error +="\"";
                Errors.push_back(error);
            }
            if((k.size()>=2)&&((valid_Label(k[1]))))
            {
                if(Symbol_Table.find(k[1])==Symbol_Table.end())
               {
                 LabelNotYetFound[k[1]]  = true;
                 LabeltoInstruction[k[1]] = l;
                 
               } 
            }
        }

}
void Preprocessing(string f) //Process each line of input file, removes comments, tabs, and divides into label, mnemonic, value//
{   
    
   stringstream iss(f); // Input them into string stream to get words in a line//
   string word;
   vector<string>Line;   
   string Ins ="";
   bool comment_pres =false;
   while((!comment_pres)&&(iss>>word))
   {    
        if(word[0]!=';')
        {
            if(word.back()==':')
            {
              Line.push_back(word);
              Ins += word;
            }
            else
            {
                string g ="";
                for(auto k :word)
                {     g+=k;
                    if(k==':')
                    {
                        Line.push_back(g);
                       if(Ins.size())
                       { Ins += " ";}
                        Ins +=g;
                        g ="";
                    }
                    else if(k==';')
                    {
                        g.pop_back();
                        comment_pres =true;
                        break;
                    }
                }
                
                
                 if(g.size())
                 {
                       Line.push_back(g);
                       if(Ins.size())
                       { Ins += " ";}
                        Ins +=g;
                 }
                
            }
      }
    else
    {
        break;
    }
   }   
    
   if(Line.size())
   {     
          
        ErrorCheck_Pass(Line,Ins);
        Instructions.push_back(Line);
       
   }   
   return;
    
}
void First_Pass() //First Pass routine -> populates symbol table and detects for error//
{   PC = -1;
    ifstream infile;
	cout << "Enter ASM file name to assemble:" << endl;
    cin >> FileName;
    
    infile.open(FileName);
    
    int cnt=0;
    while(infile)
    {   
        cnt++;
        string h;
        getline(infile,h);
        
        Preprocessing(h);      
        
    }
   
   infile.close();
  
   
   
    
    for(auto k:LabeltoInstruction)
    {
        string error = "Label Not Found Error in Instruction: \"";
        error += k.second;
        error +="\"";
        Errors.push_back(error);
    }
   
   return;
}

void Second_Pass() //Second Pass routine -> Encodes the object code for each line of instruction//
{
   PC=-1;
   bool prevflag =false;
   for(auto k :Instructions)
   {    
       Listing_Data L;
       L.Objpres =true;
              
       if(!prevflag) 
       {
           PC = PC+1;
           
       }
       else
       {
          prevflag = false;
       }
       string h (8,'0');
       L.Addr = DecToHex(to_string(PC));
       
       int64_t  e = k.size();
       
        if(e==1)
        {
            L.Instruct += k[0];            
        }
        else if(e==2)
        {
            L.Instruct += (k[0]+" "+k[1]);
        }
        else
        {
                L.Instruct +=(k[0]+" "+k[1] + " "+k[2]);
        }
         
        if(is_Label(k[0]))
        {    
           if(e==1)
           { 
                prevflag = true;
                L.Objpres =false;            
           }           
           else if(e==3)
           {
             
                int64_t A =PC;
                int64_t u = is_Num(k[2]);
               
               
                
                if(u==1)
                {
                     h =   DecToHex(k[2]);
                }
                else if(u==2)
                {
                     h =   OctToHex(k[2]);
                }
                else if(u==3)
                {
                     h =   Process_Hex(k[2]);
                }
                else
                {
                    if(is_Symbol(k[2]))  
                    {
                       if(is_Branch(k[1]))
                       {
                         h = Offset(Symbol_Table[k[2]],PC+1); 
                       }  
                       else
                       {
                         h = DecToHex(to_string(Symbol_Table[k[2]]));
                       }
                    }
                    
                }
           }


           if((e>1)&&(k[1]=="SET"))
           {
                    PC =PC-1;
                    L.Objpres = false;
           }
           if(L.Objpres)
           { 
              L.ObjCode = Encoding(k[1],h);
           }          
                     
        }
        else        
        {  
                     
           if(e!=1)
           {    
             
           
                int64_t u = is_Num(k[1]);           
                if(u==1)
                {
                        h =   DecToHex(k[1]);
                }
                else if(u==2)
                {
                        h =   OctToHex(k[1]);
                }
                else if(u==3)
                {
                        h =   Process_Hex(k[1]);
                }
                else
                {   
                        if(is_Branch(k[0]))
                        {
                            h = Offset(Symbol_Table[k[1]],PC+1); 
                        }  
                        else
                        {  
                            h = DecToHex(to_string(Symbol_Table[k[1]]));
                        }
                }  
           }   
          
            L.ObjCode = Encoding(k[0],h);
        }
        
       
        if(!L.Objpres)       
        {
            string o(8,' ' );
            L.ObjCode = o;
        }
        List.push_back(L); 
        
   }
}
//Following two functione are helper functions to create ouput files//
void Output_File() 
{
    ofstream outfile("ListFile.lst");	
    for(auto k : List)
    {
        outfile<< k.Addr<<" "<<k.ObjCode<<" "<<k.Instruct<<endl;
    }
    outfile.close();
    ofstream ObjFile;
    ObjFile.open("MachineFile.o",ios::binary | ios::out);  //Binary form//
	for(auto k: List)
    { 
		unsigned int x;		 // Input in binary form into the file//
    	stringstream ss;
    	ss << hex << k.ObjCode; //Input the hexadecimal string into the stream to get it back in binary form//
    	ss >> x;            // output it as a signed type
     	static_cast<int>(x);
        if( k.ObjCode == "        ") //No need to enter into object file//
	    {
            continue;
        }
 		ObjFile.write((const char*)&x, sizeof(unsigned int));
	}
	ObjFile.close();
}

void Output_Error_File()
{
    ofstream outfile("ErrorFile.txt");	
    for(auto k : Errors)
    {
        outfile<< k<<endl;
    }
    outfile.close();
    
}


int main()
{
  Form_Op_Table();  
  First_Pass();
  cout<<"First Pass Done"<<endl;
  if(Errors.size())
  {   
      Output_Error_File();
      ofstream outfile("LogFile.log");
      outfile<<"Errors Detected."<<endl;
      outfile<<"Errors Logged into \"ErrorFile.txt\""<<endl;
      outfile.close();
      cout<<"Errors Detected"<<endl;
  }
  else 
  {   
      Second_Pass();
      Output_File();
      ofstream outfile("LogFile.log");
      outfile<<"Program Assembled Successfully"<<endl;
      outfile<<"Listing File generated in \"ListFile.lst\""<<endl;
      outfile<<"Object code generated in \"MachineFile.o\""<<endl;
      outfile.close();
      cout<<"Second Pass Done"<<endl;
  }
  

	
   
}

