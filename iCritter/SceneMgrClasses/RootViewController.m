//
//  RootViewController.m
//  SceneMgr
//
//  Created by Tom Gilbert on 05/07/2009.
//  Copyright 2009 AlteredWorlds. All rights reserved.
//

#import "RootViewController.h"
#import "iCritterAppDelegate.h"
#import "SceneViewController.h"
#import "LocalScenesController.h"
#import "RemoteScenesController.h"

#define TEXT_TAG 1
#define BUTTON_TAG 2


// A class extension to declare private methods
@interface RootViewController ()
- (Boolean)     isOptionRow:(NSIndexPath*)forIndexPath;
@end


@implementation RootViewController
@synthesize controllers;

- (id)initWithStyle:(UITableViewStyle)style { 
    if ((self = [super initWithStyle:style])) { 
    } 
    return self; 
}

- (Boolean) isOptionRow:(NSIndexPath*)forIndexPath
{
    int count = [forIndexPath row] - [self.controllers count];
    return count >= 0;
}


- (void)viewDidLoad { 
    self.title = @"AlteredWorlds"; 
    NSMutableArray *array = [[NSMutableArray alloc] init]; 
	
	// Local Scenes 
    LocalScenesController* localScenesController = [[LocalScenesController alloc] 
													initWithStyle:UITableViewStylePlain]; 
    localScenesController.title = @"Local Scenes"; 
   	[array addObject:localScenesController]; 
	
	// Remote Scenes 
    RemoteScenesController* remoteScenesController = [[RemoteScenesController alloc] 
													initWithStyle:UITableViewStylePlain]; 
    remoteScenesController.title = @"Online Scenes"; 
   	[array addObject:remoteScenesController]; 
		
    self.controllers = array; 
    [super viewDidLoad]; 
} 

#pragma mark - 
#pragma mark Table Data Source Methods 
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section 
{ 
    return [self.controllers count] + 4;
} 

- (UITableViewCell*)tableView:(UITableView*)tableView cellForRowAtIndexPath:(NSIndexPath*)indexPath 
{ 
    static NSString* RootViewControllerCell= @"RootViewControllerCell";
    static NSString* OptionButtonCellIdentifier1 = @"OptionButtonCellIdentifier1";
    static NSString* OptionButtonCellIdentifier2 = @"OptionButtonCellIdentifier2";
    static NSString* OptionButtonCellIdentifier3 = @"OptionButtonCellIdentifier3";
    static NSString* OptionButtonCellIdentifier4 = @"OptionButtonCellIdentifier4";
    
    UITableViewCell* cell = nil;
    if (![self isOptionRow:indexPath])
    {
        cell = [tableView dequeueReusableCellWithIdentifier:RootViewControllerCell];
        if (cell == nil) {
            cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault
                                          reuseIdentifier: RootViewControllerCell];
        }
        cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
        // Configure the cell
        SceneViewController* controller = [controllers objectAtIndex:[indexPath row]];
        cell.textLabel.text = controller.title;
    }
    else
    {
        UISwitch* button = nil;
        UILabel* secondLabel = nil;
        NSString* useId;
        int count = [indexPath row] - [self.controllers count];
        switch (count)
        {
            case 0:
                useId = OptionButtonCellIdentifier1;
                break;
            case 1:
                useId = OptionButtonCellIdentifier2;
                break;
            case 2:
                useId = OptionButtonCellIdentifier3;
                break;
            case 3:
                useId = OptionButtonCellIdentifier4;
                break;
        }
        cell = [tableView dequeueReusableCellWithIdentifier: useId];
        if (cell == nil) {
            cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault
                                          reuseIdentifier: useId];
            
            cell.accessoryType = UITableViewCellAccessoryNone;
            
            secondLabel = [[UILabel alloc] initWithFrame:CGRectMake(0.0, 20.0, 200.0, 25.0)];
            secondLabel.tag = TEXT_TAG;
            secondLabel.autoresizingMask = UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleHeight;
            [cell.contentView addSubview:secondLabel];
            
            // Use the ivar here
            button = [[UISwitch alloc] initWithFrame: CGRectZero];
            button.tag = BUTTON_TAG;
            
            switch (count)
            {
                case 0:
                    [button addTarget: self action: @selector(flip:) forControlEvents: UIControlEventValueChanged];
                    break;
                case 1:
                    [button addTarget: self action: @selector(lighting:) forControlEvents: UIControlEventValueChanged];
                    break;
                case 2:
                    [button addTarget: self action: @selector(fog:) forControlEvents: UIControlEventValueChanged];
                    break;
                case 3:
                    [button addTarget: self action: @selector(vertexColours:) forControlEvents: UIControlEventValueChanged];
                    break;
            }
            // Set the desired frame location of onoff here
            [cell.contentView addSubview: button];
        }
        else {
            secondLabel = (UILabel *)[cell.contentView viewWithTag:TEXT_TAG];
            button = (UISwitch *)[cell.contentView viewWithTag:BUTTON_TAG];
        }
        
        switch (count)
        {
            case 0:
                secondLabel.text = @"OpenGL ES 1.1";
                button.on = sceneMgr.isOpenGL1dot1;
                break;
            case 1:
                secondLabel.text = @"Lighting";
                button.on = sceneMgr.useLighting;
                break;
            case 2:
                secondLabel.text = @"Fog";
                button.on = sceneMgr.useFog;
                break;
            case 3:
                secondLabel.text = @"VertexColours";
                button.on = sceneMgr.useVertexColours;
                break;
        }
    }
    return cell;
}

- (IBAction)flip:(UISwitch*)sender {
    if (sender.on)
        sceneMgr.isOpenGL1dot1 = true;
    else
        sceneMgr.isOpenGL1dot1 = false;
}

- (IBAction)lighting:(UISwitch*)sender {
    if (sender.on)
        sceneMgr.useLighting = true;
    else
        sceneMgr.useLighting = false;
}

- (IBAction)fog:(UISwitch*)sender {
    if (sender.on)
        sceneMgr.useFog = true;
    else
        sceneMgr.useFog = false;
}

- (IBAction)vertexColours:(UISwitch*)sender {
    if (sender.on)
        sceneMgr.useVertexColours = true;
    else
        sceneMgr.useVertexColours = false;
}

#pragma mark -
#pragma mark Table View Delegate Methods
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath*)indexPath
{
    if (![self isOptionRow:indexPath])
    {
        NSUInteger row = [indexPath row];
        SceneViewController* nextController = [self.controllers objectAtIndex:row];
        iCritterAppDelegate* delegate = [[UIApplication sharedApplication] delegate];
        [delegate.navController pushViewController:nextController animated:YES];
    }
} 

// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return YES;
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


@end
