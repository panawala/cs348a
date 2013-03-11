//
//  RayIntersection.h
//  RayTracer
//
//  Created by Adriana Diakite on 12/3/12.
//  Copyright (c) 2012 Adriana Diakite. All rights reserved.
//

#ifndef __RayTracer__RayIntersection__
#define __RayTracer__RayIntersection__

#include <iostream>
#include "stgl.h"
#include "st.h"
//#include "Shape.h"


class RayIntersection
{
public:
	RayIntersection(float time, STVector3 intersect_pt, STVector3 normal);
    
    float t;
    STVector3 pt;
    STVector3 ptNormal;
    // think want to store viewing ray too
protected:
    
    
};

#endif /* defined(__RayTracer__RayIntersection__) */
