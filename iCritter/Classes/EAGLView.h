//
//  EAGLView.h
//  JumpingBot
//
//  Created by Tom Gilbert on 18/08/2008.
//  Copyright AlteredWorlds 2008. All rights reserved.
//


#import <UIKit/UIKit.h>
#import <CoreMotion/CMMotionManager.h>
#import "ESRenderer.h"
#import "AWSceneMgr.h"

//This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
//The view content is basically an EAGL surface you render your OpenGL scene into.
//Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
@interface EAGLView : UIView
{	
@private
	id <ESRenderer> renderer;
	
	BOOL animating;
	NSInteger animationFrameInterval;
	// Use of the CADisplayLink class is the preferred method for controlling your animation timing.
    // CADisplayLink will link to the main display and fire every vsync when added to a given run-loop.
    id displayLink;
	//
	CMMotionManager* motionManager;
	CMAttitude* referenceAttitude;
	double filteredAcceleration[3];
	//
	AWSceneMgr* __weak sceneMgr;
}

@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;
@property (nonatomic,weak) AWSceneMgr* sceneMgr;
@property (nonatomic) CMMotionManager* motionManager;
@property (nonatomic) CMAttitude* referenceAttitude;

- (void)startAnimation;
- (void)stopAnimation;
- (void)drawView;

@end
