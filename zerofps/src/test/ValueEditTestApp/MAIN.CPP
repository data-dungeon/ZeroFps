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
	//if(!Nisse.SetValue("Storlek","1.909")) cout<<"fel vid tilldelning!";
	//cout<<Nisse.GetValue("Storlek")<<endl;

	//cout<<Nisse.GetNumberOfValues("TestVek") <<endl <<endl;
	//cout<<Nisse.GetValue("TestVek",1) <<endl;
	//if(!Nisse.SetValue("TestVek",1, "87")) cout<<"fel vid tilldelning!";						
	//cout<<Nisse.GetValue("TestVek",1) <<endl;
	cout<<"upperbound: " <<Nisse.GetUpperBound("Status") <<endl;
	if(Nisse.GetUpperBound("Status")==0) cout <<"ds";
return 0;
};

