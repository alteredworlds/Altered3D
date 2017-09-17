//
//  AWSceneMgr.h
//  JumpingBot
//
//  Created by Tom Gilbert on 27/06/2009.
//  Copyright 2009 AlteredWorlds. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface AWSceneMgr : NSObject 
{
@private
	NSMutableArray* __strong scenes;
	NSString* docsDir;
	NSString* activeSceneFileName;
}

@property  NSString* docsDir;
@property  (strong) NSMutableArray* scenes;
@property Boolean isOpenGL1dot1;
@property Boolean useLighting;
@property Boolean useFog;
@property Boolean useVertexColours;

+ (AWSceneMgr*) Mgr;

- (id) init;
- (int) loadScenes;
- (NSString*) getSceneFileName:(int)index;
- (NSString*) getSceneDisplayName:(int)index;
- (bool) sceneExists:(NSString*)displayName;
- (NSMutableArray*) filterRemoteScenes:(NSMutableArray*)remoteScenes;
- (bool) removeScene:(int)index;
- (NSString*) getActiveSceneFileName;
- (void) setActiveSceneFileName:(NSString*) sceneName;

@end
