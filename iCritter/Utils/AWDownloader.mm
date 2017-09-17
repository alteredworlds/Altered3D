//
//  AWDownloader.mm
//  JumpingBot
//
//  Created by Tom Gilbert on 23/09/2008.
//  Copyright 2008 AlteredWorlds. All rights reserved.
//

#import "AWDownloader.h"


@implementation AWDownloader

@synthesize localPath; 
@synthesize fileName;
@synthesize delegate;





- (void) startDownload:(NSString*)uri to:(NSString*)toPath withDelegate:(id<NotifyFileAvailable>)dele;
{  
	// derive the filename from the supplied URL
	NSURL* nsUrl = [NSURL URLWithString:uri];	
	self.localPath = toPath;
	self.fileName = [[nsUrl path] lastPathComponent];	
	// create the request
	NSURLRequest* theRequest=[NSURLRequest requestWithURL:nsUrl
										   cachePolicy:NSURLRequestUseProtocolCachePolicy
										   timeoutInterval:60.0];
	// create the connection with the request
	// and start loading the data
	NSURLConnection *theConnection=[[NSURLConnection alloc] initWithRequest:theRequest delegate:self];
	if (theConnection) {
		// Create the NSMutableData that will hold
		// the received data
		// receivedData is declared as a method instance elsewhere
		self.delegate = dele;
		receivedData=[NSMutableData data];
	} else {
		// inform the user that the download could not be made
	}
}

- (void)connection:(NSURLConnection*)connection didReceiveResponse:(NSURLResponse*)response
{
	// this method is called when the server has determined that it
	// has enough information to create the NSURLResponse
	
	// it can be called multiple times, for example in the case of a
	// redirect, so each time we reset the data.
	// receivedData is declared as a method instance elsewhere
	[receivedData setLength:0];
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
	// append the new data to the receivedData
	// receivedData is declared as a method instance elsewhere
	[receivedData appendData:data];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
	// do something with the data
	// receivedData is declared as a method instance elsewhere
	NSLog(@"Succeeded! Received %d bytes of data", [receivedData length]);
	// write the data out to a file...
	NSString* fullLocalPath = [localPath stringByAppendingPathComponent:fileName];
	[receivedData writeToFile:fullLocalPath atomically:YES];
	//	
	// release the connection, and the data object
	connection = nil;
	receivedData = nil;
	//
	[delegate fileAvailable:fullLocalPath];
}

- (void)connection:(NSURLConnection *)connection
  didFailWithError:(NSError*)error
{
	// release the connection, and the data object
	connection = nil;
	// receivedData is declared as a method instance elsewhere
	receivedData = nil;
	
	// inform the user
	NSLog(@"Connection failed! Error - %@ %@",
			[error localizedDescription],
			[[error userInfo] objectForKey:NSURLErrorFailingURLStringErrorKey]);
}

-(NSCachedURLResponse *)connection:(NSURLConnection *)connection
                 willCacheResponse:(NSCachedURLResponse *)cachedResponse
{
	return nil;
}


@end
