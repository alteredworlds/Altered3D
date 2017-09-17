//
//  RemoteScenesController.m
//  SceneMgr
//
//  Created by Tom Gilbert on 05/07/2009.
//  Copyright 2009 AlteredWorlds. All rights reserved.
//

#import "RemoteScenesController.h"


@implementation RemoteScenesController

@synthesize downloader;
@synthesize remoteScenes;
@synthesize remoteScenesToDisplay;
@synthesize activeRow;

NSString* remoteScenesURL = @"http://www.alteredworlds.com/updates/current/iPhone/";


- (id)initWithStyle:(UITableViewStyle)style { 
    if ((self = [super initWithStyle:style])) { 
		downloader = nil;
    } 
    return self; 
}


- (void)viewWillAppear:(BOOL)animated { 
	if (nil == downloader)
	{	// download is NOT in progress...
		if (nil == remoteScenes)
		{	// remote scene list has NOT been retrieved...
			downloader = [[AWDownloader alloc] init];
			[downloader startDownload:remoteScenesURL to:sceneMgr.docsDir withDelegate:self];
		}
		else
		{	// already retrieved remote scene list, but update again in case local scenes changed
			[self displayData];
		}
	}
    [super viewWillAppear:animated]; 
} 

- (void)fileAvailable:(NSString*)file
{   // this file should contain the directory listing...
	if (nil != downloader)
	{
		// first, load scene list so we can remove scenes already present...
		NSString* listing = [[NSString alloc] initWithContentsOfFile:file encoding:NSUTF8StringEncoding error:nil];
		remoteScenes = [AWRemoteSceneInfo getRemoteScenes:listing];
		self.downloader = nil;
		[self performSelectorOnMainThread:@selector(displayData) withObject:nil waitUntilDone:NO];
	}
}

- (void) displayData
{
	[self.sceneMgr loadScenes];
	self.remoteScenesToDisplay = [self.sceneMgr filterRemoteScenes:self.remoteScenes];
	[[self tableView] reloadData];
}

- (void)fileTransferError:(NSError*)error
{
}


- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}



#pragma mark - 
#pragma mark Table Data Source Methods 
- (NSInteger)tableView:(UITableView *)tableView 
						numberOfRowsInSection:(NSInteger)section { 
	if (nil != downloader)
		return 1;
	else
		return [remoteScenesToDisplay count];
}


- (UITableViewCell *)tableView:(UITableView *)tableView 
					 cellForRowAtIndexPath:(NSIndexPath *)indexPath { 
    static NSString * RemoteScenesControllerCellIdentifier = @"RemoteScenesControllerCellIdentifier"; 
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:RemoteScenesControllerCellIdentifier]; 
    if (cell == nil) { 
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault  
                                        reuseIdentifier: RemoteScenesControllerCellIdentifier]; 
    } 
	[cell setAccessoryView:nil];
	NSUInteger row = [indexPath row]; 
	bool shouldShowSpinner = false;
	if (nil != downloader)
	{
		cell.textLabel.text = @"Loading...";
		cell.textLabel.textColor = [UIColor grayColor];
		shouldShowSpinner = true;
	}
	else
	{
		AWRemoteSceneInfo* sceneInfo = [remoteScenesToDisplay objectAtIndex:row];
		if (nil != sceneInfo)
		{
			cell.textLabel.text = sceneInfo.displayName;
			cell.textLabel.textColor = [UIColor blackColor];
			sceneInfo.atRow = indexPath;
			if (nil != sceneInfo.downloader)
			{				
				shouldShowSpinner = true;
			}
			else
			{
				if (sceneInfo.availableLocally)
					cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
				else
					cell.accessoryType = UITableViewCellAccessoryDetailDisclosureButton;
			}
		}
	}
	if (shouldShowSpinner)
	{
		[self showSpinner:cell];
	}
    return cell; 
}

- (void) showSpinner:(UITableViewCell*)cell
{
	UIActivityIndicatorView* spinner = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray];
	[cell setAccessoryView:spinner];
	[spinner startAnimating];
}

#pragma mark - 
#pragma mark Table Delegate Methods 
- (void)tableView:(UITableView *)tableView 
				  didSelectRowAtIndexPath:(NSIndexPath *)indexPath { 
	activeRow = indexPath;
	AWRemoteSceneInfo* sceneInfo = [remoteScenesToDisplay objectAtIndex:[activeRow row]];
	if ((nil != sceneInfo) && ![sceneInfo availableLocally])
	{
		UIActionSheet *actionSheet = [[UIActionSheet alloc] 
            initWithTitle:@"Download this scene now?" 
            delegate:self 
            cancelButtonTitle:@"Cancel" 
            destructiveButtonTitle:@"Download" 
            otherButtonTitles:nil,
            nil];
		[actionSheet showInView:tableView];
	}
} 

- (void)tableView:(UITableView *)tableView 
			      accessoryButtonTappedForRowWithIndexPath:(NSIndexPath *)indexPath 
{ 
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle: 
						  @"Hey, do you see the acccesory button?" 
                                                    message:@"If you're trying to drill down, touch that instead" 
                                                   delegate:nil 
                                          cancelButtonTitle:@"Won't happen again" 
                                          otherButtonTitles:nil,
                          nil]; 
	[alert show]; 
}

- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
	NSIndexPath* theActiveRow = activeRow;
	if (0 == buttonIndex)
	{
		// first, find the relevant AWSceneInfo...
		NSUInteger row = [theActiveRow row]; 
		AWRemoteSceneInfo* sceneInfo = [remoteScenesToDisplay objectAtIndex:row];
		[sceneInfo startDownloadFrom:remoteScenesURL to:sceneMgr.docsDir withNotification:self];
		//
		UITableViewCell* cell = [self.tableView cellForRowAtIndexPath:theActiveRow];
		[self showSpinner:cell];
	}
	activeRow = nil;
}

- (void) sceneAvailable:(AWSceneInfo*)sceneInfo
{
	int idx = 1;
	for (AWRemoteSceneInfo* info in remoteScenesToDisplay)
	{
		if (NSOrderedSame == [[info displayName] compare:[sceneInfo displayName]])
		{	// this is the one...
			info.downloader = nil;
			NSIndexPath* ip = [info atRow];
			[self performSelectorOnMainThread:@selector(clearCellSpinnerOnMainThread:) withObject:ip waitUntilDone:YES];
			break;
		}
		idx += 1;
	}
}

-(void) clearCellSpinnerOnMainThread:(NSIndexPath*)row
{
	UITableViewCell* cell = [self.tableView cellForRowAtIndexPath:row];
	[cell setAccessoryView:nil];
	[cell setAccessoryType:UITableViewCellAccessoryDisclosureIndicator];
}

- (void) sceneTransferError:(NSError*)error forSceneInfo:(AWSceneInfo*)sceneInfo
{
}

@end
