#include "AWToolsCommon.h"
#include <math.h>
#include <time.h>

#include "AWGL2Renderer.h"
#include "AWFlightController.h"
#include "AWFace.h"
#include "AWLight.h"
#include "AWMaterial.h"


#define FRAME_STEP (1.0f / 24.0f)
#define MAX_NUM_LIGHTS 8

AWBoolean   AWGL2Renderer::m_useAlphaTest = TRUE;



AWGL2Renderer::AWGL2Renderer()
	: m_selectedMaterial(NULL)
{
    resetData();
}


AWGL2Renderer::~AWGL2Renderer()
{
}

//used to reset data in saver when new scene is loaded
//we keep the same HWND & Window, but kill the old GL Context
//and re-init a new one
void
AWGL2Renderer::resetData()
{
    AWGLRenderer::resetData();
	m_useAlphaTest = TRUE;
	m_selectedMaterial = NULL;	
	m_captureFrame = 0;	
	m_transformReflections = TRUE;
	m_applyObjectTextures = TRUE;  
}//void AWGL2Renderer::resetData()


void                    
AWGL2Renderer::addVertexProgram(const AWCString& vertexSrc, const AWCString& fragSrc)
{
    AWVertexProgram* vProg = new AWVertexProgram();
    vProg->loadShaders(vertexSrc, fragSrc);
    addVertexProgram(vProg);
}

void
AWGL2Renderer::addVertexProgram(AWVertexProgram* vProg)
{
    m_vertexPrograms.add(vProg);
}

void
AWGL2Renderer::setFogParams()
{	//only called if a state change is required and on initialisation
	//apply the scene background colour unless fog is being used.
	getScene().getBackground(*(AWPoint*)m_background);
	m_background[3]=0.0f;
    
    setUsingFog(userAllowsFog() && (-1 != getScene().getUseFog()));
    awUniform1i(m_activeProgram->m_enable_fog, usingFog());
	if (usingFog())
	{
		AWPoint awcol = getScene().getFogColour();
		m_background[0]=awcol.x;m_background[1]=awcol.y;
		m_background[2]=awcol.z;m_background[3]=0.0f;
        //
        glUniform4fv(m_activeProgram->m_fog_color, 1, m_background);
		
		AWTRACE4("Enable GL_FOG colour: (%5f, %5f, %5f, %5f)\n",
                 (double)m_background[0],
                 (double)m_background[1],
                 (double)m_background[2],
                 (double)m_background[3]);
        
        int fogType = getScene().getFogType();
		switch (fogType)
		{
			case GLI_FOG_MODE_EXP:    // 1
                glUniform1f(m_activeProgram->m_fog_density, getScene().getFogStart());
				break;
                
			case GLI_FOG_MODE_EXP2:   // 2
                glUniform1f(m_activeProgram->m_fog_density, getScene().getFogStart());
				break;
                
            case GLI_FOG_MODE_LINEAR: // 0
			default:
				glUniform1f(m_activeProgram->m_fog_start, -1.0 * getScene().getFogStart());
				glUniform1f(m_activeProgram->m_fog_end, -1.0 * getScene().getFogEnd());
				break;
		}
        awUniform1i(m_activeProgram->m_fog_mode, fogType);
	}
	AWTRACE4("Setting clear colour: (%5f, %5f, %5f, %5f)\n",
             (double)m_background[0],
             (double)m_background[1],
             (double)m_background[2],
             (double)m_background[3]);
	glClearColor(m_background[0], m_background[1], m_background[2], m_background[3]);
}//void AWGL2Renderer::setFogParams()


AWBoolean
AWGL2Renderer::selectCamera(AWCamera* cam)
{  //incoming camera may be NULL, which means we should
   //use the current scene camera
	AWCamera* curCam = cam ? cam : getScene().m_nodeList.getCurCamera();
	if (curCam)
	{
        glViewport(0, 0, (GLsizei)m_width, (GLsizei)m_height);
		m_projectionTM.identity();
		float ymax = curCam->m_nearClip * tan(curCam->m_fov * M_PI / 360.0);
		float xmax = ymax * m_aspect;
        m_projectionTM.frustum(-xmax, xmax, -ymax, ymax, curCam->m_nearClip, curCam->m_farClip);		
        m_viewTM.identity();
	}
	return (NULL != curCam);
}//AWBoolean AWGL2Renderer::selectCamera(AWCamera* cam)


void
AWGL2Renderer::transformObject(AWNode* node)
{
	if (node)
	{
        AWMatrix4 tm;
        //
        //model matrix AKA world space transformation matrix
        node->getWSTM(tm);
        //glUniformMatrix4fv(m_activeProgram->m_model_matrix, 1, GL_FALSE, (GLfloat*)tm.m_mat);
        //
        // model-view matrix
        tm = m_viewTM * tm;
        glUniformMatrix4fv(m_activeProgram->m_modelview_matrix, 1, GL_FALSE, (GLfloat*)tm.m_mat);
        //
        // 3x3 inverse of model-view matrix
        AWMatrix3 invNodeMV;
        AWMatrix4 tmp;
        tm.invert(tmp);
        invNodeMV = tmp;
        invNodeMV.transpose();
        glUniformMatrix3fv(m_activeProgram->m_inv_modelview_matrix, 1, GL_FALSE, (GLfloat*)invNodeMV.m_mat);
        //
        // model-view-projection matrix
        tm =  m_projectionTM * tm;
        glUniformMatrix4fv(m_activeProgram->m_modelviewprojection_matrix, 1, GL_FALSE, (GLfloat*)tm.m_mat);
    }
}


AWBoolean
AWGL2Renderer::initTextures()
{
	AWTRACE("\n");
	AWBoolean retVal = TRUE;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1 );
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
}//AWBoolean AWGL2Renderer::initTextures()


//converts texture format (eg GL_RGB) into correct form for 
//'internal format' parameter to glTexImage2D (eg GL_RGB8)
// for OpenGL ES 1.1 the acceptable return values are:
// GL_ALPHA, GL_RGB, GL_RGBA, GL_LUMINANCE, or GL_LUMINANCE_ALPHA
int            
AWGL2Renderer::getInternalFormat(int texFormat, AWBoolean allowCompression)
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
}//int AWGL2Renderer::getInternalFormat(int texFormat, AWBoolean allowCompression)


AWBoolean
AWGL2Renderer::setupTexture(AWTexture* t, AWBoolean allowAutoMipmap)
{
	//glEnable(t->m_target);
	glGenTextures(1, &t->m_texID);
	glBindTexture(t->m_target, t->m_texID);
	
	AWTRACE4(" Texture init %s: %s  ID: %i  mipMap: %i\n", 
			 t->getTargetName(), t->m_name.c_str(), 
			 (int)t->m_texID, (int)t->m_mipMap);
	glTexParameteri(t->m_target, GL_TEXTURE_WRAP_S, t->m_wrap);
	glTexParameteri(t->m_target, GL_TEXTURE_WRAP_T, t->m_wrap);
	glTexParameteri(t->m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(t->m_target, GL_TEXTURE_MIN_FILTER, 
					allowAutoMipmap && t->m_mipMap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    
	return (0 == (m_glErr = glGetError()));
}//AWBoolean AWGL2Renderer::setupTexture(AWTexture* t, AWBoolean allowAutoMipmap)


void           
AWGL2Renderer::init2DTexture(AWTexture* tex)
{
	if (tex && tex->m_width && tex->m_height && (GL_TEXTURE_2D==tex->m_target))
	{
        // AUTO MIPMAP GENERATION enabled for all textures
		setupTexture(tex);
		glTexImage2D( tex->m_target, 0, getInternalFormat(tex->m_format),
					 tex->m_width, tex->m_height,
					 0, tex->m_format, tex->getDataType(), tex->getDataPtr() );
		m_glErr = glGetError();
        
        if (tex->m_mipMap)
            glGenerateMipmap(tex->m_target);
        m_glErr = glGetError();
        
		//can now delete the loaded texture to conserve memory useage
		delete [] tex->getDataPtr();
		tex->getDataPtr() = NULL;
	}//if (tex && tex->m_width && tex->m_height)
}//void AWGL2Renderer::init2DTexture(AWTexture* tex)


void            
AWGL2Renderer::initCubeMapTexture(AWCubeEnvTexture* f, AWBoolean allowCompression)
{
    
}


void           
AWGL2Renderer::initTexture(AWTexture* tex)
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
}//void AWGL2Renderer::initTexture(AWTexture* tex)


void
AWGL2Renderer::initializeGL()
{
    m_driverVendorName = (char*)glGetString(GL_VENDOR);
	AWTRACE1("\n\nOpenGL Vendor: %s  ", m_driverVendorName.c_str());
	AWTRACE1("Version %s\n", glGetString(GL_VERSION));
	AWTRACE1("Supported Extensions: %s\n\n\n", glGetString(GL_EXTENSIONS));
	m_glErr = glGetError();
    resetToDefaults();
    //
    if (0 == m_vertexPrograms.getNum())
        AWTRACE("ABORT: No vertext programs loaded for OpenGL ES 2.0 render");
    else
    {
        //
        // for now we are just using the first Vertex Program in the list - the only one
        // should shortly be drive at Object level, hence this init needs to be moved...?
        awUseProgram(m_vertexPrograms[0]);
        //
        getScene().m_nodeList.setUseAlphaTest(m_useAlphaTest);
        //
        int num;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &num);
        setNumTexUnits(num);
        AWTRACE1("  Number of texture units:\t%i\n", (int)num);
        //
        glClearDepthf(1.0);
        //glDepthFunc(GL_LEQUAL);
        enableDepthTest(TRUE);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        
        awUniform1i(m_activeProgram->m_alphaTest, 0);
        glDisableVertexAttribArray(m_activeProgram->m_colour);
        glDisableVertexAttribArray(m_activeProgram->m_normal);
        glDisableVertexAttribArray(m_activeProgram->m_position);
        glDisableVertexAttribArray(m_activeProgram->m_texCoord0);
        
        initLights();
        m_glErr = glGetError();
        //
        //if fog is enabled, the clear colour will be set to match the fog colour
        //otherwise getScene().getBackground will be applied (gl default for older scenes)
        setFogParams();
        m_glErr = glGetError();
        //
        // this needs to be switched on if using lighting or fog
        int val = (usingLighting() | usingFog()) ? 1 : 0;
        awUniform1i(m_activeProgram->m_xform_eye_p, val);
        
        initTextures();
        m_glErr = glGetError();
        
        resizeGL(m_width, m_height);	
        m_glErr = glGetError();
    }
}//void AWGL2Renderer::initializeGL()


void
AWGL2Renderer::shutdownGL()
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
	disableTexUnits();
    //
    m_activeProgram = NULL;
    m_vertexPrograms.destroy();
}//void AWGL2Renderer::shutdownGL()


void
AWGL2Renderer::enableLighting(AWBoolean on)
{
    awUniform1i(m_activeProgram->m_enable_lighting, on);
}


//
// turns on the lights, if any are to be used for this scene
void
AWGL2Renderer::initLights()
{
    setUsingLighting(userAllowsLighting() && (getScene().m_nodeList.getNumLights() > 0));
    enableLighting(usingLighting());
    if (usingLighting())
    {
        applyMaterialDefaults();
        
        // ambient light for scene...
        getScene().getAmbient(*(AWPoint*)m_ambient);
        m_ambient[3]= 1.0f;
        
        glUniform4fv(m_activeProgram->m_ambient_scene_color, 1, m_ambient);
        awUniform1i(m_activeProgram->m_normalize_normal, TRUE);
        awUniform1i(m_activeProgram->m_rescale_normal, FALSE);
        awUniform1i(m_activeProgram->m_enable_vertex_colours, FALSE);
        awUniform1i(m_activeProgram->m_light_model_two_sided, FALSE);
  
        //now initialise any individual light sources
        AWPoint     colour;
        AWLight*    curLight;
        int         lightIdx = 0;
        bool        lightEnabled;
        //
        int numLights = getScene().m_nodeList.getNumLights();
        if (numLights >= MAX_NUM_LIGHTS)
            numLights = MAX_NUM_LIGHTS - 1; //restrict to max supported num lights
        for (int light = 0; light < numLights; light++)
        {
            //
            // for each light we need to allocate the appropriate structure to
            //  be passed through to the vertex shader.           
            curLight = getScene().m_nodeList.getLight(light);
            lightEnabled = curLight && curLight->m_on;
            awUniform1i(m_activeProgram->m_light_enabled[lightIdx], lightEnabled);
            if (lightEnabled)
            {
                /*
                 The default ambient light intensity is (0.0, 0.0, 0.0, 1.0).
                 The default diffuse intensity is (0.0, 0.0, 0.0, 1.0) for all lights other
                 than light zero. The default diffuse intensity of light zero is
                 (1.0, 1.0, 1.0, 1.0)
                 */
                curLight->getColor(colour);
                curLight->m_ID = lightIdx;
                //
                // vec4 ambient_color
                glUniform4f(m_activeProgram->m_light_ambient_color[lightIdx], colour.x, colour.y, colour.z, 1.0f);
                //
                // vec4 diffuse_color
                glUniform4f(m_activeProgram->m_light_diffuse_color[lightIdx], colour.x, colour.y, colour.z, 1.0f);
                //
                // vec4 specular_color
                glUniform4f(m_activeProgram->m_light_specular_color[lightIdx], 1.0f, 1.0f, 1.0f, 1.0f);
                //
                // float spot_exponent
                glUniform1f(m_activeProgram->m_light_spot_exponent[lightIdx], 1.0);
                //
                // float spot_cutoff_angle
                if (AWLight::AW_SPOT_LIGHT == curLight->m_type)
                    glUniform1f(m_activeProgram->m_light_spot_cutoff_angle[lightIdx], curLight->m_cutOffAngle);
                else
                    glUniform1f(m_activeProgram->m_light_spot_cutoff_angle[lightIdx], 180.0);
                //
                // bool compute_distance_attenuation
                awUniform1i(m_activeProgram->m_light_compute_distance_attenuation[lightIdx], 0);
                //
                // vec3 attenuation_factors
                glUniform3f(m_activeProgram->m_light_attenuation_factors[lightIdx],
                            curLight->m_attenuation.x,
                            curLight->m_attenuation.y,
                            curLight->m_attenuation.z);
                lightIdx++;
            }
        } //for (int light = 0; light < numLights; light++)
        //
        awUniform1i(m_activeProgram->m_num_lights, lightIdx);
    }
}//AWBoolean AWGL2Renderer::initLights()


void
AWGL2Renderer::displayEnvironment()
{
	AWObject* env = getScene().m_nodeList.getEnvironment();
	if (env)
	{
        glPushGroupMarkerEXT(0, "Environment");
		enableDepthTest(FALSE);
        if (usingLighting())
			enableLighting(FALSE);
		drawObject(env);
        if (usingLighting())
			enableLighting(TRUE);
		enableDepthTest(TRUE);
        glPopGroupMarkerEXT();
	}
}//void  AWGL2Renderer::displayEnvironment()


void
AWGL2Renderer::displayFrame()
{
	AWTRACEHI("\n\n\nSTART FRAME\n");
	m_trisThisFrame=0;

	m_glErr = glGetError();
	// VIEWPORT TRANSFORMATION
	//
	transformViewport(getScene().m_nodeList.getViewTransformer());
	//
	//DISPLAY BACKGROUND
	displayEnvironment();
    //
    // UPDATE LIGHT STATE
    transformLights();
    //
	// DISPLAY SCENE CONTENTS
    //
    awUseProgram(m_vertexPrograms[0]);
    //
    glPushGroupMarkerEXT(0, "Opaque Objects");
	showAllOpaqueObjects();
    glPopGroupMarkerEXT();
    //
	//now draw ALPHA TEST objects
	if (getScene().m_nodeList.getNumAlphaTestObjects())
	{
        glPushGroupMarkerEXT(0, "Alpha Test Objects");
        awUseProgram(m_vertexPrograms[1]);
        showAllAlphaTestObjects();
        setAlphatestEnabled(FALSE);
        glPopGroupMarkerEXT();
	}
	//render any ALPHA BLENDED objects
	if (getScene().m_nodeList.getNumTransparentObjects())
	{
        glPushGroupMarkerEXT(0, "Alpha Blended Objects");
        awUseProgram(m_vertexPrograms[0]);
		glDepthMask(GL_FALSE);
		showAllTransparentObjects();
		glDepthMask(GL_TRUE);
		enableBlend(FALSE);
        glPopGroupMarkerEXT();
	}  
	disableTexUnits();
	//
	AWTRACEHI1("\nEND FRAME - displayed %i triangles\n\n\n\n", m_trisThisFrame);
}//void AWGL2Renderer::displayFrame()


void
AWGL2Renderer::transformViewport(AWTransformController* node)
{	// VIEWPORT TRANSFORMATION
	if (node)
	{   //so need to 'prime' the gl stack with the viewTM
		node->getViewTM(m_viewTM);//
        //
        //AWPoint& eyePosWS = getScene().m_nodeList.getViewCone().m_V;
        //glUniform4f(m_activeProgram->m_eye_ws_position,
        //            eyePosWS.x, eyePosWS.y, eyePosWS.z, 1.0f);
	}
}

void
AWGL2Renderer::transformLight(AWLight* curLight)
{
	if (curLight)
	{
        AWPoint position(0.0f, 0.0f, 0.0f);
        AWPoint direction(0.0f, 0.0f, 1.0f);
        AWPoint spotDirection(0.0f, 0.0f, -1.0f);

        AWMatrix4 lightTM;        
        //get the Wold Space Transform for the node
        curLight->getWSTM(lightTM);
        //
        lightTM =  m_viewTM * lightTM;
        lightTM.getTranslation(position);
        direction = lightTM * direction;
        spotDirection = lightTM * spotDirection;
        //
        // must be supplied at draw time since calculated per frame, along with position
		//transformNode(curLight);
        //
        // vec4 position - position for a point/spot light (w=1) or normalized dir. (w=0) for directional
		switch (curLight->m_type)
		{
			case AWLight::AW_POINT_LIGHT:
				glUniform4f(m_activeProgram->m_light_position[curLight->m_ID],
                            position.x, position.y, position.z, 1.0f);
				break;
                
			case AWLight::AW_DIRECTIONAL_LIGHT:
				glUniform4f(m_activeProgram->m_light_position[curLight->m_ID],
                            direction.x, direction.y, direction.z, 0.0f);
				break;
                
			case AWLight::AW_SPOT_LIGHT:
				glUniform4f(m_activeProgram->m_light_position[curLight->m_ID],
                            position.x, position.y, position.z, 1.0f);
                
				// vec3 spot_direction
                glUniform3f(m_activeProgram->m_light_spot_direction[curLight->m_ID],
                            spotDirection.x, spotDirection.y, spotDirection.z);
				break;
		}
        if (curLight->isColorAnimated())
        {
            AWPoint col;
            curLight->getColor(col);
            float color[] = {col.x, col.y, col.z, 1.0f};
            //vec4            ambient_color, diffuse_color, specular_color;
            glUniform4fv(m_activeProgram->m_light_ambient_color[curLight->m_ID], 1, color);
            glUniform4fv(m_activeProgram->m_light_diffuse_color[curLight->m_ID], 1, color);
        }
	}//if (curLight)
}//void AWGL2Renderer::transformLight(AWLight* light)


void
AWGL2Renderer::transformLights()
{	//designed to cope with animated lights - apply settings per frame
	int numLights = getScene().m_nodeList.getNumLights();
	if (getScene().isAmbientAnimated())
	{
		AWTRACE3("Setting animated AMBIENT light to (%f, %f, %f)\n",
                 (double)m_ambient[0], (double)m_ambient[1], (double)m_ambient[2]);
		getScene().getAmbient(*(AWPoint*)m_ambient);
        glUniform4fv(m_activeProgram->m_ambient_scene_color, 1, m_ambient);
	}
    bool isLightEnabled = false;
	for (int light = 0; light < numLights; light++)
	{
		AWLight* curLight = getScene().m_nodeList.getLight(light);
        isLightEnabled = curLight && curLight->m_on;
        awUniform1i(m_activeProgram->m_light_enabled[light], isLightEnabled);
		if (isLightEnabled)
		{
			transformLight(curLight);
		}
	}
    // need to turn off rest if there's any change they were on last frame or by default
}//void AWGL2Renderer::transformLights()


void
AWGL2Renderer::applyMaterial(AWMaterial* mat)
{
	float stupid[] = {0.0f, 0.0f, 0.0f, 1.0f};
	//GLenum faces = GL_FRONT_AND_BACK;
	if (mat && !mat->getUseDefaults())
	{
		if (m_selectedMaterial != mat)
		{
			AWTRACEHI1("Apply Material %i\n", (int)mat);
#ifdef __AWTRACEHI
			mat->dump(); 
#endif
			m_selectedMaterial = mat;
			//faces = mat->m_twoSided ? GL_FRONT_AND_BACK : GL_FRONT;			
            //
            // vec4 ambient_color
			mat->m_ambient.assignToFloat4(stupid);
            glUniform4fv(m_activeProgram->m_material_state_ambient_color, 1, stupid);
			
            //
            // vec4 diffuse_color
			mat->m_diffuse.assignToFloat4(stupid);
            glUniform4fv(m_activeProgram->m_material_state_diffuse_color, 1, stupid);
			
            //
            // vec4 specular_color
			mat->m_specular.assignToFloat4(stupid);
            glUniform4fv(m_activeProgram->m_material_state_specular_color, 1, stupid);
            
            //
            // float specular_exponent ????
            glUniform1f(m_activeProgram->m_material_state_specular_exponent, mat->m_shininess);
			
            //
            // vec4 emissive_color
			mat->m_emissive.assignToFloat4(stupid);
            glUniform4fv(m_activeProgram->m_material_state_emissive_color, 1, stupid);
		}//if (m_selectedMaterial != mat)
	}//if (mat)
	else if (m_selectedMaterial)//must apply default values
	{ 
		AWTRACEHI("Apply Default Material settings\n");
		m_selectedMaterial = NULL;
		applyMaterialDefaults();
	}
}//void AWGL2Renderer::applyMaterial(AWMaterial* mat)


void
AWGL2Renderer::applyMaterialDefaults()
{
	float stupid[] = {0.0f, 0.0f, 0.0f, 1.0f};
    
    stupid[0]=0.2f; stupid[1]=0.2f; stupid[2]=0.2f;    
    glUniform4fv(m_activeProgram->m_material_state_ambient_color, 1, stupid);

    stupid[0]=0.8f; stupid[1]=0.8f; stupid[2]=0.8f;
    glUniform4fv(m_activeProgram->m_material_state_diffuse_color, 1, stupid);
    
    stupid[0]=0.0f; stupid[1]=0.0f; stupid[2]=0.0f;
    glUniform4fv(m_activeProgram->m_material_state_specular_color, 1, stupid);
    glUniform4fv(m_activeProgram->m_material_state_emissive_color, 1, stupid);
    glUniform1f(m_activeProgram->m_material_state_specular_exponent, 0.0);
}


void
AWGL2Renderer::applyTextureState(AWTextureUnit* texUnit, int unitID)
{  //ensure gl call made only if mode *CHANGES*
	if (!texUnit || !texUnit->m_tex) return;
	
	AWTexture* tex    = texUnit->m_tex;
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
		//setTexMatrix(texMatrix, unitID);
	}//if ((AWTexture::REFLECT == tex->m_genMode) && m_transformReflections)
	else
	{
		//setTexMatrixIdentity(unitID);
	}
}//void AWGL2Renderer::applyTextureState(AWTextureUnit* texUnit, int unitID)


void           
AWGL2Renderer::selectTexture(AWTexture* tex, int unitID)
{
	if (!tex) return;
	int  texID  = tex->getTextureID(); 
	
	AWTRACEHI1("Activate texUnit: %i - ", unitID);
	activeTexture(GL_TEXTURE0 + unitID);
	int test = getSelectedTex(unitID);
	if ( (test != texID) || (AW_NO_TEXTURE == getTexMode(unitID)) )
	{
		setSelectedTex(texID, unitID);
		setTexMode(tex->m_target, unitID);
		glBindTexture(tex->m_target, texID);
		AWTRACEHI3(" bind %s ID %d %s\n", tex->getTargetName(), texID, tex->m_name.c_str());  
	}//if ( (test != texID) || (AW_NO_TEXTURE == getTexMode(unitID)) )
}//void AWGL2Renderer::selectTexture(AWTexture* tex, int unitID)


void
AWGL2Renderer::disableTexUnits(int startunit)
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
			activeTexture(GL_TEXTURE0 + i);
			setTexMode(AW_NO_TEXTURE, i);
			setSelectedTex(-1, i);   
		}
	}
}//void AWGL2Renderer::disableTexUnits(int startunit)


void
AWGL2Renderer::setTextureState(AWRenderPass* pass)
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
		}
	}
	disableTexUnits(numTexUnits);
}//void AWGL2Renderer::setTextureState(AWRenderPass* pass)


int
AWGL2Renderer::getNumPasses(AWObject* o)
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
}//int AWGL2Renderer::getNumPasses(AWObject* o)


void
AWGL2Renderer::setBlendState(AWRenderPass* pass)
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
	}
	else
	{
		enableBlend(FALSE);
	}
	setAlphatestEnabled(AWTexture::ALPHA_TEST == blendType);
}


void
AWGL2Renderer::drawObject(AWObject* theObj)
{
	if (theObj && theObj->getIsVisible())
	{
        glPushGroupMarkerEXT(0, theObj->getName().c_str());
		AWRenderPass* curPass = NULL;
		int numPasses = getNumPasses(theObj);
		if (0==numPasses) 
			return;
		if (!m_applyObjectTextures) 
			numPasses = 1;
		int numTris = theObj->getNumTris();
		m_trisThisFrame += numTris;
		transformObject(theObj);	//apply the object wsTM
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
				//drawObjectAsVertArray(theObj, curPass);
                if (0 == theObj->m_vertexBuffer)
                    bufferObjectAsVBO(theObj, curPass);
                drawObjectAsVBO(theObj, curPass);
			}
		}
        glPopGroupMarkerEXT();
		AWTRACEHI1("Finished drawing %s\n", theObj->getName().c_str());
	}//if (theObj && theObj->getIsVisible())
}//void AWGL2Renderer::drawObject(AWObject* theObj)


void
AWGL2Renderer::applyObjectState(AWObject* theObj)
{
	if (theObj->isTwoSided())
        glDisable(GL_CULL_FACE);
    //
    // if dynamic lighting applies at scene level but this object should not
    //  participate, turn dynamic lighting OFF
    if (usingLighting() && !theObj->getDynamicLighting())
	{
		AWTRACEHI("Disable dynamic lighting\n");
		enableLighting(FALSE);
	}
}//void AWGL2Renderer::applyObjectState(AWObject* theObj)


void
AWGL2Renderer::restoreObjectState(AWObject* theObj)
{
	if (theObj->isTwoSided())
        glEnable(GL_CULL_FACE);
    //
    // if dynamic lighting applies at scene level but this object did not
    //  participate, need to turn dynamic lighting back on
    if (usingLighting() && !theObj->getDynamicLighting())
	{
		AWTRACEHI("Re-enable dynamic lighting\n");
		enableLighting(TRUE);
	}
}//void AWGL2Renderer::restoreObjectState(AWObject* theObj)



// 'CreateBuffers' stage for all non-dynamic mesh objects
// which is frankly most of them.
void
AWGL2Renderer::bufferObjectAsVBO(AWObject* theObj, AWRenderPass* pass)
{
    if (theObj && theObj->getIndexArray() && pass)
	{
        //
        // VERTEX POSITIONS (static) NOTE implies FALSE == theObj->isMeshAnimated()
        glGenBuffers(1, &theObj->m_vertexBuffer);
        if (!theObj->isMeshAnimated())
        {
            glBindBuffer(GL_ARRAY_BUFFER, theObj->m_vertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, theObj->getNumVerts() * 3 * sizeof(GLfloat), theObj->m_vertexList, GL_STATIC_DRAW);
        }
        //
        // VERTEX COLOURS (static)
        if (theObj->m_vertColList && userAllowsVertexColours())
        {
            glGenBuffers(1, &theObj->m_colourBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, theObj->m_colourBuffer);
            glBufferData(GL_ARRAY_BUFFER, theObj->getNumVerts() * 4 * sizeof(unsigned char), theObj->m_vertColList, GL_STATIC_DRAW);
        }
        //
        // NORMALS (static)
        if (theObj->m_normalList && (-1 != m_activeProgram->m_normal))
        {
            glGenBuffers(1, &theObj->m_normalBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, theObj->m_normalBuffer);
            glBufferData(GL_ARRAY_BUFFER, theObj->getNumVerts() * 3 * sizeof(GLfloat), theObj->m_normalList, GL_STATIC_DRAW);
        }
        //
        // TEXTURE COORDINATES (static)
        if (pass->getNumTexUnits() > 0)
		{
            glGenBuffers(1, &theObj->m_texVertexBufer);
            glBindBuffer(GL_ARRAY_BUFFER, theObj->m_texVertexBufer);
            glBufferData(GL_ARRAY_BUFFER,
                         theObj->getNumVerts() * theObj->getTVertexListDim(0) * sizeof(GLfloat),
                         theObj->getTVertexList(0),
                         GL_STATIC_DRAW);
		}
        //
        // INDICES (static)
        glGenBuffers(1, &theObj->m_indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theObj->m_indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, theObj->getNumIndeces() * sizeof(unsigned short), theObj->getIndexArray(), GL_STATIC_DRAW);
    }
}//void AWGL2Renderer::bufferObjectAsVBO(AWObject* theObj, AWRenderPass* pass)


void
AWGL2Renderer::drawObjectAsVBO(AWObject* theObj, AWRenderPass* pass)
{
    if (theObj && theObj->getIndexArray() && pass)
	{
        applyMaterial(theObj->getMaterial());
        applyObjectState(theObj);
        //
        // VERTEX DATA (may be DYNAMIC or STATIC       
        glBindBuffer(GL_ARRAY_BUFFER, theObj->m_vertexBuffer);
        if (theObj->isMeshAnimated())
            glBufferData(GL_ARRAY_BUFFER, theObj->getNumVerts() * 3 * sizeof(float), theObj->m_vertexList, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(m_activeProgram->m_position, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
        enableVertexAttribArray(m_activeProgram->m_position);
        //
        // VERTEX COLOUR DATA
        if (0 != theObj->m_colourBuffer)
        {
            glBindBuffer(GL_ARRAY_BUFFER, theObj->m_colourBuffer);
            glVertexAttribPointer(m_activeProgram->m_colour, 4, GL_UNSIGNED_BYTE, GL_TRUE, 4 * sizeof(unsigned char), 0);
            enableVertexAttribArray(m_activeProgram->m_colour);
            awUniform1i(m_activeProgram->m_enable_vertex_colours, TRUE);
        }
        else
        {   //without vertex colors, we should use white to blend with textures...
            awVertexAttribf4(m_activeProgram->m_colour, 1.0, 1.0, 1.0, 0.0);
            disableVertexAttribArray(m_activeProgram->m_colour);
            awUniform1i(m_activeProgram->m_enable_vertex_colours, FALSE);
        }
        //
        // NORMAL DATA
        if (0 != theObj->m_normalBuffer)
        {
            glBindBuffer(GL_ARRAY_BUFFER, theObj->m_normalBuffer);
            glVertexAttribPointer(m_activeProgram->m_normal, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
            enableVertexAttribArray(m_activeProgram->m_normal);
        }
        else
        {
            awVertexAttribf4(m_activeProgram->m_normal, 1.0, 1.0, 1.0, 1.0);
            disableVertexAttribArray(m_activeProgram->m_normal);
        }
        //
        // TEXTURE COORDINATE DATA
        AWBoolean hasTextures = 0 != theObj->m_texVertexBufer;
        if (hasTextures)
		{
            awUniform1i(m_activeProgram->m_tex0, 0);
            activeTexture(GL_TEXTURE0);
            glBindBuffer(GL_ARRAY_BUFFER, theObj->m_texVertexBufer);
            glVertexAttribPointer(m_activeProgram->m_texCoord0,
                                  theObj->getTVertexListDim(0),
                                  GL_FLOAT,
                                  GL_FALSE,
                                  theObj->getTVertexListDim(0) * sizeof(GL_FLOAT),
                                  0);
        }
		enableVertexAttribArray(hasTextures, m_activeProgram->m_texCoord0);
        //
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theObj->m_indexBuffer);
        glDrawElements(GL_TRIANGLES, theObj->getNumIndeces(), GL_UNSIGNED_SHORT, 0);
        //
        restoreObjectState(theObj);
	}
}//void AWGL2Renderer::drawObjectAsVBO(AWObject* theObj, AWRenderPass* pass)


void
AWGL2Renderer::drawObjectAsVertArray(AWObject* theObj, AWRenderPass* pass)
{
	if (theObj && theObj->getIndexArray() && pass)
	{
        applyMaterial(theObj->getMaterial());
        applyObjectState(theObj);
        
        // Load the vertices and normals
        glVertexAttribPointer(m_activeProgram->m_position, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), theObj->m_vertexList);
        enableVertexAttribArray(m_activeProgram->m_position);
        if (theObj->m_vertColList && userAllowsVertexColours())
        {
            glVertexAttribPointer(m_activeProgram->m_colour, 4, GL_UNSIGNED_BYTE, GL_TRUE, 4 * sizeof(unsigned char), theObj->m_vertColList);
            enableVertexAttribArray(m_activeProgram->m_colour);
            awUniform1i(m_activeProgram->m_enable_vertex_colours, TRUE);
        }
        else
        {   //without vertex colors, we should use white to blend with textures...
            awVertexAttribf4(m_activeProgram->m_colour, 1.0, 1.0, 1.0, 0.0);
            disableVertexAttribArray(m_activeProgram->m_colour);
            awUniform1i(m_activeProgram->m_enable_vertex_colours, FALSE);
        }
        
        if (theObj->m_normalList && (-1 != m_activeProgram->m_normal))
        {
            glVertexAttribPointer(m_activeProgram->m_normal, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), theObj->m_normalList);
            enableVertexAttribArray(m_activeProgram->m_normal);
        }
        else
        {   
            awVertexAttribf4(m_activeProgram->m_normal, 1.0, 1.0, 1.0, 1.0);
            disableVertexAttribArray(m_activeProgram->m_normal);
        }
        AWBoolean hasTextures = FALSE;
		for (int unit=0; unit<pass->getNumTexUnits(); unit++)
		{
            hasTextures = TRUE;
			activeTexture(GL_TEXTURE0+unit);
            awUniform1i(m_activeProgram->m_tex0, unit);
            glVertexAttribPointer(m_activeProgram->m_texCoord0, theObj->getTVertexListDim(unit), GL_FLOAT, GL_FALSE, theObj->getTVertexListDim(unit) * sizeof(GL_FLOAT), theObj->getTVertexList(unit));
		}
		glDrawElements(GL_TRIANGLES, theObj->getNumIndeces(), GL_UNSIGNED_SHORT, theObj->getIndexArray());
        enableVertexAttribArray(hasTextures, m_activeProgram->m_texCoord0);
        //
        restoreObjectState(theObj);
	}
}//void AWGL2Renderer::drawObjectAsVertArray(AWObject* theObj, AWRenderPass* pass)


AWBoolean
AWGL2Renderer::usesCubeMap(AWObject* obj)
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
}//AWBoolean AWGL2Renderer::usesCubeMap(AWObject* obj)