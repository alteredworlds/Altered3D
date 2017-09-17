#include "AWConfig.h"
#include <AW3DTools/AWScene.h>

AWCString
AWConfig::getScenePath(const AWCString& shortSceneName)
{
   AWCString retVal;
   for (int i=0; i<getNumScenes(); i++)
   {
      if (AWScene::getSceneName(getScene(i)) == shortSceneName)
      {
         retVal = getScene(i);
         break;
      }
   }
   return retVal;
}//inline AWCString AWConfig::getScenePath(const AWCString& shortSceneName)
