#include "AlteredExport.h"
#include "AWFlockHelp.h"
#include "AWFogHelp.h"
#include <AW3DTools/AWFlock.h>
#include <AW3DTools/AWFog.h>


//Helper object output

AWNode* 
TnTExprt::ExportHelperObject(INode* node, AWNode* parent)
{	//we support 3 types of helper objects ONLY
	AWNode* retVal = NULL;
	Object* helperObj = node->EvalWorldState(0).obj;
	//PROCESS AWFLOCK OBJECTS
	if (helperObj && (helperObj->ClassID() == AWFLOCKHELP_CLASS_ID)) 
	{
		TSTR className;
		helperObj->GetClassName(className);
		AWFlockHelpObject* flockHelper = (AWFlockHelpObject*)helperObj;	
		//extract the node transformation
		AWFlock* flock = new AWFlock(node->GetName(), parent);
		assignTransformController(node, flock);
		flock->setNumBoids( flockHelper->numBoids );
		INode* boid = flockHelper->boidNode;
		if (boid)
		{
         AWCString boidName(flock->getName());
         boidName += "*";
         boidName += boid->GetName();
			flock->setName(boidName);
		}
        flock->setEnabled(!flockHelper->startDisabled);
		flock->setOwnsSrce( !flockHelper->flockCopyObject);
		flock->setCollideWeight(flockHelper->collisionFraction);		
		flock->setInfluenceRadius(flockHelper->influenceRadius);	
		flock->setNormalSpeed(flockHelper->speed);
		flock->setMaxY(flockHelper->maxY);
		flock->setMinY(flockHelper->minY);
        flock->setTimeout(flockHelper->levelTime);
		
		TimeValue	t = GetStaticFrame();
		Matrix3		oTM = node->GetObjectTM(t);
		Box3		bbox;

		helperObj->GetDeformBBox(t, bbox, &oTM);

		if (GetIncludeAnim()) 
		{
			ExportAnimKeys(node, flock);
		}
		retVal = flock;
	}//if (helperObj) 
	else if (helperObj && (helperObj->ClassID() == AWFOGHELP_CLASS_ID))
	{
		AWFog* curFog = new AWFog(node->GetName());
		AWFogHelpObject* fogHelper = (AWFogHelpObject*)helperObj;
		curFog->m_type		= fogHelper->fogType;
		curFog->m_colour	= (1.0f/255.0f) * AWPoint((int) GetRValue(fogHelper->fogColour),
									  (int) GetGValue(fogHelper->fogColour),
									  (int) GetBValue(fogHelper->fogColour));
		curFog->m_startDist = fogHelper->fogStart;
		curFog->m_endDist	= fogHelper->fogEnd;
		theScene.m_fogList.add(curFog);
		retVal = NULL; //this is not an AWNode derivative, doesn't participate in hierachy
	}
    else if (helperObj && node->IsGroupHead() && !node->IsGroupMember() &&
                                strstr(node->GetName(), _T("levelEnd")))
	{   //
        AWNode* levelEnd = new AWNode(node->GetName());
        assignTransformController(node, levelEnd);
		retVal = levelEnd;
	}
   else /*if (Class_ID(DUMMY_CLASS_ID, 0) == helperObj->ClassID())*/
   {
      retVal = ExportDummyNode(node);
      /*
      AWNode* dummy = new AWNode(node->GetName());
      assignTransformController(node, dummy);
      if (GetIncludeAnim()) 
      {
         ExportAnimKeys(node, dummy);
      }
      retVal = dummy;*/
   }//else if (Class_ID(DUMMY_CLASS_ID, 0) == helperObj->ClassID())
   return retVal;
}//AWNode* TnTExprt::ExportHelperObject(INode* node, AWNode* parent)



AWNode*
TnTExprt::ExportDummyNode(INode* node)
{
   AWNode* dummy = NULL;
   if (node)
   {
      dummy = new AWNode(node->GetName());
      assignTransformController(node, dummy);
      if (GetIncludeAnim()) 
      {
         ExportAnimKeys(node, dummy);
      }//if (GetIncludeAnim()) 
   }//if (node)
   return dummy;
}//AWNode* TnTExprt::ExportDummyNode(INode* node)
