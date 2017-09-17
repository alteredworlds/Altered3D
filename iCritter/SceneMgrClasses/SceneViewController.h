//
//  SceneViewController.h
//  SceneMgr
//
//  Created by Tom Gilbert on 05/07/2009.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AWSceneMgr.h"


@interface SceneViewController : UITableViewController {
	AWSceneMgr* __weak sceneMgr;
}
@property (weak) AWSceneMgr* sceneMgr;
@end
