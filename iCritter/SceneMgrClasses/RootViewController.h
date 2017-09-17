//
//  RootViewController.h
//  SceneMgr
//
//  Created by Tom Gilbert on 05/07/2009.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "SceneViewController.h"

@interface RootViewController : SceneViewController 
	<UITableViewDelegate, UITableViewDataSource> {
	NSArray* controllers;
}
@property (nonatomic) NSArray* controllers; 
@end
