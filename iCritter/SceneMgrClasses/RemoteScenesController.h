//
//  RemoteScenesController.h
//  SceneMgr
//
//  Created by Tom Gilbert on 05/07/2009.
//  Copyright 2009 AlteredWorlds. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "SceneViewController.h"
#import "AWDownloader.h"
#import "AWRemoteSceneInfo.h"

@interface RemoteScenesController : SceneViewController 
	<UITableViewDelegate, UITableViewDataSource, NotifyFileAvailable, NotifySceneAvailable, UIActionSheetDelegate> {
	AWDownloader* downloader;
	NSMutableArray*  remoteScenes;
	NSMutableArray*  remoteScenesToDisplay;
	NSIndexPath* activeRow;
}

@property  AWDownloader* downloader;
@property (nonatomic) NSMutableArray* remoteScenes;
@property (nonatomic) NSMutableArray* remoteScenesToDisplay;
@property  NSIndexPath* activeRow;

- (void) showSpinner:(UITableViewCell*)cell;
- (void) displayData;
-(void) clearCellSpinnerOnMainThread:(NSIndexPath*)row;

@end
