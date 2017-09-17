//
//  Shader.fsh
//
//  Created by Tom Gilbert on 12/06/2010.
//  Copyright AlteredWorlds Ltd 2010. All rights reserved.
//
#define NUM_TEXTURES 2

precision lowp float;

// vertex shader supplies these values with interpolation for
// fragments that are between vertices
varying lowp vec4   v_front_color;
varying lowp vec4   v_texCoord[NUM_TEXTURES];
varying highp float v_fog_factor;

uniform lowp vec4   fog_color;
uniform sampler2D   texUnit;
uniform bool        alphaTest;

const int           indx_zero = 0;
const float         half_alpha = 0.5;

void main()
{
    vec4 baseColour = v_front_color * texture2D(texUnit, v_texCoord[indx_zero].xy);
    if (alphaTest && (baseColour.a < half_alpha))
        discard;
    else
        gl_FragColor = mix(fog_color, baseColour, v_fog_factor);
}