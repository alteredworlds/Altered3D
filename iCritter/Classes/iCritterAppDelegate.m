//
//  iCritterAppDelegate.m
//  iCritter
//
//  Created by Tom Gilbert on 17/07/2009.
//  Copyright AlteredWorlds 2009. All rights reserved.
//

#import "iCritterAppDelegate.h"
#import "iCritterViewController.h"
#import "RootViewController.h"


@implementation iCritterAppDelegate

@synthesize window;
@synthesize viewController;
@synthesize sceneMgr;
@synthesize navController;
@synthesize sceneWidth;
@synthesize sceneHeight;

- (id) init
{
	if ((self = [super init]))
	{
		self.sceneMgr = [AWSceneMgr Mgr];
	}
	return self;
}

- (void)applicationDidFinishLaunching:(UIApplication *)application {
	// Override point for customization after app launch	
    [window addSubview:viewController.view];
	[window makeKeyAndVisible];
}

- (void)applicationWillResignActive:(UIApplication *)application {
	[viewController stopAnimation];
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
	[viewController startAnimation];
}

- (void)switchToSceneMgrView
{
	if( viewController.view != nil ) 
	{
		CGRect frameSize = [viewController.view frame];
		self.sceneWidth = frameSize.size.width;
		self.sceneHeight =frameSize.size.height;
		[viewController shutdownGL];
		[viewController.view removeFromSuperview]; //remove view from window's subviews.
		 //cleanup scene resources 
		viewController = nil;
	}
	RootViewController* root = [[RootViewController alloc] initWithStyle:UITableViewStylePlain];
	UINavigationController* navCtrl = [[UINavigationController alloc] initWithRootViewController:root];
	self.navController = navCtrl;
	[[UIApplication sharedApplication] setStatusBarHidden:NO];
	[window addSubview:navController.view];
}

- (void)switchToSceneView
{
	if( navController.view != nil ) {
		[navController.view removeFromSuperview]; //remove view from window's subviews.
		navController = nil;
	}
	//
	[[UIApplication sharedApplication] setStatusBarHidden:YES];
	//
	NSString* imagePath = [[NSBundle mainBundle] pathForResource:@"Default" ofType:@"png"]; 
	UIImage* img = [[UIImage alloc] initWithContentsOfFile:imagePath];
	UIImageView* imageView = [[UIImageView alloc] initWithImage:img];
	imageView.clearsContextBeforeDrawing = FALSE;
	//
	[window addSubview:imageView];
	//
	[self performSelector:@selector(displayScene:) withObject:imageView afterDelay:0.02];
}

- (void) displayScene:(UIImageView*)imgView
{
	[imgView removeFromSuperview]; //remove view from window's subviews.
	self.viewController = [[iCritterViewController alloc] initWithCoder:nil];
	self.viewController.view = [[EAGLView alloc] initWithFrame:CGRectMake(0, 0, self.sceneWidth, self.sceneHeight)];
	//
	[window addSubview:viewController.view];
	[self.viewController start];
	[self.viewController startAnimation];
}




@end
