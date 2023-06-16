#pragma once

#include "DSCEngine/types/primitives/avl_tree.hpp"

namespace DSC
{
	template<typename T, typename Comp = Less<T>>
	class OrderedSet
	{
	private:
		TypePrimitives::AVLTree<T, Comp> tree;
	public:
		void insert(const T& t);
		void remove(const T& t);
		bool find(const T& t) const;	

		int size() const;
		
		class iterator
		{
			typename TypePrimitives::AVLTree<T, Comp>::iterator __iter { nullptr };
		public:	
			iterator(typename TypePrimitives::AVLTree<T, Comp>::iterator it) : __iter(it) {}					
			iterator operator ++() 
			{
				++__iter;
				return *this;
			}
			
			bool operator != (const iterator& other)
			{
				return this->__iter != other.__iter;
			}
			
			const T& operator *()
			{
				return *__iter;
			}
		};
		
		iterator begin() { return iterator(tree.begin());  }
		iterator end() { return iterator(tree.end()); }
	};
}

template<typename T, typename Comp>
bool DSC::OrderedSet<T,Comp>::find(const T& t) const
{
	return tree.find(t);
}

template<typename T, typename Comp>
void DSC::OrderedSet<T,Comp>::remove(const T& t)
{
	tree.remove(t);
}

template<typename T, typename Comp>
void DSC::OrderedSet<T,Comp>::insert(const T& t)
{
	tree.insert(t);
}

template<typename T, typename Comp>
int DSC::OrderedSet<T,Comp>::size() const { return tree.size(); }