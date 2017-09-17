//
//  SceneMgrAppDelegate.m
//  SceneMgr
//
//  Created by Tom Gilbert on 05/07/2009.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "SceneMgrAppDelegate.h"

@implementation SceneMgrAppDelegate

@synthesize window;
@synthesize navController;
@synthesize sceneMgr;


- (void)applicationDidFinishLaunching:(UIApplication *)application {    
    // Override point for customization after application launch
	self.sceneMgr = [[AWSceneMgr alloc] init];
	[window addSubview: navController.view];
	[window makeKeyAndVisible];
}


- (void)dealloc {
	[navController release];
    [window release];
	[sceneMgr release];
    [super dealloc];
}


@end
