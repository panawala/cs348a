#include <OpenMesh/Core/IO/Options.hh>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <iostream>
#include <cmath>
#include <cstdio>
#include <GLUT/glut.h>
#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include "curvature.h"
#include "mesh_features.h"
#include "image_generation.h"
#include "decimate.h"
#include "punch.h"
using namespace std;
using namespace OpenMesh;
using namespace Eigen;

VPropHandleT<double> viewCurvature;
FPropHandleT<Vec3f> viewCurvatureDerivative;
VPropHandleT<CurvatureInfo> curvature;
Mesh mesh;

bool leftDown = false, rightDown = false, middleDown = false;
int lastPos[2];
float cameraPos[4] = {0,0,4,1};
Vec3f up, pan;
int windowWidth = 640, windowHeight = 480;
bool showSurface = true, showAxes = true, showCurvature = false, showNormals = false, showEdges = false;

float specular[] = { 1.0, 1.0, 1.0, 1.0 };
float shininess[] = { 50.0 };

float derivativeThreshold = 2000;
float angleCosineThreshold = 0.3;

void renderSuggestiveContours(Vec3f camPos) { // use this camera position to account for panning etc.
	glColor3f(.5,.5,.5);
	
	// RENDER SUGGESTIVE CONTOURS HERE -----------------------------------------------------------------------------
    
    for (Mesh::ConstFaceIter it= mesh.faces_begin(); it != mesh.faces_end(); ++it){
		
        Vec3f derivative = mesh.property(viewCurvatureDerivative, it);
        Vector3d derivativeVector(derivative[0],derivative[1],derivative[2]);
        
        //getting the three vertices of the face and their curvature
        Mesh::FaceVertexIter fvit = mesh.fv_iter(it);
        float kw1 = mesh.property(viewCurvature, fvit);
        Vec3f vertex1 = mesh.point(fvit.handle());
        float kw2 = mesh.property(viewCurvature, ++fvit);
        Vec3f vertex2 = mesh.point(fvit.handle());
        float kw3 = mesh.property(viewCurvature, ++fvit);
        Vec3f vertex3 = mesh.point(fvit.handle());
        
        Vector3d viewRay(vertex1[0]-camPos[0],vertex1[1]-camPos[1],vertex1[2]-camPos[2]);
        viewRay = viewRay.normalized();
        
        //check if derivative is not too small
        if (derivativeVector.dot(viewRay) > derivativeThreshold) {
            Vec3f normal = mesh.normal(it.handle());
            Vector3d surfaceNormal(normal[0],normal[1],normal[2]);
            
            //check if angle between surface normal and view vector not too small
            float cosine = surfaceNormal.dot(viewRay);
            if (abs(cosine) < 1 - angleCosineThreshold) {
                
                //check if the curvatures have different signs
                std::vector<Vec3f> contourEdges;
                if (kw1*kw2 < 0) {
                    Vec3f pointWithZeroCurvature = (vertex1 * kw2 - vertex2 * kw1) / (kw2 - kw1);
                    contourEdges.push_back(pointWithZeroCurvature);
                }
                
                if (kw1*kw3 < 0) {
                    Vec3f pointWithZeroCurvature = (vertex1 * kw3 - vertex3 * kw1) / (kw3 - kw1);
                    contourEdges.push_back(pointWithZeroCurvature);
                }
                
                if (kw2*kw3 < 0) {
                    Vec3f pointWithZeroCurvature = (vertex2 * kw3 - vertex3 * kw2) / (kw3 - kw2);
                    contourEdges.push_back(pointWithZeroCurvature);
                }
                
                //render edges
                if (contourEdges.size() == 2) {
                    glBegin(GL_LINES);
                    glVertex3f(contourEdges[0][0],contourEdges[0][1],contourEdges[0][2]);
                    glVertex3f(contourEdges[1][0],contourEdges[1][1],contourEdges[1][2]);
                    glEnd();
                }
                
            }
        }
		
    }
    
	// -------------------------------------------------------------------------------------------------------------
}

void renderMesh() {
	if (!showSurface) glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE); // render regardless to remove hidden lines
	
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_POSITION, cameraPos);

	glDepthRange(0.001,1);
	glEnable(GL_NORMALIZE);
	
	// WRITE CODE HERE TO RENDER THE TRIANGLES OF THE MESH ---------------------------------------------------------
	
    //Iterate over the faces of the mesh and then draw each triangle seperately by iterating over each face's verices
	for (Mesh::ConstFaceIter it= mesh.faces_begin(); it != mesh.faces_end(); ++it){
		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(0.4,0.5,0.6);
		for(Mesh::FaceVertexIter fvit = mesh.fv_iter(it); fvit; ++fvit) {
			Vec3f v = mesh.point(fvit.handle());
			Vec3f normal = mesh.normal(fvit.handle());
			glNormal3f(normal[0],normal[1],normal[2]);
			glVertex3f(v[0],v[1],v[2]);
		}
		glEnd();
	}
    // -------------------------------------------------------------------------------------------------------------
	
	if (!showSurface) glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	
	glDisable(GL_LIGHTING);
	glDepthRange(0,0.999);
	
	Vec3f actualCamPos(cameraPos[0]+pan[0],cameraPos[1]+pan[1],cameraPos[2]+pan[2]);
	renderSuggestiveContours(actualCamPos);
	
	// We'll be nice and provide you with code to render feature edges below
	glBegin(GL_LINES);
	glColor3f(0,0,0);
	glLineWidth(2.0f);
	for (Mesh::ConstEdgeIter it = mesh.edges_begin(); it != mesh.edges_end(); ++it)
		if (isFeatureEdge(mesh,*it,actualCamPos)) {
			Mesh::HalfedgeHandle h0 = mesh.halfedge_handle(it,0);
			Mesh::HalfedgeHandle h1 = mesh.halfedge_handle(it,1);
			Vec3f source(mesh.point(mesh.from_vertex_handle(h0)));
			Vec3f target(mesh.point(mesh.from_vertex_handle(h1)));
			glVertex3f(source[0],source[1],source[2]);
			glVertex3f(target[0],target[1],target[2]);
		}
	glEnd();
    
    if(showEdges) {
        glBegin(GL_LINES);
        glColor3f(0,0,0);
        glLineWidth(2.0f);
        for (Mesh::ConstEdgeIter it = mesh.edges_begin(); it != mesh.edges_end(); ++it) {
            Mesh::HalfedgeHandle h0 = mesh.halfedge_handle(it,0);
            Mesh::HalfedgeHandle h1 = mesh.halfedge_handle(it,1);
            Vec3f source(mesh.point(mesh.from_vertex_handle(h0)));
            Vec3f target(mesh.point(mesh.from_vertex_handle(h1)));
            glVertex3f(source[0],source[1],source[2]);
            glVertex3f(target[0],target[1],target[2]);
        }
        glEnd();
	}
        
	if (showCurvature) {
		// WRITE CODE HERE TO RENDER THE PRINCIPAL DIRECTIONS YOU COMPUTED ---------------------------------------------
        glBegin(GL_LINES);
        
		for (Mesh::ConstVertexIter it = mesh.vertices_begin(); it != mesh.vertices_end(); ++it) {
			Vec3f principalDirection1 = mesh.property(curvature,it).directions[0];
			Vec3f p = mesh.point(it.handle());
			Vec3f p1 = p - principalDirection1*.01;
			Vec3f d1 = p + principalDirection1*.01;
			glColor3f(0.0,0.0,1.0);
			glVertex3f(d1[0],d1[1],d1[2]);
			glVertex3f(p1[0],p1[1],p1[2]);
            
			Vec3f principalDirection2 = mesh.property(curvature,it).directions[1];
			Vec3f p2 = p - principalDirection2*.01;
			Vec3f d2 = p + principalDirection2*.01;
			glColor3f(1.0,0.0,0.0);
			glVertex3f(d2[0],d2[1],d2[2]);
			glVertex3f(p2[0],p2[1],p2[2]);
			
		}
		
		glEnd();
		// -------------------------------------------------------------------------------------------------------------
	}
	
	if (showNormals) {
		glBegin(GL_LINES);
		glColor3f(0,1,0);
		for (Mesh::ConstVertexIter it = mesh.vertices_begin(); it != mesh.vertices_end(); ++it) {
			Vec3f n = mesh.normal(it.handle());
			Vec3f p = mesh.point(it.handle());
			Vec3f d = p + n*.01;
			glVertex3f(p[0],p[1],p[2]);
			glVertex3f(d[0],d[1],d[2]);
		}
		glEnd();
	}
	
	glDepthRange(0,1);
}

Vec3f clickPoint(int xClick, int yClick){
    
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    GLdouble modelview[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    
    GLdouble projection[16];
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    
    GLfloat winX, winY, winZ;
    winX = (float)xClick;
    winY = (float)viewport[3] - (float)yClick;
    
    glReadPixels(winX,winY,1,1,GL_DEPTH_COMPONENT,GL_FLOAT, &winZ);
    
    GLdouble posX, posY, posZ;
    
    gluUnProject(winX,winY,winZ,modelview, projection, viewport, &posX, &posY, &posZ);
    
    return Vec3f(posX,posY,posZ); //Return the clicked point in the model space
    
}

void display() {
	glClearColor(1,1,1,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
	glEnable(GL_LIGHT0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0,0,windowWidth,windowHeight);
	
	float ratio = (float)windowWidth / (float)windowHeight;
	gluPerspective(50, ratio, 1, 1000); // 50 degree vertical viewing angle, zNear = 1, zFar = 1000
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(cameraPos[0]+pan[0], cameraPos[1]+pan[1], cameraPos[2]+pan[2], pan[0], pan[1], pan[2], up[0], up[1], up[2]);
	
	// Draw mesh
	renderMesh();

	// Draw axes
	if (showAxes) {
		glDisable(GL_LIGHTING);
		glBegin(GL_LINES);
		glLineWidth(1);
			glColor3f(1,0,0); glVertex3f(0,0,0); glVertex3f(1,0,0); // x axis
			glColor3f(0,1,0); glVertex3f(0,0,0); glVertex3f(0,1,0); // y axis
			glColor3f(0,0,1); glVertex3f(0,0,0); glVertex3f(0,0,1); // z axis
		glEnd(/*GL_LINES*/);
	}
	
	glutSwapBuffers();
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
        leftDown = (state == GLUT_DOWN);
        if (state == GLUT_DOWN) {
            Vec3f clickedPoint = clickPoint(x,y);
            Vec3f actualCamPos(cameraPos[0]+pan[0],cameraPos[1]+pan[1],cameraPos[2]+pan[2]);
            
            if (clickedPoint.length() < 2) {
                deformMesh(mesh, clickedPoint, actualCamPos);
                cout << "Point clicked is:" << clickedPoint[0] << " " << clickedPoint[1] << " " << clickedPoint[2] << endl;                
                glutPostRedisplay();
            }
        }
    }
	else if (button == GLUT_RIGHT_BUTTON) rightDown = (state == GLUT_DOWN);
	else if (button == GLUT_MIDDLE_BUTTON) middleDown = (state == GLUT_DOWN);
	
	lastPos[0] = x;
	lastPos[1] = y;
}

void mouseMoved(int x, int y) {
	int dx = x - lastPos[0];
	int dy = y - lastPos[1];
	Vec3f curCamera(cameraPos[0],cameraPos[1],cameraPos[2]);
	Vec3f curCameraNormalized = curCamera.normalized();
	Vec3f right = up % curCameraNormalized;

	if (leftDown) {
		// Assume here that up vector is (0,1,0)
		Vec3f newPos = curCamera - 2*(float)((float)dx/(float)windowWidth) * right + 2*(float)((float)dy/(float)windowHeight) * up;
		newPos = newPos.normalized() * curCamera.length();
		
		up = up - (up | newPos) * newPos / newPos.sqrnorm();
		up.normalize();
		
		for (int i = 0; i < 3; i++) cameraPos[i] = newPos[i];
	}
	else if (rightDown) for (int i = 0; i < 3; i++) cameraPos[i] *= pow(1.1,dy*.1);
	else if (middleDown) {
		pan += -2*(float)((float)dx/(float)windowWidth) * right + 2*(float)((float)dy/(float)windowHeight) * up;
	}

	
	lastPos[0] = x;
	lastPos[1] = y;
	
	Vec3f actualCamPos(cameraPos[0]+pan[0],cameraPos[1]+pan[1],cameraPos[2]+pan[2]);
	computeViewCurvature(mesh,actualCamPos,curvature,viewCurvature,viewCurvatureDerivative);
	
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	Vec3f actualCamPos(cameraPos[0]+pan[0],cameraPos[1]+pan[1],cameraPos[2]+pan[2]);

	if (key == 's' || key == 'S') showSurface = !showSurface;
	else if (key == 'a' || key == 'A') showAxes = !showAxes;
	else if (key == 'c' || key == 'C') showCurvature = !showCurvature;
	else if (key == 'n' || key == 'N') showNormals = !showNormals;
    else if (key == 'e' || key == 'E') showEdges = !showEdges;
	else if (key == 'w' || key == 'W') writeImage(mesh, windowWidth, windowHeight, "renderedImage.svg", actualCamPos);
	else if (key == 'q' || key == 'Q') exit(0);
    else if (key == 'o' || key == 'O') {
        derivativeThreshold -= 10;
        cout << "Derivative Threshold = " << derivativeThreshold << endl;
        }
    else if (key == 'p' || key == 'P') {
        derivativeThreshold += 10;
        cout << "Derivative Threshold = " << derivativeThreshold << endl;
        }
    else if (key == 'k' || key == 'K') {
        angleCosineThreshold -= 0.01;
        cout << "Angle Cosine Threshold = " << angleCosineThreshold << endl;
        }
    else if (key == 'l' || key == 'L') {
        angleCosineThreshold += 0.01;
        cout << "Angle Cosine Threshold = " << angleCosineThreshold << endl;
        }
	glutPostRedisplay();
}

void reshape(int width, int height) {
	windowWidth = width;
	windowHeight = height;
	glutPostRedisplay();
}

int main(int argc, char** argv) {
	if (argc < 2) {
		cout << "Usage: " << argv[0] << " mesh_filename\n";
		exit(0);
	}
	
	IO::Options opt;
	opt += IO::Options::VertexNormal;
	opt += IO::Options::FaceNormal;
	
	mesh.request_face_normals();
	mesh.request_vertex_normals();
	
	cout << "Reading from file " << argv[1] << "...\n";
	if ( !IO::read_mesh(mesh, argv[1], opt )) {
		cout << "Read failed.\n";
		exit(0);
	}

	cout << "Mesh stats:\n";
	cout << '\t' << mesh.n_vertices() << " vertices.\n";
	cout << '\t' << mesh.n_edges() << " edges.\n";
	cout << '\t' << mesh.n_faces() << " faces.\n";
	
	simplify(mesh,1.0f);
	
	mesh.update_normals();
	
	mesh.add_property(viewCurvature);
	mesh.add_property(viewCurvatureDerivative);
	mesh.add_property(curvature);
	
	// Move center of mass to origin
	Vec3f center(0,0,0);
	for (Mesh::ConstVertexIter vIt = mesh.vertices_begin(); vIt != mesh.vertices_end(); ++vIt) center += mesh.point(vIt);
	center /= mesh.n_vertices();
	for (Mesh::VertexIter vIt = mesh.vertices_begin(); vIt != mesh.vertices_end(); ++vIt) mesh.point(vIt) -= center;

	// Fit in the unit sphere
	float maxLength = 0;
	for (Mesh::ConstVertexIter vIt = mesh.vertices_begin(); vIt != mesh.vertices_end(); ++vIt) maxLength = max(maxLength, mesh.point(vIt).length());
	for (Mesh::VertexIter vIt = mesh.vertices_begin(); vIt != mesh.vertices_end(); ++vIt) mesh.point(vIt) /= maxLength;
	
	computeCurvature(mesh,curvature);

	up = Vec3f(0,1,0);
	pan = Vec3f(0,0,0);
	
	Vec3f actualCamPos(cameraPos[0]+pan[0],cameraPos[1]+pan[1],cameraPos[2]+pan[2]);
	computeViewCurvature(mesh,actualCamPos,curvature,viewCurvature,viewCurvatureDerivative);

	glutInit(&argc, argv); 
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 
	glutInitWindowSize(windowWidth, windowHeight); 
	glutCreateWindow(argv[0]);

	glutDisplayFunc(display);
	glutMotionFunc(mouseMoved);
	glutMouseFunc(mouse);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);

	glutMainLoop();
	
	return 0;
}
