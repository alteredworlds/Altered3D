//
//  ESRenderer.h
//  Test
//
//  Created by Tom Gilbert on 12/06/2010.
//  Copyright __MyCompanyName__ 2010. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>

#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>

@protocol ESRenderer <NSObject>

- (void)render;
- (BOOL)resizeFromLayer:(CAEAGLLayer *)layer;

- (void)pause;
- (void)resume;
- (void)zoomIn;
- (void)zoomOut;
- (void)setXYZRotation:(float)x  y:(float)yCoord z:(float)zCoord;

@property (nonatomic, retain) NSString* sceneName;

@end
