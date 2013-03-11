//
//  punch.cpp
//  Final Project
//
//  Created by Nikolaus West on 2013-03-10.
//  Copyright (c) 2013 Nikolaus West. All rights reserved.
//

#include "punch.h"
#include <cmath>

float naiveDisplacement(Vec3f clickedPoint, Vec3f vertex)
{
    float deformationConstant = 0.01;
    float distanceToImpactCenter = (clickedPoint - vertex).length();
    return deformationConstant / distanceToImpactCenter;
}

float gaussianDisplacement(Vec3f clickedPoint, Vec3f vertex)
{
    float deformationConstant = 0.1;
    float sigma = 0.2;
    float displacementFactor = deformationConstant*exp(-pow((clickedPoint - vertex).length(),2) / pow(sigma,3));
    cout << displacementFactor << endl;
    return displacementFactor;
}

void deformMesh(Mesh &mesh, Vec3f clickedPoint, Vec3f camPos)
{
    //find closest vertex
//    VertexHandle closestVertexHandle;
//    float minDistance = (float)INT32_MAX;
//    
//    for (Mesh::ConstVertexIter it = mesh.vertices_begin(); it != mesh.vertices_end(); ++it) {
//        Vec3f vertex = mesh.point(it.handle());
//        float length = (clickedPoint - vertex).length();
//        
//        if (length < minDistance) {
//            closestVertexHandle = it;
//            minDistance = length;
//        }
//    }
    
    
    //perform the deformation
    Vec3f deformationDirection = (clickedPoint - camPos).normalize();
//    Vec3f impactCenter = mesh.point(closestVertexHandle);
    for (Mesh::VertexIter it = mesh.vertices_begin(); it != mesh.vertices_end(); ++it) {
        Vec3f vertex = mesh.point(it.handle());
        Vec3f displacement = gaussianDisplacement(clickedPoint, vertex) * deformationDirection;
        mesh.point(it.handle()) = vertex + displacement;
    }
    cout << "deformed" << endl;
    
    mesh.request_face_normals();
	mesh.request_vertex_normals();
}