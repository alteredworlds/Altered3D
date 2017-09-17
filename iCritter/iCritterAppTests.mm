//
//  iCritterAppTests.mm
//  iCritter
//
//  Created by Tom Gilbert on 03/08/2009.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "iCritterAppTests.h" 

@implementation iCritterAppTests 
- (void) testFail { 
	//STFail(@"Must fail to succeed."); 
	
	id app_delegate = [[UIApplication sharedApplication] delegate]; 
	STAssertNotNil(app_delegate, @"Cannot find the application delegate.");
} 
@end 