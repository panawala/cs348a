#ifndef __RayTracer__Ray__
#define __RayTracer__Ray__

#include <iostream>

#include "st.h"
#include "stgl.h"

class Ray
{
public:
    Ray();
	Ray(STVector3 start, STVector3 end);
    
    STVector3 *InterpolatedRay(float t);
    Ray TransformRay(STTransform4 transMatrix);
    STVector3 direction;
    STVector3 start;

    bool invalidT(float t);

    
    
	/** CS 148 TODO: Add methods for the scene to render itself, etc. **/
    
    // gen viewing rays    
protected:
    
    
    
    float minT;
    float maxT;
    
    
};


#endif /* defined(__RayTracer__Ray__) */
