#include "DSCEngine/events/event.hpp"

#include "DSCEngine/debug/assert.hpp"
#include "DSCEngine/debug/log.hpp"

using namespace DSC;

bool DSC::EventHandlerContainer::operator == (const EventHandlerContainer& other) const
{
	return this->type_id == 0 && other.type_id == 0;
}

bool DSC::EventHandlerContainer::has_same_type(const EventHandlerContainer& other) const
{
	return this->type_id == other.type_id;
}

DSC::StaticEventHandlerContainer::StaticEventHandlerContainer(const StaticEventHandler& handler) : __handler(handler)
{
	type_id = 1;	
}

void DSC::StaticEventHandlerContainer::execute(void* sender, void* args) const
{
	nds_assert(__handler!=nullptr); // if this is raised, something's really messed up
	__handler(sender, args);
}

bool DSC::StaticEventHandlerContainer::operator == (const EventHandlerContainer& other) const
{
	if(!this->has_same_type(other))
		return false;
	
	const StaticEventHandlerContainer* p_other = (const StaticEventHandlerContainer*)&other;
	return this->__handler == p_other->__handler;
}

DSC::Event::Event()
{
	actions.clear();
}

DSC::Event& DSC::Event::operator += (const StaticEventHandler& e)
{
	nds_assert(e != nullptr);	
	actions.push_back(new StaticEventHandlerContainer(e));
	return *this;
}

DSC::Event& DSC::Event::operator -= (const StaticEventHandler& e)
{
	StaticEventHandlerContainer eh(e);
	for(int i=0;i<actions.size();i++)
	{
		if(*actions[i] == eh)
		{
			EventHandlerContainer* target = actions[i];
			actions.remove_at(i);
			delete target;
		}
	}	
	return *this;
}
	
void DSC::Event::trigger(void* sender, void* event_args) const
{
	for(int i=0;i<actions.size();i++)
	{			
		actions[i]->execute(sender, event_args);
	}
}

DSC::Event::~Event()
{	
	for(int i=0;i<actions.size();i++)
	{		
		delete actions[i];
	}	
}