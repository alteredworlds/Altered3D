//
//  ES1Renderer.m
//  Test
//
//  Created by Tom Gilbert on 12/06/2010.
//  Copyright __MyCompanyName__ 2010. All rights reserved.
//

#import "ES1Renderer.h"
//
// AlteredWorlds headers
#import "AWToolDefs.h"
#import "AWPlayer.h"
#import "AWGLRenderer.h"
#import "MyVideoBuffer.h"
#import "AWSceneMgr.h"

@implementation ES1Renderer

@synthesize sceneName;

// Create an OpenGL ES 1.1 context
- (id)init
{
    if ((self = [super init]))
    {
        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
        if (!context || ![EAGLContext setCurrentContext:context])
        {
            return nil;
        }
        // Create default framebuffer object. The backing will be allocated for the current layer in -resizeFromLayer
        glGenFramebuffersOES(1, &viewFramebuffer);
        glGenRenderbuffersOES(1, &viewRenderbuffer);
        glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
        glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderbuffer);
		//
        AWBoolean useFog = [[AWSceneMgr Mgr]  useFog];
        AWBoolean useLights = [[AWSceneMgr Mgr]  useLighting];
        AWBoolean useVertexColours = [[AWSceneMgr Mgr] useVertexColours];
		controller = new AWPlayer(FALSE, useLights, useFog, useVertexColours);
    }
    return self;
}

- (void)render
{
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//
	((AWPlayer*)controller)->display();
	//
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
	[context presentRenderbuffer:GL_RENDERBUFFER_OES];
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

- (BOOL)resizeFromLayer:(CAEAGLLayer*)layer
{	
    // Allocate color buffer backing based on the current layer size
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
    [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:layer];
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
	
	glGenRenderbuffersOES(1, &depthRenderbuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
	glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT24_OES, backingWidth, backingHeight);
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);	
	
	if(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) 
	{
		NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
		return NO;
	}
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
	//
	const char* fileCStr = [sceneName cStringUsingEncoding:NSASCIIStringEncoding];
	AWCString fileName(fileCStr);
	if (((AWPlayer*)controller)->loadScene(fileName))
	{
		((AWPlayer*)controller)->init(backingWidth, backingHeight);
	}
    return YES;
}

- (void)dealloc
{
	((AWPlayer*)controller)->shutdown();
	//
    // Tear down GL
    if (viewFramebuffer)
    {
        glDeleteFramebuffersOES(1, &viewFramebuffer);
        viewFramebuffer = 0;
    }
    if (viewRenderbuffer)
    {
        glDeleteRenderbuffersOES(1, &viewRenderbuffer);
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

@end
