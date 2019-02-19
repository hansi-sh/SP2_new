#include "Item.h"
#include "Application.h"


Item::Item()
{
	data = -1;
	next = NULL;
	prev = NULL;
}
Item::Item(int newitem)
{
	data = newitem;
	next = NULL;
	prev = NULL;
}
Item::~Item()
{
}
//change filter

