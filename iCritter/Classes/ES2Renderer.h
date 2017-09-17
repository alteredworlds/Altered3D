//
//  ES2Renderer.h
//  Test
//
//  Created by Tom Gilbert on 12/06/2010.
//  Copyright __MyCompanyName__ 2010. All rights reserved.
//

#import "ESRenderer.h"

#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import "AWVertexProgram.h"

@interface ES2Renderer : NSObject <ESRenderer>
{
@private
    EAGLContext *context;

    // The pixel dimensions of the CAEAGLLayer
    GLint backingWidth;
    GLint backingHeight;

    // The OpenGL ES names for the framebuffer and renderbuffer used to render to this view
    GLuint viewFramebuffer, viewRenderbuffer;
    
    // OpenGL name for the depth buffer that is attached to viewFramebuffer
	//  if it exists (0 if it does not exist) 
	GLuint depthRenderbuffer;    
    //
	void* controller;
    //
    NSString* sceneName;
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

