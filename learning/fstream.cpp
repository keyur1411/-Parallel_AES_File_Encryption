#include<fstream>
#include<iostream>

using namespace std;

int main(){

  // cout<<"What did you eat ?"<<endl;

  // ofstream file("food.txt"); //->create output stream for file which  output stuff in food.txt 
  // for(int i=0;i<5;i++){
  //   string food;
  //   cin>>food;
  //   file<<food<<endl;//->file is given variable name to the file name "food.txt" we use it as that it print that string in "food.txt"
  // }
  
  // ifstream file("food.txt");//->create input stream for the file which takes input from file
  // string food;
  // while(file>>food){//file>>food way take input from "food.txt"
  //   cout<<food<<endl;
  // }
  // file.close();//It closes the file


  ofstream out("food.txt",ios::out);
  cout<<"Write Some of Your Favourite Foods ?"<<endl;
  for(int i=0;i<5;i++){
    string s;
    cin>>s;
    out<<s<<endl;
  }
  out.close();
  return 0;
}