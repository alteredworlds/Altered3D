#include <stdio.h>
#include <math.h>
#ifdef _WIN32
#   include <windows.h>
#endif
#include "AWUserInput.h"

//want to consider first KEYBOARD and MOUSE input
//
//require multiple simultaneous key presses and button presses to
//be detected.
//
//Base on whatever version of DirectInput is supported on NT4?
//hence will required Windows NT 4.0 SP3 or higher

#define MOUSE_X_THRESHOLD 4
#define MOUSE_Y_THRESHOLD 4

#define MAIN_ZONE_PERCENT 30

#ifndef _WIN32
AWBoolean PtInRect(RECT* r, POINT p)
{
   AWTRACEHI2("Point (%d, %d) ", p.x, p.y);
   AWTRACEHI4("Rect TL(%d, %d) BR(%d, %d)\n", r->left, r->top, r->right, r->bottom);     
  return (p.x >= r->left) && (p.x <= r->right) && (p.y >= r->top) && (p.y<=r->bottom);  
}
#endif //ifndef _WIN32


AWUserInput::AWUserInput() : m_gotFocus(FALSE)                            
{
#ifdef _WIN32
   m_oldCursor = NULL;
#endif
   m_inputMethod = USE_KEYBOARD;
   m_moveAccumulator.x = m_moveAccumulator.y = 0;
	clearKeys();
   m_keys[AW_KEY_SPACE] = FALSE;
   for (int i=0; i<256; i++)
   {
      m_keyPressedAt[i] = 0.0f;
   }//for (int i=0; i<256; i++)
   m_delay = 0.3333f; //key repeat delay set at 1/3s
}//AWUserInput::AWUserInput() : m_gotFocus(FALSE)    


AWUserInput::~AWUserInput()
{	//cleanup	
	killFocus();
}//AWUserInput::~AWUserInput()



void
AWUserInput::setFocus()
{
	m_gotFocus = TRUE;
}

void
AWUserInput::killFocus()
{
    m_gotFocus = FALSE;
}


int
AWUserInput::start()
{
	setFocus();
	return 0;
}//int AWUserInput::start()




void AWUserInput::setClientArea(const RECT& r)
{
    m_clientArea.top   = r.top;   m_clientArea.left   = r.left;
    m_clientArea.right = r.right; m_clientArea.bottom = r.bottom;    
    int width  = r.right - r.left;
    int height = r.bottom - r.top;
    m_clientCentre.x = width/2 + r.left;
    m_clientCentre.y = height/2 + r.top;
    //now calculate mainZone central region
    int mainZoneWidth  = (width  * MAIN_ZONE_PERCENT) / 100;
    int mainZoneHeight = (height * MAIN_ZONE_PERCENT) / 100;
    //
    m_mainZone.left   = (width - mainZoneWidth) /2 + m_clientArea.left;
    m_mainZone.right  = m_mainZone.left + mainZoneWidth;
    m_mainZone.top    = (height - mainZoneHeight) /2 + m_clientArea.top;
    m_mainZone.bottom = m_mainZone.top + mainZoneHeight;
    m_lastPos.x = m_clientCentre.x; m_lastPos.y = m_clientCentre.y;
    AWTRACE2("\nClient Area TL (%d, %d) ", m_clientArea.top, m_clientArea.left);
    AWTRACE2(" BR (%d, %d)\n", m_clientArea.bottom, m_clientArea.right );

    AWTRACE2("\nMainZone TL (%d, %d) ", m_mainZone.top, m_mainZone.left);
    AWTRACE2(" BR (%d, %d)\n", m_mainZone.bottom, m_mainZone.right );

    AWTRACE2("\nWindow Center (%d, %d)\n", m_clientCentre.x, m_clientCentre.y);
}//void AWUserInput:setClientArea(const RECT& r)



void
AWUserInput::joyMoveXEvent(int x)
{
   if (getUseJoystick())
   {
      //fprintf(stderr, "joyMoveXEvent %d\n", (int)x);
      if (abs(x) >= DEADBAND)
      {
         m_keys[AW_KEY_LEFT]  = x < 0;
         m_keys[AW_KEY_RIGHT] = !m_keys[AW_KEY_LEFT];
      }
      else m_keys[AW_KEY_LEFT] = m_keys[AW_KEY_RIGHT] = 0;
   }//if (getUseJoystick())
}//void AWUserInput::joyMoveXEvent(int x)

void
AWUserInput::joyMoveYEvent(int y)
{
   if (getUseJoystick())
   {
      //fprintf(stderr, "joyMoveYEvent %d\n", (int)y);
      if (abs(y) >= DEADBAND)
      {
         m_keys[AW_KEY_UP]    = (y > 0);
         m_keys[AW_KEY_DOWN]  = !m_keys[AW_KEY_UP];
      }
      else m_keys[AW_KEY_UP] = m_keys[AW_KEY_DOWN] = 0;
   }//if (getUseJoystick())
}//void AWUserInput::joyMoveYEvent(int y)

void        
AWUserInput::joyMoveEvent(WORD x, WORD y)
{
   if (getUseJoystick())
   {
		int xCentered = x - 32768;
      if (abs(xCentered) >= DEADBAND)
      {
         m_keys[AW_KEY_LEFT]  = xCentered < 0;
         m_keys[AW_KEY_RIGHT] = !m_keys[AW_KEY_LEFT];
      }
      int yCentered = y - 32768;
      if (abs(yCentered) >= DEADBAND)
      {
         m_keys[AW_KEY_UP]    = (yCentered > 0);
         m_keys[AW_KEY_DOWN]  = !m_keys[AW_KEY_UP];
      }
   }//if (getUseJoystick())
}//void AWUserInput::joyMoveEvent(WORD x, WORD y)


void        
AWUserInput::joyBtnEvent(int code, AWBoolean down)
{ //all joystick button events map to FIRE for now
   if (getUseJoystick()) m_keys[AW_KEY_SPACE] = down;
}//void AWUserInput::joyBtnEvent(int code, AWBoolean down)

void        
AWUserInput::mouseBtnEvent(int code, AWBoolean down)
{ //all joystick button events map to FIRE for now
   if (getUseMouse()) m_keys[AW_KEY_SPACE] = down;
}//void AWUserInput::mouseBtnEvent(int code, AWBoolean down)

void
AWUserInput::mouseMoveEvent(WORD x, WORD y)
{  // newPos in (SCREEN COORDINATES)
   //fprintf(stderr, "User Input method %d", m_inputMethod);
   if (getUseMouse())
   {
      POINT newPos; newPos.x = x; newPos.y= y;
      AWTRACE2("\nMouse (%d, %d) ", newPos.x, newPos.y);
      if (PtInRect(&m_mainZone, newPos))
      {
         m_moveAccumulator.x += (newPos.x - m_lastPos.x);
         m_moveAccumulator.y += (newPos.y - m_lastPos.y);
         AWTRACE2(" in mainZone, delta (%d, %d)\n", m_moveAccumulator.x, m_moveAccumulator.y);
         if (abs(m_moveAccumulator.x) >= MOUSE_X_THRESHOLD)
         {  //movement detected horizontally
            m_keys[AW_KEY_LEFT] = m_moveAccumulator.x < 0;
            m_keys[AW_KEY_RIGHT]= !m_keys[AW_KEY_LEFT];
            m_moveAccumulator.x = 0;
         }//if (abs(x) >= MOUSE_X_THRESHOLD)
         if (abs(m_moveAccumulator.y) >= MOUSE_Y_THRESHOLD)
         {  //movement detected vertically
            m_keys[AW_KEY_UP]   = m_moveAccumulator.y > 0;
            m_keys[AW_KEY_DOWN] = !m_keys[AW_KEY_UP];
            m_moveAccumulator.y = 0;
         }//if (abs(y) >= MOUSE_Y_THRESHOLD)
      }//if (PtInRect(&m_mainZone, newPos))
      m_lastPos = newPos;
   //m_keys[AW_KEY_SPACE]  = GetAsyncKeyState(VK_LBUTTON) & 0x8000;  // fire
   }//if (getUseMouse())
}//void AWUserInput::mouseMoveEvent(WORD x, WORD y)


void        
AWUserInput::setCursorHome()
{
#ifdef _WIN32
   SetCursorPos(m_clientCentre.x, m_clientCentre.y);
#endif //#ifdef _WIN32
   m_lastPos.x = m_clientCentre.x; m_lastPos.y = m_clientCentre.y;
}//void AWUserInput::setCursorHome()




int
AWUserInput::determineFocus()
{
   POINT newPos;
#ifdef _WIN32
   //get the current cursor position
   GetCursorPos(&newPos);
   //first see if it's within the client window area 
   //we need to ignore it if it is not
   if (PtInRect(&m_clientArea, newPos))
   {  
      if (!m_gotFocus)
      {
         //switching to owning the focus - get rid of the cursor
         m_oldCursor = SetCursor(NULL);
         m_gotFocus=TRUE;
      }     
   }//if (PtInRect(&m_clientArea, newPos))
   else
   {
      //we are NOT in the desired area, so make sure focus is NOT set
      if (m_gotFocus)
      {
         m_gotFocus = FALSE;
         SetCursor(m_oldCursor);
      }
   }//else
#else
   newPos = m_lastPos;
#endif
   if (m_gotFocus && getUseMouse() && !PtInRect(&m_mainZone, newPos))
   { //we're in the power zone, baby...
         AWTRACE("\nmouse in powerZone \n");
         m_keys[AW_KEY_LEFT]  = (newPos.x < m_mainZone.left);
         m_keys[AW_KEY_RIGHT] = (newPos.x > m_mainZone.right);
         m_keys[AW_KEY_DOWN]  = (newPos.y < m_mainZone.top);
         m_keys[AW_KEY_UP]    = (newPos.y > m_mainZone.bottom);
         m_moveAccumulator.x  = m_moveAccumulator.y = 0;
   }//if (getUseMouse() && !PtInRect(&m_mainZone, newPos))
   return 0;
}//int AWUserInput::determineFocus()



int
AWUserInput::getKeyData(float t)
{   //fill the m_keysArray
#ifdef _WIN32
   if (getUseKeyboard())
   {
      if (m_keys[AW_KEY_LEFT]    = GetAsyncKeyState(37) & 0x8000) 
         m_keyPressedAt[AW_KEY_LEFT]  = t;

      if (m_keys[AW_KEY_RIGHT]   = GetAsyncKeyState(39) & 0x8000) 
         m_keyPressedAt[AW_KEY_RIGHT] = t;

      if (m_keys[AW_KEY_UP]      = GetAsyncKeyState(40) & 0x8000) 
         m_keyPressedAt[AW_KEY_UP]    = t;

      if (m_keys[AW_KEY_DOWN]    = GetAsyncKeyState(38) & 0x8000) 
         m_keyPressedAt[AW_KEY_DOWN]  = t;

      m_keys[AW_KEY_SPACE]   = (GetAsyncKeyState(32) & 0x8000) ||
                                 (GetAsyncKeyState(VK_CONTROL) & 0x8000);  // fire
      if (m_keys[AW_KEY_SPACE]) m_keyPressedAt[AW_KEY_SPACE] = t;

      if (m_keys[AW_KEY_A]       = GetAsyncKeyState('A') & 0x8000) 
         m_keyPressedAt[AW_KEY_A]     = t;

      if (m_keys[AW_KEY_Z]       = GetAsyncKeyState('Z') & 0x8000) 
         m_keyPressedAt[AW_KEY_Z]     = t;

      float tmpVal;
      if (GetAsyncKeyState(AW_KEY_PAGEUP)&0x8000)
      {
         tmpVal = t-m_keyPressedAt[AW_KEY_PAGEUP];
         if ((tmpVal<0)||(tmpVal>m_delay))
         {
            m_keys[AW_KEY_PAGEUP]         = TRUE;
            m_keyPressedAt[AW_KEY_PAGEUP] = t;
         }//if ((tmpVal<0)||(tmpVal>m_delay))
         else
         {
            AWTRACE1("PageUp rejected as only %f seconds delay", (double)tmpVal);
         }
      }//if (GetAsyncKeyState(AW_KEY_PAGEUP)&0x8000)

      if (GetAsyncKeyState(AW_KEY_PAGEDOWN)&0x8000) 
      {
         tmpVal = t-m_keyPressedAt[AW_KEY_PAGEDOWN];
         if ((tmpVal<0)||(tmpVal>m_delay))
         {
            m_keys[AW_KEY_PAGEDOWN]         = TRUE;
            m_keyPressedAt[AW_KEY_PAGEDOWN] = t;
         }//if ((tmpVal<0)||(tmpVal>m_delay))
         else
         {
            AWTRACE1("PAGEDOWN rejected as only %f seconds delay", (double)tmpVal);
         }
      }//if (GetAsyncKeyState(AW_KEY_PAGEDOWN)&0x8000) 

      if (GetAsyncKeyState(AW_KEY_HOME)&0x8000) 
      {
         tmpVal = t-m_keyPressedAt[AW_KEY_HOME];
         if ((tmpVal<0)||(tmpVal>m_delay)) 
         {
            m_keys[AW_KEY_HOME]         = TRUE;
            m_keyPressedAt[AW_KEY_HOME] = t;
         }//if ((tmpVal<0)||(tmpVal>m_delay))
         else
         {
            AWTRACE1("HOME rejected as only %f seconds delay", (double)tmpVal);
         }
      }//if (GetAsyncKeyState(AW_KEY_HOME)&0x8000) 

      if (GetAsyncKeyState(AW_KEY_END)&0x8000) 
      {
         tmpVal = t-m_keyPressedAt[AW_KEY_END];
         if ((tmpVal<0)||(tmpVal>m_delay))
         {
            m_keys[AW_KEY_END]         = TRUE;
            m_keyPressedAt[AW_KEY_END] = t;
         }//if ((tmpVal<0)||(tmpVal>m_delay))
         else
         {
            AWTRACE1("END rejected as only %f seconds delay", (double)tmpVal);
         }
      }//if (GetAsyncKeyState(AW_KEY_END)&0x8000) 

      if (GetAsyncKeyState(AW_PRNT_SCRN)&0x8000) 
      {
         tmpVal = t-m_keyPressedAt[AW_PRNT_SCRN];
         if ((tmpVal<0)||(tmpVal>m_delay))
         {
            m_keys[AW_PRNT_SCRN]         = TRUE;
            m_keyPressedAt[AW_PRNT_SCRN] = t;
         }//if ((tmpVal<0)||(tmpVal>m_delay))
         else
         {
            AWTRACE1("PRNT_SCRN rejected as only %f seconds delay", (double)tmpVal);
         }
      }//if (GetAsyncKeyState(AW_PRNT_SCRN)&0x8000)
   }//if (getUseKeyboard())
#endif
   return 0;
}//int AWUserInput::getKeyData(float t)
