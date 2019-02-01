/************************************************************
description
	tool to make a Light shaft texture for Gusha.
	
note
	■Blend modeについて
		OF_BLENDMODE_ADDでtexture作成することも考え、実際に"gui_b_AddBlend"で切り替えもできるが、
		実は、oFでは、加算合成とスムージングは同時に効かない -> 加算合成の前にofDisableSmoothing()を記述する必要がある。
			参考
			- https://qiita.com/y_UM4/items/b03a66d932536b25b51a
			- oF Essentials : p73
			
		すると、斜線だけでなく、Light source -> dstに掛けるグラデーションも、ガタガタに(滑らかに色のグラデーションが出ない)なってしまった。
		
		今回は、OF_BLENDMODE_ALPHAで作成すること.
	
	■alpha付きで、背景のないpng出力
		screen画面を ofSaveScreen で出力すると、alphaが黒で残ってしまった。
		
		fboからスクリーンショットを保存する手法を使って、解決した。
			参考
			- https://www.atnr.net/how-to-save-fbo-screenshot-on-of/

making texture tips
	1	想像以上に、薄く作成しないと、unity側に組み込んだ時、自然にならなかった。
		具体的には、"exe 5" で1発のみくらい。
			↑修正)
				3) のmaterial offsetにより、Realityが増したせいか、もう少し明るくても自然に見えた
					exe 8(!b_Random)
				くらいやっても大丈夫。
				b_Randomだと、少し濃いかな。。。
				あとは適宜、調整。
		key = c にて、Balanceを整える程度で丁度良さそう : 均等過ぎると、また不自然。
		Baseの塗り も、oF上では薄くて見えないが、unity側では結構はっきりと見える。
		
		実際、光が空気中のモヤやホコリにあたって、これが見えるので、濃すぎると不自然になるのだろう。
	
	2	gui_a_fromの値は、unity側のEmissionと組み合わせで考える。
		空気中のチリにあたった光なので、発光しているわけではない = Emissionは小さめで。
		大きくし過ぎると、unity上で、textureのグラデーションがダンダン模様になってくるので、調整すること。
		
	3	unity側で、material/offset_Yをsetし、光源全体から光線が出ている感じにすること。
		点から出ているより、ずっとRealityが増す。
		
		textureのWrap mode = clamp
		にしておかないと、Repeatされてしまうので、注意.
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"
#include "ofxGui.h"

#include "sj_common.h"

/************************************************************
************************************************************/

/**************************************************
**************************************************/
struct VBO_SET : private Noncopyable{
	ofVbo Vbo;
	vector<ofVec3f> VboVerts;
	vector<ofFloatColor> VboColor;
	
	void setup(int size){
		VboVerts.resize(size);
		VboColor.resize(size);
		
		Vbo.setVertexData(&VboVerts[0], VboVerts.size(), GL_DYNAMIC_DRAW);
		Vbo.setColorData(&VboColor[0], VboColor.size(), GL_DYNAMIC_DRAW);
	}
	
	void set_singleColor(const ofColor& color){
		for(int i = 0; i < VboColor.size(); i++) { VboColor[i].set( double(color.r)/255, double(color.g)/255, double(color.b)/255, double(color.a)/255); }
	}
	
	void set_Color(int index, const ofColor& color){
		if( (index < 0) || (VboColor.size() <= index) ) return;
		
		VboColor[index].set( double(color.r)/255, double(color.g)/255, double(color.b)/255, double(color.a)/255);
	}
	
	void update(){
		Vbo.updateVertexData(&VboVerts[0], VboVerts.size());
		Vbo.updateColorData(&VboColor[0], VboColor.size());
	}
	
	void draw(int drawMode){
		Vbo.draw(drawMode, 0, VboVerts.size());
	}
	
	void draw(int drawMode, int total){
		if(VboVerts.size() < total) total = VboVerts.size();
		Vbo.draw(drawMode, 0, total);
	}
};

/**************************************************
**************************************************/
class ofApp : public ofBaseApp{
private:
	/****************************************
	****************************************/
	/********************
	********************/
	ofxPanel gui;
	ofxFloatSlider gui_width_source;
	ofxFloatSlider gui_a_from;
	ofxFloatSlider gui_a_to;
	ofxToggle gui_b_Random;
	ofxFloatSlider gui_width_dst_whenNotRandom;
	
	ofxToggle gui_b_AddBlend;
	
	
	bool b_DispGui;
	
	/********************
	********************/
	ofFbo fbo;
	
	bool b_1stDraw;
	const int Initial_Num_Overlays;
	int Num_Overlays;
	
	VBO_SET Vboset;
	int png_id;
	
	/****************************************
	****************************************/
	void Clear_fbo(ofFbo& fbo);
	void draw_to_fbo();
	void setup_gui();

public:
	/****************************************
	****************************************/
	void setup();
	void update();
	void draw();
	
	ofApp(int _Num_Overlays);
	~ofApp();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
};
