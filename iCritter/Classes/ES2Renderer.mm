//
//  ES2Renderer.m
//  Test
//
//  Created by Tom Gilbert on 12/06/2010.
//  Copyright __MyCompanyName__ 2010. All rights reserved.
//

#import "ES2Renderer.h"
#import "AWToolDefs.h"
#import "AWPlayer.h"
#import "AWSceneMgr.h"

#define DEBUG 0

// uniform index
enum {
    UNIFORM_TRANSLATE,
    NUM_UNIFORMS
};
GLint uniforms[NUM_UNIFORMS];

// attribute index
enum {
    ATTRIB_VERTEX,
    ATTRIB_COLOR,
    NUM_ATTRIBUTES
};

@interface ES2Renderer (PrivateMethods)
- (BOOL)loadShadersAW;
@end

@implementation ES2Renderer

@synthesize sceneName;

// Create an OpenGL ES 2.0 context
- (id)init
{
    if ((self = [super init]))
    {
        //
        AWBoolean usingFog = [[AWSceneMgr Mgr] useFog];
        AWBoolean useLighting = [[AWSceneMgr Mgr] useLighting];
        AWBoolean useVertexColours = [[AWSceneMgr Mgr] useVertexColours];
        //
        // new scene player instance created & configuration values passed in
        //
        // Configuration:
        // currently stored onAWSceneMgr class which may be doing double duty
        // relating to both download functionality and configuration ownership
        controller = new AWPlayer(TRUE, useLighting, usingFog, useVertexColours);
        //
        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        if (!context || ![EAGLContext setCurrentContext:context] || ![self loadShadersAW])
        {
            return nil;
        }		
        // Create default framebuffer object. The backing will be allocated for the current layer in -resizeFromLayer
        glGenFramebuffers(1, &viewFramebuffer);
        glGenRenderbuffers(1, &viewRenderbuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, viewFramebuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, viewRenderbuffer);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, viewRenderbuffer);
    }

    return self;
}


- (void)render
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //
    ((AWPlayer*)controller)->display();
    //
    const GLenum discards[]  = {GL_DEPTH_ATTACHMENT};
    glDiscardFramebufferEXT(GL_FRAMEBUFFER, 1, discards);
    //
    [context presentRenderbuffer:GL_RENDERBUFFER];
}


- (BOOL)resizeFromLayer:(CAEAGLLayer *)layer
{
    // Allocate color buffer backing based on the current layer size
    glBindRenderbuffer(GL_RENDERBUFFER, viewRenderbuffer);
    // auto attached to frame buffer because of next line...?
    [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:layer];
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
    //
    glGenRenderbuffers(1, &depthRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24_OES, backingWidth, backingHeight);
    //
    // attach depth buffer to frame buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
        return NO;
    }
    //
	const char* fileCStr = [sceneName cStringUsingEncoding:NSASCIIStringEncoding];
	AWCString fileName(fileCStr);
	if (((AWPlayer*)controller)->loadScene(fileName))
	{
		((AWPlayer*)controller)->init(backingWidth, backingHeight);
	}
    //
    glBindRenderbuffer(GL_RENDERBUFFER, viewRenderbuffer);
    return YES;
}


- (BOOL)loadShadersAW
{	
	// Create and compile vertex shader
    NSString* vsFile = [[NSBundle mainBundle] pathForResource:@"ES1dot1" ofType:@"vsh"];
     //NSString* vsFile = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"vsh"];
	const GLchar* vsSource = (GLchar *)[[NSString stringWithContentsOfFile:vsFile encoding:NSUTF8StringEncoding error:nil] UTF8String];
    if (!vsSource)
    {
        NSLog(@"Failed to load vertex shader");
        return FALSE;
    }
	
	// Create and compile fragment shader
    NSString* fragFile = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"fsh"];
	const GLchar* fsSource = (GLchar *)[[NSString stringWithContentsOfFile:fragFile encoding:NSUTF8StringEncoding error:nil] UTF8String];
    if (!fsSource)
    {
        NSLog(@"Failed to load vertex shader");
        return FALSE;
    }
    
    // Create and compile fragment shader
    NSString* discardFragFile = [[NSBundle mainBundle] pathForResource:@"DiscardShader" ofType:@"fsh"];
	const GLchar* dfsSource = (GLchar *)[[NSString stringWithContentsOfFile:discardFragFile encoding:NSUTF8StringEncoding error:nil] UTF8String];
    if (!fsSource)
    {
        NSLog(@"Failed to load vertex shader");
        return FALSE;
    }
    
    ((AWPlayer*)controller)->m_renderer->addVertexProgram(vsSource, fsSource);
    ((AWPlayer*)controller)->m_renderer->addVertexProgram(vsSource, dfsSource);
    
    return TRUE;
}


- (void)dealloc
{
    ((AWPlayer*)controller)->shutdown();
    //
    // Tear down GL
    if (viewFramebuffer)
    {
        glDeleteFramebuffers(1, &viewFramebuffer);
        viewFramebuffer = 0;
    }

    if (viewRenderbuffer)
    {
        glDeleteRenderbuffers(1, &viewRenderbuffer);
        viewRenderbuffer = 0;
    }
    
    if (depthRenderbuffer)
    {
        glDeleteRenderbuffersOES(1, &depthRenderbuffer);
        depthRenderbuffer = 0;
    }

    // Tear down context
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];

    context = nil;
}

- (void)pause
{
	((AWPlayer*)controller)->pause();
}

- (void)resume
{
	((AWPlayer*)controller)->resume();
}

- (void)zoomIn
{
	((AWPlayer*)controller)->zoomIn();
}

- (void)zoomOut
{
	((AWPlayer*)controller)->zoomOut();
}

- (void)setXYZRotation:(float)x  y:(float)y z:(float)z
{	
	((AWPlayer*)controller)->setXYZRotation(x, y, z, false);
}

@end
