//
//  AWSceneInfo.h
//  SceneMgr
//
//  Created by Tom Gilbert on 11/07/2009.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface AWSceneInfo : NSObject 
{
	NSString* displayName;
	NSString* fileName;
	bool availableLocally;
	NSIndexPath* atRow;
}

@property  NSString* displayName;
@property  NSString* fileName;
@property bool availableLocally;
@property  NSIndexPath* atRow;

+ (NSString*) getSceneDisplayName:(NSString*)fName;

- (id) initWithFileName:(NSString*)fName  existsLocally:(bool)exists;

@end
