//
//  LocalScenesController.h
//  SceneMgr
//
//  Created by Tom Gilbert on 05/07/2009.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AWSceneMgr.h"
#import "SceneViewController.h"

@interface LocalScenesController : SceneViewController 
	<UITableViewDelegate, UITableViewDataSource> {
		UIBarButtonItem* __strong editButton;
}
@property (nonatomic, strong) UIBarButtonItem* editButton;

-(IBAction)toggleEdit:(id)sender; 

@end
