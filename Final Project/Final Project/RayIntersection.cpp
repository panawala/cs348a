//
//  RayIntersection.cpp
//  RayTracer
//
//  Created by Adriana Diakite on 12/3/12.
//  Copyright (c) 2012 Adriana Diakite. All rights reserved.
//

#include "RayIntersection.h"

RayIntersection::RayIntersection(float time, STVector3 intersect_pt, STVector3 normal){
    t = time;
    pt = STVector3(intersect_pt);
    ptNormal = normal; //s.CalcNormal(intersect_pt);
    
}