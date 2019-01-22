#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>

using namespace std;

// *You* are not allowed to use global variables
//  but for just the output portion *we* will. ;>
int combo = 1;

// @brief Prints a single combination of teams
//
// @param[in] team1 Array containing the names of team 1
// @param[in] team2 Array containing the names of team 2
// @param[in] len Size of each array
void printSolution(const string* team1, 
		   const string* team2,
		   int len)
{
  cout << "\nCombination " << combo++ << endl;
  cout << "T1: ";
  for(int i=0; i < len; i++){
    cout << team1[i] << " ";
  }
  cout << endl;
  cout << "T2: ";
  for(int i=0; i < len; i++){
    cout << team2[i] << " ";
  }
  cout << endl;
}

// You may add additional functions here
void gen(string* name,int x,string y){
    int num0 = 0;
    int num1 = 0;
    for(int i = 0; i< y.size();i++){
        if(y[i]=='1'){
            num1++;
        }
        else{
            num0++;
        }
    }
    if(num0 == x/2&&num1 == x/2){
      int count0 = 0;
      int count1 = 0;
      string* team0 = new string[x/2];
      string* team1 = new string[x/2];
      for(int i = 0; i<y.size();i++){
        if(y[i]=='0'){
          team0[count0]= name[i];
          count0++;
        }
        else{
          team1[count1]= name[i];
          count1++;
        }
      }
      printSolution(team0,team1,x/2);
      delete[] team1;
      delete[] team0;
      return;
    }
    else{
        if(num0==x/2){
            gen(name,x,y+"1");
        }
        else if(num1 == x/2){
            gen(name,x,y+"0");
        }
        else{
            gen(name,x,y+"1");
            gen(name,x,y+"0");
        }
    }
}




int main(int argc, char* argv[])
{
  if(argc < 2){
    cerr << "Please provide a file of names" << endl;
    return 1;
  }
  // Complete the rest of main
  ifstream ifile(argv[1]);
  int num;
  if(ifile.fail()){
    cout<<"Error"<<endl;
    return 1;
  }
  ifile>>num;
  if(ifile.fail()){
    cout<<"Error"<<endl;
    return 1;
  }
  string word;
  int count = 0;
  while(ifile>>word){
    count++;
  }
  if (count!=num){
    cout<<"Error"<<endl;
    return 1;
  }
  ifile.clear();
  ifile.seekg(0,ifile.beg);
  ifile>>num;
  string* name = new string[count];
  for(int i = 0; i<count; i++){
    ifile>>word;
    name[i]=word;
  }
  string y;
  gen(name, count, y);
  delete[] name;
  return 0;
}
