//
//  AWSceneInfo.mm
//  SceneMgr
//
//  Created by Tom Gilbert on 11/07/2009.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "AWSceneInfo.h"
//#import "AWSceneMgr.h"


@implementation AWSceneInfo

@synthesize displayName;
@synthesize fileName;
@synthesize availableLocally;
@synthesize atRow;

NSString* localExtString = @".tnt";




- (id) initWithFileName:(NSString*)fName existsLocally:(bool)exists
{
	if ((self = [super init])) 
	{
        self.fileName = fName;
		self.displayName = [AWSceneInfo getSceneDisplayName:fileName];
		availableLocally = exists;
    }
	return self;
}

+ (NSString*) getSceneDisplayName:(NSString*)fName
{
	NSArray* pathCom = [fName pathComponents]; 
	NSString* fullFileName = [pathCom objectAtIndex:([pathCom count]-1)];
    NSMutableString* retVal = [[NSMutableString alloc] initWithString:fullFileName];
	[retVal replaceOccurrencesOfString:localExtString withString:@"" options:NSLiteralSearch range:NSMakeRange(0, [retVal length])];
	return retVal;	
}

@end
