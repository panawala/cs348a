#include "mesh_features.h"
using namespace OpenMesh;

bool isSilhouette(Mesh &mesh, const Mesh::EdgeHandle &e, Vec3f cameraPos)  {
	// CHECK IF e IS A SILHOUETTE HERE -----------------------------------------------------------------------------
    Mesh::HalfedgeHandle h0 = mesh.halfedge_handle(e,0);
    Mesh::HalfedgeHandle h1 = mesh.halfedge_handle(e,1);
    //Extract viewing ray
    Vec3f vertexPoint(mesh.point(mesh.from_vertex_handle(h0)));
    Vec3f viewingRay = vertexPoint - cameraPos;
    
    //Calculate dot products
    Vec3f normal0(mesh.normal(mesh.face_handle(h0)));
    Vec3f normal1(mesh.normal(mesh.face_handle(h1)));
    
    float dot0 = normal0[0]*viewingRay[0] + normal0[1]*viewingRay[1] + normal0[2]*viewingRay[2];
    float dot1 = normal1[0]*viewingRay[0] + normal1[1]*viewingRay[1] + normal1[2]*viewingRay[2];
    
    return dot0*dot1 <= 0;
    
	// -------------------------------------------------------------------------------------------------------------
    }

bool isSharpEdge(Mesh &mesh, const Mesh::EdgeHandle &e) {
	// CHECK IF e IS SHARP HERE ------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------------------------
    Mesh::HalfedgeHandle h0 = mesh.halfedge_handle(e,0);
    Mesh::HalfedgeHandle h1 = mesh.halfedge_handle(e,1);
    
    Vec3f normal0(mesh.normal(mesh.face_handle(h0)));
    Vec3f normal1(mesh.normal(mesh.face_handle(h1)));
    
    float dot = normal0[0]*normal1[0] + normal0[1]*normal1[1] + normal0[2]*normal1[2];
    
    return dot <= .5;
    
}

bool isFeatureEdge(Mesh &mesh, const Mesh::EdgeHandle &e, Vec3f cameraPos) {
	return mesh.is_boundary(e) || isSilhouette(mesh,e, cameraPos) || isSharpEdge(mesh,e);
}

