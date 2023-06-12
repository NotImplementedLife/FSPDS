/*!
* \file stack.hpp
* \brief Generic stack definition
 */

#pragma once

#include "DSCEngine/debug/error.hpp"

namespace DSC
{
	template<typename T> class Stack
	{
	private:
		struct Item 
		{
			T value;
			Item* next;
		};
		Item* top_item = nullptr;
	public:				
		Stack() = default;
		Stack(Stack<T>&& stack);
		Stack<T>& operator = (Stack<T>&& stack);		
		
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
		
		~Stack();
		
		/*!
		* \brief Creates a stack from all elements ranging between certain values
		* 
		* \param [in] a start of the count
		* \param [in] b count limit
		* \param [in] step count step
		* \return A stack containing a, a+step, ..., up to b, not including b
		* 
		* \details This method is only available with aritmetic types: `Stack<int>::from_range()`.
		* 		For example, `Stack<Pizza>::from_range()` raises a syntax error.
		* 
		* \details A type is considered arithmetic if it allows for addition (+), substraction (-),
		* 		comparison ( <, >) and has a "zero" element (x+0=x, x-0=x).
		* 
		* \details Extra care is needed when choosing the step
		* \code{.cpp}
		* Stack<int> s1 = Stack<int>::from_range(1, 10, 2); // ok: 1,3,5,7,9
		* Stack<int> s2 = Stack<int>::from_range(10, 1, -3); // ok: 10,7,4
		* 
		* Stack<int> s2 = Stack<int>::from_range(5, 1, 1); // out of range! 
		* Stack<int> s2 = Stack<int>::from_range(1, 5, -1); // out of range!
		* \endcode
		 */
		static Stack<T> from_range(const T& a, const T& b, const T& step);
	};
}

template<typename T>
DSC::Stack<T>::Stack(DSC::Stack<T>&& stack) 
{
	top_item = stack.top_item;	
	stack.top_item = nullptr;
}

template<typename T>
DSC::Stack<T>& DSC::Stack<T>::operator = (DSC::Stack<T>&& stack)
{
	top_item = stack.top_item;	
	stack.top_item = nullptr;
	return *this;
}

template<typename T>
bool DSC::Stack<T>::is_empty() const
{
	return top_item == nullptr;
}

template<typename T>
void DSC::Stack<T>::push(const T& value)
{
	Item* item = new Item();
	item->value = value;
	item->next = top_item;
	top_item = item;
}

template<typename T>
T DSC::Stack<T>::pop()
{
	nds_assert(top_item!=nullptr, "Cannot perform pop on an empty stack");
	Item* item = top_item;
	T value = item->value;
	top_item = top_item->next;
	delete item;
	callstack_ret value;
	
}

template<typename T>
DSC::Stack<T>::~Stack()
{
	clear();
}

template<typename T>
void DSC::Stack<T>::clear()
{
	while(top_item!=nullptr) 
		pop();
}

template<typename T>
DSC::Stack<T> DSC::Stack<T>::from_range(const T& a, const T& b, const T& step) \
{
	DSC::Stack<T> s;
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