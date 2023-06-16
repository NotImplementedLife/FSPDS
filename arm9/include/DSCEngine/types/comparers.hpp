#pragma once

namespace DSC
{
	template<typename T> 
    struct Less
    {
        int operator() (const T& a, const T& b) const 
        { 
            if (a == b) return 0;
            return a < b ? -1 : 1;
        }
    };

	template<typename T> 
	struct Less<T*>
	{
		int operator() (const T* a, const T* b) const 
		{ 
			if (a == b) return 0;
			return (int)a < (int)b ? -1 : 1;
		}
	};		

    template<typename T>
    struct Greater
    {
        int operator() (const T& a, const T& b) const
        {
            if (a == b) return 0;
            return a > b ? -1 : 1;
        }
    };

    template<typename T, typename U, U T::*M>
    struct MemberLess
    {
        int operator() (const T& a, const T& b) const
        {
            if (a.*M == b.*M) return 0;
            return a.*M < b.*M ? -1 : 1;
        }
    };
	
	template<typename T, typename U, U T::*M>
    struct MemberGreater
    {
        int operator() (const T& a, const T& b) const
        {
            if (a.*M == b.*M) return 0;
            return a.*M > b.*M ? -1 : 1;
        }
    };
	
	template<typename T, typename U, U T::*M, typename Comp>
	struct MemberCustom
	{
		int operator() (const T& a, const T& b) const
		{
			return Comp()(a.*M, b.*M);
		}			
	};
}