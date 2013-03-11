//
//  Camera.cpp
//  RayTracer
//
//  Created by Adriana Diakite on 12/1/12.
//  Copyright (c) 2012 Adriana Diakite. All rights reserved.
//

#include "Camera.h"

Camera::Camera(const STPoint3& e, const STVector3& u, const STPoint3& lA, float f, float a)
{
    eye = new STVector3(e);
    up = new STVector3(u);
    lookAt= new STVector3(lA);
    fovy = f*2*M_PI/360;
    aspect = a;
    w = new STVector3(*lookAt - *eye);
    (*w).Normalize();

}



Ray *Camera::GetViewingRay(STVector3 world_img_plane_pt){
    return new Ray(*eye, world_img_plane_pt);
}


