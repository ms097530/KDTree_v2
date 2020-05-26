#include "KDTree.h"
#include <string>
#include <iostream>

using namespace std;

int main()
{
	KDTree<string> myTree(3);
	myTree.insert({ 1.03, 2.5, 10.11 }, "Seattle");
	myTree.insert({ 1.5, 2.0, 7.9 }, "Detroit");
	myTree.insert({ 70.2, 2.1, 8.3 }, "St. Orleans");
	myTree.insert({ 1.01, 31, 23 }, "Noob York");
	myTree.insert({ 0.993, 33.2, 0.12 }, "Washington");
	myTree.insert({ 1, 20, 3 }, "Dallas");
	myTree.insert({ 1.5, 3.14, 5 }, "Paris");
	
	myTree.print();
	cout << endl;
	
	myTree.regionQuery(50.00, { 1.5, 2.0, 7.9 });
	cout << endl;

	
	
	myTree.remove("Detroit");
	myTree.print();
	cout << endl;
	myTree.regionQuery(50.00, { 1.5, 2.0, 7.9 });

	
	Node<string>* searchTest = myTree.search({1.5, 2.0, 7.9});	//works
	if (searchTest != nullptr)
	{
		cout << searchTest->getVal() << endl;
		for (auto elem : searchTest->getDiscrims())
			cout << elem << " ";
		cout << endl;
	}
	searchTest = myTree.search("St. Orleans");			//works

	if (searchTest != nullptr)
	{
		cout << searchTest->getVal() << endl;
		for (auto elem : searchTest->getDiscrims())
			cout << elem << " ";
		cout << endl;
	}
	return 0;
}