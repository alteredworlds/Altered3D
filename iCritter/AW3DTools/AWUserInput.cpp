#include <stdio.h>
#include <math.h>
#include "AWUserInput.h"

//want to consider first KEYBOARD and MOUSE input
//
//require multiple simultaneous key presses and button presses to
//be detected.


#define MOUSE_X_THRESHOLD 4
#define MOUSE_Y_THRESHOLD 4

#define MAIN_ZONE_PERCENT 30

AWBoolean PtInRect(RECT* r, POINT p)
{
   AWTRACEHI2("Point (%d, %d) ", p.x, p.y);
   AWTRACEHI4("Rect TL(%d, %d) BR(%d, %d)\n", r->left, r->top, r->right, r->bottom);     
	return (p.x >= r->left) && (p.x <= r->right) && (p.y >= r->top) && (p.y<=r->bottom);  
}

AWUserInput::AWUserInput() : m_gotFocus(TRUE)                            
{
   setUseMouse();
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

void 
AWUserInput::setClientArea(const RECT& r)
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
      if (fabs(x) >= DEADBAND)
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
      if (fabs(y) >= DEADBAND)
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
      if (fabs(xCentered) >= DEADBAND)
      {
         m_keys[AW_KEY_LEFT]  = xCentered < 0;
         m_keys[AW_KEY_RIGHT] = !m_keys[AW_KEY_LEFT];
      }
      int yCentered = y - 32768;
      if (fabs(yCentered) >= DEADBAND)
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
         if (fabs(m_moveAccumulator.x) >= MOUSE_X_THRESHOLD)
         {  //movement detected horizontally
            m_keys[AW_KEY_LEFT] = m_moveAccumulator.x < 0;
            m_keys[AW_KEY_RIGHT]= !m_keys[AW_KEY_LEFT];
            m_moveAccumulator.x = 0;
         }//if (abs(x) >= MOUSE_X_THRESHOLD)
         if (fabs(m_moveAccumulator.y) >= MOUSE_Y_THRESHOLD)
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
   m_lastPos.x = m_clientCentre.x; m_lastPos.y = m_clientCentre.y;
}//void AWUserInput::setCursorHome()

int
AWUserInput::determineFocus()
{
   POINT newPos;
   newPos = m_lastPos;
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
   return 0;
}//int AWUserInput::getKeyData(float t)
