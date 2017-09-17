//
//  iCritterViewController.m
//  iCritter
//
//  Created by Tom Gilbert on 17/07/2009.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "iCritterViewController.h"
#import "EAGLView.h"

@implementation iCritterViewController


//The ViewController is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id)initWithCoder:(NSCoder*)coder {
	self = [super initWithCoder:coder];
	return self;
}



// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
	[self start];
	[super viewDidLoad];
}

-(void) start {
	if ([self.view isKindOfClass:[EAGLView class]])
	{
		glView = (EAGLView*)self.view;
	}
}

- (void) viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    //[self startAnimation];
}

-(void) stopAnimation
{
	[glView stopAnimation];
}

-(void) startAnimation
{
	[glView startAnimation];
}

-(void) shutdownGL
{
	[glView stopAnimation];
	//[glView destroyFramebuffer];
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
