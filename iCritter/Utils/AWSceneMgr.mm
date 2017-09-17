//
//  AWSceneMgr.m
//  JumpingBot
//
//  Created by Tom Gilbert on 27/06/2009.
//  Copyright 2009 AlteredWorlds. All rights reserved.
//
// the intention is to make this a SINGLETON

#import "AWSceneMgr.h"
#import "AWSceneInfo.h"
#import "AWRemoteSceneInfo.h"

// A class extension to declare private methods
@interface AWSceneMgr ()

@end


@implementation AWSceneMgr

@synthesize scenes;
@synthesize docsDir;


+ (AWSceneMgr*) Mgr
{
    static AWSceneMgr*      sharedSceneMgr = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedSceneMgr = [[AWSceneMgr alloc] init];
    });
    return sharedSceneMgr;
}


-(id)init
{
	if ((self = [super init])) 
	{
		// find the Documents folder in which all app data lives
		NSArray* tmpArray = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
		self.docsDir = [tmpArray objectAtIndex:0];
        self.useLighting = TRUE;
        self.useFog = TRUE;
        self.useVertexColours = TRUE;
        self.isOpenGL1dot1 = FALSE;
	}
	return self;
}

// NOT USED...!

-(NSString*) getActiveSceneFileName
{
	if (nil == activeSceneFileName)
	{
		[self loadScenes];
		activeSceneFileName = [self getSceneFileName:0];
	}
	return activeSceneFileName;
}

-(void) setActiveSceneFileName:(NSString*) sceneName
{
	if (nil != activeSceneFileName)
	{
		//[activeSceneFileName release];
		activeSceneFileName = nil;
	}
	activeSceneFileName = sceneName;
}


- (int) loadScenes
{  
	self.scenes = nil;	
	self.scenes = [[NSMutableArray alloc] init];
	// FIRST: add default scene (distributed with application) is in the Application Bundle
	NSString* pathToResource = [[NSBundle mainBundle] pathForResource:@"iCritter" ofType:@"tnt"];
	AWSceneInfo* sceneInfo = [[AWSceneInfo alloc] initWithFileName:pathToResource existsLocally:true];
	[scenes addObject:sceneInfo];
	//
	NSString* file;
	NSString* pathElement;
	NSDirectoryEnumerator* dirEnum = [[NSFileManager defaultManager] enumeratorAtPath:docsDir];	
	while ((file = [dirEnum nextObject])) {
		if ([file hasSuffix:@".tnt"]) 
		{
			pathElement = [docsDir stringByAppendingPathComponent:file];
			sceneInfo = [[AWSceneInfo alloc] initWithFileName:pathElement existsLocally:true];
			[scenes addObject:sceneInfo];
		}
	}
	// now find the path to each of the scenes found in here, if any, adding each to array
	return [scenes count];
}

- (bool) removeScene:(int)index
{
	bool retVal = false;
	NSError* err;
	AWSceneInfo* sceneInfo = [scenes objectAtIndex:index];
	if (YES==[[NSFileManager defaultManager] removeItemAtPath:sceneInfo.fileName error:&err])
	{
		[scenes removeObjectAtIndex:index];
		retVal = true;
	}
	return retVal;
}

- (NSString*) getSceneFileName:(int)index
{
	NSString* retVal = nil;
	AWSceneInfo* sceneInfo = [scenes objectAtIndex:index];
	retVal = [sceneInfo fileName];
	return retVal;
}

- (NSString*) getSceneDisplayName:(int)index
{
	NSString* retVal = nil;
	AWSceneInfo* sceneInfo = [scenes objectAtIndex:index];
	retVal = [sceneInfo displayName];
	return retVal;
}

- (bool) sceneExists:(NSString*)displayName
{
	bool retVal = false;
	NSString* localDisplayName;
	for (AWSceneInfo* sceneInfo in scenes) 
	{ 
		localDisplayName = [sceneInfo displayName];
		if (NSOrderedSame == [displayName compare:localDisplayName])
		{
			retVal = true;
			break;
		}
	}
	return retVal;
}

- (NSMutableArray*) filterRemoteScenes:(NSMutableArray*)remoteScenes
{
	NSMutableArray* retVal = [[NSMutableArray alloc] init];
	for (AWRemoteSceneInfo* sceneInfo in remoteScenes)
	{
		if (![self sceneExists:sceneInfo.displayName])
			[retVal addObject:sceneInfo];
	}
	return retVal;
}

@end
