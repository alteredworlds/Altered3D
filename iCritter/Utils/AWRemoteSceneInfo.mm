//
//  AWSceneInfo.mm
//  SceneMgr
//
//  Created by Tom Gilbert on 11/07/2009.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "AWRemoteSceneInfo.h"

extern void file_uncompress(const char* file);

@implementation AWRemoteSceneInfo

@synthesize downloader;
@synthesize delegate;


NSString* startString = @"<a href=\"";
NSString* endString = @"\">";
NSString* extString = @".tnt.gz";


+ (NSMutableArray*) getRemoteScenes:(NSString*)content
{
	NSMutableArray* retVal = [[NSMutableArray alloc] init];
	NSRange start, end, searchIn;
	NSString* fragment;
	
	int len = [content length];
	
	searchIn.location = 0;
	searchIn.length = len;
	
	@autoreleasepool {
	
		bool carryOn = true;	
		do
		{
			if ((searchIn.location + searchIn.length) > len)
			{
				carryOn = false;
			}
			else
			{
				start = [content rangeOfString:startString options:NSLiteralSearch range:searchIn];
				if (NSNotFound == start.location)
				{
					carryOn = false;
				}
				else 
				{
					searchIn.location = start.location + start.length;
					searchIn.length = len - searchIn.location;
					if ((searchIn.location + searchIn.length) > len)
					{
						carryOn = false;
					}
					else
					{
						end = [content rangeOfString:endString options:NSLiteralSearch range:searchIn]; 
						if (NSNotFound == end.location)
						{
							carryOn = false;
						}
						else 
						{
							searchIn.length = end.location - searchIn.location;
							fragment = [content substringWithRange:searchIn];
							if ([fragment hasSuffix:extString])
							{
								AWRemoteSceneInfo* sceneInfo = [[AWRemoteSceneInfo alloc] initWithFileName:[fragment copy] existsLocally:false];
								[retVal addObject:sceneInfo];
							}
							searchIn.length = len - searchIn.location;
						}
					}
				}
			}
		}
		while (carryOn);
		//
		//
		return retVal;
	}
}




- (id) initWithFileName:(NSString*)fName existsLocally:(bool)exists
{
	if ((self = [super initWithFileName:fName existsLocally:exists])) 
	{
		self.displayName = [AWRemoteSceneInfo getSceneDisplayName:fName];
    }
	return self;
}

+ (NSString*) getSceneDisplayName:(NSString*)fName
{
	NSMutableString* retVal = [[NSMutableString alloc] initWithString:fName];
	[retVal replaceOccurrencesOfString:extString withString:@"" options:NSLiteralSearch range:NSMakeRange(0, [retVal length])];
	return retVal;
}

- (void) startDownloadFrom:(NSString*)folder to:(NSString*)toPath withNotification:(id<NotifySceneAvailable>)dele
{
	if (nil == downloader)
	{
		downloader = [[AWDownloader alloc] init];
		NSMutableString* retVal = [[NSMutableString alloc] initWithString:folder];
		[retVal appendString:fileName];
		self.delegate = dele;
		[downloader startDownload:retVal to:toPath withDelegate:self];
	}
}

- (void) fileAvailable:(NSString*)file
{
	if ([file hasSuffix:extString])
	{
		const char* uncompressName = [file cStringUsingEncoding:NSASCIIStringEncoding];
		file_uncompress(uncompressName);
	}
	availableLocally = true;
	[delegate sceneAvailable:self];
}

- (void) fileTransferError:(NSError*)error
{
}


@end
