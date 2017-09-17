#ifndef __AWUSERINPUT_H
#define __AWUSERINPUT_H

#include "AW3DTools/AWToolsCommon.h"

//want to consider first KEYBOARD and MOUSE input
//
//require multiple simultaneous key presses and button presses to
//be detected.

#define USE_KEYBOARD 0
#define USE_JOYSTICK 1
#define USE_MOUSE    2

#define ANGLAR_ROTN       65.0f 

#define DEADBAND     3000.0f

//#define ROLL_SPEED   1.5
//#define YAW_SPEED    5.5
//#define PITCH_SPEED  3.5

//use value of a char [0..255] to index the keys array
#define AW_KEY_UP              40    /* UpArrow on arrow keypad */
#define AW_KEY_LEFT            37    /* LeftArrow on arrow keypad */
#define AW_KEY_RIGHT           39    /* RightArrow on arrow keypad */
#define AW_KEY_DOWN            38    /* DownArrow on arrow keypad */
#define AW_KEY_SPACE           32
#define AW_KEY_Z               'z'
#define AW_KEY_A               'a'
//these will break any Linux version if I can ever be bothered again
#define AW_KEY_PAGEUP          VK_PRIOR
#define AW_KEY_PAGEDOWN        VK_NEXT
#define AW_KEY_HOME            VK_HOME
#define AW_KEY_END             VK_END
#define AW_PRNT_SCRN           VK_SNAPSHOT


class AWUserInput
{
public:
	AWUserInput();
	~AWUserInput();

	int	        start();

	void        killFocus();
	void        setFocus();

	int         determineFocus();
	int         getKeyData(float t=0.0f);
//#ifdef _WIN32
   void        setClientArea(const RECT& r);
   const RECT& getClientArea()             {return m_clientArea;}

   void 			joyMoveXEvent(int x);
   void 			joyMoveYEvent(int x);
   void        joyMoveEvent(WORD x, WORD y);
   void        joyBtnEvent(int code, AWBoolean down=true);
   void        mouseMoveEvent(WORD x, WORD y);
   void        mouseBtnEvent(int code, AWBoolean down=true);
   void        getClientCenter(POINT& p)  {p.x=m_clientCentre.x; p.y=m_clientCentre.y;}
   void        setCursorHome();
   void        setLastPos(WORD x, WORD y) {m_lastPos.x=x; m_lastPos.y=y;}

   void        setInputMethod(int v) {if ((USE_JOYSTICK==v) || (USE_KEYBOARD==v) || (USE_MOUSE==v)) m_inputMethod=v;}
   void        setUseJoystick()   {m_inputMethod=USE_JOYSTICK;}
   AWBoolean        getUseJoystick()   {return USE_JOYSTICK == m_inputMethod;}
   void        setUseKeyboard()   {m_inputMethod=USE_KEYBOARD;}
   AWBoolean        getUseKeyboard()   {return USE_KEYBOARD == m_inputMethod;}
   void        setUseMouse()      {m_inputMethod=USE_MOUSE;}
   AWBoolean        getUseMouse()      {return USE_MOUSE == m_inputMethod;}
//#endif
   void        keyEvent(unsigned char code, AWBoolean down);
   void        clearKeys();

	//get control data
	//the following calls are specific to Space Gnats. Will in the end create 
	//derived classes for specific game modules, but experimental for now.
   AWBoolean        captureFrame();
   AWBoolean        toggleMesh();
   AWBoolean        toggleNormals();
	AWBoolean        forwardCam();
   AWBoolean        backCam();
   AWBoolean        speedUp();
	AWBoolean        speedDown();
	AWBoolean        left();
	AWBoolean        right();
	AWBoolean        up();
	AWBoolean        down();
    AWBoolean        fire();
    AWBoolean        gotFocus()                  {return m_gotFocus;}

protected:
   int        m_inputMethod;
#ifdef _WIN32
   HCURSOR     m_oldCursor;
#endif
   RECT        m_clientArea;
   RECT        m_mainZone;
   POINT		   m_clientCentre;
   POINT       m_lastPos;
   POINT       m_moveAccumulator;
	AWBoolean		   m_keys[256];                //keyboard state buffer
   float             m_keyPressedAt[256];
   float          m_delay;
   AWBoolean        m_gotFocus;  
};//class AWUserInput


inline void 
AWUserInput::clearKeys()
{
   if (getUseMouse())
   {  //only want to clear these fore the case of the mouse on Linux
      //examine Windows joystick behavior here
      m_keys[AW_KEY_LEFT]  = FALSE;
      m_keys[AW_KEY_RIGHT] = FALSE;
      m_keys[AW_KEY_DOWN]  = FALSE;
      m_keys[AW_KEY_UP]    = FALSE;
      m_keys[AW_KEY_A]     = FALSE;
      m_keys[AW_KEY_Z]     = FALSE;
   }
   m_keys[AW_KEY_PAGEUP]   = FALSE;
   m_keys[AW_KEY_PAGEDOWN] = FALSE;
   m_keys[AW_KEY_HOME]     = FALSE;
   m_keys[AW_KEY_END]      = FALSE;
   m_keys[AW_PRNT_SCRN]    = FALSE;
}//void AWUserInput::clearKeys()



inline void        
AWUserInput::keyEvent(unsigned char code, AWBoolean down)
{//key value will be set by keydown, cleared by keyup
    m_keys[code] = down;
}//void AWUserInput::keyEvent(unsigned char code, AWBoolean down)


inline AWBoolean 
AWUserInput::speedUp()
{
	return m_keys[AW_KEY_A];
}


inline AWBoolean 
AWUserInput::speedDown()
{
	return m_keys[AW_KEY_Z];
}


inline AWBoolean 
AWUserInput::left()
{
	return m_keys[AW_KEY_LEFT];
}


inline AWBoolean
AWUserInput::fire()
{
    return m_keys[AW_KEY_SPACE];
}


inline AWBoolean 
AWUserInput::right()
{
	return m_keys[AW_KEY_RIGHT];
}


inline AWBoolean 
AWUserInput::up()
{
	return m_keys[AW_KEY_UP];
}


inline AWBoolean 
AWUserInput::down()
{
	return m_keys[AW_KEY_DOWN];
}


inline AWBoolean 
AWUserInput::backCam()
{
   return m_keys[AW_KEY_PAGEUP];
}


inline AWBoolean 
AWUserInput::forwardCam()
{
   return m_keys[AW_KEY_PAGEDOWN];
}


inline AWBoolean 
AWUserInput::toggleMesh()
{
   return m_keys[AW_KEY_HOME];
}


inline AWBoolean 
AWUserInput::toggleNormals()
{
   return m_keys[AW_KEY_END];
}


inline AWBoolean
AWUserInput::captureFrame()
{
   return m_keys[AW_PRNT_SCRN];
}
#endif //sentinel __AWUSERINPUT_H
