///////////////////////////////////////////////////////////////////////////////

//	File Name:		AMrcpObserver.h
//
//	Description:	class implementation
//
//	Declared in this file:
//		none
//
//	Implemented in this file:
//
///////////////////////////////////////////////////////////////////////////////
//	Revisions:
//	Date		Initial 	Description
//	----		------- 	-----------

///////////////////////////////////////////////////////////////////////////////

#ifndef AMrcpObserverSubject_H
#define AMrcpObserverSubject_H

#ifdef WIN32
#pragma warning( disable : 4786 )  
#endif 

#include <list>
#include <map>
#include <boost/thread/mutex.hpp>


const int ALL_NOTIFICATIONS   = 0xFFFFFFFF;

namespace MrcpV2RefLib
{
	class AMrcpEventHandler;
	typedef std::list<AMrcpEventHandler *> AMrcpTaskHandlerList;

	typedef std::map<int, AMrcpTaskHandlerList *> AMrcpNotificationListMap;

	class AMrcpObserverSubject 
	{
	public:
		AMrcpObserverSubject();
		virtual ~AMrcpObserverSubject();
		virtual void RegisterForNotification(AMrcpEventHandler* a_observer, int a_notificationID);
		virtual void Unregister(AMrcpEventHandler* a_observer, int a_notificationID = ALL_NOTIFICATIONS);
//		void AMrcpObserverSubject::NotifyObservers(int a_notificationID, void* objPtr = NULL);
		void NotifyObservers(int a_notificationID, void* objPtr = NULL);

	protected:
	private:
		AMrcpNotificationListMap m_notifications;
		void RemoveObserver(AMrcpEventHandler* observer,AMrcpTaskHandlerList* a_observerList);

		boost::mutex m_observerMutex;

	};
}//end namespace

#endif

