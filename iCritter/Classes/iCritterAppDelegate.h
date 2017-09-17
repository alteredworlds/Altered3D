//
//  iCritterAppDelegate.h
//  iCritter
//
//  Created by Tom Gilbert on 17/07/2009.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AWSceneMgr.h"

@class iCritterViewController;

@interface iCritterAppDelegate : NSObject <UIApplicationDelegate> {
	IBOutlet UIWindow* window;
	IBOutlet __strong iCritterViewController* viewController;
	UINavigationController* navController;
	AWSceneMgr* __strong sceneMgr;
	
@private
	/* The pixel dimensions of the screen */
	CGFloat sceneWidth;
	CGFloat sceneHeight;
}

@property (nonatomic) IBOutlet UIWindow* window;
@property (nonatomic, strong) IBOutlet iCritterViewController* viewController;
@property (nonatomic) UINavigationController* navController;
@property (strong) AWSceneMgr* sceneMgr;
@property (assign) CGFloat sceneWidth;
@property (assign) CGFloat sceneHeight;

- (id) init;
- (void)switchToSceneMgrView;
- (void)switchToSceneView;
//- (AWConfig&) cfg;

@end

