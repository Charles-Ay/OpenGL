#include <iostream>

int DrawLine(int x, int y)
{
	std::cout 
		<< "Hey " 
		<< x 
		<< ", " 
		<< y 
		<< std::endl;

	x = x * 3;

	return x + y;
}

void DrawCircle(int x, int y, int z)
{
	std::cout
		<< "Hey "
		<< x
		<< ", "
		<< y
		<< std::endl;

	x = x * 3;

	return;
}


class cGameObject
{
public:
	cGameObject() {};
	int x, y, z;
};

void DoSomething(cGameObject theObject)
{
	std::cout << theObject.x << std::endl;
}
void DoSomething(cGameObject* pObject)
{
	cGameObject* pSally = new cGameObject();
	pSally->x = 2;

//	std::cout << pObject->x << std::endl;

	pObject->x += pSally->x;

	delete pSally;

	return;
}


int main(int argc, char* argv[])
{
	cGameObject Ted;
	DoSomething(Ted);

	cGameObject* pTed = new cGameObject();
	pTed->x = 5;
	for ( int x = 0; x != 100'000'000; x++)
	{
		DoSomething(pTed);
	}


	int a = 9;
	int b = 11;

	int* pA = new int();
	*pA = 10;

//	int toobig[1'000'000];
//	toobig[0] = a;

	int* toobig = new int[1'000'000];

	toobig[0] = a;


//	void* pFunction = DrawCircle;

//	int result = pFunction(a, b);

	int r = DrawLine(a, b);

	std::cout << r << std::endl;


	return 0;
}
