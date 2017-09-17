#include "AWScene.h"
#include "AWTransformController.h"
#include "AWFlightController.h"
#include "AWTexture.h"
#include "AWFog.h"
#include "AWMaterial.h"
#include "AWSea.h"
#include <GL/glext.h>

AWMaterial* defaultMaterial = NULL;


#define VER_AWScene_2 101
#define VER_AWScene_3 102
#define VER_AWScene_4 103
#define VER_AWScene_5 104
#define VER_AWScene_6 105
#define VER_AWScene_7 106
#define VER_AWScene_8 107

AWDEFINE_VERSIONABLE_PERSISTABLE(AWScene, CLSID_AWSCENE, VER_AWScene_8);

#ifdef AW_DEBUG_OUT
extern ofstream outputff;
#endif

AWScene::AWScene() :
   m_landscape(NULL),
   m_materialList(1),
   m_useFog(-1), 
   m_fogColour(0.1f, 0.2f, 0.2f),
   m_fogStart(0.0f),
   m_fogEnd(100.0f),
   m_fogType(0),
   m_fogList(1),
   m_loopTime(0),
   m_numSections(1),
   m_targetSection(1),
   m_autoCubes(1, AW_PTRLIST_OWNS_LIST|AW_PTRLIST_OWNS_OBJECTS),
   m_projectors(1,AW_PTRLIST_OWNS_LIST),
   m_shadowers(1,AW_PTRLIST_OWNS_LIST),
   m_defaultMaterial(NULL)
{
   //m_autoCubes list DOES own its contents - a series of 
   //AWCubeGen objects - which DO NOT delete the data they index.
   //m_projectors contains lights also in AWWorker::m_lights
   defaultMaterial = new AWMaterial;
   defaultMaterial->setUseDefaults(TRUE);
}



AWScene::~AWScene()
{
	clearAndDestroy();
	delete m_landscape;
}


void
AWScene::clearAndDestroy()
{
   m_nodeList.clearAndDestroy();
   //materials are owned by this scene object
   //materials own their AWTextureEffects but not the underlying textures
   //which may be shared and are owned by the scene object
   if (m_defaultMaterial && 
       (AW_NPOS==m_materialList.find(m_defaultMaterial)))
   {
      delete m_defaultMaterial;
   }
   m_defaultMaterial = NULL;
   m_materialList.destroy();
   m_textureList.destroy();
   m_autoCubes.destroy();
   m_fogList.destroy();
   m_projectors.destroy();
   m_shadowers.destroy();
   m_useFog=-1;
   m_loopTime = 0;
   m_numSections = 1;
}//void AWScene::clearAndDestroy()


void
AWScene::compact()
{
	m_textureList.pack();
	for (int i=0; i<m_textureList.getNum(); i++)
	{
		((AWTexture*)m_textureList[i])->compact();
	}
	m_fogList.pack();
	m_nodeList.compact();
	if (m_landscape) m_landscape->compact();
}//void AWScene::compact()



AWMaterial*       
AWScene::getDefaultMaterial()
{
   if (!m_defaultMaterial)
   {
      m_defaultMaterial = new AWMaterial("Default");     
   }//if (!m_defaultMaterial)
   return m_defaultMaterial;
}//AWMaterial* AWScene::getDefaultMaterial()


int		
AWScene::getNumOpaqueObjects()
{
	return m_nodeList.getNumOpaqueObjects();
}


int				
AWScene::getNumTextures()
{
	int numTextureObjects = 0;
	for (int i=0; i < m_textureList.getNum(); i++)
	{
		AWTexture* mat = (AWTexture*)m_textureList[i];
		numTextureObjects += mat->getNumFrames();
	}
	return numTextureObjects;
}//int AWScene::getNumTextures()


void		   
AWScene::setUseFog(int val)				
{
	//valid ranges [-1..num_fog -1] - -1 OFF, otherwise index
	if ((val > -2) && (val < m_fogList.getNum()))
	{
		m_useFog = val;
		if (m_useFog != -1)
		{//update the fog parameters
			AWFog* nextFog = (AWFog*)m_fogList[m_useFog];
			m_fogColour	= nextFog->m_colour;
			m_fogStart  = nextFog->m_startDist;
			m_fogEnd	   = nextFog->m_endDist;
			m_fogType   = nextFog->m_type;
		}//if (m_useFog != -1)
	}//if ((val > -2) && (val < m_fogList.entries()))
}//void	AWScene::setUseFog(int val)



void		        
AWScene::setCamera(AWCamera* cam)
{
	if (cam)
	{
		m_nodeList.setCurCamera(cam);
	}//if (cam)
}//void AWScene::setCamera(AWCamera* cam)



void
AWScene::center(const AWPoint& origin)
{
	m_nodeList.center(origin);
}//void AWScene::center(const AWPoint& origin)



void	
AWScene::fitToRange(float x, float y, float z)
{
	AWPoint scale(m_nodeList.fitToRange(AWPoint(x,y,z)));
	for (int i=0; i< m_fogList.getNum();i++)
	{
		((AWFog*)m_fogList[i])->scale(scale);
	}
	setCamera(m_nodeList.getCurCamera());
}//void	AWScene::fitToRange(float x, float y, float z)



AWCString 
AWScene::getSceneDirectory(const AWCString& scenePath)
{
   AWCString retVal(scenePath);
   string::size_type strPos = retVal.find(".tnt");
   if (string::npos == strPos)
   {
      strPos = retVal.find(".TNT");
   }//if (string::npos == strPos)
   if (string::npos != strPos)
   {	//need to remove the filename for specification of initial directory
      size_t index = retVal.rfind('/');
      if (string::npos == index) 
      {
         index = retVal.rfind('\\');
      }//if (string::npos == index) 
      if (string::npos != index)
      {
         if (index+1 < retVal.length()) 
         {
            retVal.replace(index+1, retVal.length()-index-1, "");
         }//if (index+1 < retVal.length()) 
      }//if (string::npos != index)
   }//if (string::npos != strPos)
   return retVal;
}//AWCString AWScene::getSceneDirectory(const AWCString& scenePath)



AWCString 
AWScene::getSceneName(const AWCString& extendedName, AWBoolean removeExtension)
{  //incoming data can be in one of the following valid formats:
   //(i)  c:\Program Files\AlteredWorlds\AlteredSaver\scenes\bird_bomb.tnt
   //(ii) bird_bomb.tnt[/x00A/x00Aplus any other junk...]
   //(iii)bird_bomb.cgi[/x00A/x00Aplus any other junk...]
   // - to be added as necessary -
   AWCString retVal(extendedName);
   if (extendedName.length())
   {  //find valid extension for an AlteredWorld
      string::size_type pos = extendedName.find(".tnt");
      if (string::npos == pos)
      {  //check for extension used by web server
         //redirection mechanism
         pos = extendedName.find(".cgi");
      }//if (string::npos == extPos)
      if (string::npos != pos)
      {   
         if (removeExtension)
         {  //Remove extension if desired
            retVal.eqSubString(extendedName, 0, pos-1);
         }
         else if (string::npos != (pos = retVal.find("\t")) )
         {  //OR remove the file size, if present. Will have been removed
            //along with the extension, iff this was removed
            retVal.eqSubString(extendedName, pos);
         }        
         //now remove any path information in from of the scene name
         //delimited via either '/' or '\'
         pos = retVal.rfind("\\");
         if (string::npos == pos)
         {
            retVal.rfind("/");
         }//if (string::npos == pos)
         if (string::npos != pos)
         {
            retVal.replace(0, pos+1, "");
         }//if (string::npos != pos)
      }//if (string::npos != extPos)
   }//if (retVal.length())
   return retVal;
}//AWScene AWScene::getSceneName(const AWCString& extendedName, AWBoolean removeExtension)


void
AWScene::calcTextures(float t)
{
   AWTexture* tex = NULL;
   for (int i=0; i<m_textureList.getNum(); i++)
   {
      tex = (AWTexture*)m_textureList[i];
      if (tex) tex->calc(t);
   }//for (int i=0; i<m_textureList.entries(); i++)
}//void AWScene::calcTextures(float t)


void              
AWScene::update(float pitch, float yaw, float roll, 
                float viewPitch, float viewYaw, float speed, 
                float time)
{
   m_nodeList.updateTransformer(AWPoint(pitch, yaw, roll), speed, time);
	m_nodeList.setViewRot(viewPitch, viewYaw);
	m_nodeList.transformAllNodes(time);
   m_ambient.calc(time);
   m_bgCol.calc(time);
   calcTextures(time);
}//void AWScene::update(...)


void              
AWScene::buildAutoCubeList()
{  //iterate through all textures in the scene
   //if any of them are auto-generating cube maps, then
   //we need to build a new entry for the m_autoCubes
   //collection consisting of:
   int               t, o, numO;
   AWTexture*        tex  = NULL;
   AWCubeEnvTexture* ctex = NULL;
   AWMaterial*       mat  = NULL;   
   AWObject*         obj  = NULL;
   AWCubeGen*        cg   = NULL;
   for (t=0; t<m_textureList.getNum(); t++)
   {  //for each texture in the scene list...
      if ( (tex = (AWTexture*)m_textureList[t]) && 
           (CLSID_AWCUBEENVTEXTURE==tex->isA()) &&
           (ctex = (AWCubeEnvTexture*)tex) &&
           ctex->getAuto() )
      {  //detected an auto-generating texture
         //generate a new helper object, then
         //build a list of all objects using this texture
         cg = new AWCubeGen(ctex); 
         if (!cg) return; //memory problems in the extreme...
         numO = m_nodeList.getNumObjects();        
         for (o=0; o<numO; o++)
         {
            if (obj = m_nodeList.getObject(o))
            {
               mat = obj->getMaterial();
               if (mat && mat->findTexture(ctex))
               {
                  cg->addObject(obj);
               }//if (mat && mat->findTexture(ctex))
            }//if (obj = getObject(o))         
         }//for (o=0; o<numO; o++)
         if (cg->getNumObjects())
         {
            m_autoCubes.add(cg);
            //NOTE - use normalize cube as convenient way
            //of initializing buffers. Will be GL_RGB.
            ctex->generateNormalizeVectorCube(ctex->m_width);
            ctex->m_mipMap  = 1;
            ctex->m_genMode = AWTexture::REFLECT;
         }//if (cg->getNumObjects())
         else
         {  //apparently no objects actually USE this cube map
            delete cg;
         }        
      }// (tex = m_textureList[t])          
   }//for (int t=0; t<m_textureList.getNum(); t++)
}//void AWScene::buildAutoCubeList()



void 
AWScene::buildProjectorList()
{
   AWLight* light = NULL;
   m_projectors.destroy();
   m_projectors.setOwnsObjects(FALSE);
   for (int i=0; i<m_nodeList.getNumLights(); i++)
   {
      light = m_nodeList.getLight(i);
      if (light && light->m_shadow)
      {  //shadowers will also handle projecting the light if necessary
         light->m_shadow->m_mipMap  = FALSE;
         light->m_shadow->m_genMode = AWTexture::EYE_LINEAR;
         light->m_shadow->m_wrap    = GL_CLAMP_TO_EDGE;
         m_shadowers.add(light);
      }//if (light && light->m_shadow)
      if (light && light->m_proj)
      {  //only want to add it to one of: projectors / lights
         if (!light->m_shadow) m_projectors.add(light);
         //we don't want mipmapping - screws up the projection
         light->m_proj->m_mipMap = FALSE;
         //we don't want the actual light either :)
         light->m_on = FALSE;
         //we're going to use ALPHA TEST for speed when doing
         //projected spotlights - so we want alpha values to
         //reflect the colour intensity black fragments rejected
         //light->m_proj->assignAlphaFromColorIntensity();
         light->m_proj->m_genMode = AWTexture::EYE_LINEAR;
         light->m_proj->m_wrap    = GL_CLAMP_TO_EDGE;
         if (light->m_proj->getNumFrames()>1)
         {
            for (int fr=0; fr<light->m_proj->getNumFrames(); fr++)
            {
               AWTexture* frame = light->m_proj->getTexFromArrayIndex(fr);
               frame->m_genMode = AWTexture::EYE_LINEAR;
               frame->m_wrap    = GL_CLAMP_TO_EDGE;
            }//for (int fr=0; fr<light->m_proj->getNumFrames(); fr++)
         }//if (light->m_proj->getNumFrames()>1)
      }//else if (light && light->m_proj)
   }//for (int i=0; i<m_nodeList.getNumLights(); i++)
}//void AWScene::buildProjectorList()



int 
AWScene::getFogIndex(const AWCString& name)
{	//takes index into list of fog names owned by self
	//returns index into AWScene's fog list
	//or -1 if not found (or -1 initially)
	int retVal = -1;
   for (int fogNum=0; fogNum < m_fogList.getNum(); fogNum++)
	{
		if (name == ((AWFog*)m_fogList[fogNum])->m_name)
		{
			retVal = fogNum;
			break;
		}
	}//for (int fogNum=0; fogNum < m_fogList.getNum(); fogNum++)
	return retVal;
}//int AWScene::getFogIndex(const AWCString& name)



void
AWScene::grabMaterials(AWNode* n)
{
   if (!n) return;
   AWClassID thisIsA = n->isA();
   if ((CLSID_AWOBJECT==thisIsA)    ||
       (CLSID_AWSEAOBJECT==thisIsA) ||
       (CLSID_AWROCKET==thisIsA)      )
   {
      m_materialList.addUnique(((AWObject*)n)->getMaterial());
      //assign any FOG INDECES
      if (CLSID_AWSEAOBJECT == n->isA())
		{	//get the name of the AWFog object from the index
			//into the fog names array
			//find the correct index (if any) in the scene's fog list
			//and assign to the sea object
			AWSeaObject* tmp = (AWSeaObject*)n;
         if (tmp &&(-1==tmp->m_posFog)&&(-1==tmp->m_negFog))
         {
			   tmp->m_posFog = getFogIndex(tmp->m_posFogName);
			   tmp->m_negFog = getFogIndex(tmp->m_negFogName);
         }//if (tmp &&(-1==tmp->m_posFog)&&(-1==tmp->m_negFog))
		}//if (CLSID_AWSEAOBJECT == nextNode->isA())
   }
   for (int i=0; i<n->getNumChildren(); i++)
   {
      AWNode* nextChild = n->getChild(i);
      if (nextChild)
      {
         grabMaterials(nextChild);
      }//if (nextChild)
   }//for (int i=0; i<n->getNumChildren(); i++)
}//void AWScene::grabMaterials(AWNode* n)


//helpers for persistence
void 
AWScene::restoreBody(AWFile& file, unsigned int version)
{	
   file >> m_nodeList;
   m_textureList.read(file, version < VER_AWScene_6);
   if (version >= VER_AWScene_8)
   {  //all material used will be present in this list.
      file >> m_materialList;
   }
   //we need to add all materials used to this list otherwise they
   //never get deleted - this code should really only be necessary 
   //when we haven't read the material list above - but sadly we 
   //must still cater for AWObject::m_material == NULL but 
   //the object has textures - generate new material which must 
   //end up in this list. should have read the material list first -
   //before node list - would make life much easier...
   m_fogList.read(file, version < VER_AWScene_6);
   grabMaterials(&m_nodeList.getRoot());
   if (version <= VER_AWScene_4)
   {
      AWPoint tmpAmbient;
      tmpAmbient.restoreGuts(file);
      m_ambient.setVal(tmpAmbient);
   }
   else
   {
      file >> m_ambient;
   }
   file >> m_landscape;
   if (version >= VER_AWScene_2)
   {
      file >> m_loopTime;
   }
   if (version >= VER_AWScene_7)
   {
      file >> m_bgCol;
      setUseFog(m_fogList.getNum()>0 ? 0 : -1);
   }//if (version >= VER_AWScene_7)
   else
   {  //should default to this anyway, but may as well
      m_bgCol.setVal(AWPoint(0.0f, 0.0f, 0.0f));
   }
   //the auto cube maps collection is not filled until
   //loadm time to avoid unecessary data file bloat
   buildAutoCubeList();
   //same with list of lights that act as projectors
   buildProjectorList();
}//void	AWScene::restoreBody(AWFile& file, unsigned int version)


AWBoolean
AWScene::load(const AWCString& fullPath)
{	//want to open the file for binary READ ONLY
   m_name = fullPath;
   AWFile inWave(m_name, "rb");
   inWave >> *this;
   return TRUE;
}//AWBoolean AWScene::load(const AWCString& fullPath)



AWBoolean 
AWScene::save(const AWCString& fullPath)
{//use internal m_filename if "" supplied here
   AWBoolean retVal = FALSE;
   AWCString useName(fullPath);
   if (!useName.length())
   {
      useName = m_name;
   }
   
   AWFile of(useName, "wb+");
   if (retVal = of.isValid())
   {
      of << *this;
      of.Flush();
   }//if (retVal = of.isValid())
   return retVal;
}//AWBoolean AWScene::save(const AWCString& fullPath)



//////////////////////////////////////////////////////////
//
//AWPersistable virtual persistence functions
//
//for class:  AWScene
//
//////////////////////////////////////////////////////////
void	
AWScene::restoreGuts(AWFile& file)
{
   unsigned int version;
   restoreVersion(file, &version);   // Get the saved version #
   if (version <= VER_AWScene_8)
   {
      m_numSections = 1;
      AWPersistable::restoreGuts(file);
      if (version < VER_AWScene_4)
      {
         restoreBody(file, version);
      }//if (version < VER_AWScene_4)
      else if (version >= VER_AWScene_4)
      {
         file >> m_numSections;//MUST BE POSITIVE INTEGER > 0
         if (m_numSections == 1)
         {
            restoreBody(file, version);
         }
         else if ((m_targetSection>0) && (m_targetSection<=m_numSections))
         {   //there should be a section map in this file
            //containing m_numSections 'long' values
            long* sectionOffsets = new long [m_numSections];
            file.Read(sectionOffsets, m_numSections);
            //now we can get the shared texture collection
            AWOrdered shareTex;
            shareTex.read(file, version < VER_AWScene_6);
            shareTex.setOwnsObjects(FALSE);
            //OK, so we should now be able to get the
            //offset for the start of the desired section
            if (file.SeekTo(sectionOffsets[m_targetSection-1]))
            {//we should now be positioned at the start of the data
               restoreBody(file, version);
            }//if (file.SeekTo(sectionOffsets[m_targetSection-1]))
            int foundIndex = 0;
            for (int tex=0; tex<shareTex.getNum(); tex++)
            {
               foundIndex = m_textureList.find(shareTex[tex]);
               if (AW_NPOS == foundIndex)
               {  //remove any shared textures that are not in fact used
                  //in this section of the scene - don't need 'em
                  delete shareTex[tex];
                  shareTex[tex] = NULL;
               }//if (AW_NPOS == foundIndex)
            }//for (int tex=0; tex<(int)shareTex.entries(); tex++)
            shareTex.destroy();
         }//else if ((m_targetSection>0) && (m_targetSection<=m_numSections))
      }//else if (version >= VER_AWScene_4)    
	}//if (version <= VER_AWScene_7)
}//void	AWScene::restoreGuts(AWFile& file)




void
AWScene::saveBody(AWFile& file) const
{
   file << m_nodeList;
   file << m_textureList;
   file << m_materialList;
   file << m_fogList;
   file << m_ambient;
   file << m_landscape;
   file << m_loopTime;
   file << m_bgCol;
}//void AWScene::saveBody(AWFile& file)



void                
AWScene::saveStart(AWFile& file)const
{
   saveVersion(file, uVersion_ );	// version number
   AWPersistable::saveGuts(file);
}//void AWScene::saveStart(AWFile& file)const



void        
AWScene::saveGuts(AWFile& file) const
{
   saveStart(file);
   file << m_numSections;
   saveBody(file);
}//void AWScene::saveGuts(AWFile& file) const




AWspace     
AWScene::binaryStoreSize() const
{
	return sizeof uVersion_ +
         AWPersistable::binaryStoreSize() +
         m_nodeList.recursiveStoreSize() +
         m_textureList.getStoreSize() +
         m_materialList.getStoreSize() +
         m_fogList.getStoreSize() +
         m_ambient.binaryStoreSize() +
         m_bgCol.binaryStoreSize() +
         sizeof (m_loopTime) +
         sizeof (m_numSections) +
         AWPERSISTABLE_POINTER_SIZE(m_landscape);
}//AWspace AWScene::binaryStoreSize() const