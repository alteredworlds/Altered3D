//
//  ES1Renderer.h
//  Test
//
//  Created by Tom Gilbert on 12/06/2010.
//  Copyright __MyCompanyName__ 2010. All rights reserved.
//

#import "ESRenderer.h"

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

@class MyVideoBuffer;

@interface ES1Renderer : NSObject <ESRenderer>
{
@private
    EAGLContext *context;

    // The pixel dimensions of the CAEAGLLayer
    GLint backingWidth;
    GLint backingHeight;
	
	// OpenGL names for the renderbuffer and framebuffers used to render to this view
	GLuint viewRenderbuffer, viewFramebuffer;
	
	// OpenGL name for the depth buffer that is attached to viewFramebuffer
	//  if it exists (0 if it does not exist) 
	GLuint depthRenderbuffer;
	//
	void* controller;
	//
	NSString* sceneName;
	
	//MyVideoBuffer* videoTexture;
}

- (void)render;
- (BOOL)resizeFromLayer:(CAEAGLLayer *)layer;
- (void)pause;
- (void)resume;
- (void)zoomIn;
- (void)zoomOut;
- (void)setXYZRotation:(float)x  y:(float)yCoord z:(float)zCoord;

@property (nonatomic) NSString* sceneName;

@end
