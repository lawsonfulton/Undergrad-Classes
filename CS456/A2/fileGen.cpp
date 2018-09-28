#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;


int main (int argc, char const *argv[]) {
  ofstream myfile;
  myfile.open ("This is a test.txt");

  int size = atoi(argv[1]);

  for (int i = 0; i < size; ++i)
  {
  	int val = rand();
  	myfile << val << endl;	
  }

  myfile.close();
  return 0;
}