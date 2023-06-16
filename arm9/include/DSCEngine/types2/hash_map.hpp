#pragma once

#include "DSCEngine/types/templates.hpp"
#include "DSCEngine/types/vector.hpp"
#include "DSCEngine/debug/assert.hpp"
#include "DSCEngine/types/hash.hpp"

namespace DSC
{
	/*!
	* \brief Generic hash map
	* \tparam K type of keys
	* \tparam V type of values
	* \tparam H hash function for keys
	* \tparam S hash container size (H(k) maps k to 0..S-1)
	 */
	template<typename K, typename V, int (*H)(const K&) = default_hash<K, 128>, int S = 128> class HashMap
	{
	public:
		struct Entry { K key; V value;};
	private:
		Vector<Vector<Entry>> container = Vector<Vector<Entry>>(S);
		int _size = 0;
	public:
		/*! \brief checks if a key exists in the hash map
			\param key key to check
			\return true if key exists, false otherwise
		 */
		bool contains_key(const K& key) const;
		
		/*! \brief get left-value reference to the value held by a key
			\param key key to access
			\return value of the key
			
			\details if the key does not exist, it is created automatically
		 */
		V& operator[] (const K& key);
		
		/*! \brief get right-value reference to the value held by a key
			\param key key to access
			\return value of the key
			
			\details raises fatal error if key does not exist in the hash map
		 */
		const V& operator[] (const K& key) const;

		/*! \brief removes key from the hash map
			\param key key to remove
			
			\details if key doesn't exist, it is simply ignored
		 */
		void remove_key(const K& key);
		
		inline int size() const {return _size;}
		
		void clear();
		
		HashMap() = default;
		HashMap(const HashMap<K,V,H,S>& other);
		
		HashMap(HashMap<K,V,H,S>&& other);
		
		
		HashMap<K,V,H,S>& operator = (const HashMap<K,V,H,S>& other);
		HashMap<K,V,H,S>& operator = (HashMap<K,V,H,S>&& other);
		
		// make map iterable
		struct MapEntry { const int& key; int& value; };
		
		class iterator
		{
		private:
			HashMap<K,V,H,S>* hmap;
			int h, i;
		public:
			iterator(HashMap<K,V,H,S>* hmap, int h=0, int i=0) : hmap(hmap), h(h), i(i) {}
			iterator operator ++() 
			{
				if(h>=S) return *this;
				i++;
				if(i>=hmap->container[h].size()) { i=0; h++; }
				return *this;
			}
			bool operator != (const iterator& other)
			{
				if(hmap != other.hmap) return false;
				if(h>=S && other.h>=S) return true;
				return h==other.h && i==other.i;
			}
			MapEntry operator*() 
			{
				nds_assert(h<S);
				nds_assert(i<hmap->container[h].size());
				return {hmap->container[h][i].key, hmap->container[h][i].value};
			}
		};
		
		iterator begin() { return iterator(this, 0, 0);}
		iterator end() { return iterator(this, S, 0);}
	};		
}

template<typename K, typename V, int (*H)(const K&), int S>
void DSC::HashMap<K,V,H,S>::clear()
{
	for(int h=0;h<S;h++)
		container[h].reset();
	_size=0;
}

template<typename K, typename V, int (*H)(const K&), int S>
bool DSC::HashMap<K,V,H,S>::contains_key(const K& key) const
{
	int h = H(key);
	nds_assert(0<=h && h<S);
	
	for(int i=0;i<container[h].size();i++)
		if(container[h][i].key==key)
			return true;
	return false;	
}
	
template<typename K, typename V, int (*H)(const K&), int S>	
V& DSC::HashMap<K,V,H,S>::operator[] (const K& key)
{
	int h = H(key);
	nds_assert(0<=h && h<S);
	
	for(int i=0;i<container[h].size();i++)
		if(container[h][i].key==key)
			return container[h][i].value;		
		
	container[h].push_back({key, V()});
	_size++;
	return container[h][container[h].size()-1].value;
	
}

template<typename K, typename V, int (*H)(const K&), int S>
const V& DSC::HashMap<K,V,H,S>::operator[] (const K& key) const
{
	int h = H(key);
	nds_assert(0<=h && h<S);
	
	for(int i=0;i<container[h].size();i++)
		if(container[h][i].key==key)
			return container[h][i].value;
	__assert__(false, "Key not found");
}

template<typename K, typename V, int (*H)(const K&), int S>
void DSC::HashMap<K,V,H,S>::remove_key(const K& key)
{
	int h = H(key);
	nds_assert(0<=h && h<S);
	
	for(int i=0;i<container[h].size();i++)
		if(container[h][i].key==key)
		{
			container[h].remove_at(i);
			_size--;
			return;
		}			
}

template<typename K, typename V, int (*H)(const K&), int S>
DSC::HashMap<K,V,H,S>::HashMap(const DSC::HashMap<K,V,H,S>& other)
{
	for(int i=0;i<S;i++)
		container[i] = DSC::Vector<Entry>(other.container[i]);
	_size = other._size;
}

template<typename K, typename V, int (*H)(const K&), int S>
DSC::HashMap<K,V,H,S>::HashMap(DSC::HashMap<K,V,H,S>&& other)
{
	for(int i=0;i<S;i++)	
		container[i] = DSC::_move_(other.container[i]);
	_size = other._size;	
	other._size = 0;
}

template<typename K, typename V, int (*H)(const K&), int S>
DSC::HashMap<K,V,H,S>& DSC::HashMap<K,V,H,S>::operator = (const DSC::HashMap<K,V,H,S>& other)
{
	for(int i=0;i<S;i++)
		container[i] = DSC::Vector<Entry>(other.container[i]);
	_size = other._size;
	return *this;
}

template<typename K, typename V, int (*H)(const K&), int S>
DSC::HashMap<K,V,H,S>& DSC::HashMap<K,V,H,S>::operator = (DSC::HashMap<K,V,H,S>&& other)
{
	for(int i=0;i<S;i++)	
		container[i] = DSC::_move_(other.container[i]);
	_size = other._size;	
	other._size = 0;
	return *this;
}
