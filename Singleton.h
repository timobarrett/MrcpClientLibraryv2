
///////////////////////////////////////////////////////////////////////////////
//	File Name:		Singleton.h
//
//	Description:	 class implementation
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
//
//
///////////////////////////////////////////////////////////////////////////////
#ifndef Singleton_h
#define Singleton_h

#include <boost/thread/mutex.hpp>

	template <typename T>
	class Singleton {
	public:
		static T *Instance () {
			// first check
			if (instance_ == 0) {
				// scoped lock
				boost::mutex::scoped_lock lock(mutex_);
 
				// second check
				if (instance_ == 0)
					instance_ = new T;
			}
			return instance_;
		}
	private:
		static T *instance_;
		static boost::mutex mutex_;
	};

	template <class T>
	T *Singleton<T>::instance_ = 0;

	template <class T>
	boost::mutex Singleton<T>::mutex_;

#endif
