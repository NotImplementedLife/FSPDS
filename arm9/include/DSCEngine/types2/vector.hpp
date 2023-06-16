/*!
* \file vector.hpp
* \brief Generic dynamic vector definition
 */

// Known flaw: this code does not deal with empty vectors (capacity 0, container nullptr)

#pragma once

#include "DSCEngine/debug/log.hpp"
#include "DSCEngine/debug/assert.hpp"

#define callstack_call ;
#define callstack_ret return

namespace DSC
{
	/*!
	* \brief Generic dynamic vector
	* \tparam T type of contained items
	 */
	template<typename T> class Vector
	{
	private:
		T *container;
		int _size;
		int capacity;
		void container_resize(int new_cap);
	public:				
		Vector(int size=0);
		Vector(const Vector<T>& vector);
		Vector(Vector<T>&& vector);
		
		template <typename... Args>
		Vector(T _first, Args... args)
			: container(new T[sizeof...(args) + 1]{_first, args...}), 
			_size(sizeof...(args) + 1),
			capacity(sizeof...(args) + 1)
		{}

		
		Vector<T>& operator = (Vector<T>&& vector);
		Vector<T>& operator = (const Vector<T>& vector);
		
		/*!
		* \brief Sets all the elements in the vector to their default value determined by their type
		* \details Not to be confused with reset(), which also sets the vector to its initials size
		 */
		void clear();
		
		/*!
		* \brief Sets the vector to its initial state		
		 */
		void reset();
		
		
		/*!
		* \brief Changes vector's number of elements
		* 
		* \param [in] new_cap New vector size		
		* 
		* \details If the vector becomes smaller after this operation, the left-out elements are discarded
		 */
		void resize(int new_size);
		
		/*!
		* \brief Adds new element to the end of the vector
		* 
		* \param [in] item Element to be added				
		 */
		void push_back(const T& item);
		
		/*!
		* \brief Gets the last element in vector
		* 
		* \param [output] The element on the last position in vector 
		* \details Error is raised is the vector is empty
		 */
		T& back();
		
		/*!
		* \brief Finds the position of an item in the vector
		* 
		* \param [in] item Item to be found
		* \return position of the first occurence of the item if it was found, -1 otherwise		
		 */
		int index_of(const T& item) const;
		
		/*!
		* \brief Removes the first occurence of an element from the vector
		* 
		* \param [in] item Element to be removed
		* \return true if removal was successful, false otherwise (item not found)		
		 */
		bool remove(const T& item);
		
		/*!
		* \brief Removes the element at a given position
		* 
		* \param [in] index Position of the element to remove		
		* 
		* \details Range checks are performed only in debug mode.
		 */
		void remove_at(int index);
				
		/*!
		* \brief Gets the vector length
		* 
		* \return Number of elements in vector		
		 */
		int size() const;
		
		/*!
		* \brief Random access index operator
		* 
		* \param [in] index Position in vector
		* \return Element at the specified position	
		* 		
		* \details Range checks are performed only in debug mode.
		 */
		T& operator[] (int index);		
		
		/*!
		* \brief Random access index operator
		* 
		* \param [in] index Position in vector
		* \return Element at the specified position		
		* 
		* \details Can be used as an lvalue e.g. `v[i] = my_T_Element;`
		* \details Range checks are performed only in debug mode.
		 */
		const T& operator[] (int index) const;		
		
		/*!
		* \brief Gets element at a certain index
		* 
		* \param [in] index Position in vector
		* \return Element at the specified position		
		* 
		* \details Range checks are performed only in debug mode.
		 */
		const T& get_at(int index) const;
		
		~Vector();
	};
}

template<typename T>
DSC::Vector<T>::Vector(int size)
{	
	_size = size;	
	int cap = size == 0 ? 1 : size;
	if(sizeof(T)&1)
	{
		cap>>=1; cap++; cap<<=1; // assure capacity is 16-bit aligned
	}	
	capacity = cap;	
	container = new T[cap]();
}

template<typename T>
DSC::Vector<T>::Vector(Vector<T>&& vector)
{
	_size = vector._size; 	
	capacity = vector.capacity;
	container = vector.container;
	vector.container = nullptr;		
	vector._size = 0;
	vector.capacity = 0;
}

template<typename T>
DSC::Vector<T>& DSC::Vector<T>::operator = (DSC::Vector<T>&& vector)
{
	_size = vector.size();
	capacity = vector.capacity;
	container = vector.container;
	vector.container = nullptr;	
	vector._size = 0;
	vector.capacity = 0;
	return *this;
}

template<typename T>
DSC::Vector<T>::Vector(const Vector<T>& vector)
{
	_size = vector._size;
	capacity = vector.capacity;	
	container = new T[capacity];	
	
	for(int i=0;i<_size;i++)
		container[i] = vector.container[i];
	
	// uncomment this to see where the vector copy is called
	//#warning vector copy constructor
	
	//dmaCopy(vector.container, container, _size * sizeof(T));	
}

template<typename T>
DSC::Vector<T>& DSC::Vector<T>::operator = (const DSC::Vector<T>& vector)
{
	_size = vector._size;
	capacity = vector.capacity;
	delete[] container;
	container = new T[capacity];	
	
	for(int i=0;i<_size;i++)
		container[i] = vector.container[i];
	
	//#warning vector copy =
	
	//dmaCopy(vector.container, container, _size * sizeof(T));	
	return *this;
}

template<typename T>
void DSC::Vector<T>::clear()
{
	_size=0;
}

template<typename T>
void DSC::Vector<T>::reset()
{
	container_resize(1);
	_size = 0;
}

template<typename T>
void DSC::Vector<T>:: container_resize(int new_cap)
{
	if(sizeof(T)&1)
	{
		new_cap>>=1; new_cap++; new_cap<<=1;
	}
	T* new_container = new T[new_cap];
	int min_cap = new_cap < _size ? new_cap : _size;
	if(min_cap>0)
	{
		//dmaCopy(container, new_container, min_cap * sizeof(T));
		for(int i=0;i<min_cap;i++)
		{
			new_container[i] = (T&&)container[i];
		}
	}
	delete[] container;
	container = new_container;
	capacity = new_cap;
}

template<typename T>
void DSC::Vector<T>::resize(int new_size)
{		
	nds_assert(new_size>=0);
	container_resize(new_size);
	_size = new_size;
}

template<typename T>
void DSC::Vector<T>::push_back(const T& item)
{
	if(_size==capacity)
	{
		container_resize(capacity<<1);
	}
	container[_size++]=item;
}

template<typename T>
T& DSC::Vector<T>::operator[] (int index)
{
	callstack_call
	
	nds_assert(index>=0, "Index out of range");
	nds_assert(index<_size, "Index out of range");
	
	callstack_ret container[index];
}

template<typename T>
const T& DSC::Vector<T>::operator[] (int index) const
{
	callstack_call
	
	nds_assert(index>=0, "Index out of range");
	nds_assert(index<_size, "Index out of range");
	
	callstack_ret container[index];
}

template<typename T>
const T& DSC::Vector<T>::get_at(int index) const
{
	callstack_call
	
	nds_assert(index>=0, "Index out of range");
	nds_assert(index<_size, "Index out of range");
	
	callstack_ret container[index];
}

template<typename T>
int DSC::Vector<T>::index_of(const T& item) const
{
	for(int i=0;i<_size;i++)
	{
		if(container[i]==item)
			return i;
	}
	return -1;
}

template<typename T>
bool DSC::Vector<T>::remove(const T& item)
{
	for(int i=0;i<_size;i++)
	{
		if(container[i]==item)
		{
			_size--;
			for(int j=i;j<_size;j++)
			{
				container[j] = container[j+1];
			}
			return true;
		}
	}
	return false;
}

template<typename T>
void DSC::Vector<T>::remove_at(int index)
{
	callstack_call
	
	nds_assert(index>=0, "Index out of range");
	nds_assert(index<_size, "Index out of range");	
	
	_size--;
	for(int j=index;j<_size;j++)
	{
		container[j] = container[j+1];
	}	
	callstack_ret;
}

template<typename T>
int DSC::Vector<T>::size() const
{
	return _size;
}

template<typename T>
DSC::Vector<T>::~Vector()
{
	delete[] container;
}

template<typename T>
T& DSC::Vector<T>::back()
{
	callstack_call
	nds_assert(size()>0);
	callstack_ret container[size()-1];
}
