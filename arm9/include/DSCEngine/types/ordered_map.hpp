#pragma once

#include "DSCEngine/types/primitives/avl_tree.hpp"

namespace DSC
{
	template<typename K, typename V, typename Comp = Less<K>>
	class OrderedMap
	{
	public:
		struct Entry { K key; V value; };						
	private:
		using EntryComparator = MemberCustom<Entry, K, &Entry::key, Comp>;
		TypePrimitives::AVLTree<Entry, EntryComparator> tree;
	public:
		V& operator [] (const K& key);
		const V& operator [] (const K& key) const;
		
		bool contains_key(const K& key) const;
		void remove_key(const K& key);
		int size() const;		
		
		class iterator
		{
			typename TypePrimitives::AVLTree<Entry, EntryComparator>::iterator __iter { nullptr };
		public:	
			iterator(typename TypePrimitives::AVLTree<Entry, EntryComparator>::iterator it) : __iter(it) {}					
			iterator operator ++() 
			{
				++__iter;
				return *this;
			}
			
			bool operator != (const iterator& other)
			{
				return this->__iter != other.__iter;
			}
			
			const Entry& operator *()
			{
				return *__iter;
			}
		};
		
		iterator begin() { return iterator(tree.begin());  }
		iterator end() { return iterator(tree.end()); }
	};
}

#include "DSCEngine/debug/log.hpp"

template<typename K, typename V, typename Comp>
V& DSC::OrderedMap<K,V,Comp>::operator[](const K& key)
{
	//Debug::log("HERE");
	Entry* entry = tree.insert({key, V()}, false);
	//Debug::log("%X", entry);
	return (*entry).value;
}

template<typename K, typename V, typename Comp>
const V& DSC::OrderedMap<K,V,Comp>::operator[](const K& key) const
{
	Entry entry = tree.insert({key, V()}, false);
	return entry.value;
}

template<typename K, typename V, typename Comp>
bool DSC::OrderedMap<K,V,Comp>::contains_key(const K& key) const
{
	return tree.find({key, V()});
}

template<typename K, typename V, typename Comp>
void DSC::OrderedMap<K,V,Comp>::remove_key(const K& key)
{
	tree.remove({key, V()});
}

template<typename K, typename V, typename Comp>
int DSC::OrderedMap<K,V,Comp>::size() const { return tree.size(); }