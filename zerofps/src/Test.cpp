#include "Test.h"


Test olle("ollespelet");


Test::Test(char* aName): Application(aName) {


}


void Test::OnInit(void) {
	cout<<"Ollespelet e har =)"<<endl;
	cout<<"balle"<<endl;
}


void Test::OnIdle(void) {



}

