//
//  SceneViewController.m
//  SceneMgr
//
//  Created by Tom Gilbert on 05/07/2009.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "SceneViewController.h"


@implementation SceneViewController

@synthesize sceneMgr;

- (id)initWithStyle:(UITableViewStyle)style { 
    if ((self = [super initWithStyle:style])) { 
		self.sceneMgr = [AWSceneMgr Mgr];
    } 
    return self; 
} 

// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return YES;
}

@end
