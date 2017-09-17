#include "AlteredExport.h"



/****************************************************************************

  Light output
  
****************************************************************************/

AWNode* 
TnTExprt::ExportLightObject(INode* node, AWNode* parent)
{
	//light objects now exported - but not fully ANIMATED yet.
	//first extract the general info that MAX provides for any light
	TimeValue t = GetStaticFrame();
	ObjectState os = node->EvalWorldState(t);
	if (!os.obj) return NULL;
	
	GenLight* light = (GenLight*)os.obj;
	struct LightState ls;
	Interval valid = FOREVER;
	Interval animRange = ip->GetAnimRange();

	light->EvalLightState(t, valid, &ls);

	// this type determines what kind of light it's going to be
	// for now AWEngine just uses a single AWLight class with a similar
	// enumerated type
	AWLight::LightType awType;
	switch(ls.type) 
	{
	case OMNI_LGT: 
		awType = AWLight::AW_POINT_LIGHT;
		break;
	case DIRECT_LGT:     
		awType = AWLight::AW_DIRECTIONAL_LIGHT;
		break;
	case SPOT_LGT :  
		awType = AWLight::AW_SPOT_LIGHT;
		break;
	}
   
	AWLight* curLight = new AWLight(node->GetName(), parent, awType);
	//extract the node transformation
	assignTransformController(node, curLight);
	// Export (TRANSLATION) animation keys for the light node
	if (GetIncludeAnim()) 
	{
		ExportAnimKeys(node, curLight, TRUE);
	}//if (GetIncludeAnim())
   //if we have a spotlight and it is a projector, grab map info.
   if (AWLight::AW_SPOT_LIGHT==awType)
   {
      Texmap* texMap = light->GetProjMap();
      if (texMap)
      {
         curLight->m_proj = getMapData((char*)texMap->GetName(), texMap, 1.0);
         if (curLight->m_proj)
         {
            curLight->m_proj->m_genMode = AWTexture::EYE_LINEAR;
            curLight->m_proj->m_wrap    = GL_CLAMP_TO_EDGE;
            if (curLight->m_proj->getNumFrames()>1)
            {
               for (int fr=0; fr<curLight->m_proj->getNumFrames(); fr++)
               {
                  AWTexture* frame = curLight->m_proj->getTexFromArrayIndex(fr);
                  frame->m_genMode = AWTexture::EYE_LINEAR;
                  frame->m_wrap    = GL_CLAMP_TO_EDGE;
               }//for (int fr=0; fr<curLight->m_proj->getNumFrames(); fr++)
            }//if (curLight->m_proj->getNumFrames()>1)
            //the following line won't work as intended as m_mipmap
            //IS NOT CURRENTLY PERSISTED!
            //curLight->m_proj->m_mipMap  = FALSE;
         }//if (curLight->m_proj)
      }//if (texMap)
      if (light->GetShadow())
      {  //this light should cast shadows 
         //need to find out how to access the MAP SIZE parameter
         //BIAS and SAMPLE RANGE also useful (but may set in code)
         //supply default values for now
         AWShadowMap* sMap  = new AWShadowMap(node->GetName(), 512, 4.0f, 1.1f);
         sMap->m_genMode    = AWTexture::EYE_LINEAR;
         sMap->m_wrap       = GL_CLAMP_TO_EDGE;
         sMap->m_format     = GL_DEPTH_COMPONENT;
         //the following line won't work as intended as m_mipmap
         //IS NOT CURRENTLY PERSISTED!
         //sMap->m_mipMap  = FALSE;
         curLight->m_shadow = sMap;
         theScene.m_textureList.add(sMap);
      }//if (light->GetShadow())
   }//if (AWLight::AW_SPOT_LIGHT==awType)

	// Export light settings for frame 0
	ExportLightSettings(*curLight, &ls, light, t);

	//AWLight now supports animated colour
	//Export animated light settings
	if (!valid.InInterval(animRange) && GetIncludeCamLightAnim()) 
	{
		TimeValue t = animRange.Start();	
		while (1) 
		{
			valid = FOREVER; // Extend the validity interval so the camera can shrink it.
			light->EvalLightState(t, valid, &ls);
			t = valid.Start() < animRange.Start() ? animRange.Start() : valid.Start();
			// Export the light settings at this frame
			ExportLightSettings(*curLight, &ls, light, t, TRUE);		
			if (valid.End() >= animRange.End()) 
			{
				break;
			}
			else 
			{
				t = (valid.End()/GetTicksPerFrame()+GetMeshFrameStep()) * GetTicksPerFrame();
			}
		}//while (1)
	}//if (!valid.InInterval(animRange) && GetIncludeCamLightAnim())
	return curLight;
}//void TnTExprt::ExportLightObject(INode* node)




void 
TnTExprt::ExportLightSettings(AWLight& awLight, LightState* ls, 
							  GenLight* light, TimeValue t, AWBoolean isAnimatedData)
{
	//get Colour
	Color col = ls->color;
	AWPoint colour(col.r, col.g, col.b);
	//get intensity
	colour *= ls->intens;
	if (isAnimatedData)
	{
		awLight.addColor(colour, TicksToSec(t) /*/ GetTicksPerFrame()*/);
	}
	else
	{
		awLight.setColor(colour);
		awLight.m_on = ls->on;
		if (awLight.m_type == AWLight::AW_SPOT_LIGHT) 
		{
			awLight.m_cutOffAngle = ls->hotsize / 2.0f;
			//or do I want ls->fallsize, or an average between the two?
		}
	}//else
}//void TnTExprt::ExportLightSettings(AWLight& awLight, LightState* ls, GenLight* light, TimeValue t)
