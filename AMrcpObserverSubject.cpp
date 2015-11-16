///////////////////////////////////////////////////////////////////////////////
//	File Name:		AMrcpObserver.cpp
//
//	Description:	class implementation
//
//	Declared in this file:
//		none
//
//	Implemented in this file:
//    AMrcpObserverSubject::AMrcpObserverSubject()
//    AMrcpObserverSubject::~AMrcpObserverSubject()
//    void AMrcpObserverSubject::RegisterForNotification(AMrcpEventHandler* a_observer, int a_notificationID)
//    void AMrcpObserverSubject::Unregister(AMrcpEventHandler* a_observer, int a_notificationID)
//    void AMrcpObserverSubject::RemoveObserver(AMrcpEventHandler* a_observerTarget,
//                                              AMrcpTaskHandlerList* a_observerList)
//    void AMrcpObserverSubject::NotifyObservers(int a_notificationID, void* objPtr)
//    
// 
///////////////////////////////////////////////////////////////////////////////
//	Revisions:
//	Date		Initial 	Description
//	----		------- 	-----------

///////////////////////////////////////////////////////////////////////////////

#include "AMrcpObserverSubject.h"
#include "AMrcpEventHandler.h"
#include "CMrcpEvent.h"


namespace MrcpV2RefLib
{
////////////////////////////////////////////////////////////////////////// 
//
// Description - Constructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
 AMrcpObserverSubject::AMrcpObserverSubject()
 {

 }
////////////////////////////////////////////////////////////////////////// 
//
// Description - Destructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
 AMrcpObserverSubject::~AMrcpObserverSubject()
 {

        { // scope for lock - make sure locked mutex is free?
             boost::mutex::scoped_lock l_controlLock (m_observerMutex);
        }
 }
////////////////////////////////////////////////////////////////////////// 
//
// Description - RegisterForNotification
// Input - pointer to event handler
//         event registering for
// Output - None
//          Objects register themselves by event in order to receive events 
//          broadcast from objects they are observing
///////////////////////////////////////////////////////////////////////////
void AMrcpObserverSubject::RegisterForNotification(AMrcpEventHandler* a_observer, int a_notificationID)
{

    boost::mutex::scoped_lock lock(m_observerMutex);

	AMrcpNotificationListMap::iterator l_observerList;

    l_observerList = m_notifications.find(a_notificationID);
    if (l_observerList == m_notifications.end())
    {
        AMrcpTaskHandlerList* l_newObserverList = new AMrcpTaskHandlerList;
        l_newObserverList->push_back(a_observer);
        m_notifications[a_notificationID] = l_newObserverList;
    }
    else
    {
        m_notifications[a_notificationID]->push_back(a_observer);
    }
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - Unregister
// Input - pointer to event handler
//         event unregistering for
// Output - None
//          Objects will unregister for events, this should be done before
//          destructing.
///////////////////////////////////////////////////////////////////////////

void AMrcpObserverSubject::Unregister(AMrcpEventHandler* a_observer, int a_notificationID)
{
	boost::mutex::scoped_lock lock(m_observerMutex);

    AMrcpNotificationListMap::iterator l_observerList;

    if (a_notificationID == ALL_NOTIFICATIONS)
    {
        for ( l_observerList = m_notifications.begin();
            l_observerList != m_notifications.end();
            ++l_observerList)
        {
           RemoveObserver( a_observer, l_observerList->second);
	}
    }
    else
    {
        l_observerList = m_notifications.find(a_notificationID);
        if (l_observerList != m_notifications.end())
        {
           RemoveObserver( a_observer, l_observerList->second);
           if (l_observerList->second->empty())
           {
              delete l_observerList->second;
              m_notifications.erase(l_observerList);
           }
        }

    }
 
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - RemoveObserver
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////

void AMrcpObserverSubject::RemoveObserver(AMrcpEventHandler* a_observerTarget,
                                         AMrcpTaskHandlerList* a_observerList)
{
    AMrcpTaskHandlerList::iterator l_observer;

    for ( l_observer = a_observerList->begin();
        l_observer != a_observerList->end();
        ++l_observer)
    {
        if ((*l_observer) == a_observerTarget)
        {
            break;
        }
    }
    if (l_observer != a_observerList->end())
    {
        a_observerList->erase(l_observer);
    }
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - NotifyObservers
// Input - ID of event 
//         pointer to object to receive notification
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
void AMrcpObserverSubject::NotifyObservers(int a_notificationID, void* objPtr)
{
    AMrcpNotificationListMap::iterator l_observerList;
    AMrcpEventHandler* l_obj = NULL;
   
    {//scope for the lock
       boost::mutex::scoped_lock lock(m_observerMutex);
       l_observerList = m_notifications.find(a_notificationID);
       if (l_observerList != m_notifications.end())
       {
          AMrcpTaskHandlerList::iterator l_observer;
          for ( l_observer = (l_observerList->second)->begin();
              l_observer != (l_observerList->second)->end();
              ++l_observer)
          {
              l_obj = (*l_observer);
              if (l_obj != NULL)
              {
		 CMrcpEvent* l_mrcpEvent = new CMrcpEvent();
		 l_mrcpEvent->EventID(a_notificationID);
		 l_mrcpEvent->EventHandler(l_obj);
                 l_mrcpEvent->EventData(objPtr);

                 l_obj->PostEvent(l_mrcpEvent);
              }
          }
       }
    }//end lock scope
}
}//end namespace

