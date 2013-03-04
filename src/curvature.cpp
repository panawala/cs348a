#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <iostream>
#include "curvature.h"
using namespace OpenMesh;
using namespace Eigen;
using namespace std;

void computeCurvature(Mesh &mesh, OpenMesh::VPropHandleT<CurvatureInfo> &curvature) {
	
    for (Mesh::VertexIter it = mesh.vertices_begin(); it != mesh.vertices_end(); ++it) {
		// WRITE CODE HERE TO COMPUTE THE CURVATURE AT THE CURRENT VERTEX ----------------------------------------------
        //Initialize for computation on neighbouring vertices
		Vec3f normal = mesh.normal(it.handle());
		Vector3d currentNormal(normal[0],normal[1],normal[2]);
		Vec3f vertex = mesh.point(it.handle());
		Vector3d currentVertex(vertex[0],vertex[1],vertex[2]);
		Matrix3d projectionMatrix = Matrix3d::Identity() - currentNormal*currentNormal.transpose();
		Matrix3d Mvi = Matrix3d::Zero();
		double sumWeights = 0;
        
		//Iterate over neighbouring vertices
		for(Mesh::VertexVertexIter vvit = mesh.vv_iter(it); vvit; ++vvit){
			Vec3f neighbour = mesh.point(vvit.handle());
			Vector3d neighbourVertex(neighbour[0],neighbour[1],neighbour[2]);
			Vector3d Tij = projectionMatrix*(currentVertex-neighbourVertex);
			Tij = Tij.normalized();
			
			double kij = 2*currentNormal.dot((neighbourVertex-currentVertex))/((neighbourVertex-currentVertex).squaredNorm());
			
			//Getting the outgoing halfedge from the vertex to its neighbour
			Mesh::HalfedgeHandle commonHalfedgeHandle;
			for (Mesh::VertexOHalfedgeIter voh_it = mesh.voh_iter(it); voh_it; ++voh_it) {
                
				if(mesh.to_vertex_handle(voh_it.handle()) == vvit.handle()) {
					commonHalfedgeHandle = voh_it.handle();
					break;
				}
			}
            
			//Calculating the weights
			double wij = mesh.calc_sector_area(commonHalfedgeHandle);
			wij += mesh.calc_sector_area(mesh.opposite_halfedge_handle(commonHalfedgeHandle));
			sumWeights +=wij;
            
			//Updating the Mvi matrix
			Mvi += wij*kij*Tij*Tij.transpose();
		}
        
		//Normalizing the weights
		Mvi = Mvi/sumWeights;
		
		//Computing the eigenvectors of Mvi
		EigenSolver<Matrix3d> solver(Mvi);
		
		Vector3d firstEigenVector = solver.pseudoEigenvectors().block(0,0,3,1);
		Vector3d secondEigenVector = solver.pseudoEigenvectors().block(0,1,3,1);
		Vector3d thirdEigenVector = solver.pseudoEigenvectors().block(0,2,3,1);
		
		//Removing the one that is parallel to N
		Vector3d principalDirection1(0,0,0);
		Vector3d principalDirection2(0,0,0);
        
		double norm1 = firstEigenVector.cross(currentNormal).norm();
		double norm2 = secondEigenVector.cross(currentNormal).norm();
		double norm3 = thirdEigenVector.cross(currentNormal).norm();
        
		if (norm1 < norm2 && norm1 < norm3) {
			principalDirection1 = secondEigenVector;
			principalDirection2 = thirdEigenVector;
		} else if (norm2 < norm1 && norm2 < norm3) {
			principalDirection1 = firstEigenVector;
			principalDirection2 = thirdEigenVector;
		} else if (norm3 < norm2 && norm3 < norm1) {
			principalDirection1 = firstEigenVector;
			principalDirection2 = secondEigenVector;
		}
        
		//Calculating the curvatures
		double mp11 = principalDirection1.transpose()*Mvi*principalDirection1;
		double mp22 = principalDirection2.transpose()*Mvi*principalDirection2;
		double k1 = 3*mp11 - mp22;
		double k2 = 3*mp22 - mp11;
        
		// In the end you need to fill in this struct
		CurvatureInfo info;
		info.curvatures[0] = k1;
		info.curvatures[1] = k2;
		info.directions[0] = Vec3f(principalDirection1[0],principalDirection1[1],principalDirection1[2]);
		info.directions[1] = Vec3f(principalDirection2[0],principalDirection2[1],principalDirection2[2]);
        
		mesh.property(curvature,it) = info;
		// -------------------------------------------------------------------------------------------------------------
	}
}

void computeViewCurvature(Mesh &mesh, OpenMesh::Vec3f camPos, OpenMesh::VPropHandleT<CurvatureInfo> &curvature, OpenMesh::VPropHandleT<double> &viewCurvature, OpenMesh::FPropHandleT<OpenMesh::Vec3f> &viewCurvatureDerivative) {
	// WRITE CODE HERE TO COMPUTE CURVATURE IN THE VIEW PROJECTION PROJECTED ON THE TANGENT PLANE ------------------
	// Compute vector to viewer and project onto tangent plane, then use components in principal directions to find curvature
	// -------------------------------------------------------------------------------------------------------------

	// We'll use the finite elements piecewise hat method to find per-face gradients of the view curvature
	// CS 348a doesn't cover how to differentiate functions on a mesh (Take CS 468! Spring 2013!) so we provide code here
	
	for (Mesh::FaceIter it = mesh.faces_begin(); it != mesh.faces_end(); ++it) {
		double c[3];
		Vec3f p[3];
		
		Mesh::ConstFaceVertexIter fvIt = mesh.cfv_iter(it);
		for (int i = 0; i < 3; i++) {
			p[i] = mesh.point(fvIt.handle());
			c[i] = mesh.property(viewCurvature,fvIt.handle());
			++fvIt;
		}
		
		Vec3f N = mesh.normal(it.handle());
		double area = mesh.calc_sector_area(mesh.halfedge_handle(it.handle()));

		mesh.property(viewCurvatureDerivative,it) = (N%(p[0]-p[2]))*(c[1]-c[0])/(2*area) + (N%(p[1]-p[0]))*(c[2]-c[0])/(2*area);
	}
}
