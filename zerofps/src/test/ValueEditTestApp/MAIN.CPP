#include <iostream>
#include <string>
#include "Korv.h"
#include <stdlib.h>
#include <vector>

using namespace std;
int main( int argc, char* argv[] )
{
Korv Nisse;


	vector<string> hej = Nisse.GetValueNames();
	vector<string>::iterator i = hej.begin();
		while (i != hej.end())
		{
			cout<< *i <<endl;
			i++;
		
		}
	cout<< endl;
	if(!Nisse.SetValue("Storlek","1.909")) cout<<"fel vid tilldelning!";
	cout<<Nisse.GetValue("Storlek")<<endl;


					
return 0;
};

