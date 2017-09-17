
/*
 *  AWVertexProgram.cpp
 *  iCritter
 *
 *  Created by Tom Gilbert on 22/07/2010.
 *  Copyright 2010 AlteredWorlds Ltd. All rights reserved.
 *
 */
#include "AWVertexProgram.h"
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>


#define VER_AWVertexProgram_1	100
#define DEBUG 1

AWDEFINE_VERSIONABLE_PERSISTABLE(AWVertexProgram, CLSID_AWVERTEXPROGRAM, VER_AWVertexProgram_1);


AWVertexProgram::AWVertexProgram()
{
	m_program = 0;
    reset();
}

void				
AWVertexProgram::clearAndDestroy()
{
	;
}

void
AWVertexProgram::useProgram()
{
    glUseProgram(m_program);
}

bool
AWVertexProgram::linkProgram()
{
	bool retVal = true;
    GLint status;
	
    glLinkProgram(m_program);
	
#if defined(DEBUG)
    GLint logLength;
    glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar* log = new GLchar[logLength];
        glGetProgramInfoLog(m_program, logLength, &logLength, log);
        AWTRACE1("Program link log: %s", log);
        delete [] log;
    }
#endif
	
    glGetProgramiv(m_program, GL_LINK_STATUS, &status);
    if (status == 0)
        retVal = false;
	
    return retVal;
}


bool
AWVertexProgram::validateProgram()
{
	bool retVal = true;
    GLint logLength, status;
	
    glValidateProgram(m_program);
    glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar* log = new GLchar[logLength];
        glGetProgramInfoLog(m_program, logLength, &logLength, log);
        AWTRACE1("Program validate log: %s", log);
        delete [] log;
    }
	
    glGetProgramiv(m_program, GL_VALIDATE_STATUS, &status);
    if (status == 0)
        retVal = false;
	
    return retVal;
}

bool
AWVertexProgram::compileShader(unsigned int* shader, int type, const char* source)
{
	bool retVal = true;
    GLint status;
	
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);
	
#ifdef __AWTRACE
    GLint logLength;
    glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar* log = new GLchar[logLength];
        glGetShaderInfoLog(*shader, logLength, &logLength, log);
        AWTRACE1("Shader compile log: %s", log);
        delete [] log;
    }
#endif
	
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if (status == 0)
    {
        glDeleteShader(*shader);
        retVal = false;
    }
	
    return retVal;
}

bool
AWVertexProgram::loadShaders(const char* vertShaderSrce, const char* fragShaderSrce)
{
    GLuint vertShader, fragShader;
	
    // Create shader program
    m_program = glCreateProgram();
	
    // Create and compile vertex shader
    if (!compileShader(&vertShader, GL_VERTEX_SHADER, vertShaderSrce))
    {
        AWTRACE("Failed to compile vertex shader");
        return false;
    }
	
    // Create and compile fragment shader
    if (!compileShader(&fragShader, GL_FRAGMENT_SHADER, fragShaderSrce))
    {
        AWTRACE("Failed to compile fragment shader");
        return false;
    }
	
    // Attach vertex shader to program
    glAttachShader(m_program, vertShader);
	
    // Attach fragment shader to program
    glAttachShader(m_program, fragShader);
	
    // Link program
    if (!linkProgram())
    {
        AWTRACE1("Failed to link program: %d", m_program);
		
        if (vertShader)
        {
            glDeleteShader(vertShader);
            vertShader = 0;
        }
        if (fragShader)
        {
            glDeleteShader(fragShader);
            fragShader = 0;
        }
        if (m_program)
        {
            glDeleteProgram(m_program);
            m_program = 0;
        }
        
        return false;
    }
    //
    // Release vertex and fragment shaders
    if (vertShader)
        glDeleteShader(vertShader);
	if (fragShader)
		glDeleteShader(fragShader);
    //
    findSlots();
    //
	return true;
}

void
AWVertexProgram::reset()
{
    // Get the attribute locations
    m_position = -1;
    m_colour =  -1;
    m_texCoord0 =  -1;
    m_normal =  -1;
    m_modelviewprojection_matrix =  -1;
    m_tex0 =  -1;
    m_alphaTest =  -1;
    m_modelview_matrix =  -1;
    m_inv_modelview_matrix =  -1;
    m_enable_vertex_colours =  -1;
    m_enable_lighting =  -1;
    m_num_lights =  -1;
    m_xform_eye_p =  -1;
    m_ambient_scene_color =  -1;
    m_normalize_normal =  -1;
    m_rescale_normal =  -1;
    m_light_model_two_sided =  -1;
    m_material_state_ambient_color =  -1;
    m_material_state_diffuse_color =  -1;
    m_material_state_specular_color =  -1;
    m_material_state_emissive_color = -1;
    
    // detailed information for each of up to MAX_LIGHTS lights (8)
    char lightState[50];
    for (int lightIdx=0; lightIdx<MAX_LIGHTS; lightIdx++)
    {
        sprintf(lightState, "light_enable_state[%u]", lightIdx);
        m_light_enabled[lightIdx] =  -1;
        m_light_ambient_color[lightIdx] =  -1;
        m_light_diffuse_color[lightIdx] =  -1;
        m_light_specular_color[lightIdx] =  -1;
        m_light_position[lightIdx] =  -1;
        m_light_spot_direction[lightIdx] =  -1;
        m_light_attenuation_factors[lightIdx] =  -1;
        m_light_spot_exponent[lightIdx] =  -1;
        m_light_spot_cutoff_angle[lightIdx] =  -1;
        m_light_compute_distance_attenuation[lightIdx] = -1;
    }
    
    m_enable_vertex_colours =  -1;
    m_enable_fog =  -1;
    m_fog_color =  -1;
    m_fog_density =  -1;
    m_fog_start =  -1;
    m_fog_end =  -1;
    m_fog_mode = -1;
}


void
AWVertexProgram::findSlots()
{
    // Get the attribute locations
	m_position = glGetAttribLocation(m_program, "a_position");
	m_colour = glGetAttribLocation(m_program, "a_colour");
    m_texCoord0 = glGetAttribLocation(m_program, "a_texCoord");
    m_normal = glGetAttribLocation(m_program, "a_normal");
    //
	// Get the uniform locations
	m_modelviewprojection_matrix = glGetUniformLocation(m_program, "modelviewprojection_matrix");
    m_tex0 = glGetUniformLocation(m_program, "texUnit");
    m_alphaTest = glGetUniformLocation(m_program, "alphaTest");
    m_modelview_matrix = glGetUniformLocation(m_program, "modelview_matrix");
    m_inv_modelview_matrix = glGetUniformLocation(m_program, "inv_modelview_matrix");
    m_enable_vertex_colours = glGetUniformLocation(m_program, "enable_vertex_colours");
    m_enable_lighting = glGetUniformLocation(m_program, "enable_lighting");
    m_num_lights = glGetUniformLocation(m_program, "num_lights");
    m_xform_eye_p = glGetUniformLocation(m_program, "xform_eye_p");
    
    m_ambient_scene_color = glGetUniformLocation(m_program, "ambient_scene_color");
    m_normalize_normal = glGetUniformLocation(m_program, "normalize_normal");
    m_rescale_normal = glGetUniformLocation(m_program, "rescale_normal");
    m_light_model_two_sided = glGetUniformLocation(m_program, "light_model_two_sided");
    
    m_material_state_ambient_color = glGetUniformLocation(m_program, "material_state.ambient_color");
    m_material_state_diffuse_color = glGetUniformLocation(m_program, "material_state.diffuse_color");
    m_material_state_specular_color = glGetUniformLocation(m_program, "material_state.specular_color");
    m_material_state_emissive_color = glGetUniformLocation(m_program, "material_state.emissive_color");
    m_material_state_specular_exponent = glGetUniformLocation(m_program, "material_state.specular_exponent");
    //
    // detailed information for each of up to MAX_LIGHTS lights (8)
    char lightState[50];
    for (int lightIdx=0; lightIdx<MAX_LIGHTS; lightIdx++)
    {
        sprintf(lightState, "light_enable_state[%u]", lightIdx);
        m_light_enabled[lightIdx] = glGetUniformLocation(m_program, lightState);
        
        sprintf(lightState, "light_state[%u]%s", lightIdx, ".ambient_color");
        m_light_ambient_color[lightIdx] = glGetUniformLocation(m_program, lightState);
                
        sprintf(lightState, "light_state[%u]%s", lightIdx, ".diffuse_color");
        m_light_diffuse_color[lightIdx] = glGetUniformLocation(m_program, lightState);
       
        sprintf(lightState, "light_state[%u]%s", lightIdx, ".specular_color");
        m_light_specular_color[lightIdx] = glGetUniformLocation(m_program, lightState);
        
        sprintf(lightState, "light_state[%u]%s", lightIdx, ".position");
        m_light_position[lightIdx] = glGetUniformLocation(m_program, lightState);
        
        sprintf(lightState, "light_state[%u]%s", lightIdx, ".spot_direction");
        m_light_spot_direction[lightIdx] = glGetUniformLocation(m_program, lightState);
        
        sprintf(lightState, "light_state[%u]%s", lightIdx, ".attenuation_factors");
        m_light_attenuation_factors[lightIdx] = glGetUniformLocation(m_program, lightState);
        
        sprintf(lightState, "light_state[%u]%s", lightIdx, ".spot_exponent");
        m_light_spot_exponent[lightIdx] = glGetUniformLocation(m_program, lightState);
        
        sprintf(lightState, "light_state[%u]%s", lightIdx, ".spot_cutoff_angle");
        m_light_spot_cutoff_angle[lightIdx] = glGetUniformLocation(m_program, lightState);
        
        sprintf(lightState, "light_state[%u]%s", lightIdx, ".compute_distance_attenuation");
        m_light_compute_distance_attenuation[lightIdx] = glGetUniformLocation(m_program, lightState);
    }
    
    m_enable_fog = glGetUniformLocation(m_program, "enable_fog");
    m_fog_color = glGetUniformLocation(m_program, "fog_color");
    m_fog_density = glGetUniformLocation(m_program, "fog_density");
    m_fog_start = glGetUniformLocation(m_program, "fog_start");
    m_fog_end = glGetUniformLocation(m_program, "fog_end");
    m_fog_mode = glGetUniformLocation(m_program, "fog_mode");
}
