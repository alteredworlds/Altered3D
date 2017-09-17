//
//  AWSceneInfo.h
//  SceneMgr
//
//  Created by Tom Gilbert on 11/07/2009.
//  Copyright 2009 AlteredWorlds. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AWDownloader.h"
#import "AWSceneInfo.h"


@protocol NotifySceneAvailable 
- (void) sceneAvailable:(AWSceneInfo*)sceneInfo; 
- (void) sceneTransferError:(NSError*)error forSceneInfo:(AWSceneInfo*)sceneInfo; 
@end 


@interface AWRemoteSceneInfo : AWSceneInfo <NotifyFileAvailable> 
{
	AWDownloader* downloader;
	id<NotifySceneAvailable> __weak delegate;
}

@property  AWDownloader* downloader;
@property (weak) id<NotifySceneAvailable> delegate;

+ (NSMutableArray*) getRemoteScenes:(NSString*)content;
+ (NSString*) getSceneDisplayName:(NSString*)fName;

- (id) initWithFileName:(NSString*)fName  existsLocally:(bool)exists;
- (void) startDownloadFrom:(NSString*)folder to:(NSString*)toPath withNotification:(id<NotifySceneAvailable>)dele;

@end
