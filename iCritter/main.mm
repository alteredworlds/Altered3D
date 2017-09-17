//
//  main.mm
//  iCritter
//
//  Created by Tom Gilbert on 17/07/2009.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import <UIKit/UIKit.h>
#if TARGET_IPHONE_SIMULATOR 
#import "AWTooldefs.h"
AWTRACE_INIT("/Users/twcgilbert/Development/Code/iCritter.log") 
#endif

int main(int argc, char *argv[]) {
    
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    int retVal = UIApplicationMain(argc, argv, nil, nil);
    [pool release];
    return retVal;
}
