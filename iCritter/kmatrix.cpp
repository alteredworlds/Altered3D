/*
 *  kmatrix.cpp
 *  iCritter
 *
 *  Created by Tom Gilbert on 02/09/2009.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "kmatrix.hpp" 

using namespace std;


template<class M1, class M2, class M3>
void matrixMultiply(const M1& m1, const M2& m2, M3& m3)
{
	assert(m1.cols( ) == m2.rows( ));
	assert(m1.rows( ) == m3.rows( ));
	assert(m2.cols( ) == m3.cols( ));
	for (int i=m1.rows( )-1; i >= 0; --i) {
		for (int j=m2.cols( )-1; j >= 0; --j) {
			for (int k = m1.cols( )-1; k >= 0; --k) {
				m3[i][j] += m1[i][k] * m2[k][j];
			}
		}
	}
}
