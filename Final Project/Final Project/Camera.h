//
//  Camera.h
//  RayTracer
//
//  Created by Adriana Diakite on 12/1/12.
//  Copyright (c) 2012 Adriana Diakite. All rights reserved.
//

#ifndef __RayTracer__Camera__
#define __RayTracer__Camera__

#include <iostream>
#include "st.h"
#include "Ray.h"
#include "stgl.h"



class Camera
{
public:
	Camera(const STPoint3& eye, const STVector3& up, const STPoint3& lookAt, float fovy, float aspect);
    
    STVector3 *eye;    // where the camera is located in the scene
    STVector3 *lookAt;    // the location that the camera is pointing to
    STVector3 *up;           // up vector
    float fovy;
    float aspect;
    
    STVector3 *w;
    STVector3 *u;
    STVector3 *v;
    
    
    
	/** CS 148 TODO: Add methods for the scene to render itself, etc. **/
    
    // gen viewing rays
    Ray *GetViewingRay(STVector3 world_pt);
//    STVector3 ConvertToWorld(STVector2 pt);
    
private:
    

};




#endif /* defined(__RayTracer__Camera__) */
