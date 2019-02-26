#ifndef ITEM_H
#define ITEM_H
#include<string>
class Item
{
public:
	Item();
	Item(int newitem);
	~Item();
	int data;
	Item* next;
	Item* prev;
};
#endif

