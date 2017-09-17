#define NUM_TEXTURES 2
#define MAX_LIGHTS 8
#define GLI_FOG_MODE_LINEAR 0
#define GLI_FOG_MODE_EXP 1
#define GLI_FOG_MODE_EXP2 2


struct light {
    vec4            position;   // light position for a point/spot light or
                                // normalized dir. for a directional light 
    vec4            ambient_color;
    vec4            diffuse_color;
    vec4            specular_color;
    vec3            spot_direction;
    vec3            attenuation_factors; 
    float           spot_exponent;
    float           spot_cutoff_angle;
    bool            compute_distance_attenuation; 
};

struct material {
    vec4            ambient_color; 
    vec4            diffuse_color; 
    vec4            specular_color; 
    vec4            emissive_color; 
    float           specular_exponent;
};

const float         c_zero = 0.0;
const float         c_one = 1.0;
const int           indx_zero = 0;
const int           indx_one = 1;

uniform mat4        modelviewprojection_matrix; // combined model view + projection matrix
uniform mat4        modelview_matrix; // model view matrix  
uniform mat3        inv_modelview_matrix; // inverse model-view
uniform vec4        eye_ws_position;

uniform mat4        tex_matrix[NUM_TEXTURES]; // texture matrices
uniform bool        enable_tex[NUM_TEXTURES]; // texture enables
uniform bool        enable_tex_matrix[NUM_TEXTURES]; // texture matrix enables

uniform material    material_state;
uniform vec4        ambient_scene_color;
uniform light       light_state[MAX_LIGHTS];
uniform bool        light_enable_state[MAX_LIGHTS];      // 8 lights can be on/off
                                                
uniform int         num_lights;
uniform bool        enable_lighting;
uniform bool        light_model_two_sided;
uniform bool        enable_vertex_colours;
//
// FOG
uniform bool        enable_fog;
uniform float       fog_density;
uniform float       fog_start, fog_end;
uniform int         fog_mode;                   // linear, exp or exp2

uniform bool        xform_eye_p;                // set if we need Peye for lighting, fog
uniform bool        rescale_normal;
uniform bool        normalize_normal;
uniform float       rescale_normal_factor;


// vertext attributes - all are not always supplied
//
attribute vec4      a_position;
attribute vec2      a_texCoord;    // available if enable_tex[0] true
//attribute vec2      a_texcoord1;    // available if enable_tex[1] true
attribute vec4      a_colour;        // available if !enable_lighting || (enable_lighting && enable_vertex_colours)
attribute vec3      a_normal;       // available if xform_normal set

// varing outputs from the vertex shader
varying vec4        v_texCoord[NUM_TEXTURES];
varying vec4        v_front_color;
varying vec4        v_back_color;
varying float       v_fog_factor;

// temporary variables 
vec4                p_eye;
vec3                n;
vec4                mat_ambient_color;
vec4                mat_diffuse_color;

vec4
lighting_equation(int i)
{
    vec4    computed_color = vec4(c_zero, c_zero, c_zero, c_zero);
    vec3    h_vec;
    float   ndot1, ndoth;
    float   att_factor;
    vec3    VPpli;
    
    att_factor = c_one;
    if (c_zero != light_state[i].position.w)
    {
        float   spot_factor;
        vec3    att_dist;
        
        // this is a point or spot light
        // we assume "w" values for PPli and V are the same
        VPpli = light_state[i].position.xyz - p_eye.xyz;
        if (light_state[i].compute_distance_attenuation)
        {
            // compute distance attenuation
            att_dist.x = c_one;
            att_dist.z = dot(VPpli, VPpli);
            att_dist.y = sqrt(att_dist.z);
            att_factor = c_one / dot(att_dist, light_state[i].attenuation_factors);
        }
        VPpli = normalize(VPpli);
        
        if (light_state[i].spot_cutoff_angle < 180.0)
        {
            // compute spot factor
            spot_factor = dot(-VPpli, light_state[i].spot_direction);
            if (spot_factor >= cos(radians(light_state[i].spot_cutoff_angle)))
                spot_factor = pow(spot_factor, light_state[i].spot_exponent);
            else
                spot_factor = c_zero;
            
            att_factor *= spot_factor;
        }
    }
    else
    {   // directional light
        VPpli = light_state[i].position.xyz;
    }    
    if (att_factor > c_zero)
    {
        // process lighting equation => compute the light color
        computed_color += (light_state[i].ambient_color * mat_ambient_color);
        ndot1 = max(c_zero, dot(n, VPpli));
        computed_color += (ndot1 * light_state[i].diffuse_color * mat_diffuse_color);
        h_vec = normalize(VPpli + vec3(c_zero, c_zero, c_zero));
        ndoth = dot(n, h_vec);
        if (ndoth > c_zero)
        {
            computed_color += (pow(ndoth, material_state.specular_exponent) *
                               material_state.specular_color *
                               light_state[i].specular_color);
        }
        computed_color *= att_factor;
    }
    return computed_color;
}

float
compute_fog()
{
    float   f;
    
    // use eye Z as approximation
    if (GLI_FOG_MODE_LINEAR == fog_mode)
    {
        //f = 0.44;
        f = (fog_end - p_eye.z) / (fog_end - fog_start);
    }
    else if (GLI_FOG_MODE_EXP == fog_mode)
    {
        f = exp(-1.0 * abs(p_eye.z * fog_density));
    }
    else
    {
        f = (p_eye.z * fog_density);
        f = exp(-(f * f));
    }
    
    f = clamp(f, c_zero, c_one);
    
    return f;
}

vec4 
do_lighting()
{
    vec4    vtx_color;
    int     i, j;
    
    vtx_color = material_state.emissive_color + (mat_ambient_color * ambient_scene_color);

    j = indx_zero;
    for (i=indx_zero; i<8; i++)
    {
        if (j >= num_lights)
            break;
            
        if (light_enable_state[i])
        {
            j++;
            vtx_color += lighting_equation(i);
        }
    }
    
    vtx_color.a = mat_diffuse_color.a;
    
    return vtx_color;
}

void main(void)
{
    // do we need vertex in (view | model) spaces as well?
    if (xform_eye_p)
    {
        p_eye = modelview_matrix * a_position;
    }
    
    if (enable_lighting || enable_fog)
    {
        n = inv_modelview_matrix * a_normal;
        
        if (rescale_normal)
            n = rescale_normal_factor * n;
        
        if (normalize_normal)
            n = normalize(n);
    }
    
    if (enable_lighting)
    {
        mat_ambient_color = enable_vertex_colours ? 
                                    a_colour :
                                    material_state.ambient_color;
                                    
        mat_diffuse_color = enable_vertex_colours ? 
                                    a_colour  :
                                    material_state.diffuse_color;
        
        v_front_color = do_lighting();
        v_back_color = v_front_color;
        if (light_model_two_sided)
        {
            n = -n;
            v_back_color = do_lighting();
        }
    }
    else 
    {
        // set the default output colour to be:
        if (enable_vertex_colours)
        {   //
            //the per-vertex / per-primitive color
            v_front_color = a_colour;
            v_back_color = a_colour;
        }
        else
        {   // any texture on an object is blended with white in absence of
            // both lighting *and* explicit vertex colours
            // 
            v_front_color = vec4(1.0, 1.0, 1.0, 1.0);
            v_back_color = vec4(1.0, 1.0, 1.0, 1.0);
        }
    }
                        
    v_fog_factor = enable_fog ?
                        compute_fog() :
                        c_one;
    
    gl_Position = modelviewprojection_matrix * a_position;
    v_texCoord[indx_zero] = vec4(a_texCoord, c_zero, c_zero);
}