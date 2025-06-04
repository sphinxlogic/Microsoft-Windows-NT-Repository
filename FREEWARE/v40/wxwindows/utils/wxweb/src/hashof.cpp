///////////////////////////////////////////////////////////////////////////////
//
// Description
//
//	The hash-table size must be a prime number, but an unlimited number
//	of entries can be added.
//
// Edit History
//
//	Written November 1993 by Andrew Davison.
//	Modified September 1994 by AD to allow insertion of duplicates.
//	Modified September 1994 by AD to allow key iteration.
//
///////////////////////////////////////////////////////////////////////////////

#include "hashof.h"

///////////////////////////////////////////////////////////////////////////////
class HashOfElement
{
public:

	HashOfElement(const string& _key, const T& _item) :
		key(_key), item(_item) {};

private:

	const string key;
	T item;

	friend class HashOf;
};

///////////////////////////////////////////////////////////////////////////////
HashOf::HashOf(unsigned _hash_prime)
{
	hash_prime = _hash_prime;
    items = 0;
#if HASHOF_DEBUG
	sets = tries = 0;
#endif
	table = new ListOf[(size_t)hash_prime];
}

///////////////////////////////////////////////////////////////////////////////
HashOf::~HashOf()
{
	Clear();
	delete [] table;
}

///////////////////////////////////////////////////////////////////////////////
void HashOf::Clear()
{
	for (unsigned i = 0; i < hash_prime; i++)
	{
		void* t;

		while (table[i].Pop(t))
		{
			HashOfElement* element = (HashOfElement*)t;
			delete element;
		}
	}

	items = 0;
}

///////////////////////////////////////////////////////////////////////////////
unsigned long HashOf::hash(const char* key)
{
	unsigned long h = 0;

	while (*key)
	{
		h = (h << 4) + (unsigned char)*key++;
		unsigned long g;

		if ((g = h & 0xF0000000L) != 0)
			h ^= g >> 24;

		h &= ~g;
	}

	return h;
}

///////////////////////////////////////////////////////////////////////////////
int HashOf::Get(const string& key, T& item) const
{
	if (!items)
		return 0;

	unsigned long val = hash(key.data());
	unsigned idx = (unsigned)(val % hash_prime);

	ListOfIterator ilist(table[idx]);
	void* t;

	for (int ok = ilist.First(t); ok; ok = ilist.Next(t))
	{
		HashOfElement* element = (HashOfElement*)t;

		if (element->key == key)
		{
			item = element->item;
			return 1;
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int HashOf::Get(const string& key) const
{
	T item;
	return Get(key, item);
}

///////////////////////////////////////////////////////////////////////////////
int HashOf::Drop(const string& key, T& item)
{
	if (!items)
		return 0;

	unsigned long val = hash(key.data());
	unsigned idx = (unsigned)(val % hash_prime);

	ListOfIterator ilist(table[idx]);
	void* t;

	for (int ok = ilist.First(t); ok; ok = ilist.Next(t))
	{
		HashOfElement* element = (HashOfElement*)t;

		if (element->key == key)
		{
			ilist.Delete();
			item = element->item;
			delete element;
			items--;
			return 1;
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int HashOf::Drop(const string& key)
{
	T item;
	return Drop(key, item);
}

///////////////////////////////////////////////////////////////////////////////
int HashOf::Set(const string& key, const T& item)
{
	unsigned long val = hash(key.data());
	unsigned idx = (unsigned)(val % hash_prime);

#if HASHOF_DEBUG
	sets++;
	tries++;
#endif

	ListOfIterator ilist(table[idx]);
	void* t;

	for (int ok = ilist.First(t); ok; ok = ilist.Next(t))
	{
		HashOfElement* element = (HashOfElement*)t;

		if (element->key == key)
		{
			element->item = item;
			return 0;
		}

#if HASHOF_DEBUG
		tries++;
#endif
	}

	HashOfElement* element = new HashOfElement(key, item);
	table[idx].Push(element);
	items++;

   return 1;
}

///////////////////////////////////////////////////////////////////////////////
#if HASHOF_DEBUG
void HashOf::Map(ostream& stream)
{
	stream << " |";

	for (unsigned long i = 0; i < hash_prime; i++)
	{
		if (table[i].IsEmpty())
			stream << '.';
		else
			stream << '+';

		if (!((i+1) % 70))
			stream << "|\n |";
	}

	while (i++ % 70)
		stream << ' ';

	stream << "|\n";

	stream << "Items: " << items << "/" << hash_prime;
	stream << " = " << (double)items * 100 / hash_prime << "%";

	if (sets)
		stream << " Hit ratio: " << (double)tries / sets;

	stream << '\n';
}
#endif

///////////////////////////////////////////////////////////////////////////////

void HashOf::Copy(ListOf& list)
{
	for (unsigned long i = 0; i < hash_prime; i++)
	{
		ListOfIterator ilist = table[i];
		void* t;

		for (int ok = ilist.First(t); ok; ok = ilist.Next(t))
		{
			HashOfElement* element = (HashOfElement*)t;

			list.Append(element->item);
		}
	}
}

