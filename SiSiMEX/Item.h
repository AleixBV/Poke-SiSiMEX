#pragma once
#include <Net.h>

/**
 * A single item and its properties.
 */
class Item
{
public:

	// Constructors and destructor
	Item();
	Item(int id);
	Item(int id, int quantity);
	~Item();

	// Item identifier
	int id() const { return _id; }
	int quantity() const { return _quantity; }

	//// Serialization methods
	//void Write(OutputMemoryStream &stream);
	//void Read(InputMemoryStream &stream);

	// Comparison operator
	bool operator<(const Item &item) { return _id < item._id; }

	// add / subtract quantity
	void add() { _quantity++; }
	void subtract() { _quantity--; }

private:
	int _id; /**< Item identifier. */
	int _quantity;
};

/**
 * A list of items.
 */
class ItemList
{
public:

	// Constructor and destructor
	ItemList();
	~ItemList();

	// It initializes the item list randomly
	void randomInitialization();

	// Methods to add and remove items to/from the list
	void addItem(const Item &item);
	void removeItem(int itemId);

	// It returns the inner std::list of items
	const std::list<Item> &items() const { return _items; }

	// It returns a new list with all the repeated items
	ItemList getSpareItems() const;
	std::list<int> getWantedItems() const;

	bool is_in_list(int id) const; //find item by id

	//// Serialization methods
	//void Write(OutputMemoryStream &stream);
	//void Read(InputMemoryStream &stream);

private:

	std::list<Item> _items; /**< A list of items. */
	Item* find(int id); //find item by id
};
