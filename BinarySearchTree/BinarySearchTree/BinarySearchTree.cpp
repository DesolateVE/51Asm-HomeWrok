#include "BTree.h"
#include <vector>

int main()
{
	BTree<int, string> tree;
	tree.Insert(2, "666");
	tree.Insert(1, "696");
	tree.Insert(5, "626");
	tree.Insert(4, "6h6");
	tree.Insert(6, "asdasdad");
	tree.Insert(3, "634");
	tree.Pre();
	tree.Delete(4);
	cout << endl;
	tree.Pre();
}