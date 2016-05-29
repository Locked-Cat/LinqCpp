#include <iostream>
#include <vector>
#include "linq.h"

using namespace std;

int main()
{
	vector<int> numbers = { -1, 0 , 1, 2, 3, 4 };
	auto range = LINQ(from(number, numbers) where(number > 2) select(number * number));
	
	for (auto number : range)
	{
		cout << number << " ";
	}
	cout << endl;
	//Surely it will print 9 16.
}



