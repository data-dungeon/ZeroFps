#include <iostream>

using namespace std;

#include "../../zerofps/basic/basicmath.pkg"

void TestVector3(void)
{
	cout << "Test Vector3" << endl;

	Vector3 a(1,0,0);
	Vector3 b(0,1,0);
	Vector3 c(0,0,1);
	Vector3 d = a + b + c;
	
	d.print();

	cout << endl << endl;
}

void TestMatrix4(void)
{
	cout << endl << "Test Matrix4" << endl;

	Matrix4 a,b,c;
	a.Identity();
	b.Identity();

	c = a * b;
	c.AxisX = Vector3(0,1,2);
	c.Print();

	cout << endl << endl;
}

int main(int argc, char *argv[])
{
	TestVector3();
	TestMatrix4();

	return 0;
}