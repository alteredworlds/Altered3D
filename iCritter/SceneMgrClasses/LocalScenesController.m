//
//  LocalScenesController.m
//  SceneMgr
//
//  Created by Tom Gilbert on 05/07/2009.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LocalScenesController.h"
#import "iCritterAppDelegate.h"


@implementation LocalScenesController

@synthesize editButton;

- (id)initWithStyle:(UITableViewStyle)style { 
    if ((self = [super initWithStyle:style])) { 
    } 
    return self; 
}

- (void)viewDidLoad { 
    self.editButton = [[UIBarButtonItem alloc] 
								   initWithTitle:@"Edit" 
								   style:UIBarButtonItemStyleBordered 
								   target:self 
								   action:@selector(toggleEdit:)];
	self.navigationItem.rightBarButtonItem = self.editButton;
    [super viewDidLoad]; 
} 

- (void)viewWillAppear:(BOOL)animated { 
	[sceneMgr loadScenes];
    [super viewWillAppear:animated]; 
	[[self tableView] reloadData];
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


-(IBAction)toggleEdit:(id)sender 
{ 
	bool newEditingValue = !self.tableView.editing;
    [self.tableView setEditing:newEditingValue animated:YES]; 
	if (newEditingValue)
	{
		[editButton setTitle:@"Done"];
	}
	else
	{
		[editButton setTitle:@"Edit"];
	}
}

#pragma mark - 
#pragma mark Table Data Source Methods 
- (NSInteger)tableView:(UITableView *)tableView 
					   numberOfRowsInSection:(NSInteger)section { 
    return [sceneMgr.scenes count]; 
}

- (UITableViewCell *)tableView:(UITableView *)tableView 
		 cellForRowAtIndexPath:(NSIndexPath *)indexPath { 
    static NSString * LocalScenesControllerCellIdentifier = @"LocalScenesControllerCellIdentifier"; 
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier: 
							 LocalScenesControllerCellIdentifier]; 
    if (cell == nil) { 
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault  
                                        reuseIdentifier: LocalScenesControllerCellIdentifier]; 
    } 
	NSUInteger row = [indexPath row]; 
    cell.textLabel.text = [sceneMgr getSceneDisplayName:row];
	cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    return cell; 
}

#pragma mark - 
#pragma mark Table Delegate Methods 
- (void)tableView:(UITableView*)tableView didSelectRowAtIndexPath:(NSIndexPath*)indexPath 
{ 
	NSString* activeSceneFileName = [sceneMgr getSceneFileName:[indexPath row]];
	[sceneMgr setActiveSceneFileName:activeSceneFileName];
	iCritterAppDelegate* del = [[UIApplication sharedApplication] delegate]; 
	[del switchToSceneView];
} 

- (void)tableView:(UITableView*)tableView 
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

- (void)tableView:(UITableView*)tableView 
					commitEditingStyle:(UITableViewCellEditingStyle)editingStyle 
					forRowAtIndexPath:(NSIndexPath*)indexPath { 
    NSUInteger row = [indexPath row]; 
	if ([self.sceneMgr removeScene:row])
	{
		[tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] 
						 withRowAnimation:UITableViewRowAnimationFade]; 
	}
}

@end
