//
//  EAGLView.mm
//  JumpingBot
//
//  Created by Tom Gilbert on 18/08/2008.
//  Copyright AlteredWorlds 2008. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>

#import "EAGLView.h"
//
// AlteredWorlds headers
#import "AWToolDefs.h"
#import "AWPlayer.h"
#import "ES1Renderer.h"
#import "ES2Renderer.h"

#import "iCritterAppDelegate.h"

#define kAccelerometerFrequency        40.0 //Hz
#define kFilteringFactor               0.1

#define HORIZ_SWIPE_DRAG_MIN 20 
#define VERT_SWIPE_DRAG_MAX 20

// A class extension to declare private methods
@interface EAGLView ()

@property (nonatomic, weak) NSTimer *animationTimer;

- (IBAction) handlePinchGesture:(UIGestureRecognizer*)sender;
- (id)		 initGL;
- (void)	 configureAccelerometer;
- (void)     updateAccelerationInfo;

@end


@implementation EAGLView

@dynamic animationFrameInterval;
@synthesize animationTimer;
@synthesize sceneMgr;
@synthesize animating;
@synthesize motionManager;
@synthesize referenceAttitude;


// You must implement this
+ (Class)layerClass {
	return [CAEAGLLayer class];
}

//The GL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id)initWithCoder:(NSCoder*)coder {
	if ((self = [super initWithCoder:coder])) 
	{
		self = [self initGL];
	}
	return self;
} 

-(id) initWithFrame:(CGRect)frame
{
    if ((self = [super initWithFrame:frame])) 
	{
		self = [self initGL];
    }
    return self;
}

- (id) initGL
{
	self.sceneMgr = [AWSceneMgr Mgr];
	//
	self.multipleTouchEnabled = TRUE;
	self.exclusiveTouch = TRUE;
	[self setContentScaleFactor:[UIScreen mainScreen].scale];
	
	animating = FALSE;
	animationFrameInterval = 1;
	displayLink = nil;
	animationTimer = nil;
    //
	UIPinchGestureRecognizer* pinchGesture = 
		[[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(handlePinchGesture:)];
	[self addGestureRecognizer:pinchGesture]; 
	//
	UITapGestureRecognizer* singleFingerDTap = 
		[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleSingleDoubleTap:)]; 
	singleFingerDTap.numberOfTapsRequired = 2; 
	[self addGestureRecognizer:singleFingerDTap]; 
	//
	// Get the layer
	CAEAGLLayer* eaglLayer = (CAEAGLLayer*)self.layer;
	//
	eaglLayer.opaque = YES;
	eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
									[NSNumber numberWithBool:NO], 
									kEAGLDrawablePropertyRetainedBacking, 
									kEAGLColorFormatRGBA8, 
									kEAGLDrawablePropertyColorFormat, 
									nil];
    if (self.sceneMgr.isOpenGL1dot1)
    {
        renderer = [[ES1Renderer alloc] init];
    }
    else
    {
        renderer = [[ES2Renderer alloc] init];
    }
	if (!renderer)
	{
		return nil;
	}
	//
	NSString* file = [sceneMgr getActiveSceneFileName];
	[renderer setSceneName:file];
	//
	return self;
}

- (NSInteger)animationFrameInterval
{
    return animationFrameInterval;
}

- (void)setAnimationFrameInterval:(NSInteger)frameInterval
{
    // Frame interval defines how many display frames must pass between each time the
    // display link fires. The display link will only fire 30 times a second when the
    // frame internal is two on a display that refreshes 60 times a second. The default
    // frame interval setting of one will fire 60 times a second when the display refreshes
    // at 60 times a second. A frame interval setting of less than one results in undefined
    // behavior.
    if (frameInterval >= 1)
    {
        animationFrameInterval = frameInterval;		
        if (animating)
        {
            [self stopAnimation];
            [self startAnimation];
        }
    }
}

-(void)configureAccelerometer
{
    motionManager = [[CMMotionManager alloc] init];
	motionManager.deviceMotionUpdateInterval = 0.02; // 50 Hz
	if (false)//motionManager.isDeviceMotionAvailable)
	{
		[motionManager startDeviceMotionUpdates];
	}
	else 
		[motionManager startAccelerometerUpdates];
}

-(void)cleanupAccelerometer
{
	if (motionManager.isDeviceMotionActive)
	{
		referenceAttitude = motionManager.deviceMotion.attitude;
		[motionManager stopDeviceMotionUpdates];
	}
	else 
		[motionManager stopAccelerometerUpdates];
}


- (void)updateAccelerationInfo
{
	if (motionManager.isDeviceMotionActive)
	{
		if (!referenceAttitude)
			referenceAttitude = motionManager.deviceMotion.attitude;
		CMAttitude* currentAttitude = motionManager.deviceMotion.attitude; 
		[currentAttitude multiplyByInverseOfAttitude: referenceAttitude];  
		//[renderer setXYZRotation:currentAttitude.pitch y:currentAttitude.roll z:currentAttitude.yaw];
		[renderer setXYZRotation:0.0f y:0.0f z:currentAttitude.yaw];
	}
	else
	{
		CMAccelerometerData* accel = motionManager.accelerometerData;
		if (accel)
		{
			// Use a basic low-pass filter to keep only the gravity component of each axis.
			filteredAcceleration[0] = (accel.acceleration.x * kFilteringFactor) + 
			(filteredAcceleration[0] * (1.0 - kFilteringFactor));
			filteredAcceleration[1] = (accel.acceleration.y * kFilteringFactor) + 
			(filteredAcceleration[1] * (1.0 - kFilteringFactor));
			filteredAcceleration[2] = (accel.acceleration.z * kFilteringFactor) + 
			(filteredAcceleration[2] * (1.0 - kFilteringFactor));
			//
			float radians = atan2f(filteredAcceleration[0], filteredAcceleration[1]) + M_PI;
			[renderer setXYZRotation:0.0f y:0.0f z:radians];			
		}
	}
}

- (IBAction)handlePinchGesture:(UIGestureRecognizer*)sender 
{ 
	CGFloat factor = [(UIPinchGestureRecognizer*)sender scale];
	if (factor > 1.0)
	{	// fingers moving TOGETHER => PINCH CLOSE
		[renderer zoomIn];
	}
	else
	{	// fingers moving APART => PINCH OPEN
		[renderer zoomOut];
	}
}

- (IBAction)handleSingleDoubleTap:(UIGestureRecognizer*)sender
{
	iCritterAppDelegate* appDel = [UIApplication sharedApplication].delegate;
	[appDel switchToSceneMgrView];
}

- (void)drawView 
{
	[self updateAccelerationInfo];
	[renderer render];
}

- (void)layoutSubviews 
{
	[renderer resizeFromLayer:(CAEAGLLayer*)self.layer];
	[self drawView];
}

- (void)startAnimation 
{
	if (!animating)
    {
        displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self 
            selector:@selector(drawView)];
        [displayLink setFrameInterval:animationFrameInterval];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
		//
		[self configureAccelerometer];
		[renderer resume];
		//
        animating = TRUE;
    }
}

- (void)stopAnimation 
{
	if (animating)
    {
		[renderer pause];
		[self cleanupAccelerometer];
        [displayLink invalidate];
        displayLink = nil;		
        animating = FALSE;
    }
}


@end
