#pragma once

#include <hogboxStage/Entity.h>

namespace hogboxStage 
{

//
//CollidableEntity
//
class HOGBOXSTAGE_EXPORT CollidableEntity : public osg::Object
{
public:
	CollidableEntity(bool isProcedural=false)
		: osg::Object(),
		m_isProcedural(isProcedural)
	{
		//add the on desturct message
		AddCallbackEventType("OnDestruct");
		AddCallbackEventType("OnUpdate");
	}

	/** Copy constructor using CopyOp to manage deep vs shallow copy.*/
	Entity(const Entity& ent,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
		: osg::Object(ent, copyop)
	{
		//add the on desturct message
		AddCallbackEventType("OnDestruct");
		AddCallbackEventType("OnUpdate");
	}

	META_Object(hogboxStage, Entity);

	//
	//register a callback to one of our events, returns false if the event does not exist
	bool RegisterCallbackForEvent(EntityEventCallback* callback, const std::string& eventName){
		if(!callback){return false;}
		int eventIndex = GetCallbackEventIndex(eventName);
		if(eventIndex == -1){return false;}

		if(!_callbackEvents[eventIndex]->AddCallbackReceiver(callback)){
			return false;
		}
		return true;
	}

	//
	//Handle an event by name, signaling all callback receivers if the event is used by this entity
	//returns false if event name does not exist
	bool HandleEvent(const std::string& eventName, EntityEvent* entityEvent)
	{	
		//see if anyone can handle the event
		if(TryToHandleEvent(eventName, entityEvent))
		{
			//if we use it then trigger any callback associated
			TriggerEventCallback(eventName, entityEvent);
			return true;
		}
		return false;
	}

	//
	//returns the index of the callback if it exists else -1
	int GetCallbackEventIndex(const std::string& eventName){
		for(unsigned int i=0; i<_callbackEvents.size(); i++){
			if(_callbackEvents[i]->GetEventName() == eventName){
				return (int)i;
			}
		}
		return -1;
	}

protected:

	virtual ~Entity(void) {
		//inform callback receivers of destuct (TEST)
		TriggerEventCallback("OnDestruct");
	}

	//
	//Our main update function 
	virtual bool OnUpdate(EventUpdate* update){return true;}

	virtual bool TryToHandleEvent(const std::string& eventName, EntityEvent* entityEvent){
		//cast entity event to known types
		EventUpdate* update = dynamic_cast<EventUpdate*>(entityEvent);
		if(update){
			OnUpdate(update);
			return true;
		}
		return false;
	}

	//
	//Adds a new EntityCallbackEvent to our list ensuring the name is unique, this should be used
	//during a types constuctor to register any new event types, returns false if name already used
	bool AddCallbackEventType(const std::string& eventName){
		int existingIndex = GetCallbackEventIndex(eventName);
		if(existingIndex == -1){return false;}
		_callbackEvents.push_back(new EntityCallbackEvent(this, eventName));
		return true;
	}

	//
	//Trigger an event callback
	bool TriggerEventCallback(const std::string& eventName, EntityEvent* entityEvent){
		int eventIndex = GetCallbackEventIndex(eventName);
		if(eventIndex == -1){return false;}
		_callbackEvents[eventIndex]->TriggerCallback(entityEvent);
		return true;
	}

protected:

	//has the entity been generated by code (ture) or loaded
	//from xml (false)
	bool m_isProcedural;

	//the list of callback events this etity has registered
	std::vector<EntityCallbackEventPtr> _callbackEvents;

};
typedef osg::ref_ptr<Entity> EntityPtr;

};