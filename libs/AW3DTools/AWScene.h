#ifndef __AWSCENE_H
#define __AWSCENE_H

#define VER_AWScene_1	100

#include "AWObject.h"
#include "AWCamera.h"
#include "AWWorker.h"
#include "AWCubeGen.h"
#include "AWLight.h"


class AWExport AWScene : public AWPersistable
{
public:
	AWDECLARE_VERSIONABLE_PERSISTABLE(AWScene);

	AWScene();
	~AWScene();

	void	            clearAndDestroy();
	virtual void      compact();

   void              update(float pitch, float yaw, float roll, 
                            float viewPitch, float viewYaw, float speed, 
                            float time);
   void              calcTextures(float t);

	void	            center(const AWPoint& origin);
	void	            fitToRange(float x, float y = -1, float z = -1);

	int		         getNumOpaqueObjects();
	int		         getNumTextures();
	void	            setCamera(AWCamera* cam);

   static AWCString  getSceneName(const AWCString& extendedName, AWBoolean removeExtension=TRUE);
   static AWCString  getSceneDirectory(const AWCString& scenePath);

   AWMaterial*       getDefaultMaterial();
   void              grabMaterials(AWNode* n);//YUK YUK YUK

   int               getFogIndex(const AWCString& name);

   AWAnimatedPoint&  getAmbient()             {return m_ambient;}
   AWBoolean	      isAmbientAnimated()				{return m_ambient.isAnimated();}
   
	void              getAmbient(AWPoint& col)	    {col=m_ambient.getVal();}
	void		         setAmbient(const AWPoint& val) {m_ambient.setVal(val);}
   void		         addAmbient(const AWPoint& col, float t)	
												{m_ambient.addKey(col, t);}

   AWBoolean	      isBackgroundAnimated()		 {return m_bgCol.isAnimated();}
   AWAnimatedPoint&  getBackground()             {return m_bgCol;}
   void              getBackground(AWPoint& col)	    {col=m_bgCol.getVal();}
	void		         setBackground(const AWPoint& val) {m_bgCol.setVal(val);}
   void		         addBackground(const AWPoint& col, float t)	
												{m_bgCol.addKey(col, t);}
	//const AWPoint& getAmbient()						{return m_ambientLight;}
	//void		   setAmbient(const AWPoint& val)	{m_ambientLight=val;}

	const AWPoint&    getFogColour()					{return m_fogColour;}
	void		         setFogColour(const AWPoint& val)	{m_fogColour=val;}

	float		         getFogStart()					{return m_fogStart;}
	void		         setFogStart(float val)			{m_fogStart=val;}

	float		         getFogEnd()						{return m_fogEnd;}
	void		         setFogEnd(float val)				{m_fogEnd=val;}

	int			      getUseFog()						{return m_useFog;}
	void		         setUseFog(int val);

	int			      getFogType()						{return m_fogType;}
	void		         setFogType(int val)				{m_fogType=val;}

	float		         getLoopTime()					{return m_loopTime;}
	void		         setLoopTime(float loopTime)		{m_loopTime=loopTime;}

   void              buildAutoCubeList();
   void              buildProjectorList();

   AWBoolean         load(const AWCString& fullPath);
   AWBoolean         save(const AWCString& fullPath);


   void              restoreBody(AWFile& file, unsigned int version);
   virtual void      restoreGuts(AWFile& file);

   void              saveStart(AWFile& file)const;
   void              saveBody(AWFile& file)const;
   virtual void      saveGuts(AWFile& file) const;

   virtual AWspace   binaryStoreSize() const;

protected:
   //global ambient light settings
   AWAnimatedPoint   m_ambient;
    //scene background color - defaults to black
   AWAnimatedPoint   m_bgCol;
   //fog settings (can be generalised to a collection of fog objects
   //allowing switching between pre-chosen settins depending on scene 
   //state) for now, use exponential fog, one setting.
   int			      m_useFog;
   AWPoint		      m_fogColour;
   float		         m_fogStart;
   float		         m_fogEnd;
   int			      m_fogType;

public:
   AWMaterial*          m_defaultMaterial;
   AWCString            m_name;
   AWPtrList<AWLight*>   m_shadowers;
   AWPtrList<AWLight*>   m_projectors;
   AWPtrList<AWCubeGen*> m_autoCubes;
	AWOrdered         m_textureList;
   AWOrdered	      m_materialList;
	AWOrdered	      m_fogList;
	AWWorker	         m_nodeList;
	float		         m_loopTime;

   int               m_numSections; //for saver game 1 composite file PERSIST
   int               m_targetSection;//which to load in restore guts NO_PERSIST

   AWObject*	      m_landscape;
};//class AWScene	 : public AWPersistable

#endif //sentinel __AWSCENE_H