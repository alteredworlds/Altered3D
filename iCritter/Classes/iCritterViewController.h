//
//  iCritterViewController.h
//  iCritter
//
//  Created by Tom Gilbert on 17/07/2009.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "EAGLView.h"


@interface iCritterViewController : UIViewController {
	EAGLView* glView;
}

-(void) start;
-(void) stopAnimation;
-(void) startAnimation;
-(void) shutdownGL;

@end
