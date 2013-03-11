//
//  Triangle.h
//  RayTracer
//
//  Created by Adriana Diakite on 12/1/12.
//  Copyright (c) 2012 Adriana Diakite. All rights reserved.
//

#ifndef __RayTracer__Triangle__
#define __RayTracer__Triangle__

#include <iostream>
#include "Shape.h"
#include "stgl.h"
#include "Ray.h"
#include "RayIntersection.h"

class Triangle: public Shape
{
public:
	Triangle(const STPoint3& one, const STPoint3& two, const STPoint3& three);
    
    RayIntersection *IntersectsRay(Ray r, STTransform4 transMatrix);
    STVector3 CalcNormal(STVector3 surface_pt, Ray r);

    
    
	/** CS 148 TODO: Add methods for the scene to render itself, etc. **/
    
    // gen viewing rays
private:
    STVector3 v1, v2, v3;
    

    
    
    
};

#endif /* defined(__RayTracer__Triangle__) */
