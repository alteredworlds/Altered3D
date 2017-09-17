//
//  AWDownloader.h
//  JumpingBot
//
//  Created by Tom Gilbert
//  Copyright 2009 AlteredWorlds. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol NotifyFileAvailable 
- (void) fileAvailable:(NSString*)file; 
- (void) fileTransferError:(NSError*)error; 
@end 


@interface AWDownloader : NSObject 
{
	NSMutableData* receivedData;
	NSString* localPath;
	NSString* fileName;
	id<NotifyFileAvailable> __weak delegate;
}


@property  NSString* localPath; 
@property  NSString* fileName;
@property (weak) id<NotifyFileAvailable> delegate;


- (void) startDownload:(NSString*)url to:(NSString*)toPath withDelegate:(id<NotifyFileAvailable>)dele;
- (void) connection:(NSURLConnection*)connection didReceiveResponse:(NSURLResponse*)response;
- (void) connection:(NSURLConnection *)connection didReceiveData:(NSData *)data;
- (void) connectionDidFinishLoading:(NSURLConnection *)connection;

@end
