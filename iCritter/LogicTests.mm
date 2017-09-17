//
//  LogicTests.mm
//  iCritter
//
//  Created by Tom Gilbert on 27/07/2009.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LogicTests.h"
#include "AWToolsCommon.h"
#include "AWAnimatedQuat.h"
#include "AWAnimatedPoint.h"
#include "AWMatrix4.h"
#include "kmatrix.hpp" 

using namespace std;

@implementation LogicTests

-(void) setUp 
{ 
	//no setup yet required... 
	//STAssertNotNil(test_subject, @"Could not create test subject."); 
}

- (void) tearDown 
{ 
	// no tear down yet required...
	//[test_subject release]; 
}

void initMatrix(AWMatrix4& m)
{
	int k = 0;
	for (int i=0; i<4; i++)
	{
		for (int j=0; j<4; j++)
		{
			m(i,j) = (float)k++;
		}
	}
}

- (void) testFail 
{  
	/*AWMatrix4			m_wsTM;
	AWAnimatedPoint		m_posn;
	AWAnimatedPoint		m_scale;
	AWAnimatedQuat		m_rotn;
	//
	//
	m_wsTM.awTranslate(m_posn.getVal());
	m_wsTM.awRotate(m_rotn.getQuat());
	m_wsTM.awScale(m_scale.getVal());*/	
	AWMatrix4 a, b, c;
	
	initMatrix(a);
	initMatrix(b);
	
	for (int i=0; i<10000000; i++)
	{
		c = a * b;
	}
	
    STAssertTrue(TRUE, @"" );
}



void initMatrix(kmatrix<float, 4, 4>& m)
{
	int k = 0;
	for (int i=0; i<m.rows(); i++)
	{
		for (int j=0; j<m.cols(); j++)
		{
			m[i][j] = (float)k++;
		}
	}
}


-(void) testAlternateMatrix
{	
	kmatrix<float, 4, 4> a;
	kmatrix<float, 4, 4> b;
	kmatrix<float, 4, 4> c;
	
	initMatrix(a);
	initMatrix(b);
	for (int i=0; i<10000000; i++)
	{
		mult(a, b, c);
	}
	
    STAssertTrue(TRUE, @"" );	
}


@end
