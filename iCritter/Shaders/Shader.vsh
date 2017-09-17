//
//  Shader.vsh
//
//  Created by Tom Gilbert on 12/06/2010.
//  Copyright AlteredWorlds Ltd 2010. All rights reserved.

#define NUM_TEXTURES 2

const float         c_zero = 0.0;


uniform mat4        u_nodeMVP;     // model to clip space

attribute vec4      a_position;  // input vertex (position)
attribute vec4      a_normal;    // input vertex normal
attribute vec4      a_colour;    // input vertex colour
attribute vec2      a_texCoord;  // input texture coordinate

varying vec4        v_front_color;      // output vertex colour
varying vec4        v_texCoord[NUM_TEXTURES]; // output texture coordinate as 0

const int           indx_zero = 0;


void main()
{
    gl_Position = u_nodeMVP * a_position;
    v_front_color = a_colour;
    v_texCoord[indx_zero].x = a_texCoord.x;
    v_texCoord[indx_zero].y = a_texCoord.y;
    v_texCoord[indx_zero].z = c_zero;
    v_texCoord[indx_zero].a = c_zero;
}
