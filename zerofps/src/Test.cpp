#include "Test.h"


Test olle("MegaGame",640,480,16);


Test::Test(char* aName,int iWidth,int iHeight,int iDepth): Application(aName,iWidth,iHeight,iDepth) {


}


void Test::OnInit(void) {
	cout<<"HEllo =)"<<endl;
	
}


void Test::OnIdle(void) {



}

