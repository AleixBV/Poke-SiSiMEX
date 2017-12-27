#include "Item.h"
#include "Globals.h"
#include <algorithm>


Item::Item() : _id(NULL_ITEM_ID), _quantity(1)
{
}

Item::Item(int id) : _id(id), _quantity(1)
{
}

Item::Item(int id, int quantity) : _id(id), _quantity(quantity)
{
}


Item::~Item()
{
}

//void Item::Write(OutputMemoryStream &stream)
//{
//	stream.Write(_id);
//}
//
//void Item::Read(InputMemoryStream &stream)
//{
//	stream.Read(_id);
//}


ItemList::ItemList()
{
}


ItemList::~ItemList()
{
}

void ItemList::randomInitialization()
{
	// Create random items
	int num_items = 120 + rand() % (145 - 120);
	int repeated = 10 + rand() % (25 - 10);

	for (int i = 0; i < num_items; ++i) {
		int itemId = rand() % MAX_ITEMS;
		while (true) {
			if (find(itemId) == nullptr)
				break;
			else
				itemId = rand() % MAX_ITEMS;
		}

		if (i < repeated) {
			int num_repeated = 2 + rand() % 2;
			Item item(itemId, num_repeated);
			addItem(item);
		}
		else {
			Item item(itemId, 1);
			addItem(item);
		}
	}

	// Sort items
	_items.sort();
}

void ItemList::addItem(const Item &item)
{
	if (item.id() != NULL_ITEM_ID) {
		Item* existing_item = find(item.id());
		if (existing_item != nullptr) {
			for (int i = 0; i < item.quantity(); ++i) {
				existing_item->add();
			}
		}
		else
			_items.push_back(item);
	}
}

void ItemList::removeItem(int itemId)
{
	for (auto it = _items.begin(); it != _items.end(); ++it)
	{
		Item &item(*it);
		if (item.id() == itemId) {
			if (item.quantity() > 1) {
				item.subtract();
			}
			else
				_items.erase(it);
			break;
		}
	}
}

ItemList ItemList::getSpareItems() const
{
	ItemList spareItems;
	for (auto item : _items) {
		for (int i = 1; i < item.quantity(); i++) {
			spareItems.addItem(item);
		}
	}
	return spareItems;
}

std::list<int> ItemList::getWantedItems() const
{
	std::list<int> wantedItems;
	for (int i = 0; i < MAX_ITEMS; ++i) {
		if (!is_in_list(i)) {
			wantedItems.push_back(i);
		}
	}
	return wantedItems;
}

//void ItemList::Write(OutputMemoryStream &stream)
//{
//	int16_t itemCount = static_cast<int16_t>(_items.size());
//	stream.Write(itemCount);
//	for (auto item : _items) {
//		item.Write(stream);
//	}
//}
//
//void ItemList::Read(InputMemoryStream &stream)
//{
//	int16_t itemCount;
//	stream.Read(itemCount);
//	_items.resize(itemCount);
//	for (auto item : _items) {
//		item.Read(stream);
//	}
//}

Item* ItemList::find(int id)
{
	for (auto item : _items) {
		if (item.id() == id)
			return &item;
	}
	return nullptr;
}

bool ItemList::is_in_list(int id) const
{
	for (auto item : _items) {
		if (item.id() == id)
			return true;
	}
	return false;
}