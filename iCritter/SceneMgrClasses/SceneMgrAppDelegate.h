//
//  SceneMgrAppDelegate.h
//  SceneMgr
//
//  Created by Tom Gilbert on 05/07/2009.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AWSceneMgr.h"

@interface SceneMgrAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow* window;
	IBOutlet UINavigationController* navController;
	AWSceneMgr* sceneMgr;
}

@property (nonatomic, retain) IBOutlet UIWindow* window;
@property (nonatomic, retain) UINavigationController* navController;
@property (retain) AWSceneMgr* sceneMgr;

@end

