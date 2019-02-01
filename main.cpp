#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( int argc, char** argv ){
	ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL context

	if(argc < 2){
		printf("exe [Num_InitialOverlays]\n");
		return 0;
		
	}else{
		int Num_InitialOverlays = atoi(argv[1]);
		
		ofRunApp(new ofApp(Num_InitialOverlays));
	}
}
