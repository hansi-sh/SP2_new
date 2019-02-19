#ifndef ITEM_H
#define ITEM_H
#include<string>
class Item
{
public:
	Item();
	Item(int newitem);//std::string newitem);
	~Item();
	//std::string  data;
	int data;
	Item* next;
	Item* prev;
	/*void Load(char dir, Item* cnode);
	Item* getLink(char dir);*/
private:
	//Item * left;
	//Item * right;
};
#endif

