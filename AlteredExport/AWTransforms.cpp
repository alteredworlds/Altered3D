#include "AlteredExport.h"
#include <AW3DTools/AWFace.h>
#include <AW3DTools/AWFlightController.h>
#include <AW3DTools/AWEnvironController.h>
#include <AW3DTools/AWBillboardController.h>

//This file includes all the required routines to transform from
//MAX native transforms to OpenGL suitable forms
//all are members of class TnTExprt

//static float fTwoPi = 2.0f * PI;
//static float fRadToDeg = 360.0f / fTwoPi;



// Get the tranform matrix that take a point from its local coordinate
// system to it's parent's coordinate system
static Matrix3
GetLocalTM(INode* node, TimeValue t, AWBoolean parentIsCamera=FALSE)
{
   Matrix3 tm = node->GetNodeTM(t);
   INode*  p  = node->GetParentNode();
   if (p && !p->IsRootNode())// && !p->IsGroupHead()) 
   {     
      Matrix3 ip = p->GetNodeTM(t);
      if (parentIsCamera)
      {
         //Quat    q(ip), q90(rot), check;		
         //q = q / q90;
         Matrix3 rot = RotateXMatrix(PI/2);
         rot = Inverse(rot);
         ip =  rot * ip;
         //check = Quat(ip);
      }//if (parentIsCamera)
      tm = tm * Inverse(ip);
   }//if (p && !p->IsRootNode())
   return tm;
}//static Matrix3 GetLocalTM(INode* node, TimeValue t, AWBoolean parentIsCamera=FALSE)



AWBoolean
TnTExprt::isMatrixIdentity(Matrix3& tm)
{
	// Check for SCALE and ROTATION (only) parts of matrix being identity.
    AWBoolean isIdentity = TRUE;
    for (int i=0; i<3; i++) 
	{
        for (int j=0; j<3; j++) 
		{
            if (i==j) 
			{
                if (tm.GetRow(i)[j] != 1.0f) 
				{
                    isIdentity = FALSE;
                    return isIdentity;
                }//if (tm.GetRow(i)[j] != 1.0f)
            }//if (i==j) 
			else if (fabs(tm.GetRow(i)[j]) > 1.0e-05) 
			{
                isIdentity = FALSE;
                return isIdentity;
            }//else if (fabs(tm.GetRow(i)[j]) > 1.0e-05)
        }//for (j=0; j<3; j++)
    }//for (i=0; i<3; i++)
	return isIdentity;
}//AWBoolean TnTExprt::isMatrixIdentity(Matrix3& tm)


AWPoint 
TnTExprt::transformPoint(const Point3& pt)
{	//transform point from MAX coordinate system to OpenGL format
	return AWPoint(pt.x, pt.z, -pt.y);
}//AWPoint TnTExprt::transformPoint(AWPoint& pt)



AWPoint 
TnTExprt::transformAxis(Point3& pt)
{	//transform point from MAX coordinate system to OpenGL format
	return AWPoint(pt.x, pt.z, -pt.y);
}//AWPoint TnTExprt::transformAxis(AWPoint& pt)


AWPoint
TnTExprt::transformScale(const AffineParts& parts)
{
	AWPoint retVal(1.0f, 1.0f, 1.0f);
	ScaleValue sv(parts.k, parts.u);
    Point3 s = sv.s;
    if (parts.f < 0.0f)
        s = - s;
    if (s.x != 1.0f || s.y != 1.0f || s.z != 1.0f) 
	{
		retVal.x = s.x;
		retVal.y = s.z;
		retVal.z = s.y; //NOTE - no NEGATION HERE - is this correct?
    }
	return retVal;
}//AWPoint TnTExprt::transformScale(AWPoint& scale, const AffineParts& parts)


void
TnTExprt::transformQuaternion(const Quat& srce, AWPoint& axis, float& angle)
{	//must first move to angle, axis format
	//then transform the axis as usual
	//and negate the angle, returning it in DEGREES!!!
	float  maxAngle;
	Point3 maxAxis;
	AngAxisFromQ(srce, &maxAngle, maxAxis);
	axis  = transformPoint(maxAxis);
	angle = -maxAngle*fRadToDeg;
}//void TnTExprt::transformQuaternion(const Quat& quat)


AWQuat
TnTExprt::transformQuaternion(const Quat& quat)
{	//must first move to angle, axis format
	//then transform the axis as usual
	//and negate the angle
	//returning as an AWQuat
	AWPoint axis;
	float   angle;
	transformQuaternion(quat, axis, angle);
	//we now have the correct, transformed angle and axis
	//so create an AWQuat from this and return
	return AWQuat(axis, angle);
}//AWQuat TnTExprt::transformQuaternion(const Quat& quat)


AWBoolean
TnTExprt::getTransformFromMatrix3(Matrix3& tm, AWPoint& posn, 
								  AWPoint& scale, AWQuat& rotn, AWBoolean lookNegY)
{
	AWBoolean retVal = !tm.IsIdentity();
	//rotn.x = rotn.y = rotn.z = 0.0f; rotn.w = 1.0f;
	//posn.x = posn.y = posn.z = 0.0f;
	//scale.x = scale.y = scale.z = 1.0f;
	if (retVal)
	{	
		Quat q;
		q.Identity();
		// Check for scale and rotation part of matrix being identity.    
		if ( isMatrixIdentity(tm) ) 
		{
			posn = transformPoint(tm.GetTrans());
		} 
		else 
		{	//extract info into components
			AffineParts ap;
			decomp_affine(tm, &ap);
			//first the rotation
			q = ap.q;
			//now the scale
			scale  = transformScale(ap);
			//and finally the position
			posn   = transformPoint(ap.t);
		}//else
		if (lookNegY)
		{
			Matrix3 rot = RotateXMatrix(PI/2);
			Quat qRot(rot);
			q = q/qRot;	
		}
		rotn = transformQuaternion(q);
	}//if (retVal)
	return retVal;
}//AWBoolean TnTExprt::getTransformFromMatrix3(Matrix3& tm, AWPoint& posn, AWPoint& scale, 
//  AWQuat& rotn, AWBoolean lookNegY)

void 
TnTExprt::checkForNegZ(INode* n, AWBoolean& self, AWBoolean& parent)
{
   self   = FALSE;
   parent = FALSE;
   TCHAR*    testName = n->GetName();
   Object*   obj      = n->GetObjectRef();
   if (obj)
   {
      SClass_ID sClass   = obj->SuperClassID();
      self   = (CAMERA_CLASS_ID == sClass) || (LIGHT_CLASS_ID == sClass);
      if (n->GetParentNode())
      {
         obj = n->GetParentNode()->GetObjectRef();
         if (obj)
         {
            sClass   = obj->SuperClassID();
            parent  = (CAMERA_CLASS_ID==sClass) || (LIGHT_CLASS_ID==sClass);
         }//if (obj)
      }//if (n->GetParentNode())
   }//if (obj)
}//void TnTExprt::checkForNegZ(INode* n, AWBoolean& self, AWBoolean& parent)


//alternative method of getting the node's LocalTM
//broken down into components.
//now includes a parameter indicating node type:
//
AWBoolean
TnTExprt::getNodeTransform(INode* node, const TimeValue& t,
							AWPoint& posn, AWPoint& scale, AWQuat& rotn)
{
	if (!node || node->IsRootNode())
	{	//root node is always identity
		return FALSE;
	}
   Matrix3     TM;
	AWBoolean   iLookNegZ		 = FALSE;
   AWBoolean   parentLooksNegZ = FALSE;

   checkForNegZ(node, iLookNegZ, parentLooksNegZ);
	//environment controllers need to evaluate w.r.t.
	//the world not the parent camera
	
	TM = GetLocalTM(node, t, parentLooksNegZ);
	return getTransformFromMatrix3(TM, posn, scale, rotn, iLookNegZ);
}//void TnTExprt::getNodeTransform(INode& node, AWPoint& posn, AWPoint& scale, AWQuat& rotn)



void
TnTExprt::assignTransformController(INode* node, AWNode* awNode)
{
   AWPoint posn, scale(1.0f, 1.0f, 1.0f);
   AWQuat  rotn;
   AWBoolean pointBillboard=FALSE;
   //this routine should create an AWTransformController
   //assign it to the AWObject passed in
   //determine the Local Transforms for the node as posn, scale, rotn
   //initialise the steadyStateTransform of the controller
   if (!node || node->IsRootNode()) return;
   //moved from xport geom object
   AWTrType controllerType = AW_STANDARD;
   AWBoolean isPlayer = FALSE;
   AWBoolean isEnviron= FALSE;
   AWBoolean isJunkParam = FALSE;
   int iVal;

   if (node->GetUserPropBool(_T("MOVE"), isPlayer))
   {	//make this typesafe ASAP
      controllerType = AW_FLIGHT;
      isPlayer = TRUE;
   }
   if (node->GetUserPropBool(_T("TIMERBAR"), isJunkParam))
   {
      controllerType = AW_TIMERBAR;
   }
	if (node->GetUserPropBool(_T("ENVIRON"), isEnviron))
	{
		controllerType = AW_ENVIRON;
		isEnviron=TRUE;
		if (CLSID_AWOBJECT == awNode->isA())
		{
			theScene.m_nodeList.setEnvironment((AWObject*)awNode);
		}//if (CLSID_AWOBJECT == awNode->isA())
	}//if (node->GetUserPropBool(_T("ENVIRON"), isEnviron))
	if (node->GetUserPropInt(_T("BILLBOARD"), iVal))
	{
		controllerType = iVal ? AW_BILLBOARD_ALL : AW_BILLBOARD_Y;
	}
	if ( /*(CLSID_AWLIGHT == awNode->isA()) || */(CLSID_AWCAMERA == awNode->isA()))
	{//light or camera - why was light included?
		controllerType = AW_FLIGHT;
	}
	//getNodeTransform(..) returns FALSE if the transform is the INDENTITY
	if (getNodeTransform(node, GetStaticFrame(), posn, scale, rotn))
	{
		AWTransformController* trans = NULL;
		switch (controllerType)
		{
		case AW_FLIGHT:
			trans = new AWFlightController;
			if (isPlayer)
			{
				theScene.m_nodeList.setTransformer((AWFlightController*)trans);
			}
			break;
        case AW_TIMERBAR:
            trans = new AWTimerBarController;
            theScene.m_nodeList.setTimerBarCtl((AWTimerBarController*)trans);
            break;
		case AW_ENVIRON:
			trans = new AWEnvironController;
			break;
		case AW_BILLBOARD_ALL:
			pointBillboard=TRUE;
		case AW_BILLBOARD_Y:
			trans = new AWBillboardController;
			((AWBillboardController*)trans)->m_pointSymmetry=pointBillboard;
			break;
		case AW_STANDARD:
		default:
			trans = new AWTransformController;
			break;
		}
		//now assign the deduced values to the AWTransformController for this node
		if (trans) trans->setTransform(posn, scale, rotn);
		awNode->setController(trans);
	}//if (getNodeTransform(node, GetStaticFrame(), posn, scale, rotn))
}//void TnTExprt::assignTransformController(INode* node, AWNode* obj)
