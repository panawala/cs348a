#ifndef __Final_Project__utils__
#define __Final_Project__utils__

#include <iostream>
#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <iostream>
using namespace OpenMesh;
using namespace Eigen;
using namespace std;

Vec3f vector3dToVec3f(Vector3d vec);
Vector3d vec3fToVector3d(Vec3f vec);
float vec3fDotProduct(Vec3f vec1, Vec3f vec2);

#endif /* defined(__Final_Project__utils__) */
