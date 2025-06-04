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
//
///////////////////////////////////////////////////////////////////////////////

#ifndef HASHOF_H
#define HASHOF_H

#define HASHOF_DEBUG 0

#if HASHOF_DEBUG
class ostream;
#endif

#ifndef LISTOF_H
#include "listof.h"
#endif

#ifndef ASTRING_H
#include "astring.h"
#endif

class HashOf
{
public:

	HashOf(unsigned hash_prime);
	~HashOf();

	void Clear();

	int Get(const string& key, T& item) const;
	int Get(const string& key) const;
	int Drop(const string& key, T& item);
	int Drop(const string& key);
	int Set(const string& key, const T& item);

	unsigned long Count() const { return items; };
	void Copy(ListOf& list);

#if HASHOF_DEBUG
	void Map(ostream& stream);
#endif

private:

	static unsigned long hash(const char* key);

	ListOf* table;
	unsigned long hash_prime;
	unsigned long items;
#if HASHOF_DEBUG
	unsigned long sets, tries;
#endif
};

#endif
