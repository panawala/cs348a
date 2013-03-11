#include <iostream>
#include "st.h"
#include "stgl.h"
#include "stglut.h"
#include "Scene.h"

using namespace std;

void usage()
{
	printf("usage: raytracer sceneFileName\n");
	exit(0);
}

int main(int argc, const char * argv[])
{

    for (int i=1; i<argc; i++) {        
        string sceneFileName = argv[i];
        Scene *scene = new Scene(sceneFileName);
        scene->Render();
    }

    return 0;
}

