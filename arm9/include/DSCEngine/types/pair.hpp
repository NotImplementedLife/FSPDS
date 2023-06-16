#pragma once

namespace DSC
{
	template<typename T1, typename T2>
	struct Pair
	{
		T1 value1;
		T2 value2;
	};

	template<typename T1, typename T2>	
	inline bool operator == (const Pair<T1,T2>& p1, const Pair<T1,T2>& p2) { return p1.value1==p2.value2 && p1.value2==p2.value2; }
	
	template<typename T1, typename T2>
	inline bool operator != (const Pair<T1,T2>& p1, const Pair<T1,T2>& p2) { return p1.value1!=p2.value2 || p1.value2!=p2.value2; }
		
}