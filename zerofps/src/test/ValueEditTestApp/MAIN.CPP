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
		int select=0;
		vector<string>::iterator i = hej.begin();
		while (i != hej.end())
		{
			
			cout<< j++ <<". "<< *i <<endl;
			i++;
			
		}
		cout << endl 
			<< hej.size() <<". Quit" <<endl
			<< ":>"; 
		
		cin>>select;
		if(select==hej.size()) 
			slut=true;
		else 
		{
			int select2=0;
			cout << endl 
				<< hej[select] 
				<<"  UpperBound: " << Nisse.GetUpperBound(hej[select])
				<<"  LowerBound: " << Nisse.GetLowerBound(hej[select]) <<endl
				<<"Value(s) "; 
			
			
			if(!Nisse.CheckIfVector(hej[select]))
			{
				cout << " : " <<Nisse.GetValue(hej[select]);
				cout << endl
					<< "1. Change Value" <<endl
					<< ":>"; 
				cin>>select2;
				cout <<endl;
				if(select2==1) 
				{
					string str;
					cout<<"New Value: ";
					cin >> str;
					Nisse.SetValue(hej[select], str);
					cout<< "Value Set to:" <<Nisse.GetValue(hej[select]) <<endl <<endl; 
				}
			}	
			else 
			{
				int temp=Nisse.GetNumberOfValues(hej[select]);
				for(int ind=0; ind<temp; ind++)
					cout<< endl << ind <<". " <<Nisse.GetValue(hej[select],ind);
				
				
				if (Nisse.CheckIfResize(hej[select])) 
						cout<<endl << temp <<" . ReSize!";
				
				cout<< endl 
					<<"select value to change" <<endl
					<< ":>"; 
				cin>>select2;
				cout <<endl;
				
				
				if(select2<temp) 
				{
					string str;
					cout<<"New Value: ";
					cin >> str;
					Nisse.SetValue(hej[select], select2, str);
					cout<< "Value Set to:" <<Nisse.GetValue(hej[select], select2) <<endl <<endl; 
				} 
				else if ((Nisse.CheckIfResize(hej[select])&&(select2==temp)))
				{
					int newsize;
					cout <<"new size: " ;
					cin >>newsize;
					Nisse.Resize(hej[select], newsize);
				}
			}
			
			/*else
			{
				for(int ind=0; ind<temp; ind++)
					cout<< endl << ind <<". " <<Nisse.GetValue(hej[select],ind);
					cin>>select2;
				cout <<endl;
				if(select2<Nisse.GetNumberOfValues(hej[select])) 
				{
					string str;
					cout<<"New Value: ";
					cin >> str;
					Nisse.SetValue(hej[select], select2, str);
					cout<< "Value Set to:" <<Nisse.GetValue(hej[select], select2) <<endl <<endl
				};
			
			}*/
				
		}
		
	};
	
	//cout<<"upperbound: " <<Nisse.GetUpperBound("Status") <<endl;
	//if(Nisse.GetUpperBound("Status")==0) cout <<"ds";
	return 0;
};

