#include <iostream>
#include <string>
#include "Korv.h"
#include <stdlib.h>
#include <vector>
#include "Vector3.h"

using namespace std;
int main( int argc, char* argv[] )
{
	Korv Nisse;
	
	

	
	vector<string> hej = Nisse.GetValueNames();
	cout<<hej[0] <<endl;
	cout<<Nisse.GetValue(hej[0]) <<endl;
	Nisse.SetValue(hej[0], "ap212123123123123123");
	cout<<Nisse.GetValue(hej[0]) <<endl;
	
	
	return 0;

};

