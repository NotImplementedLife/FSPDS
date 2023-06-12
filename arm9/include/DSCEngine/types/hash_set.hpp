#pragma once

#include "DSCEngine/types/hash.hpp"
#include "DSCEngine/types/vector.hpp"
#include "DSCEngine/debug/assert.hpp"

namespace DSC
{
	template<typename T, int (*H)(const T&) = default_hash<T, 128>, int S = 128>
	class HashSet
	{
	private:
		Vector<T> container[S];
	public:
		void add(const T& item);
		void remove(const T& item);		
		bool includes(const T& item) const;		
	};
	
	template<typename T, int (*H)(const T&), int S>
	void HashSet<T,H,S>::add(const T& item)
	{
		int h = H(item);
		nds_assert(0<=h && h<S);
		
		bool found = false;
		for(int i=0;i<container[h].size() && !found;i++)
			found = (container[h][i]==item);
		
		if(found) return;
		container[h].push_back(item);				
	}
	
	template<typename T, int (*H)(const T&), int S>
	void HashSet<T,H,S>::remove(const T& item)
	{
		int h = H(item);
		nds_assert(0<=h && h<S);		
		container[h].remove(item);
	}
	
	template<typename T, int (*H)(const T&), int S>
	bool HashSet<T,H,S>::includes(const T& item) const
	{
		int h = H(item);
		nds_assert(0<=h && h<S);
		
		for(int i=0;i<container[h].size();i++)
			if(container[h][i]==item)
				return true;
		return false;	
	}
}