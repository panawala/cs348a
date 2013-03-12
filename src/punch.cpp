#include "punch.h"
#include <Eigen/Core>
#include <cmath>

using namespace Eigen;

static float sigma = 0.15;

float naiveDisplacement(Vec3f clickedPoint, Vec3f vertex)
{
    float deformationConstant = 0.01;
    float distanceToImpactCenter = (clickedPoint - vertex).length();
    return deformationConstant / distanceToImpactCenter;
}

float gaussianDisplacement(Vec3f impactCenter, Vec3f vertex)
{
    float deformationConstant = 10;
    float displacementFactor = deformationConstant * exp(-pow((impactCenter - vertex).length(),2) / pow(sigma,3));
    return displacementFactor;
}

float gaussianDisplacementWithCosModulation(Vec3f impactCenter, Vec3f clickedPoint, Vec3f vertex)
{
    float deformationConstant = 10;
    float distanceToImpactCenter = (impactCenter - vertex).length();
    float distanceToClickedPoint = (clickedPoint - vertex).length();
    float displacementFactor = deformationConstant * exp(-pow(distanceToImpactCenter,2) / pow(sigma,3)) * cos(distanceToClickedPoint*M_PI/sigma);
    return displacementFactor;
}

float cosineModulation(Vec3f clickedPoint, Vec3f vertex)
{
    float distanceToClickedPoint = (clickedPoint - vertex).length();
    return cos(distanceToClickedPoint*M_PI/sigma);
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
    //Vec3f deformationDirection = (clickedPoint - camPos).normalize();
    
    float viewRayBackupConstant = 0.1;
    Vec3f impactCenter = clickedPoint - viewRayBackupConstant * (clickedPoint - camPos).normalize();
    int numIterations = 100;
    float iterationStepLength = ((float)1) / numIterations;
    float normalWeight = 0.9;
    float tangentWeight = powf(1- powf(normalWeight,2),0.5);
    
    for (int i = 0; i < numIterations; ++i) {
        for (Mesh::VertexIter it = mesh.vertices_begin(); it != mesh.vertices_end(); ++it) {
            Vec3f vertex = mesh.point(it.handle());
            Vec3f impactToVertexVector = impactCenter - vertex;
            float impactToVertexDistance = (impactToVertexVector).length();
            
            if (impactToVertexDistance < 1) {
                Vec3f normalDirection = mesh.normal(it.handle());
                Vector3d normal = Vector3d(normalDirection[0], normalDirection[1], normalDirection[2]);
                Matrix3d projection = normalWeight*normal*normal.transpose() + tangentWeight*(Matrix3d::Identity() - normal*normal.transpose());
                
                Vector3d eigenImpactToVertexVector = Vector3d(impactToVertexVector[0],impactToVertexVector[1],impactToVertexVector[2]);
                Vector3d eigenDeformationDirection = projection*eigenImpactToVertexVector;
                Vec3f deformationDirection = Vec3f(eigenDeformationDirection[0], eigenDeformationDirection[1],eigenDeformationDirection[2]);
                
                Vec3f displacement = -iterationStepLength * gaussianDisplacement(impactCenter, vertex) * deformationDirection;
                mesh.point(it.handle()) = vertex + displacement;
                
            }
        }
        mesh.request_vertex_normals();
    }
    
    mesh.request_face_normals();
}