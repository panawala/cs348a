//
//  punch.h
//  Final Project
//
//  Created by Nikolaus West on 2013-03-10.
//  Copyright (c) 2013 Nikolaus West. All rights reserved.
//

#ifndef __Final_Project__punch__
#define __Final_Project__punch__

#include <iostream>
#include "mesh_definitions.h"

using namespace OpenMesh;
using namespace std;

void deformMesh(Mesh &mesh, Vec3f clickedPoint, Vec3f camPos);

#endif /* defined(__Final_Project__punch__) */
