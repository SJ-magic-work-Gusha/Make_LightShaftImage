/************************************************************
************************************************************/
#include "ofApp.h"

/************************************************************
************************************************************/

/******************************
******************************/
ofApp::ofApp(int _Num_Overlays)
: Num_Overlays(_Num_Overlays)
, Initial_Num_Overlays(_Num_Overlays)
, b_1stDraw(true)
, png_id(0)
, b_DispGui(true)
{
	/********************
	********************/
	Vboset.setup(5);
}

/******************************
******************************/
ofApp::~ofApp()
{
}

/******************************
******************************/
void ofApp::setup(){
	/********************
	********************/
	ofSetBackgroundAuto(true);
	
	ofSetWindowTitle("make light shaft");
	ofSetVerticalSync(true);
	ofSetFrameRate(30);
	ofSetWindowShape(WINDOW_WIDTH, WINDOW_HEIGHT);
	ofSetEscapeQuitsApp(false);
	
	/********************
	********************/
	fbo.allocate(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, 4);
	Clear_fbo(fbo);
	
	/********************
	********************/
	setup_gui();
}

/******************************
******************************/
void ofApp::setup_gui()
{
	/********************
	********************/
	gui.setup("param", "gui.xml", 10, 10);
	
	/********************
	********************/
	gui.add(gui_b_AddBlend.setup("b_AddBlend", false));
	
	gui.add(gui_width_source.setup("w_src", 20, 1, ofGetWidth()));
	
	gui.add(gui_a_from.setup("a_from", 100, 0, 255));
	gui.add(gui_a_to.setup("a_to", 0, 0, 255));
	
	gui.add(gui_b_Random.setup("b_Random", false));
	gui.add(gui_width_dst_whenNotRandom.setup("w_dst(NoRandom)", 50, 1, ofGetWidth()));
	
	/********************
	********************/
	gui.minimizeAll();
}

/******************************
******************************/
void ofApp::Clear_fbo(ofFbo& fbo)
{
	fbo.begin();
	
	// Clear with alpha, so we can capture via syphon and composite elsewhere should we want.
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	ofClear(0, 0, 0, 0);
	
	fbo.end();
}

/******************************
******************************/
void ofApp::update(){
	/********************
	********************/
	Vboset.set_Color(0, ofColor(255, 255, 255, gui_a_from));
	Vboset.set_Color(1, ofColor(255, 255, 255, gui_a_from));
	Vboset.set_Color(2, ofColor(255, 255, 255, gui_a_to));
	Vboset.set_Color(3, ofColor(255, 255, 255, gui_a_to));
		
	if(b_1stDraw){
		Vboset.VboVerts[0].set(ofGetWidth()/2 - gui_width_source/2, 0);
		Vboset.VboVerts[1].set(ofGetWidth()/2 + gui_width_source/2, 0);
		Vboset.VboVerts[2].set(ofGetWidth(), ofGetHeight());
		Vboset.VboVerts[3].set(0, ofGetHeight());
		
		Vboset.update();
		
	}else{
		/* */
		if(Num_Overlays <= 0) return;
		
		/* */
		if(gui_b_Random){
			// gui_width_sourceの中でRandomにsrc pointを選択.
			// 実際のwidthは2 pix.
			float src_center = ofRandom(ofGetWidth()/2 - gui_width_source/2, ofGetWidth()/2 + gui_width_source/2);
			Vboset.VboVerts[0].set(src_center-1, 0);
			Vboset.VboVerts[1].set(src_center+1, 0);
			
			// dst : 実験の結果、頂点の順序が変わっても、塗りが入るようだが、一応、仕様で指定された順序にしておく。
			float x0 = ofRandom(0, ofGetWidth());
			float x1 = ofRandom(0, ofGetWidth());
			
			Vboset.VboVerts[2].set(max(x0, x1), ofGetHeight());
			Vboset.VboVerts[3].set(min(x0, x1), ofGetHeight());
			
		}else{
			// gui_width_sourceの中でRandomにsrc pointを選択.
			// 実際のwidthは2 pix.
			float src_center = ofRandom(ofGetWidth()/2 - gui_width_source/2, ofGetWidth()/2 + gui_width_source/2);
			
			Vboset.VboVerts[0].set(src_center-1, 0);
			Vboset.VboVerts[1].set(src_center+1, 0);
			
			// dst.
			float dst_center = ofRandom(0, ofGetWidth());
			float x_l = max(float(dst_center - gui_width_dst_whenNotRandom/2), 0.0f);
			float x_r = min(float(dst_center + gui_width_dst_whenNotRandom/2), float(ofGetWidth()));
			
			Vboset.VboVerts[2].set(x_r, ofGetHeight());
			Vboset.VboVerts[3].set(x_l, ofGetHeight());
		}
		
		Vboset.update();
	}
}

/******************************
******************************/
void ofApp::draw_to_fbo(){
	fbo.begin();
		
		/********************
		********************/
		if(gui_b_AddBlend){
			ofDisableSmoothing();
			
			ofEnableAlphaBlending();
			ofEnableBlendMode(OF_BLENDMODE_ADD);
		}else{
			ofEnableSmoothing();
			
			ofEnableAlphaBlending();
			ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		}
		
		/********************
		********************/
		ofSetColor(255);
		glPointSize(1.0);
		glLineWidth(1);
		
		ofFill();
		
		if(b_1stDraw){
			ofClear(0, 0, 0, 0);
			
			Vboset.draw(GL_QUADS);
			b_1stDraw = false;
			
		}else{
			if(Num_Overlays <= 0){
				// no overdraw.
			}else{
				Vboset.draw(GL_QUADS);
				Num_Overlays--;
			}
		}
		
	fbo.end();
}

/******************************
******************************/
void ofApp::draw(){
	/********************
	********************/
	draw_to_fbo();
	
	/********************
	********************/
	ofEnableSmoothing();
	ofDisableAlphaBlending();
	
	ofClear(0, 0, 0, 0);
	ofSetColor(255);
	
	fbo.draw(0, 0);
	
	/********************
	********************/
	if(b_DispGui)	gui.draw();
}

/******************************
******************************/
void ofApp::keyPressed(int key){
	switch(key){
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			Num_Overlays += key - '0';
			break;
			
		case 'c':
			b_1stDraw = true;
			Num_Overlays = Initial_Num_Overlays;
			break;
			
		case 'd':
			b_DispGui = !b_DispGui;
			break;
			
		case ' ':
			{
				ofImage img;
				ofPixels pixels;
				
				fbo.readToPixels(pixels);
				img.setFromPixels(pixels);
				char fileNameStr[BUF_SIZE_S];
				string date = ofGetTimestampString();//タイムスタンプをファイル名にする
				sprintf(fileNameStr, "%s.png", date.c_str());
				img.save(fileNameStr,OF_IMAGE_QUALITY_BEST);
				
				printf("> %s saved\n", fileNameStr);
		
				/*
				// 画面保存だと、alphaで背景抜くことができなかった.
				char buf[BUF_SIZE_S];
				
				sprintf(buf, "image_%d.png", png_id);
				ofSaveScreen(buf);
				// ofSaveFrame();
				printf("> %s saved\n", buf);
				
				png_id++;
				*/
			}
			break;
			
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
