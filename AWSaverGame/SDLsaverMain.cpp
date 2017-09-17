#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <iostream.h>
#include <stdlib.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glx.h>

//#include <rw/bstream.h>
//#include <rw/pstream.h>

#include "AWSpaceGnats.h"
#include "AWUserInput.h"
#include "AWSndProviderLin.h"
#include "AWConfigLin.h"

#include "SDL.h"
#include "SDL_syswm.h"
#include <sys/wait.h>

//#define ALTERED_APPNAME "AlteredSaver"

static long useCmdLineWin = 0L;
static int done = 0;

static AWBoolean wasGame = false;
static AWBoolean usingExternalSound = false;
static AWBoolean useRootWindow = false;
static AWBoolean redrawRequired = false;
static AWBoolean useAlphaTest = true;
static int howManyTimes    = 0;
static int useInput = 0; //keyboard by default
static WORD posX = 0;
static WORD posY = 0;

int   gunSoundID       = 0;
int   failLevelSoundID = 0;
int   doneLevelSoundID = 0;
int   explodeSoundID   = 0;

SDL_CD*      cdrom  = NULL;
SDL_Surface* screen = NULL;
SDL_Joystick* joystick = NULL;


typedef void (*termHandler_t)(int);


AWConfigLin          cfg(AWConfig::awSaver);
AWSpaceGnats         g(cfg);
AWSoundProviderLin   snd;
termHandler_t        oldTermHandler = NULL;

static char* nameStr =  "AlteredSaver v2.3 for Linux";

//FUNKY SHIT TO FIND XSCREENSAVER WINDOW (YUK)
#include <X11/Xatom.h>
static Atom XA_SCREENSAVER_VERSION;
static XErrorHandler old_handler = 0;
static Bool got_badwindow = False;

static int
BadWindow_ehandler (Display *dpy, XErrorEvent *error)
{
  if (error->error_code == BadWindow)
    {
      got_badwindow = True;
      return 0;
    }
  else
    {
      fprintf (stderr, "%s: ", "AlteredSaver");
      if (!old_handler) abort();
      return (*old_handler) (dpy, error);
    }
}

//support for xscreensaver
static Window
find_xscreensaver_window (Display *dpy, char **version)
{
  int i;
  Window root = RootWindowOfScreen (DefaultScreenOfDisplay (dpy));
  Window root2, parent, *kids;
  unsigned int nkids;

  if (version) *version = 0;

  if (! XQueryTree (dpy, root, &root2, &parent, &kids, &nkids))
    abort ();
  if (root != root2)
    abort ();
  if (parent)
    abort ();
  if (! (kids && nkids))
    abort ();
  for (i = 0; i < nkids; i++)
    {
      Atom type;
      int format;
      unsigned long nitems, bytesafter;
      char *v;
      int status;

      /* We're walking the list of root-level windows and trying to find
         the one that has a particular property on it.  We need to trap
         BadWindows errors while doing this, because it's possible that
         some random window might get deleted in the meantime.  (That
         window won't have been the one we're looking for.)
       */
      XSync (dpy, False);
      if (old_handler) abort();
      got_badwindow = False;
      old_handler = XSetErrorHandler (BadWindow_ehandler);
      status = XGetWindowProperty (dpy, kids[i],
                                   XA_SCREENSAVER_VERSION,
                                   0, 200, False, XA_STRING,
                                   &type, &format, &nitems, &bytesafter,
                                   (unsigned char **) &v);
      XSync (dpy, False);
      XSetErrorHandler (old_handler);
      old_handler = 0;

      if (got_badwindow)
        {
          status = BadWindow;
          got_badwindow = False;
        }

      if (status == Success && type != None)
	{
	  if (version)
	    *version = v;
	  return kids[i];
	}
    }
  return 0;
}

/*
void releaseInput()
{
   if (cfg.m_fullScreen && useCmdLineWin)
   {
      SDL_SysWMinfo info;
      SDL_VERSION(&info.version);
      if ( SDL_GetWMInfo(&info) > 0 )
      {
         if ( info.subsystem == SDL_SYSWM_X11 )
         {
            //info.info.x11.lock_func();
            XUngrabKeyboard( info.info.x11.display, CurrentTime );
	         XUngrabPointer( info.info.x11.display, CurrentTime );
         }
      }
   }
}
*/

void shutdownSound()
{
   if (cfg.getPlayCD() && cdrom)
   {  //start the CD playing, if found. Drive info stored in config
      //we need to have already called SDL_Init(SDL_INIT_CDROM)
      //for now, use the default CDR until GUI supports selection
      //play the CD from start
      SDL_CDStop(cdrom);
      SDL_CDClose(cdrom);
      cdrom = NULL;
   }//if (cfg.getPlayCD())
   if (usingExternalSound)
   {
   	cfg.playSoundFileStop();
   }
   else snd.shutDown();
}//void shutdownSound()

void awExit()
{
   //SDL_ShowCursor(TRUE); //not necessary - causes bug
   //releaseInput();
   if (joystick)
   {
      SDL_JoystickClose(joystick);
      joystick = NULL;
   }
   cfg.releaseLock();
   shutdownSound();
   AWTRACE("awExit called\n");
   if (oldTermHandler)
   {
      AWTRACE(" reinstall old SIGTERM handler\n");
      signal(SIGTERM, oldTermHandler);
      oldTermHandler = NULL;
   }
   AWTRACE("calling scene::shutdown()\n");
   g.m_scene.shutdown();

   AWTRACE("calling SDL_Quit()\n");
   SDL_Quit();

   exit(0);
}//void awExit()


void
terminate_handler(int val)
{
   AWTRACE("terminate_handler called\n");
   termHandler_t needToCallBaseTermHandler = oldTermHandler;
   awExit();
   if (needToCallBaseTermHandler)
   {
      AWTRACE("calling old term handler\n");
      //(*needToCallBaseTermHandler)(1);
   }
   // _exit(0);
   done = 1;
}

void catch_child(int sig_num)
{
   //when we get here, we know there's a zombie child waiting
   fprintf(stderr, "altered catch_child\n");
   int child_status;
   wait(&child_status);
   fprintf(stderr, "altered child quit\n");
}


static void drawFrame( )
{
	g.display();
	int k = glGetError();
	SDL_GL_SwapBuffers();
   if (g.getFail())
   {
       //snd.pause(gameLoopSoundID);
       snd.stop(gunSoundID);
       snd.play(failLevelSoundID);
   }//if (game.m_scene.getLevelFailed())
   else if (g.getWin())
   {
       //snd.pause(gameLoopSndID);
       snd.stop(gunSoundID);
       snd.play(doneLevelSoundID);
   }//else if (game.m_scene.getSwitchLevel())
   else if (g.justFired() && g.getUserActive())
   {
       snd.play(gunSoundID);
   }
   if (g.timeToExplode())
   {
      //snd.setDist(explodeSndID,  g.m_scene.getExplodeDist());
      snd.play(explodeSoundID);
   }//if (g.timeToExplode())
   g.resetEvents();
}//static void drawFrame()


/* new window size or exposure */
static void resize( int width, int height )
{
   if (width && height && ((cfg.m_height!=height) || (cfg.m_width!=width)))
   {
      AWTRACE4("\nsaver resize old (%i, %i) new (%i, %i)\n", cfg.m_width, cfg.m_height, width, height);
      RECT r;
      r.left = r.top = 0;
      r.right = width; r.bottom = height;
      g.m_userInput.setClientArea(r);
      cfg.m_width = width;
      cfg.m_height = height;
      g.m_scene.resizeGL(width, height);
      drawFrame();
   }
} //static void resize( int width, int height )


long find_root_window()
{
   long root    = 0;
   Display* dpy = XOpenDisplay(NULL);
   if (!dpy) return root;
   //look for funky xscreensaver window
   XA_SCREENSAVER_VERSION = XInternAtom (dpy, "_SCREENSAVER_VERSION",False);
   root = find_xscreensaver_window(dpy, NULL);
   if (!root)
   {  //get the root the usual way
      int     scrnum;
      Screen* crazy;
      scrnum = DefaultScreen( dpy );
      crazy  = ScreenOfDisplay( dpy, scrnum);
      root   = RootWindowOfScreen( crazy );
   }//if (!root)

   XCloseDisplay(dpy);
   return root;
}//long find_root_window()


static Window make_rgb_db_window( unsigned int width, unsigned int height )
{
   //check for unsuppported video modes -
   const SDL_VideoInfo* info = SDL_GetVideoInfo();
   int currentBpp = info->vfmt->BitsPerPixel;
   AWTRACE4("requested mode: %dx%d %dbit fullScreen: %d", cfg.m_width, cfg.m_height, cfg.m_bpp, (int)cfg.m_fullScreen);
   AWTRACE1(" current bpp: %d\n", currentBpp);
   if ( ((8 == cfg.m_bpp)||(8 == currentBpp)) &&
        !( cfg.m_fullScreen && ((32 == cfg.m_bpp)||(16 == cfg.m_bpp)) ) )
   {
     fprintf(stderr, "8 bit (256) Color mode not supported - try 16 or 32 bit\n");
     SDL_Quit();
     _exit(2);
   }//if (8 == currentBpp)
   Uint32 mode = SDL_OPENGL;
   mode = mode | (cfg.m_fullScreen ? SDL_FULLSCREEN : SDL_RESIZABLE);
   screen = SDL_SetVideoMode(cfg.m_width, cfg.m_height, cfg.m_bpp, mode);
   if ( ! screen )
   {
     fprintf(stderr, "Couldn't set GL video mode: %s\n", SDL_GetError());
     SDL_Quit();
     _exit(2);
   }
   RECT r;
   r.top =0; r.left = 0;
   r.right = screen->w; r.bottom = screen->h;
   g.m_userInput.setClientArea(r);

   char* vendorName = (char*)glGetString(GL_RENDERER);
//ALPHA TEST HACK necessary only for Rage Pro - set once on Window creation
//causes alpha test to be disabled if the scene contains fog
   g.m_scene.setUseRageProHack(vendorName && strstr(vendorName, "RAGE PRO"));
   g.m_scene.initialize(cfg.m_width, cfg.m_height);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   g.m_userInput.start();
   oldTermHandler = signal(SIGTERM, terminate_handler);
}//static Window* make_rgb_db_window( Display ...)



static int processKeys(SDLKey& sym, AWBoolean down)
{
   int localDone = 0;
   AWTRACEHI3("processKeys called with sym %d  down %d [UP is %d]\n", (int)sym, (int)down, (int)SDLK_UP);
   switch (sym)
   {
   case SDLK_ESCAPE:
      localDone = 1;
      break;
   case SDLK_v:  //DEBUGGING CODE FOR NVIDIA TEST VERSION
      //state is initially set to
      //YES, TRANSFORM REFLECTION TEXTURE MATRIX so as to
      //rotate eye-space reflection vectors generated by
      //GL_REFLECTION_MAP_EXT  back into world space
      if (down) g.m_scene.toggleTransformReflections();
      break;
  case SDLK_n:
      if (down) g.m_scene.toggleDrawNormals();
      break;
   case SDLK_UP:
      g.keyEvent(AW_KEY_DOWN, down);
      break;
   case SDLK_DOWN:
      g.keyEvent(AW_KEY_UP, down);
      break;
   case SDLK_LEFT:
      g.keyEvent(AW_KEY_LEFT, down);
      break;
   case SDLK_RIGHT:
      g.keyEvent(AW_KEY_RIGHT, down);
      break;
   case SDLK_SPACE:
   case SDLK_LCTRL:
   case SDLK_RCTRL:
      g.keyEvent(AW_KEY_SPACE, down);
      break;
	case SDLK_F1://f1 - back thru cam list
		if (down) g.backCamera();
      break;
	case SDLK_F2://f2 - forward thru cam list
      if (down) g.forwardCamera();
      break;
   case SDLK_F3://f3 - screen capture (single frame)
	   if (down) g.captureFrame(cfg.m_snapshots);
	   break;
   case SDLK_F4://f4 - screen capture (multi frame)
      if (down) g.toggleMultiFrameCapture(cfg.m_snapshots);
	   break;
   case SDLK_F5://F5
	   if (down) g.m_scene.setShowStatistics(!g.m_scene.getShowStatistics());
      break;
   default:
      localDone = wasGame ? 0 : 1;
   break;
   }//switch (sym)
   return localDone;
}//static int processKeys(SDLKey& sym, AWBoolean down)



static void handleActivation(SDL_ActiveEvent& e)
{
   if ((e.state & SDL_APPACTIVE) || (e.state & SDL_APPINPUTFOCUS))
   {
      if (e.gain)
      {
         //if (!useRootWindow && !useCmdLineWin) XAutoRepeatOn(awDpy);
         if (!usingExternalSound)
   		{
   			snd.resume();
   		}
	      g.m_userInput.setFocus();
		   if (g.m_scene.getPaused())
         {
            g.m_scene.togglePause();
         }
      } //if (e.gain)
      else
      {
         ////if (!useRootWindow && !useCmdLineWin) XAutoRepeatOff(awDpy);
         if (!usingExternalSound)
   		{
   			snd.pause();
   		}
         g.m_userInput.killFocus();
         if (!g.m_scene.getPaused())
         {
            g.m_scene.togglePause();
         }
      }
   }//if ((e.state & SDL_APPACTIVE) || (e.state & SDL_APPINPUTFOCUS))
   else if ((e.state & SDL_APPMOUSEFOCUS) && !e.gain)
   {  //when mouse leaves window, stop motion
      g.m_userInput.setCursorHome(); //centers in client
   }
}//static void handleActivation(SDL_ActiveEvent& e)



static void event_loop()
{
   while (!done)
   {
      SDL_Event event;
      while ( SDL_PollEvent(&event) )
      {
         switch(event.type)
         {
         case SDL_ACTIVEEVENT:
            handleActivation(event.active);
            break;

         case SDL_KEYDOWN:
         case SDL_KEYUP:
             //grab the sdl keysym & process
             done = processKeys(event.key.keysym.sym, event.type == SDL_KEYDOWN);
             break;

         case SDL_MOUSEBUTTONDOWN:
         case SDL_MOUSEBUTTONUP:
            if (wasGame)
            {
               g.m_userInput.mouseBtnEvent(0, SDL_MOUSEBUTTONDOWN == event.type);
            }
            else done = 1; //for saver, mouse move means BEGONE!
            break;

         case SDL_MOUSEMOTION: //mouse movement
            if (howManyTimes)
            {
               if (wasGame)
               {
                  g.m_userInput.mouseMoveEvent(event.motion.x, event.motion.y);
               }
               else
               {
               	if (howManyTimes++>1)
               	{
               		done = 1; //for saver, (not first 2) mouse move means BEGONE!
               	}
               }
            }
            else
            {
               g.m_userInput.setCursorHome(); //centers in client
               howManyTimes = 1;
            }
            break;


         case SDL_JOYHATMOTION:		/* Joystick hat position change */
         	break;
         case SDL_JOYAXISMOTION:		/* Joystick axis motion */
            //if (0 == event.jaxis.value) break;
            if (wasGame)
            {
            	switch (event.jaxis.axis)
            	{
            	case 0:
            	   //fprintf(stderr, "SDL_JOYAXISMOTION in X %d\n", (int)event.jaxis.value);
            	   g.m_userInput.joyMoveXEvent(event.jaxis.value);
            		break;
            	case 1:
            		//fprintf(stderr, "SDL_JOYAXISMOTION in Y %d\n", (int)event.jaxis.value);
            	   g.m_userInput.joyMoveYEvent(event.jaxis.value);
            	   break;
            	default:
            	   break;
            	}//switch (event.jaxis.axis)
            }//if (wasGame)
            else done = 1;
            break;

         case SDL_JOYBALLMOTION:		/* Joystick trackball motion */
            break;

         case SDL_JOYBUTTONDOWN:		/* Joystick button pressed */
         case SDL_JOYBUTTONUP:
         	if (wasGame)
         	{
            	g.m_userInput.joyBtnEvent( 0, SDL_JOYBUTTONDOWN == event.type);
            }
            else done = 1;
            break;

         case SDL_VIDEORESIZE:
             //following line is SDL bug workaround - get spurious 32x32 resize
             if (event.resize.w == event.resize.h && event.resize.h == 32) break;
             //following line causes problems for utah-glx / Rage Pro
             screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 16,
                                      SDL_OPENGL|SDL_RESIZABLE);
             if ( screen )
             {
               //resize(event.resize.w, event.resize.h);
               resize(screen->w, screen->h);
             }
             break;

         case SDL_QUIT:
             done = 1;
             break;
         }//switch(event.type)
      }//while ( SDL_PollEvent(&event) )
      drawFrame();
   } //while (!done)
}//static void event_loop()



int checkParam(char* str, int posn, char* argv[], int argc)
{
	return ( (posn+1 < argc) && (0 == strcmp(argv[posn], str)) );
}//int checkParam(char* str, int posn, char* argv[], int argc)


int find_SDL_CDNum(const AWCString& cdname)
{
   int retVal = 0;
   for (int i=0; i < SDL_CDNumDrives(); i++ )
   {
      if (cdname == AWCString(SDL_CDName(i)))
      {
         retVal = i;
         break;
      }
   }//for (int i=0; i < numDrives; i++ )
   return retVal;
}//int find_SDL_CDNum(const AWCString& cdname)


void initSound()
{
   //first take care of any CD Audio
   if (cfg.getPlayCD())
   {  //start the CD playing, if found. Drive info stored in config
      //we need to have already called SDL_Init(SDL_INIT_CDROM)
      cdrom = SDL_CDOpen(find_SDL_CDNum(cfg.m_cdrom));
      if (!cdrom) {
         fprintf(stderr, "Couldn't open CD drive %s\n", SDL_GetError());
         return;
      }
      //play the CD from start
      if ( CD_INDRIVE(SDL_CDStatus(cdrom)) )
      {
         SDL_CDPlayTracks(cdrom, 0, 0, 0, 0);
      }
      else
      {
         fprintf(stderr, "Please insert an Audio CD to play!\n");
      }
   }//if (cfg.getPlayCD())
   //if we don't need the mixer, return now.
   if (cfg.getNoMusic() && !cfg.getPlayEffects()) return;
   //we need sound mixer support

   if (cfg.getPlayFile())
   {
   	AWCString helper;
   	cfg.getHelperForFile(cfg.m_waveFile, helper);
   	if (helper.contains("internal"))  //use SDL mp3/wav output
   	{
   		snd.init();
   		snd.loadMusic(cfg.m_waveFile);
         snd.playMusic(-1);
   	}
   	else
   	{
   		usingExternalSound = true;
   		cfg.playSoundFileStart();
   	}
   }
   else if (cfg.getPlayGameMusic())
   {
   	snd.init();
        AWCString musicPath(cfg.getHome());
        musicPath += "sounds/gnats.wav";
      snd.loadMusic(musicPath);
      snd.playMusic(-1);
   }

   if (!usingExternalSound && cfg.getPlayEffects())
   {
      snd.init(); //safe to call again / may have been called above
      AWCString musicPath(cfg.getHome());
      musicPath += "sounds/Bgun.wav";
      gunSoundID       = snd.loadWave(musicPath);
      musicPath = cfg.getHome();
      musicPath += "sounds/fail.wav" ;
      failLevelSoundID = snd.loadWave(musicPath);
      musicPath = cfg.getHome();
      musicPath += "sounds/winner.wav";
      doneLevelSoundID = snd.loadWave(musicPath);
      musicPath = cfg.getHome();
      musicPath += "sounds/explode.wav";
      explodeSoundID   = snd.loadWave(musicPath);
   }
}//void initSound()


AWBoolean grabInput()
{
   if (useCmdLineWin && cfg.m_fullScreen)
   {
      XSetWindowAttributes attr;
      XWindowAttributes winattr;

	   SDL_SysWMinfo info;
      int retval = 0;
      SDL_VERSION(&info.version);
      if ( SDL_GetWMInfo(&info) > 0 )
      {
        if ( info.subsystem == SDL_SYSWM_X11 )
        {
            //info.info.x11.lock_func();
            XGetWindowAttributes(info.info.x11.display, useCmdLineWin, &winattr);
            attr.event_mask = winattr.your_event_mask | KeyPressMask | KeyReleaseMask;
            XChangeWindowAttributes(info.info.x11.display, useCmdLineWin, CWEventMask, &attr);
            int rv = XGrabKeyboard( info.info.x11.display, useCmdLineWin,
                True, GrabModeAsync, GrabModeAsync, CurrentTime );
            if (rv == AlreadyGrabbed)
            {
               return FALSE;
            }/*
	         rv = XGrabPointer( info.info.x11.display, useCmdLineWin, True,
			         ButtonPressMask
			         | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask
			         | PointerMotionMask | PointerMotionHintMask | Button1MotionMask
			         | Button2MotionMask | Button3MotionMask | Button4MotionMask
			         | Button5MotionMask | ButtonMotionMask | KeymapStateMask,
			         GrabModeAsync, GrabModeAsync, None, w->cursor().handle(),
			         CurrentTime );

            if (rv == AlreadyGrabbed)
            {
               return FALSE;
            }*/
            return TRUE;
            //info.info.x11.unlock_func();
        }//if ( info.subsystem == SDL_SYSWM_X11 )
      }//if ( SDL_GetWMInfo(&info) > 0 )
   }//if (useCmdLineWin && cfg.m_fullScreen)
   if (g.getUseJoystick() && SDL_NumJoysticks())
   {
      joystick = SDL_JoystickOpen(0);
      if (!joystick) g.setUseKeyboard();
   }
}//AWBoolean grabInput


int main( int argc, char *argv[] )
{
   AWBoolean preview = FALSE;
   if (!cfg.getLock()) _exit(0);
   g.loadConfig();
   g.m_scene.setUseDisplayLists(cfg.m_useDisplayLists);
   g.m_scene.setUseVertArrays(cfg.m_useVertexArrays);
	//extract command line arguments, if any
   for (int i = 0; i < argc; i++)
   {
        if (checkParam("-scene", i, argv, argc))
        {
            cfg.m_scene = AWCString(argv[i+1]);
            AWTRACE1("cmd line scene: %s\n", cfg.m_scene.c_str());	
        }
        if (checkParam("-switchTime", i, argv, argc))
        { //scene switch time is supplied in minutes - we need seconds
            cfg.m_sceneSwitchMinutes = atoi(argv[i+1]);
            cfg.m_autoSwitchScenes   = TRUE;
        }
        if (checkParam("-width", i, argv, argc))
        {
	        cfg.m_width = atoi(argv[i+1]);
        }
        if (checkParam("-height", i, argv, argc))
        {
	        cfg.m_height = atoi(argv[i+1]);
        }
        if (checkParam("-bpp", i, argv, argc))
        {
	        cfg.m_bpp = atoi(argv[i+1]);
        }
        if (checkParam("-fullScreen", i, argv, argc))
        {
	        cfg.m_fullScreen = atoi(argv[i+1]) ? 1 : 0;
        }
        if (checkParam("-win", i, argv, argc))
        {
           useCmdLineWin = strtol(argv[i+1], NULL, 10);
        }
        if (0 == strcmp(argv[i], "-time"))
        {//output timing information
            g.m_outputTimingInfo=TRUE;
        }
        if (0 == strcmp(argv[i], "-noalphatest"))
        {
           g.m_scene.setUseAlphaTest(false);
        }
        if (0 == strcmp(argv[i], "-root"))
        {
           useRootWindow = true;
        }
        if (0 == strcmp(argv[i], "-game")) 
        {
           cfg.setIsSaver(AWConfig::awGame);
        }
        if (0 == strcmp(argv[i], "-mouse"))
        {
           useInput = 1; //mouse
        }
        if (0 == strcmp(argv[i], "-keys"))
        {
          useInput = 2;
        }
        if (0 == strcmp(argv[i], "-joystick"))
        {
          useInput = 3;
        }
        if ( (0 == strcmp(argv[i], "-preview")) || (0 == strcmp(argv[i], "-nosounds")) )
        {
           preview = TRUE;
        }
        if (checkParam("-level", i, argv, argc))
        {
           cfg.m_useLevel = true;
           int level = atoi(argv[i+1]);
           if (level > 0)
           {
              cfg.m_level = level;
           }
        }//if (checkParam("-level", i, argv, argc))
    }//for (int i = 0; i < argc; i++)

   cfg.buildSceneList();
   if (!g.loadScene(cfg.m_scene))
   {
      AWTRACE("EXIT - No valid scene could be loaded\n");
      return 0;
   } 	
//#ifdef _AW_SPACEGNATS_DEMO
   g.showRegDisplay(false);
//#endif //_AW_SPACEGNATS_DEMO
   if (useRootWindow)
   {
      useCmdLineWin = find_root_window();
      AWTRACE1("Root Window is %ld\n", useCmdLineWin);
   }
   if (useCmdLineWin)
   {
      char buf[100];
      sprintf(buf, "SDL_WINDOWID=%ld\n", useCmdLineWin);
      putenv(buf);
      AWTRACE(buf);
   }
   if (preview)
   {
      cfg.setNoMusic();
      cfg.setPlayEffects(FALSE);
   };
   if (cfg.m_isSaver) useInput = 2; //saver always uses key mode
   switch (useInput)
   {
   case 1:
      g.setUseMouse();
      break;
   case 2:
      g.setUseKeyboard();
      break;
   case 3:
      g.setUseJoystick();
      break;
   default: break;
   }//switch (useInput)
   Uint32 SDLInitFlags = SDL_INIT_VIDEO;
   if (cfg.getPlayCD())
   {  //play cdrom - drive name indicated in the config file
      SDLInitFlags |= SDL_INIT_CDROM;
   }
   if (!cfg.getNoMusic() || cfg.getPlayEffects())
   {  //move to using SDL for audio mixing
      //don't use iff no audio playback apart from Audio CD
      SDLInitFlags |= SDL_INIT_AUDIO;
   }
   if (g.getUseJoystick())
   {
      SDLInitFlags |= SDL_INIT_JOYSTICK;
   }
   SDL_Init(SDLInitFlags);
   initSound();
   SDL_WM_SetCaption(nameStr, nameStr);
   if (!useCmdLineWin) SDL_ShowCursor(FALSE);
   make_rgb_db_window(cfg.m_width, cfg.m_height);
   //set cursor position to middle of new window
   //NOTE - adds an extra, spurious MOUSEMOVE for saver to ignore
   if (!preview)
      SDL_WarpMouse(cfg.m_width/2, cfg.m_height/2);
   grabInput();
   AWTRACE1("AlteredSaver being launched as SAVER (1) or GAME (0): %d\n", (int)cfg.m_isSaver);

   //signal(SIGCHLD, catch_child);
   wasGame = cfg.m_isSaver == 0;
   //fprintf(stderr, "wasGame %d  g.m_isSaver %d\n", (int)wasGame, (int)g.m_isSaver);
   event_loop();
   awExit();
   return 0;
}//int main( int argc, char *argv[] )





