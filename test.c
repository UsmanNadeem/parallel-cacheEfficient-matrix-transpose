#include <iostream>
#include <stdint.h>
#include <vector>
using namespace std;
class test
{
public:
	test( ){ 
		cout <<" ctor\n";
	}
	test( int a ){
		cout << a<<" ctor\n";
	}
	
};
int main(int argc, char const *argv[])
{
	// test* arr = new test[5];
	// vector<test*> setArray(5, new test(4));
	uint64_t a = 0xf;
	a = a <<4;
	a = a|0xf;
	a=0xf;
	uint64_t b = 0x00000f;
	cout << (void*)a<<endl;
	cout << a<<endl;
	cout << (void*)b<<endl;
	cout << b<<endl;

	return 0;
}