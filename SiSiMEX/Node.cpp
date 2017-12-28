#include "Node.h"


Node::Node(int id) : _id(id)
{
}


Node::~Node()
{
}


void Node::initialize()
{
	// Initialize items
	_itemList.randomInitialization();
}
