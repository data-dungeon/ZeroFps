#include <iostream>
#include <string>
#include "Korv.h"
#include <stdlib.h>
#include <vector>

using namespace std;
int main( int argc, char* argv[] )
{
Korv Nisse;

	bool slut = false;
	vector<string> hej = Nisse.GetValueNames();
	
	while(!slut)
	{
		int j = 0;
		vector<string>::iterator i = hej.begin();
		while (i != hej.end())
		{
			
			cout<< j++ <<". "<< *i <<endl;
			i++;
		
		}
		cout << endl 
			 << hej.size() <<". Quit" <<endl
			 << ":>"; 
		int select;
		cin>>select;
		if(select==hej.size()) 
			slut=true;
		else 
		{
			cout << endl 
				 << hej[select] 
				 <<"  Value: " << Nisse.GetValue(hej[select])
				 <<"  UpperBound: " << Nisse.GetUpperBound(hej[select])
				 <<"  LowerBound: " << Nisse.GetLowerBound(hej[select])
				 << endl
				 << "1. Change Value" <<endl
				 << "2. Quit"<<endl
				 << ":>"; 
			int select2=0;
			cin>>select2;
				cout <<endl;
				if(select2==2) 
					slut=true;
				else if(select2==1) 
				{
					string str;
					cout<<"New Value: ";
					cin >> str;
					Nisse.SetValue(hej[select], str);
					cout<< endl <<Nisse.GetValue(hej[select]);
				}
		}

	};

	//cout<<"upperbound: " <<Nisse.GetUpperBound("Status") <<endl;
	//if(Nisse.GetUpperBound("Status")==0) cout <<"ds";
return 0;
};

