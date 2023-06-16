/*!
* \file stack.hpp
* \brief Generic fixed stack definition
 */

#pragma once

#define callstack_ret return

namespace DSC
{
	template<typename T> class FixedStack
	{
	private:				
		int max_size = 0;
		int top_pos = 0;
		T* stack = nullptr;		
	public:				
		FixedStack(int max_size);
		FixedStack(FixedStack<T>&& stack);
		FixedStack<T>& operator = (FixedStack<T>&& stack);			

		FixedStack(const FixedStack<T>& stack);
		FixedStack<T>& operator = (const FixedStack<T>& stack);
		
		/*!
		* \brief Clears the stack	
		 */
		void clear();
		
		/*!
		* \brief Checks if the stack is empty
		* 
		* \return true is stack contains no elements, false otherwise		
		 */
		bool is_empty() const;
		
		/*!
		* \brief Adds an element at the top of the stack
		* 
		* \param [in] value Element to be added		
		 */
		void push(const T& value);
		
		/*!
		* \brief Removes the element at the top of the stack
		* 
		* \return The popped element
		 */
		T pop();

		T top() const;
		
		T peek(int position) const;
		
		int size() const;
		
		~FixedStack();
		
		/*!
		* \brief Creates a stack from all elements ranging between certain values
		* 
		* \param [in] a start of the count
		* \param [in] b count limit
		* \param [in] step count step
		* \return A stack containing a, a+step, ..., up to b, not including b
		* 
		* \details This method is only available with aritmetic types: `FixedStack<int>::from_range()`.
		* 		For example, `FixedStack<Pizza>::from_range()` raises a syntax error.
		* 
		* \details A type is considered arithmetic if it allows for addition (+), substraction (-),
		* 		comparison ( <, >) and has a "zero" element (x+0=x, x-0=x).
		* 
		* \details Extra care is needed when choosing the step
		* \code{.cpp}
		* FixedStack<int> s1 = FixedStack<int>::from_range(1, 10, 2); // ok: 1,3,5,7,9
		* FixedStack<int> s2 = FixedStack<int>::from_range(10, 1, -3); // ok: 10,7,4
		* 
		* FixedStack<int> s2 = FixedStack<int>::from_range(5, 1, 1); // out of range! 
		* FixedStack<int> s2 = FixedStack<int>::from_range(1, 5, -1); // out of range!
		* \endcode
		 */
		static FixedStack<T> from_range(const T& a, const T& b, const T& step);
	};
}

#include "DSCEngine/debug/assert.hpp"

template<typename T>
int DSC::FixedStack<T>::size() const { return top_pos; }

template<typename T>
T DSC::FixedStack<T>::peek(int position) const
{	
	nds_assert(position<top_pos, "Peek position too high");		
	return stack[top_pos-1-position];
}

template<typename T>
DSC::FixedStack<T>::FixedStack(DSC::FixedStack<T>&& stack) 
{
	max_size = stack.max_size;
	top_pos = stack.top_pos;
	stack = stack.stack;
	
	stack.max_size=0;
	stack.top_pos=0;		
	stack.stack = nullptr;	
}

template<typename T>
DSC::FixedStack<T>& DSC::FixedStack<T>::operator = (DSC::FixedStack<T>&& stack)
{
	max_size = stack.max_size;
	top_pos = stack.top_pos;
	stack = stack.stack;
	
	stack.max_size=0;
	stack.top_pos=0;		
	stack.stack = nullptr;	
	return *this;
}

template<typename T>
DSC::FixedStack<T>::FixedStack(const DSC::FixedStack<T>& stack)
{
	max_size = stack.max_size;
	top_pos = stack.top_pos;
	stack = new T[max_size];

	for(int i=0;i<top_pos;i++)
		stack[i]=stack.stack[i];
}

template<typename T>
DSC::FixedStack<T>& DSC::FixedStack<T>::operator = (const DSC::FixedStack<T>& stack)
{
	max_size = stack.max_size;
	top_pos = stack.top_pos;
	stack = new T[max_size];

	for(int i=0;i<top_pos;i++)
		stack[i]=stack.stack[i];
	return *this;
}

template<typename T>
bool DSC::FixedStack<T>::is_empty() const
{
	return top_pos==0;
}

template<typename T>
void DSC::FixedStack<T>::push(const T& value)
{	
	nds_assert(top_pos<max_size, "Stack overflow");	
	stack[top_pos++] = value;
}

template<typename T>
T DSC::FixedStack<T>::pop()
{
	nds_assert(top_pos>0, "Cannot perform pop on an empty stack");	
	return stack[--top_pos];
}

template<typename T>
T DSC::FixedStack<T>::top() const
{
	nds_assert(top_pos>0, "Empty stack does not have a top");
	return stack[top_pos];
}

template<typename T>
DSC::FixedStack<T>::FixedStack(int max_size) : max_size(max_size)
{
	stack = new T[max_size];	
}

template<typename T>
DSC::FixedStack<T>::~FixedStack()
{
	delete[] stack;
}

template<typename T>
void DSC::FixedStack<T>::clear()
{
	top_pos=0;
}

template<typename T>
DSC::FixedStack<T> DSC::FixedStack<T>::from_range(const T& a, const T& b, const T& step)
{
	DSC::FixedStack<T> s;
	T Z = T(); // 0
	if(step>Z)
	{
		nds_assert(a<=b);
		for(T i=a;i<b;i+=step) 
			s.push(i);
	}
	else if(step<Z)
	{
		nds_assert(a>=b);
		for(T i=a;i>b;i+=step) 
			s.push(i);
	}
	return s;
}