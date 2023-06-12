/*!
 * \file event.hpp
 */

#pragma once

#include "DSCEngine/types/vector.hpp"
#include "DSCEngine/debug/assert.hpp"

namespace DSC
{
	/*! \brief Function type to be executed when event fires	 
		\param sender instance that triggered the event
		\param args event arguments	 
	 */
	typedef void (*StaticEventHandler)(void* sender, void* args);
	
	
	/*! \brief Class method type to be executed when event fires
		\param sender instance that triggered the event
		\param args event arguments	 
	 */
	template <class C>
	using NonStaticEventHandler = void (C::*)(void* sender, void* args);
		

	class EventHandlerContainer
	{		
	protected:
		int type_id = 0;		
	public:				
		bool has_same_type(const EventHandlerContainer& other) const;
		EventHandlerContainer() = default;
		virtual void execute(void* sender, void* args) const;
		
		virtual bool operator == (const EventHandlerContainer& other) const;
		
		virtual ~EventHandlerContainer() = default;		
	};
			
	class StaticEventHandlerContainer final : public EventHandlerContainer
	{
	private:
		StaticEventHandler __handler;		
	public:
		StaticEventHandlerContainer(const StaticEventHandler& handler);
		virtual void execute(void* sender, void* args) const override;
		
		virtual bool operator == (const EventHandlerContainer& other) const;	

		~StaticEventHandlerContainer() = default;		
	};
	
	template<class C>
	class NonStaticEventHandlerContainer final : public EventHandlerContainer
	{
	protected:
		NonStaticEventHandler<C> __handler;
		C* __instance;		
	public:
		NonStaticEventHandlerContainer(const NonStaticEventHandler<C>& handler, C* instance);
		virtual void execute(void* sender, void* args) const override;
		
		virtual bool operator == (const EventHandlerContainer& other) const;		
		
		~NonStaticEventHandlerContainer() = default;
	};
	
	
	/*!
	 * \brief Class responsible with event registration and execution
	 *
	 *
	 */
	class Event
	{
	private:
		Vector<EventHandlerContainer*> actions;
		
	public:
		/*!
		 * \brief Creates a new Event instance
		 */		 
		Event();
		
		/*!
		 * \brief Adds a new event handler to this event
		 * \param [in] e event handler
		 * \returns this Event instance
		 */		
		Event& operator += (const StaticEventHandler& e);
		
		/*!
		 * \brief Removes an event handler from this event
		 * \param [in] e event handler
		 * \returns this Event instance
		 */
		Event& operator -= (const StaticEventHandler& e);
		
		template<class C>
		void add_event(const NonStaticEventHandler<C> e, C* instance);
		
		template<class C>
		void remove_event(const NonStaticEventHandler<C> e, C* instance);
		
		/*!
		 * \brief Fires the event
		 * \param [in] sender the object which fires the event
		 * \param [in] event args (can be anything)
		 */
		void trigger(void* sender, void* args) const;	

		~Event();
	};
	
	
	template<class C>
	NonStaticEventHandlerContainer<C>::NonStaticEventHandlerContainer(const NonStaticEventHandler<C>& handler, C* instance)
		: __handler(handler), __instance(instance)
	{
		type_id = 2;
	}

	template<class C>
	void NonStaticEventHandlerContainer<C>::execute(void* sender, void* args) const
	{
		nds_assert(__handler!=nullptr); // if this is raised, something's really messed up		
		//Debug::log("Executing event...");
		
		(__instance->*__handler)(sender, args);
				
		//Debug::log("Event executed");
	}

	template<class C>
	bool NonStaticEventHandlerContainer<C>::operator == (const EventHandlerContainer& other) const
	{
		if(!this->has_same_type(other))
			return false;
		const NonStaticEventHandlerContainer<C>* p_other 
			= (const NonStaticEventHandlerContainer<C>*)(&other);		
		return this->__handler == p_other->__handler && this->__instance == p_other->__instance;
	}

	
	
	template<class C>
	void Event::add_event(const NonStaticEventHandler<C> e, C* instance)
	{
		nds_assert(e != nullptr);	
		actions.push_back(new NonStaticEventHandlerContainer<C>(e, instance));		
	}

	template<class C>
	void Event::remove_event(const NonStaticEventHandler<C> e, C* instance)
	{
		NonStaticEventHandlerContainer<C> eh(e, instance);
		for(int i=0;i<actions.size();i++)
		{
			if(*actions[i] == eh)
			{
				EventHandlerContainer* target = actions[i];
				actions.remove_at(i);
				delete target;
			}
		}			
	}
}