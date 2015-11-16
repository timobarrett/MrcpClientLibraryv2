///////////////////////////////////////////////////////////////////////////////
//	File Name:		AMrcpEventHandler.h
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
//
///////////////////////////////////////////////////////////////////////////////

#ifndef AMrcpEventHandler_H
#define AMrcpEventHandler_H

namespace MrcpV2RefLib
{
  class CMrcpEvent;
  class CMrcpTaskProcessor;

  class AMrcpEventHandler
  {
  public:
	  AMrcpEventHandler();
	  virtual ~AMrcpEventHandler();

	  int PostEvent(void){return PostEvent( GetEvent());};
	  virtual int PostEvent (CMrcpEvent*a_event);

	  virtual CMrcpEvent* GetEvent() {return 0;};
	//concrete must implement the following
	  virtual void HandleEvent( CMrcpEvent *a_event)= 0;
  
  protected:

  private:
  	  MrcpV2RefLib::CMrcpTaskProcessor* m_mrcpTaskProcessor;
  }; 

}//end namespace

#endif //AMrcpEventHandler_h
