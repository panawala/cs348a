//
//  Triangle.cpp
//  RayTracer
//
//  Created by Adriana Diakite on 12/1/12.
//  Copyright (c) 2012 Adriana Diakite. All rights reserved.
//

#include "Triangle.h"

Triangle::Triangle(const STPoint3& one, const STPoint3& two, const STPoint3& three){
    v1 = STVector3(one.x, one.y, one.z);
    v2 = STVector3(two.x, two.y, two.z);
    v3 = STVector3(three.x, three.y, three.z);
    
}

STVector3 Triangle::CalcNormal(STVector3 surface_pt, Ray r){
    STVector3 leg1 = v2 -v1;
    STVector3 leg2 = v3 -v1;
    STVector3 normal = STVector3::Cross(leg1, leg2);
    normal.Normalize();
    if (STVector3::Dot(r.direction, normal) > 0) return -normal;
    return normal;
}




float determinant(float a, float b, float c, float d, float e, float f, float g, float h, float i) {
    return a*e*i + b*f*g + c*d*h - c*e*g - b*d*i - a*f*h;
}

RayIntersection * Triangle::IntersectsRay(Ray r, STTransform4 transMatrix) {
//    printf("TODO: implement Sphere::IntersectsRay\n");
    float a,b,c,d,e,f,g,h,i;
    //find determinant of a
    a = v1.x - v2.x;
    b = v1.x - v3.x;
    c = r.direction.x;
    d = v1.y - v2.y;
    e = v1.y - v3.y;
    f = r.direction.y;
    g = v1.z - v2.z;
    h = v1.z - v3.z;
    i = r.direction.z;
    float detA = determinant(a, b, c, d, e, f, g, h, i);
    
    //Find T
    a = v1.x - v2.x;
    b = v1.x - v3.x;
    c = v1.x - r.start.x;
    d = v1.y - v2.y;
    e = v1.y - v3.y;
    f = v1.y - r.start.y;
    g = v1.z - v2.z;
    h = v1.z - v3.z;
    i = v1.z - r.start.z;
    float detTNum = determinant(a, b, c, d, e, f, g, h, i);
    float t = detTNum/ detA;
    
    if (r.invalidT(t)) {
        return NULL;
    }
    
    // Find gamma
    a = v1.x - v2.x;
    b = v1.x - r.start.x;
    c = r.direction.x;
    d = v1.y - v2.y;
    e = v1.y - r.start.y;
    f = r.direction.y;
    g = v1.z - v2.z;
    h = v1.z - r.start.z;
    i = r.direction.z;
    float detGammaNum = determinant(a, b, c, d, e, f, g, h, i);
    float Gamma = detGammaNum/detA;
    
    if (Gamma < 0 || Gamma > 1) {
        return NULL;
    }
    
    // Find beta
    a = v1.x - r.start.x;
    b = v1.x -v3.x;
    c = r.direction.x;
    d = v1.y - r.start.y;
    e = v1.y -v3.y;
    f = r.direction.y;
    g = v1.z - r.start.z;
    h = v1.z -v3.z;
    i = r.direction.z;
    float detBetaNum = determinant(a, b, c, d, e, f, g, h, i);
    float Beta = detBetaNum/detA;
    
    if (Beta < 0 || Beta > 1 - Gamma) {
        return NULL;
    }
    STVector3 normal = CalcNormal(*(r.InterpolatedRay(t)), r);
    RayIntersection * rt = new RayIntersection(t, *(r.InterpolatedRay(t)), normal);
    
    return rt;
}














