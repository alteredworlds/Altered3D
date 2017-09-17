#include "AWToolsCommon.h"
#include <math.h>
#include <time.h>

#include "AWGL1Renderer.h"
#include "AWFlightController.h"
#include "AWFace.h"
#include "AWLight.h"
#include "AWMaterial.h"
#include <OpenGLES/ES1/glext.h>


#define FRAME_STEP (1.0f / 24.0f)

AWBoolean   AWGL1Renderer::m_useAlphaTest = TRUE;


AWGL1Renderer::AWGL1Renderer()
	:  
      m_selectedMaterial(NULL),
      m_useBump(FALSE),
      m_useCubeEnv(FALSE),
      m_hasBlendColor(FALSE),
      m_hasDestAlpha(FALSE),
      m_hasVertexPrograms(FALSE),
      m_hasTextureShaders(FALSE)
{
    resetData();
}


AWGL1Renderer::~AWGL1Renderer()
{
}//AWGL1Renderer::~AWGL1Renderer()


//used to reset data in saver when new scene is loaded
//we keep the same HWND & Window, but kill the old GL Context
//and re-init a new one
void
AWGL1Renderer::resetData()
{
    AWGLRenderer::resetData();
	m_useAlphaTest = TRUE;
	m_selectedMaterial = NULL;	
	m_captureFrame = 0;	
	m_transformReflections = TRUE;
	m_applyObjectTextures = TRUE;  
}//void AWGL1Renderer::resetData()


void
AWGL1Renderer::getExtensions()
{	
	int num;
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &num);
	setNumTexUnits(num);
	AWTRACE1("  Number of texture units:\t%i\n", (int)num);
		
	//check to see if DESTINATION ALPHA supported
	GLint alphaBits;
	glGetIntegerv(GL_ALPHA_BITS, &alphaBits);
	m_hasDestAlpha = (alphaBits > 0);
	AWTRACE1("  Destination ALPHA supported:\t%i\n", (int)m_hasDestAlpha);
}//void AWGL1Renderer::getExtensions()


AWBoolean
AWGL1Renderer::initTextures()
{
	AWTRACE("\n");
	AWBoolean retVal = TRUE;
	glEnable(GL_TEXTURE_2D);
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	int numMaterials = getScene().m_textureList.getNum();
	for (int i = 0; i < numMaterials; i++)
	{
		AWTexture* mat = (AWTexture*)getScene().m_textureList[i];
		AWTRACE2("Texture: %s has %d frames\n", mat->m_name.c_str(), (int)mat->getNumFrames());
		for (int frame = 0; frame < mat->getNumFrames(); frame++)
		{
			AWTexture* curFrame = mat->getTexFromArrayIndex(frame);
			initTexture(curFrame);
			AWTRACE2("  init frame: %i with ID %d\n", frame, (int)curFrame->m_texID);
		}//for (int frame = 0; frame < mat->getNumFrames(); frame++)
	}//for (int i = 0; i < numMaterials; i++)
	AWTRACE("\n");
	return retVal;
}//AWBoolean AWGLRenderer::initTextures()


//converts texture format (eg GL_RGB) into correct form for 
//'internal format' parameter to glTexImage2D (eg GL_RGB8)
// for OpenGL ES 1.1 the acceptable return values are:
// GL_ALPHA, GL_RGB, GL_RGBA, GL_LUMINANCE, or GL_LUMINANCE_ALPHA
int            
AWGL1Renderer::getInternalFormat(int texFormat, AWBoolean allowCompression)
{
	int internalFormat = texFormat;
	switch (internalFormat)
	{
		case GL_RGB:
			internalFormat = GL_RGB;
			break;
			
		case GL_RGBA:
			internalFormat =GL_RGBA;
			break;
			
		default:
			break;
	}//switch (internalFormat)
	return internalFormat;
}//int AWGLRenderer::getInternalFormat(int texFormat, AWBoolean allowCompression)


AWBoolean
AWGL1Renderer::setupTexture(AWTexture* t, AWBoolean allowAutoMipmap)
{
	glEnable(t->m_target);
	glGenTextures(1, &t->m_texID);
	glBindTexture(t->m_target, t->m_texID);
	
	AWTRACE4(" Texture init %s: %s  ID: %i  mipMap: %i\n", 
			 t->getTargetName(), t->m_name.c_str(), 
			 (int)t->m_texID, (int)t->m_mipMap);
	glTexParameteri(t->m_target, GL_TEXTURE_WRAP_S, t->m_wrap);
	glTexParameteri(t->m_target, GL_TEXTURE_WRAP_T, t->m_wrap);
	glTexParameteri(t->m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(t->m_target, GL_TEXTURE_MIN_FILTER, 
					t->m_mipMap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	if (allowAutoMipmap && t->m_mipMap)
        glTexParameteri(t->m_target, GL_GENERATE_MIPMAP, GL_TRUE);
	//
	return (0 == (m_glErr = glGetError()));
}//AWBoolean AWGLRenderer::setupTexture(AWTexture* t, AWBoolean allowAutoMipmap)


void           
AWGL1Renderer::init2DTexture(AWTexture* tex)
{
	if (tex && tex->m_width && tex->m_height && (GL_TEXTURE_2D==tex->m_target))
	{
		setupTexture(tex);     
		// AUTO MIPMAP GENERATION enabled...
		glTexImage2D( tex->m_target, 0, getInternalFormat(tex->m_format),
					 tex->m_width, tex->m_height,
					 0, tex->m_format, tex->getDataType(), tex->getDataPtr() );
		m_glErr = glGetError();
        
		//can now delete the loaded texture to conserve memory useage
		delete [] tex->getDataPtr();
		tex->getDataPtr() = NULL;
	}//if (tex && tex->m_width && tex->m_height)
}//void AWGLRenderer::init2DTexture(AWTexture* tex)


void            
AWGL1Renderer::initCubeMapTexture(AWCubeEnvTexture* f, AWBoolean allowCompression)
{
    
}


void           
AWGL1Renderer::initTexture(AWTexture* tex)
{
	if (!tex) return;
	//perform any required pre-calc / init / whatever
	tex->init();
	if (CLSID_AWCUBEENVTEXTURE == tex->isA())
	{
		initCubeMapTexture((AWCubeEnvTexture*)tex);
	}
    else if (GL_TEXTURE_2D==tex->m_target)
	{
		init2DTexture(tex);
	}
}//void AWGLRenderer::initTexture(AWTexture* tex)



void
AWGL1Renderer::setFogParams()
{	//only called if a state change is required and on initialisation
	//apply the scene background colour unless fog is being used.
    //
    int useFog = getScene().getUseFog();
    setUsingFog(userAllowsFog() && (-1 != useFog));
	if (usingFog())
	{
		AWPoint awcol = getScene().getFogColour();
		m_background[0]=awcol.x;m_background[1]=awcol.y;
		m_background[2]=awcol.z;m_background[3]=1.0f;
        //
		glEnable(GL_FOG);
		AWTRACE4("Enable GL_FOG colour: (%5f, %5f, %5f, %5f)\n", (double)m_background[0], (double)m_background[1], (double)m_background[2], (double)m_background[3]);
		//glHint(GL_FOG_HINT, GL_FASTEST);
		switch (getScene().getFogType())
		{
			case 1:
				glFogf(GL_FOG_MODE, GL_EXP);
				glFogf(GL_FOG_DENSITY, getScene().getFogStart());
				break;
			case 2:
				glFogf(GL_FOG_MODE, GL_EXP2);
				glFogf(GL_FOG_DENSITY, getScene().getFogStart());
				break;
			default:
				glFogf(GL_FOG_MODE, GL_LINEAR);
				glFogf(GL_FOG_START, getScene().getFogStart());
				glFogf(GL_FOG_END,   getScene().getFogEnd());
				break;
		}//switch
		glFogfv(GL_FOG_COLOR, m_background);
        //
        AWTRACE4("Setting clear colour: (%5f, %5f, %5f, %5f)\n", (double)m_background[0], (double)m_background[1], (double)m_background[2], (double)m_background[3]);
        glClearColor(m_background[0], m_background[1], m_background[2], m_background[3]);

	}
	else
	{	//back to the default clear colour?
		AWTRACE("Disable GL_FOG\n");
		glDisable(GL_FOG);
	}
}//void AWGL1Renderer::setFogParams()


AWBoolean
AWGL1Renderer::selectCamera(AWCamera* cam)
{  //incoming camera may be NULL, which means we should
   //use the current scene camera
	AWCamera* curCam = cam ? cam : getScene().m_nodeList.getCurCamera();
	if (curCam)
	{
        glViewport(0, 0, (GLsizei)m_width, (GLsizei)m_height);
		glMatrixMode(GL_PROJECTION);
        AWMatrix4 projTM;
		float ymax = curCam->m_nearClip * tan( curCam->m_fov * M_PI / 360.0);
		float xmax = ymax * m_aspect;
        projTM.frustum(-xmax, xmax, -ymax, ymax, curCam->m_nearClip, curCam->m_farClip);
        glLoadMatrixf(projTM.m_mat);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}//if (curCam)
	return (NULL != curCam);
}//AWBoolean AWGL1Renderer::selectCamera(AWCamera* cam)


void
AWGL1Renderer::initializeGL()
{
    m_driverVendorName = (char*)glGetString(GL_VENDOR);
	AWTRACE1("\n\nOpenGL Vendor: %s  ", m_driverVendorName.c_str());
	AWTRACE1("Version %s\n", glGetString(GL_VERSION));
	AWTRACE1("Supported Extensions: %s\n\n\n", glGetString(GL_EXTENSIONS));
	m_glErr = glGetError();
    //
	getScene().m_nodeList.setUseAlphaTest(m_useAlphaTest);
    getExtensions();
	resetToDefaults();
	//clear colour set as part of setFogParams
	glClearDepthf(1.0);
	glShadeModel(GL_SMOOTH);
	
	
	initLights();
	
	setDepthFunc(GL_LEQUAL);
	enableDepthTest(TRUE);
	m_glErr = glGetError();
	
	initTextures();//calls glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	
	m_glErr = glGetError();
	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	m_glErr = glGetError();
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	m_glErr = glGetError();
	
	resizeGL(m_width, m_height);
	
	m_glErr = glGetError();
	
	glLineWidth(getLineWidth());
    //
	//if fog is enabled, the clear colour will be changed later on to match the fog colour
	//otherwise the scene value will be applied (gl default for older scenes)
	getScene().getBackground(*(AWPoint*)m_background);
	m_background[3]=0.0f;
	
	m_glErr = glGetError();
}//void AWGL1Renderer::initializeGL()


void
AWGL1Renderer::shutdownGL()
{  //shut off any gl states or whatever
	//for example, need to delete all the texture objects
	if (!m_scene)
		return;
	int i = 0;
	int numTexturesDeleted = 0;
	for (i = 0; i < getScene().m_textureList.getNum(); i++)
	{
		AWTexture* curFrame = NULL;
		AWTexture* mat      = (AWTexture*)getScene().m_textureList[i];
		for (int frame = 0; frame < mat->getNumFrames(); frame++)
		{  //delete each texture object in turn
			curFrame          = mat->getTexFromArrayIndex(frame);
			AWTRACE1(" Delete texture ID: %i\n", (int)curFrame->m_texID);
			glDeleteTextures(1, &curFrame->m_texID);
			curFrame->m_texID = 0;
			numTexturesDeleted++;
		}//for (int frame = 0; frame < mat->getNumFrames(); frame++)
	}//for (int i = 0; i < getScene().getNumTextures(); i++
	AWTRACE1("Deleted %i textures\n", numTexturesDeleted);
	
	if (usingLighting())
	{
        int numLights = getScene().m_nodeList.getNumLights();
        for (int light=0; light<numLights; light++)
        {
            AWTRACE1("Disable GL_LIGHT %i\n", (int)light);
            glDisable(GL_LIGHT0 + light);
        }//for (int light=0; light<numLights; light++)
        float def[] = {0.2f, 0.2f, 0.2f, 1.0f};
        getScene().setAmbient(AWPoint(0.2f, 0.2f, 0.2f));
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT , def);//back to default
        //
		AWTRACE("Disable GL_LIGHTING\n");
		glDisable(GL_LIGHTING);
		glDisable(GL_NORMALIZE);
        setUsingLighting(FALSE);
	}

	glDisable(GL_FOG);
	disableTexUnits();
	setVertexArray(NULL);
	setVertexColorArray(NULL);
	setNormalArray(NULL);
}//void AWGL1Renderer::shutdownGL()


void
AWGL1Renderer::transformLight(int lightID, AWLight* curLight)
{
	if (curLight)
	{
		float pos[]	 = {0.0f, 0.0f, 0.0f, 1.0f};
		float dir[]	 = {0.0f, 0.0f, 1.0f, 0.0f};
		float dirSpot[] = {0.0f, 0.0f, -1.0f};
		glPushMatrix();
		transformNode(curLight);
		switch (curLight->m_type)
		{
			case AWLight::AW_POINT_LIGHT:
				//use GL_POSITION with w=1 to specify POSITION
				glLightfv(lightID, GL_POSITION, pos);
				break;
			case AWLight::AW_DIRECTIONAL_LIGHT:
				//use GL_POSITION with w=0 to specify DIRECTION
				glLightfv(lightID, GL_POSITION, dir);
				break;
			case AWLight::AW_SPOT_LIGHT:
				//specify direction
				glLightfv(lightID, GL_SPOT_DIRECTION, dirSpot);
				//use GL_POSITION with w=1 to specify position
				glLightfv(lightID, GL_POSITION, pos);
				break;
		}//switch (curLight->m_type
		glPopMatrix();
	}//if (curLight)
}//void AWGL1Renderer::transformLight(int lightID, )


void
AWGL1Renderer::transformLights()
{	//designed to cope with animated lights - apply settings per frame
	AWPoint col;
	int numLights = getScene().m_nodeList.getNumLights();
	if (getScene().isAmbientAnimated())
	{
		AWTRACE3("Setting animated AMBIENT light to (%f, %f, %f)\n", (double)m_ambient[0], (double)m_ambient[1], (double)m_ambient[2]);
		getScene().getAmbient(*(AWPoint*)m_ambient);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, m_ambient);
	}//if (getScene().isAmbientAnimated())
	for (int light = 0; light < numLights; light++)
	{
		AWLight* curLight = getScene().m_nodeList.getLight(light);
		if (curLight && curLight->m_on)
		{
			transformLight(curLight->m_ID, curLight);
			if (curLight->isColorAnimated())
			{
				curLight->getColor(col);
				float color[] = {col.x, col.y, col.z, 1.0f};
				glLightfv(curLight->m_ID, GL_AMBIENT, color);
				glLightfv(curLight->m_ID, GL_DIFFUSE, color);
			}//if (curLight->isColorAnimated())
		}//if (curLight && curLight->m_on)
	}//for (int light = 0; light < numLights; light++)
}//void AWGL1Renderer::transformLights()


void
AWGL1Renderer::initLights()
{
    setUsingLighting(userAllowsLighting() && (getScene().m_nodeList.getNumLights() > 0));
    if (usingLighting())
    {
		AWTRACE("Enable GL_LIGHTING\n");
		glEnable(GL_LIGHTING);
		glEnable(GL_NORMALIZE);
        //
        // GLOBAL scene values
        //
        // initialise the scene's global ambient light settings
        //  ambient used in gl lighting and in bump mapping
        getScene().getAmbient(*(AWPoint*)m_ambient);
		m_ambient[3]= 1.0f;
		AWTRACE3(" supplying Ambient (%f, %f, %f)\n", (double)m_ambient[0], (double)m_ambient[1], (double)m_ambient[2]);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, m_ambient);
        //
        //static const float specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
        int numLights = getScene().m_nodeList.getNumLights();
        for (int light = 0; light < numLights; light++)
        {
            AWPoint  colour;
            int      lightID = GL_LIGHT0 + light;
            AWLight* curLight = getScene().m_nodeList.getLight(light);
            if (curLight && curLight->m_on)
            {
                curLight->getColor(colour);
                float    diffuse[] = {colour.x, colour.y, colour.z, 1.0f};
                
                curLight->m_ID	= lightID;
                AWTRACE1(" Enable GL_LIGHT %i\n", light);
                glEnable(lightID);
                glLightfv(lightID, GL_AMBIENT, diffuse);
                glLightfv(lightID, GL_DIFFUSE, diffuse);
                glLightfv(lightID, GL_SPECULAR, diffuse);
                switch (curLight->m_type)
                {
                    case AWLight::AW_SPOT_LIGHT:
                        glLightf(lightID, GL_SPOT_CUTOFF, curLight->m_cutOffAngle);
                        break;
                    default:
                        break;
                }//switch (curLight->m_type
            }//if (curLight && curLight->m_on)
            //The default ambient light intensity is (0.0, 0.0, 0.0, 1.0).
            //The default diffuse intensity is (0.0, 0.0, 0.0, 1.0) for all lights other
            //than light zero. The default diffuse intensity of light zero is
            //(1.0, 1.0, 1.0, 1.0).
        } //for (int light = 0; light < numLights; light++)
	}
}//void AWGL1Renderer::initLights()


void
AWGL1Renderer::displayEnvironment()
{
	AWObject* env = getScene().m_nodeList.getEnvironment();
	if (env)
	{
		enableDepthTest(FALSE);
		if (usingLighting()) 
			glDisable(GL_LIGHTING);
		drawObject(env);
		if (usingLighting()) 
			glEnable(GL_LIGHTING);
		enableDepthTest(TRUE);
	}//if (env)
}//void  AWGL1Renderer::displayEnvironment()


void
AWGL1Renderer::displayFrame()
{
	AWTRACEHI("\n\n\nSTART FRAME\n");
	m_trisThisFrame=0;
	
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_glErr = glGetError();
	glPushMatrix();
	// VIEWPORT TRANSFORMATION
	//
	transformViewport(getScene().m_nodeList.getViewTransformer());
	
	//DISPLAY BACKGROUND
	displayEnvironment();
	// DISPLAY SCENE CONTENTS
	//
	transformLights();
	
	showAllOpaqueObjects();
	//now draw ALPHA TEST objects
	if (getScene().m_nodeList.getNumAlphaTestObjects())
	{
		showAllAlphaTestObjects();
		setAlphatestEnabled(FALSE);
	}
	//render any ALPHA BLENDED objects
	if (getScene().m_nodeList.getNumTransparentObjects())
	{
		glDepthMask(GL_FALSE);
		showAllTransparentObjects();
		glDepthMask(GL_TRUE);
		enableBlend(FALSE);
	}  
	disableTexUnits();
	//
	glPopMatrix();
	//
	AWTRACEHI1("\nEND FRAME - displayed %i triangles\n\n\n\n", m_trisThisFrame);
}//void AWGL1Renderer::displayFrame()


void
AWGL1Renderer::transformViewport(AWTransformController* node)
{	// VIEWPORT TRANSFORMATION
	if (node)
	{	//wsTM of nodes in the scene does not include the viewTM
		//so need to 'prime' the gl stack with the viewTM
		node->getViewTM(m_viewTM);
		glLoadMatrixf(m_viewTM.m_mat);
	}//if (node)
}//void AWGL1Renderer::transformViewport(AWTransformController* node)


void
AWGL1Renderer::applyMaterial(AWMaterial* mat)
{
	float stupid[] = {0.0f, 0.0f, 0.0f, 1.0f};
	GLenum faces = GL_FRONT_AND_BACK;
	if (mat && !mat->getUseDefaults())
	{
		if (m_selectedMaterial != mat)
		{
			AWTRACEHI1("Apply Material %i\n", (int)mat);
#ifdef __AWTRACEHI
			mat->dump(); 
#endif
			m_selectedMaterial = mat;
			faces = mat->m_twoSided ? GL_FRONT_AND_BACK : GL_FRONT;
			
			mat->m_ambient.assignToFloat4(stupid);
			glMaterialfv(faces, GL_AMBIENT, stupid);
			
			mat->m_diffuse.assignToFloat4(stupid);
			glMaterialfv(faces, GL_DIFFUSE, stupid);
			
			mat->m_specular.assignToFloat4(stupid);
			glMaterialfv(faces, GL_SPECULAR, stupid);
			
			mat->m_emissive.assignToFloat4(stupid);
			glMaterialfv(faces, GL_EMISSION, stupid);
			glMaterialf(faces,  GL_SHININESS, mat->m_shininess);
		}//if (m_selectedMaterial != mat)
	}//if (mat)
	else if (m_selectedMaterial)//must apply default values
	{ 
		AWTRACEHI("Apply Default Material settings\n");
		m_selectedMaterial = NULL;
		float shiny = 0.0f;
		stupid[0]=0.2f; stupid[1]=0.2f; stupid[2]=0.2f; stupid[3]=1.0f;
		glMaterialfv(faces, GL_AMBIENT, stupid);
		
		stupid[0]=0.8f; stupid[1]=0.8f; stupid[2]=0.8f; stupid[3]=1.0f;
		glMaterialfv(faces, GL_DIFFUSE, stupid);
		
		stupid[0]=0.0f; stupid[1]=0.0f; stupid[2]=0.0f; stupid[3]=1.0f;
		glMaterialfv(faces, GL_SPECULAR, stupid);
		glMaterialfv(faces, GL_EMISSION, stupid);		
		glMaterialf(faces,  GL_SHININESS, shiny);
	}
}//void AWGL1Renderer::applyMaterial(AWMaterial* mat)


void
AWGL1Renderer::applyTextureState(AWTextureUnit* texUnit, int unitID)
{  //ensure gl call made only if mode *CHANGES*
	m_glErr = glGetError();
	if (!texUnit || !texUnit->m_tex) return;
	
	AWTexture* tex    = texUnit->m_tex;
	setTexFunc(*texUnit, unitID);
	//setTexGenMode(tex->m_genMode, unitID);
	//now alter TEXTURE MATRIX if required
	AWMatrix4 texMatrix; //constructor inits to identity
	if ( ((AWTexture::REFLECT == tex->m_genMode)||(AWTexture::SPHERE == tex->m_genMode)) 
		&& m_transformReflections)
	{  //NOTE - can rotate an (s,t,r) vector from one space to another:
		//if cube map texture is oriented in world space
		//and M is the matrix transform that moves from world space
		//to eye coordinates, you can load the inverse of the
		//affine portion of M into the texture matrix to
		//rotate eye-space reflection or normal vectors generated by
		//GL_REFLECTION_MAP_EXT or GL_NORMAL_MAP_EXT
		//back into world space.
		
		//***************BUG - NVIDIA BUG***********
		//DISPLAY LISTS CRASH...
		texMatrix = m_viewTM;
		texMatrix.invertAffine(); 
		setTexMatrix(texMatrix, unitID);
	}//if ((AWTexture::REFLECT == tex->m_genMode) && m_transformReflections)
	else
	{
		setTexMatrixIdentity(unitID);
	} 
	m_glErr = glGetError();
}//void AWGL1Renderer::applyTextureState(AWTextureUnit* texUnit, int unitID)


void           
AWGL1Renderer::selectTexture(AWTexture* tex, int unitID)
{
	if (!tex) return;
	int  texID  = tex->getTextureID(); 
	
	AWTRACEHI1("Activate texUnit: %i - ", unitID);
	glActiveTexture(GL_TEXTURE0 + unitID);
	
	int test = getSelectedTex(unitID);
	if ( (test != texID) || (AW_NO_TEXTURE == getTexMode(unitID)) )
	{
		setSelectedTex(texID, unitID);
		setTexMode(tex->m_target, unitID);
		glBindTexture(tex->m_target, texID);
		AWTRACEHI3(" bind %s ID %d %s\n", tex->getTargetName(), texID, tex->m_name.c_str());  
	}//if ( (test != texID) || (AW_NO_TEXTURE == getTexMode(unitID)) )
}//void AWGL1Renderer::selectTexture(AWTexture* tex, int unitID)


void
AWGL1Renderer::disableTexUnits(int startunit)
{
	for (int i=startunit; i<getNumTexUnits(); i++)
	{ 
		if (AW_NO_TEXTURE == getTexMode(i))
		{
			AWTRACEHI1("TexUnit %i already disabled\n", i);
		}
		else
		{
			AWTRACEHI1("Disable texUnit %i\n", i);
			glActiveTexture(GL_TEXTURE0 + i);
			setTexMode(AW_NO_TEXTURE, i);
			setSelectedTex(-1, i);
			setTexCoordArray(NULL, i, 2);   
		}
	}//for (i=numTex; i<MAX_TEX_UNITS; i++)
}//void AWGL1Renderer::disableTexUnits(int startunit)


void
AWGL1Renderer::setTextureState(AWRenderPass* pass)
{
	//set correct states for as many textures as we can render in
	//given render pass. Max number == NUM TEX UNITS for given platform
	//we need to be able to determine the correct textures for
	//UNIT0 and UNIT1 for current render pass
	int            i           = 0;
	AWTextureUnit* texUnit     = NULL;
	int            numTexUnits = pass->getNumTexUnits();  
	for (i=0; i<numTexUnits; i++)
	{    
		texUnit = pass->getTexUnit(i);
		if (texUnit)
		{			
			selectTexture(texUnit->m_tex, i);
			applyTextureState(texUnit, i);
			m_glErr = glGetError();
		}//if (tex)
	}//for (int i=0; i<numTex; i++)
	disableTexUnits(numTexUnits);
}//void AWGL1Renderer::setTextureState(AWRenderPass* pass)


int
AWGL1Renderer::getNumPasses(AWObject* o)
{
	int retVal = 0;
	if (o)
	{
		retVal = o->getNumPasses();
		AWTRACEHI2("\nOBJECT: %s has %i passes\n", o->getName().c_str(), retVal);
		if (0==retVal)
		{  // first attempt at drawing any object results in preparation for rendering.
			m_effectMgr.prepareForRendering(*this, o);
			retVal = o->getNumPasses();
			if (0==retVal)
			{
				o->setIsVisible(FALSE);
			}
		}
	}
	return retVal;
}//int AWGL1Renderer::getNumPasses(AWObject* o)


void
AWGL1Renderer::setBlendState(AWRenderPass* pass)
{
	if (!pass) return;
	int   blendType = pass->getBlendType();
	int   srcFac    = pass->getSrcFac();
	int   dstFac    = pass->getDstFac();
	int   depthFunc = pass->getDepthFunc();
	
	setDepthFunc(depthFunc);
	if (AWTexture::ALPHA_BLEND == blendType)
	{
		enableBlend(TRUE);
		setBlendFac(srcFac, dstFac);
	}//else if (AWTexture::ALPHA_BLEND == blendType)
	else
	{
		enableBlend(FALSE);
	}
	setAlphatestEnabled(AWTexture::ALPHA_TEST == blendType);
}//void AWGL1Renderer::setBlendState(AWRenderPass* pass)


void
AWGL1Renderer::drawObject(AWObject* theObj)
{
	if (theObj && theObj->getIsVisible())
	{
		AWRenderPass* curPass = NULL;
		int numPasses = getNumPasses(theObj);
		if (0==numPasses) 
			return;
		if (!m_applyObjectTextures) 
			numPasses = 1;
		int numTris = theObj->getNumTris();
		m_trisThisFrame += numTris;
		glPushMatrix();			//preserve the viewTM
		transformNode(theObj);	//apply the object wsTM
		for (int pass=0; pass<numPasses; pass++)
		{
			AWTRACEHI2("Rendering PASS %i (%i triangles)\n", pass, numTris);
			curPass = theObj->setPass(pass); 
			
			if (m_applyObjectTextures)
			{
				setBlendState(curPass); 
				setTextureState(curPass);
			}
			
			AWTRACEHI1("Drawing: %s\t", theObj->getName().c_str());
			if (theObj->getIndexArray())
			{
				AWTRACEHI(" (VertArray)\n");
				drawObjectAsVertArray(theObj, curPass);
			}
		}
		glPopMatrix();	//restore the viewTM
		AWTRACEHI1("Finished drawing %s\n", theObj->getName().c_str());
	}//if (theObj && theObj->getIsVisible())
}//void AWGL1Renderer::drawObject(AWObject* theObj)


void
AWGL1Renderer::applyObjectState(AWObject* theObj)
{
	if (theObj->isTwoSided()) 
        glDisable(GL_CULL_FACE);
	if (usingLighting() && !theObj->getDynamicLighting()) 
	{
		AWTRACEHI("Disable GL_LIGHTING\n");
		glDisable(GL_LIGHTING);
	}
}//void AWGL1Renderer::applyObjectState(AWObject* theObj)


void
AWGL1Renderer::restoreObjectState(AWObject* theObj)
{
	if (theObj->isTwoSided()) glEnable(GL_CULL_FACE);
	if (usingLighting() && !theObj->getDynamicLighting()) 
	{
		AWTRACEHI("Enable GL_LIGHTING\n");
		glEnable(GL_LIGHTING);
	}
}//void AWGL1Renderer::restoreObjectState(AWObject* theObj)


void
AWGL1Renderer::drawObjectAsVertArray(AWObject* theObj, AWRenderPass* pass)
{
	if (theObj && theObj->getIndexArray() && pass)
	{
		applyMaterial(theObj->getMaterial());
		applyObjectState(theObj);
		//without vertex colors, we should use white to blend with textures...
		if (!theObj->m_vertColList) 
            glColor4f(1, 1, 1, 1);
		//enable relevant array capabilities
		setVertexArray(theObj->m_vertexList);
		setVertexColorArray(userAllowsVertexColours() ? theObj->m_vertColList : NULL);
		setNormalArray(theObj->m_normalList);
		//texture coordinate arrays - note that for units not used this pass
		//the tex cooord arrays disabled by passing (NULL, unit, 0)
		for (int unit=0; unit<pass->getNumTexUnits(); unit++)
		{
			glClientActiveTexture(GL_TEXTURE0+unit);
			setTexCoordArray(m_applyObjectTextures?theObj->getTVertexList(unit):NULL, unit, theObj->getTVertexListDim(unit));   
		}
		glDrawElements(GL_TRIANGLES, theObj->getNumIndeces(), GL_UNSIGNED_SHORT, theObj->getIndexArray());
		restoreObjectState(theObj);
	}//if (theObj)
}//void AWGL1Renderer::drawObjectAsVertArray(AWObject* theObj, AWRenderPass* pass)


AWBoolean
AWGL1Renderer::usesCubeMap(AWObject* obj)
{  //ASSUME: we know we have (at least) one pass 
   //if we enter this function
	AWBoolean retVal = FALSE;
	AWRenderPass* pass = obj->getPass(0);
	for (int i=0; i<pass->getNumTexUnits(); i++)
	{
		AWTextureUnit* tUnit = pass->getTexUnit(i);
		retVal = (tUnit && tUnit->m_tex && (CLSID_AWCUBEENVTEXTURE==tUnit->m_tex->isA()));
		if (retVal) break;
	}//for (int i=0; i<pass->getNumTexUnits(); i++)
	return retVal;
}//AWBoolean AWGL1Renderer::usesCubeMap(AWObject* obj)