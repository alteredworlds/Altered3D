#ifndef __AW_VERTEXPROGRAM_H
#define __AW_VERTEXPROGRAM_H

/*
 *  AWVertexProgram.h
 *
 *  Created by Tom Gilbert on 22/07/2010.
 *  Copyright 2010 AlteredWorlds Ltd. All rights reserved.
 *
 */

#include "AWToolsCommon.h"
#include "AWPersistable.h"
#define MAX_LIGHTS 8
//
//
//struct {
//    int            position;        // light position for a point/spot light or
//                                    // normalized dir. for a directional light
//    int            ambient_color;
//    int            diffuse_color;
//    int            specular_color;
//    int            spot_direction;
//    int            attenuation_factors;
//    float           spot_exponent;
//    float           spot_cutoff_angle;
//    bool            compute_distance_attenuation;
//} AWLightId;


class AWExport AWVertexProgram : public AWPersistable
{
public:
	AWDECLARE_VERSIONABLE_PERSISTABLE(AWVertexProgram);
	
	AWVertexProgram();
	~AWVertexProgram()			{clearAndDestroy();}
	
	unsigned int		getProgram() { return m_program; }
	
	void				clearAndDestroy();
    void                reset();
	
	void				useProgram();
	bool				linkProgram();
	bool				validateProgram();
	bool				compileShader(unsigned int* shader, int type, const char* source);
	bool				loadShaders(const char* vertShader, const char* fragShader);
    virtual void        findSlots();

public:
	unsigned int    m_program;
    int             m_position;
    int             m_normal;
    int             m_colour;
    int             m_texCoord0;
    int             m_tex0;
    int             m_modelviewprojection_matrix;
    int             m_alphaTest;
    
    int             m_modelview_matrix;
    int             m_inv_modelview_matrix;
    //int             m_model_matrix;
    //int             m_eye_ws_position;
    
    //int             m_enable_color_material;
    int             m_enable_lighting;
    int             m_enable_vertex_colours;
    
    int             m_ambient_scene_color;
    int             m_normalize_normal;
    int             m_rescale_normal;
    int             m_light_model_two_sided;
    
    int             m_material_state_ambient_color;
    int             m_material_state_diffuse_color;
    int             m_material_state_specular_color;
    int             m_material_state_emissive_color;
    int             m_material_state_specular_exponent;
    
    int             m_light_enabled[MAX_LIGHTS];
    int             m_light_position[MAX_LIGHTS];
    int             m_light_ambient_color[MAX_LIGHTS];
    int             m_light_diffuse_color[MAX_LIGHTS];
    int             m_light_specular_color[MAX_LIGHTS];
    int             m_light_spot_direction[MAX_LIGHTS];
    int             m_light_attenuation_factors[MAX_LIGHTS];
    int             m_light_spot_exponent[MAX_LIGHTS];
    int             m_light_spot_cutoff_angle[MAX_LIGHTS];
    int             m_light_compute_distance_attenuation[MAX_LIGHTS];
    
    int             m_xform_eye_p;
    int             m_num_lights;    
    
    int             m_enable_fog;
    int             m_fog_color;
    int             m_fog_density;
    int             m_fog_start;
    int             m_fog_end;
    int             m_fog_mode;
};

#endif //sentinel __AW_VERTEXPROGRAM_H