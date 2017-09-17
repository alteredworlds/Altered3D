//************************************************************************** 
//* Animout.cpp	- Ascii File Exporter
//* 
//* By Tom Gilbert
//*
//*
//* This module handles controller key output and controller sampling.
//*
//* Copyright (c) 1998, All Rights Reserved. 
//***************************************************************************

#include "AlteredExport.h"



AWBoolean EqualPoint3(const Point3& p1, const Point3& p2);
AWBoolean AWEqualPoint3(const AWPoint& p1, const AWPoint& p2);

// Not truly the correct way to compare floats of arbitary magnitude...
AWBoolean EqualPoint3(const Point3& p1, const Point3& p2)
{
	if (fabs(p1.x - p2.x) > ALMOST_ZERO)
		return FALSE;
	if (fabs(p1.y - p2.y) > ALMOST_ZERO)
		return FALSE;
	if (fabs(p1.z - p2.z) > ALMOST_ZERO)
		return FALSE;

	return TRUE;
}

// Not truly the correct way to compare floats of arbitary magnitude...
AWBoolean AWEqualPoint3(const AWPoint& p1, const AWPoint& p2)
{
	if (fabs(p1.x - p2.x) > ALMOST_ZERO)
		return FALSE;
	if (fabs(p1.y - p2.y) > ALMOST_ZERO)
		return FALSE;
	if (fabs(p1.z - p2.z) > ALMOST_ZERO)
		return FALSE;

	return TRUE;
}//AWBoolean EqualPoint3(const AWPoint& p1, const AWPoint& p2)

/****************************************************************************

  TM Animation output
  
****************************************************************************/

// Get hold of the transform controllers for the node... 
void TnTExprt::ExportAnimKeys(INode* node, AWNode* awNode,
									AWBoolean usesNegativeY) 
{
	AWBoolean bPosAnim;
	AWBoolean bRotAnim;
	AWBoolean bScaleAnim;
	AWBoolean bDoKeys = FALSE;
	AWBoolean noInitialTransform = FALSE;
	Matrix3 tm = node->GetObjTMAfterWSM(GetStaticFrame());

	if (!awNode->getController())
	{
		noInitialTransform = TRUE;
		awNode->setController(new AWTransformController);
	}
	

	// We can only export keys if all TM controllers are "known" to us.
	// The reason for that is that some controllers control more than what
	// they should. Consider a path position controller, if you turn on
	// follow and banking, this position controller will also control
	// rotation. If a node that had a path position controller also had a
	// TCB rotation controller, the TCB keys would not describe the whole
	// rotation of the node.
	// For that reason we will only export keys if all controllers
	// position, rotation and scale are linear, hybrid (bezier) or tcb.

	if (!GetAlwaysSample()) 
	{
		Control* pC = node->GetTMController()->GetPositionController();
		Control* rC = node->GetTMController()->GetRotationController();
		Control* sC = node->GetTMController()->GetScaleController();

		if (IsKnownController(pC) && IsKnownController(rC) && IsKnownController(sC)) {
			bDoKeys = TRUE;
		}
	}

	if (bDoKeys) 
	{
      int nPosKeys = node->GetTMController()->GetPositionController()->NumKeys();
      int nRotKeys = node->GetTMController()->GetRotationController()->NumKeys();
      int nScaleKeys = node->GetTMController()->GetScaleController()->NumKeys();
		// Only dump the track header if any of the controllers have keys
		if ( nPosKeys || nRotKeys || nScaleKeys ) 
      {

			DumpPosKeys(node->GetTMController()->GetPositionController(), 
				awNode->getController(), tm);

			DumpRotKeys(node, node->GetTMController()->GetRotationController(), 
				awNode->getController(), tm, usesNegativeY);

			DumpScaleKeys(node->GetTMController()->GetScaleController(), 
				awNode->getController(), tm);

			//fprintf(pStream,"%s\t}\n", indent.data());
		}
	}
	else if (CheckForAnimation(node, bPosAnim, bRotAnim, bScaleAnim)) 
   {
      if (bPosAnim) 
      {
			DumpPosSample(node, awNode->getController(), tm);
		}
		if (bRotAnim) 
		{
			 DumpRotSample(node, awNode->getController());
		}
		if (bScaleAnim) 
      {
			DumpScaleSample(node, awNode->getController(), tm);
		}
   }//else if (CheckForAnimation(node, bPosAnim, bRotAnim, bScaleAnim)) 
	if (awNode->getController() && awNode->getController()->isAnimated())
	{
		
	}
	else if (noInitialTransform)
	{
		delete awNode->getController();
		awNode->setController(NULL);
	}
   //for debugging purposes...
   if (awNode->getController())
   {
      //awNode->getController()->dumpKeys();
   }//if (awNode->getController())
}

// To really see if a node is animated we can step through the animation range
// and decompose the TM matrix for every frame and examine the components.
// This way we can identify position, rotation and scale animation separately.
// 
// Some controllers makes it problematic to examine the TMContollers instead of
// the actual TMMatrix. For example, a path controller is a position controller, 
// but if you turn on follow and banking, it will also affect the rotation component.
// If we want to, we can examine the position, rotation and scale controllers and
// if they all are Linear, Hybrid (bezier) or TCB, then we could export the actual keys.
// This is not at all difficult, but the importer has to know the exact interpolation
// algorithm in order to use it. The source code to the interpolation routines are available
// to ADN members.
// 
// For an example of how to export actual keys, look at DumpPoint3Keys() below.
// This method will check the actual controller to determine if the controller is known.
// If we know how to work this controller, its actual keys will be exported,
// otherwise the controller will be sampled using the user specified sampling frequency.

AWBoolean TnTExprt::CheckForAnimation(INode* node, AWBoolean& bPos, AWBoolean& bRot, AWBoolean& bScale)
{
	TimeValue start = ip->GetAnimRange().Start();
	TimeValue end = ip->GetAnimRange().End();
	TimeValue t;
	int delta = GetTicksPerFrame();
	Matrix3 tm;
	AffineParts ap;
	Point3 firstPos;
	float rotAngle, firstRotAngle;
	Point3 rotAxis;
	Point3 firstScaleFactor;

	bPos = bRot = bScale = FALSE;

	for (t=start; t<=end; t+=delta) {
		tm = node->GetNodeTM(t) * Inverse(node->GetParentTM(t));

		decomp_affine(tm, &ap);

		AngAxisFromQ(ap.q, &rotAngle, rotAxis);

		if (t != start) {
			if (!EqualPoint3(ap.t, firstPos)) {
				bPos = TRUE;
			}
			// We examine the rotation angle to see if the rotation component
			// has changed.
			// Although not entierly true, it should work.
			// It is rare that the rotation axis is animated without
			// the rotation angle being somewhat affected.
			if (fabs(rotAngle - firstRotAngle) > ALMOST_ZERO) {
				bRot = TRUE;
			}
			if (!EqualPoint3(ap.k, firstScaleFactor)) {
				bScale = TRUE;
			}			
		}
		else {
			firstPos = ap.t;
			firstRotAngle = rotAngle;
			firstScaleFactor = ap.k;
		}

		// No need to continue looping if all components are animated
		if (bPos && bRot && bScale)
			break;
	}

	return bPos || bRot || bScale;
}




void 
TnTExprt::DumpPosSample(INode* node, AWTransformController* controller, Matrix3& ttm) 
{
	int delta = GetTicksPerFrame() * GetKeyFrameStep();
	TimeValue start = ip->GetAnimRange().Start();
	TimeValue end   = ip->GetAnimRange().End();
	AWPoint posn, scale, prevPos;
	AWQuat  rotn;
   TimeValue t = start;
   while (t <=end)
   {
      getNodeTransform(node, t, posn, scale, rotn);
		if ((t!= start) && (t!= end) && AWEqualPoint3(posn, prevPos)) 
		{	// Skip identical keys		 
			;
		}
      else
      {
		   prevPos = posn;
		   controller->addTranslation(posn, TicksToSec(t)/* / GetTicksPerFrame()*/);
      }
      t+=delta; //move to next sample frame
      if ((t > end) && (t - end < delta))
      {  //make sure we take a sample frame at the end of the interval.
         t = end;
      }//if ((t > end) && (t - end < delta))
   }//while (t <=end)
}//void TnTExprt::DumpPosSample(INode* node, AWTransformController* controller) 



void 
TnTExprt::DumpRotSample(INode* node, AWTransformController* controller) 
{	
	TimeValue start = ip->GetAnimRange().Start();
	TimeValue end   = ip->GetAnimRange().End();
	int delta       = GetTicksPerFrame() * GetKeyFrameStep();
	AWPoint posn, scale;
	AWQuat  rotn, prevRotn;
   TimeValue t = start;
   while (t <=end)
   {
      getNodeTransform(node, t, posn, scale, rotn);
		if ((t!= start) && (t!= end) && (rotn == prevRotn)) 
		{	// Skip identical keys		 
			;
		}
      else
      {
		   prevRotn = rotn;
		   controller->addRotation(rotn, TicksToSec(t));
         //AWTRACE4("AWQuat(%f, %f, %f, %f) ", 
         //         (double)rotn.w, (double)rotn.x, (double)rotn.y, (double)rotn.z);
         //AWTRACE1(" at %f\n", (double)TicksToSec(t));
      }
      t+=delta; //move to next sample frame
      if ((t > end) && (t - end < delta))
      {  //make sure we take a sample frame at the end of the interval.
         t = end;
      }//if ((t > end) && (t - end < delta))
   }//while (t <=end)
   //now process these keys to find case where q should be replaced by -q
	//note that this should NOT be done unless the keys are SAMPLED
   controller->preProcessRotationKeys();
}//void TnTExprt::DumpRotSample(INode* node, AWTransformController* controller) 




void 
TnTExprt::DumpScaleSample(INode* node, AWTransformController* controller, Matrix3& ttm) 
{	
	TimeValue start = ip->GetAnimRange().Start();
	TimeValue end   = ip->GetAnimRange().End();
	int delta = GetTicksPerFrame() * GetKeyFrameStep();
	AWPoint posn, scale, prevScale;
	AWQuat  rotn;
   TimeValue t = start;
   while (t <=end)
   {
      getNodeTransform(node, t, posn, scale, rotn);
		if ((t!= start) && (t!= end) && AWEqualPoint3(scale, prevScale)) 
		{	// Skip identical keys		 
			;
		}
      else
      {
		   prevScale = scale;
		   controller->addScale(scale, TicksToSec(t)/* / GetTicksPerFrame()*/);
      }
      t+=delta; //move to next sample frame
      if ((t > end) && (t - end < delta))
      {  //make sure we take a sample frame at the end of the interval.
         t = end;
      }//if ((t > end) && (t - end < delta))
   }//while (t <=end)
}//void TnTExprt::DumpScaleSample(INode* node, AWTransformController* controller) 



AWPoint TnTExprt::getAWPoint(const Point3& p, AWBoolean transform)
{
   AWPoint retVal;
   if (transform)
   {
      retVal = transformPoint(p);
   }//if (transform)
   else
   {
      retVal.x = p.x;
      retVal.y = p.y;
      retVal.z = p.z;
   }
   return retVal;
}//AWPoint TnTExprt::getAWPoint(const Point3& p, AWBoolean transform)



// Output point3 keys if this is a known point3 controller that
// supports key operations. Otherwise we will sample the controller 
// once for each frame to Get the value.
// Point3 controllers can control, for example, color.
void TnTExprt::DumpPoint3Keys(Control* cont, AWAnimatedPoint& animPt, AWBoolean transform) 
{
   // Bug out if no controller.
	if (!cont) return;
	
	int            i;
	IKeyControl*   ikc = NULL;
   AWPoint        val;

	// If the user wants us to always sample, we will ignore the KeyControlInterface
	if (!GetAlwaysSample())
   {
		ikc = GetKeyControlInterface(cont);
   }
	// TCB point3
	if (ikc && cont->ClassID() == Class_ID(TCBINTERP_POINT3_CLASS_ID, 0)) 
   {
      for (i=0; i<ikc->GetNumKeys(); i++) 
      {
			ITCBPoint3Key key;
			ikc->GetKey(i, &key);
         val = getAWPoint(key.val, transform);
         animPt.addKey(val, TicksToSec(key.time));
			//TCB specific data
			//key.tens, key.cont, key.bias, key.easeIn, key.easeOut
		}//for (i=0; i<ikc->GetNumKeys(); i++) 
	}//if (ikc && cont->ClassID() == Class_ID(TCBINTERP_POINT3_CLASS_ID, 0)) 	
	else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_POINT3_CLASS_ID, 0)) 
   {  // Bezier point3
      for (i=0; i<ikc->GetNumKeys(); i++) 
      {
			IBezPoint3Key key;
			ikc->GetKey(i, &key);
         val = getAWPoint(key.val, transform);
			animPt.addKey(val, TicksToSec(key.time));
			//key.intan, key.outtan, key.flags
		}//for (i=0; i<ikc->GetNumKeys(); i++) 
	}//else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_POINT3_CLASS_ID, 0))	
	else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_COLOR_CLASS_ID, 0)) 
   {  // Bezier color
		for (i=0; i<ikc->GetNumKeys(); i++) 
      {
			IBezPoint3Key key;
			ikc->GetKey(i, &key);
         val = getAWPoint(key.val, transform);
         animPt.addKey(val, TicksToSec(key.time));
			//key.intan, key.outtan, key.flags
		}//for (i=0; i<ikc->GetNumKeys(); i++) 
	}//else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_COLOR_CLASS_ID, 0))
	else 
   {  // Unknown controller, no key interface or sample on demand -
		// This might be a procedural controller or something else we
		// don't know about. The last resort is to Get the value from the 
		// controller at every n frames.		
		TSTR name;
		cont->GetClassName(name);
		// If it is animated at all...
		if (cont->IsAnimated()) 
      {  
         Point3    value; 
         AWPoint   awVal, awPreviousVal;
			Interval  range;	
         TimeValue start = ip->GetAnimRange().Start();
	      TimeValue end   = ip->GetAnimRange().End();
         int       delta = GetTicksPerFrame() * GetKeyFrameStep();
			TimeValue t     = start;			
			// While we are inside the animation... 
			while (t <=end) 
         {  // Sample the controller
				range = FOREVER;
				cont->GetValue(t, &value, range);
            awVal = getAWPoint(value, transform);
            if ((t!= start) && (t!= end) && AWEqualPoint3(awVal, awPreviousVal)) 
		      {	// Skip identical keys unless start or end the animation sequence		 
			      ;
		      }
            else
            {
		         awPreviousVal = awVal;
               animPt.addKey(awVal, TicksToSec(t));
            }
            t+=delta; //move to next sample frame
				if ((t > end) && (t - end < delta))
            {  //make sure we take a sample frame at the end of the interval.
               t = end;
            }//if ((t > end) && (t - end < delta))
			}//while (t <=end) 
		}//if (cont->IsAnimated()) 
	}//else - this is not a recognised controller type
}//void TnTExprt::DumpPoint3Keys(Control* cont, AWAnimatedPoint& animPt, AWBoolean transform) 



// Output float keys if this is a known float controller that
// supports key operations. Otherwise we will sample the controller 
// once for each frame to Get the value.
int TnTExprt::DumpFloatKeys(Control* cont, AWAnimatedFloat& flt) 
{
	if (!cont)
		return 0;
	int i;
	IKeyControl *ikc = NULL;

	// If the user wants us to always sample, we will ignore the KeyControlInterface
	if (!GetAlwaysSample())
		ikc = GetKeyControlInterface(cont);
	
	// TCB float
	if (ikc && cont->ClassID() == Class_ID(TCBINTERP_FLOAT_CLASS_ID, 0)) {
		for (i=0; i<ikc->GetNumKeys(); i++) 
		{
			ITCBFloatKey key;
			ikc->GetKey(i, &key);
			//fprintf(pStream, "\t%s\t%s\t%s\t%s\t%s\n", Format(key.tens), Format(key.cont), Format(key.bias), Format(key.easeIn), Format(key.easeOut));
			flt.addKey(key.val, TicksToSec(key.time) /*/ GetTicksPerFrame()*/);
		}
	}
	// Bezier float
	else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_FLOAT_CLASS_ID, 0)) 
	{
		for (i=0; i<ikc->GetNumKeys(); i++) 
		{
			IBezFloatKey key;
			ikc->GetKey(i, &key);
			flt.addKey(key.val, TicksToSec(key.time) /*/ GetTicksPerFrame()*/);
			//fprintf(pStream, "\t%s\t%s\t%d\n", Format(key.intan), Format(key.outtan), key.flags);
		}
	}
	else if (ikc && cont->ClassID() == Class_ID(LININTERP_FLOAT_CLASS_ID, 0)) 
	{
		for (i=0; i<ikc->GetNumKeys(); i++) 
		{
			ILinFloatKey key;
			ikc->GetKey(i, &key);
			flt.addKey(key.val, TicksToSec(key.time) /*/ GetTicksPerFrame()*/);
		}
	}
	else {
		
		// Unknown controller, no key interface or sample on demand -
		// This might be a procedural controller or something else we
		// don't know about. The last resort is to Get the value from the 
		// controller at every n frames.
		
		TSTR name;
		cont->GetClassName(name);
		//fprintf(pStream,"%s\t\t%s \"%s\" {\n", indent.data(), ID_CONTROL_FLOAT_SAMPLE,
			//FixupName(name));
		
		// If it is animated at all...
		if (cont->IsAnimated()) {
			// Get the range of the controller animation 
			Interval range; 
			// Get range of full animation
			Interval animRange = ip->GetAnimRange(); 
			TimeValue t = cont->GetTimeRange(TIMERANGE_ALL).Start();
			float value;
			
			// While we are inside the animation... 
			while (animRange.InInterval(t)) {
				// Sample the controller
				range = FOREVER;
				cont->GetValue(t, &value, range);
				
				// Set time to start of controller validity interval 
				t = range.Start();
				
				// Output the sample
				flt.addKey(value, TicksToSec(t) /*/ GetTicksPerFrame()*/);
				
				// If the end of the controller validity is beyond the 
				// range of the animation
				if (range.End() > cont->GetTimeRange(TIMERANGE_ALL).End()) {
					break;
				}
				else {
					t = (range.End()/GetTicksPerFrame()+GetKeyFrameStep()) * GetTicksPerFrame();
				}
			}
		}
		//fprintf(pStream,"%s\t\t}\n", indent.data());
	}
    return flt.isAnimated();
}


void 
TnTExprt::DumpPosKeys(Control* cont, AWTransformController* controller, Matrix3& tm) 
{
	if (!cont | !controller) return;
	
	int i;
	IKeyControl *ikc = GetKeyControlInterface(cont);	
	// TCB position
	if (ikc && cont->ClassID() == Class_ID(TCBINTERP_POSITION_CLASS_ID, 0)) 
	{
		int numKeys;
		if (numKeys = ikc->GetNumKeys()) 
		{
			for (i=0; i<numKeys; i++) 
			{
				ITCBPoint3Key key;
				ikc->GetKey(i, &key);
				controller->addTranslation(transformPoint(key.val), 
													TicksToSec(key.time) /*/ GetTicksPerFrame()*/);
			}//for (i=0; i<numKeys; i++) 
		}//if (numKeys = ikc->GetNumKeys())
	}//if (ikc && cont->ClassID() == Class_ID(TCBINTERP_POSITION_CLASS_ID, 0)) 
	// Bezier position
	else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_POSITION_CLASS_ID, 0))
	{
		int numKeys;
		if(numKeys = ikc->GetNumKeys())
		{
			for (i=0; i<numKeys; i++) 
			{
				IBezPoint3Key key;
				ikc->GetKey(i, &key);
				controller->addTranslation(transformPoint(key.val), 
													TicksToSec(key.time) /*/ GetTicksPerFrame()*/);
			}
		}//if(numKeys = ikc->GetNumKeys())
	}//else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_POSITION_CLASS_ID, 0))
	// Linear position
	else if (ikc && cont->ClassID() == Class_ID(LININTERP_POSITION_CLASS_ID, 0)) 
	{
		int numKeys;
		if(numKeys = ikc->GetNumKeys())
		{
			//fprintf(pStream,"%s\t\t%s {\n", indent.data(), ID_CONTROL_POS_LINEAR); 
			for (i=0; i<numKeys; i++) 
			{
				ILinPoint3Key key;
				ikc->GetKey(i, &key);
				controller->addTranslation(transformPoint(key.val), 
													TicksToSec(key.time) /*/ GetTicksPerFrame()*/);
			}//for (i=0; i<numKeys; i++) 
		}//if(numKeys = ikc->GetNumKeys())
	}//else if (ikc && cont->ClassID() == Class_ID(LININTERP_POSITION_CLASS_ID, 0)) 
}//void TnTExprt::DumpPosKeys(Control* cont, AWTransformController* controller) 




void
TnTExprt::DumpRotKeys(INode* n, Control* cont, AWTransformController* controller, 
					  Matrix3& tm, AWBoolean lookNegY) 
{
	if (!cont | !controller) return;
	Quat	newQuat, prevQuat;
   AWQuat awQuat, lastAWQuat;
   TimeValue lastKeyTime, thisKeyTime;
	prevQuat.Identity();
	Matrix3 rot = RotateXMatrix(PI/2);
	Quat    qRot(rot);
	int i;
	IKeyControl *ikc = GetKeyControlInterface(cont);

	if (ikc && cont->ClassID() == Class_ID(TCBINTERP_ROTATION_CLASS_ID, 0)) 
	{
		int numKeys;
      ITCBRotKey key;
		if (numKeys = ikc->GetNumKeys()) 
		{
			for (i=0; i<numKeys; i++) 
			{				
				ikc->GetKey(i, &key);
				newQuat     = key.val;
            thisKeyTime = key.time;
				if (i) newQuat = prevQuat * newQuat;
				newQuat.Normalize();           
            prevQuat = newQuat;
				if (lookNegY) 
            {
               awQuat  = transformQuaternion(newQuat/qRot);
            }
            else
            {
               awQuat  = transformQuaternion(newQuat);
            }
            if (i)
            {  //this isn't the first key
               if (awQuat == -lastAWQuat)
               {  //request a sample mid-way in between to give more detail
                  TimeValue sampleTime = (thisKeyTime - lastKeyTime)/2.0f + lastKeyTime;
                  AWPoint junkPosn, junkScale;
                  AWQuat sampledAWQuat;
                  getNodeTransform(n, sampleTime, junkPosn, junkScale, sampledAWQuat);
                  controller->addRotation(sampledAWQuat, TicksToSec(sampleTime));  
               }//if (awQuat == -lastAWQuat)
            }//if (i)
				controller->addRotation(awQuat, TicksToSec(key.time));           
            lastKeyTime = thisKeyTime;
            lastAWQuat  = awQuat;
			}//for (i=0; i<numKeys; i++)
		}//if (numKeys = ikc->GetNumKeys())
	}//if (ikc && cont->ClassID() == Class_ID(TCBINTERP_ROTATION_CLASS_ID, 0))
	else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_ROTATION_CLASS_ID, 0))
	{
		int numKeys;
		if (numKeys = ikc->GetNumKeys()) 
		{
         IBezQuatKey key;
			for (i=0; i<numKeys; i++) 
			{				
				ikc->GetKey(i, &key);
				newQuat = key.val;
            thisKeyTime = key.time;
				if (i) newQuat = prevQuat * newQuat;
				newQuat.Normalize();
				prevQuat = newQuat;
				if (lookNegY) 
            {
               awQuat  = transformQuaternion(newQuat/qRot);
            }
            else
            {
               awQuat  = transformQuaternion(newQuat);
            }
            if (i)
            {  //this isn't the first key
               if (awQuat == -lastAWQuat)
               {  //request a sample mid-way in between to give more detail
                  TimeValue sampleTime = (thisKeyTime - lastKeyTime)/2.0f + lastKeyTime;
                  AWPoint junkPosn, junkScale;
                  AWQuat sampledAWQuat;
                  getNodeTransform(n, sampleTime, junkPosn, junkScale, sampledAWQuat);
                  controller->addRotation(sampledAWQuat, TicksToSec(sampleTime));  
               }//if (awQuat == -lastAWQuat)
            }//if (i)
				controller->addRotation(awQuat,  TicksToSec(key.time));
            lastKeyTime = thisKeyTime;
            lastAWQuat  = awQuat;
				// There is no intan/outtan for Quat Rotations
			}//for (i=0; i<numKeys; i++)
		}//if (numKeys = ikc->GetNumKeys())
	}//else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_ROTATION_CLASS_ID, 0))
	else if (ikc && cont->ClassID() == Class_ID(LININTERP_ROTATION_CLASS_ID, 0)) 
	{
		int numKeys;
		if (numKeys = ikc->GetNumKeys()) 
		{
         ILinRotKey key;
			for (i=0; i<numKeys; i++) 
			{				
				ikc->GetKey(i, &key);
				newQuat = key.val;
            thisKeyTime = key.time;
				if (i) newQuat = prevQuat * newQuat;
				newQuat.Normalize();
				prevQuat = newQuat;
				if (lookNegY) 
            {
               awQuat  = transformQuaternion(newQuat/qRot);
            }
            else
            {
               awQuat  = transformQuaternion(newQuat);
            }
            if (i)
            {  //this isn't the first key
               if (awQuat == -lastAWQuat)
               {  //request a sample mid-way in between to give more detail
                  TimeValue sampleTime = (thisKeyTime - lastKeyTime)/2.0f + lastKeyTime;
                  AWPoint junkPosn, junkScale;
                  AWQuat sampledAWQuat;
                  getNodeTransform(n, sampleTime, junkPosn, junkScale, sampledAWQuat);
                  controller->addRotation(sampledAWQuat, TicksToSec(sampleTime));  
               }//if (awQuat == -lastAWQuat)
            }//if (i)
				controller->addRotation(awQuat, TicksToSec(key.time));
            lastKeyTime = thisKeyTime;
            lastAWQuat  = awQuat;
			}//for (i=0; i<numKeys; i++) 
		}//if (numKeys = ikc->GetNumKeys()) 
	}//else if (ikc && cont->ClassID() == Class_ID(LININTERP_ROTATION_CLASS_ID, 0)) 
   //now post process these keys - at the moment this is special case for
   //key derived values only. Need to examine this and make more general
   //so that we always get smooth sampled animations also.
   //let AMX do all the work - so use SAMPLE logic to get a suitable key
}//void TnTExprt::DumpRotKeys(Control* cont, AWTransformController* controller) 


void 
TnTExprt::DumpScaleKeys(Control* cont, AWTransformController* controller, Matrix3& tm) 
{
	if (!cont)
		return;	
	int i;
	IKeyControl *ikc = GetKeyControlInterface(cont);
	AWPoint awKey;
	if (ikc && cont->ClassID() == Class_ID(TCBINTERP_SCALE_CLASS_ID, 0))
	{
		int numKeys;
		if (numKeys = ikc->GetNumKeys()) 
		{
			for (i=0; i<numKeys; i++) 
			{
				ITCBScaleKey key;
				ikc->GetKey(i, &key);
                awKey.x=key.val.s.x;
                awKey.y=key.val.s.z;
                awKey.z=key.val.s.y;
				controller->addScale(awKey, TicksToSec(key.time) );
			}//for (i=0; i<numKeys; i++)
		}
	}
	else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_SCALE_CLASS_ID, 0)) 
	{
		int numKeys;
		if (numKeys = ikc->GetNumKeys()) {
			//fprintf(pStream,"%s\t\t%s {\n", indent.data(), ID_CONTROL_SCALE_BEZIER); 
			for (i=0; i<numKeys; i++) 
			{
				IBezScaleKey key;
				ikc->GetKey(i, &key);
                awKey.x=key.val.s.x;
                awKey.y=key.val.s.z;
                awKey.z=key.val.s.y;
				controller->addScale(awKey, TicksToSec(key.time) );
			}
			//fprintf(pStream,"%s\t\t}\n", indent.data());
		}
	}
	else if (ikc && cont->ClassID() == Class_ID(LININTERP_SCALE_CLASS_ID, 0)) 
	{
		int numKeys;
		if (numKeys = ikc->GetNumKeys()) 
		{
			//fprintf(pStream,"%s\t\t%s {\n", indent.data(), ID_CONTROL_SCALE_LINEAR); 
			for (i=0; i<numKeys; i++) 
			{
				ILinScaleKey key;
				ikc->GetKey(i, &key);
                awKey.x=key.val.s.x;
                awKey.y=key.val.s.z;
                awKey.z=key.val.s.y;
				controller->addScale(awKey, TicksToSec(key.time) );
			}//for (i=0; i<numKeys; i++)
		}//if (numKeys = ikc->GetNumKeys())
	}
}




// Determine if a TM controller is known by the system.
TnTExprt::IsKnownController(Control* cont)
{
	ulong partA, partB;

	if (!cont)
		return FALSE;

	partA = cont->ClassID().PartA();
	partB = cont->ClassID().PartB();

	if (partB != 0x00)
		return FALSE;

	switch (partA) {
		case TCBINTERP_POSITION_CLASS_ID:
		case TCBINTERP_ROTATION_CLASS_ID:
		case TCBINTERP_SCALE_CLASS_ID:
		case HYBRIDINTERP_POSITION_CLASS_ID:
		case HYBRIDINTERP_ROTATION_CLASS_ID:
		case HYBRIDINTERP_SCALE_CLASS_ID:
		case LININTERP_POSITION_CLASS_ID:
		case LININTERP_ROTATION_CLASS_ID:
		case LININTERP_SCALE_CLASS_ID:
			return TRUE;
	}

	return FALSE;
}
