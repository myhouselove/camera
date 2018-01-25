#ifndef __RGBVIEWER_H__
#define __RGBVIEWER_H__

#include <iostream>
#include "V4L2Camera.h"
#pragma once

#define MAX_DEPTH (10000)
#define Astra_Pro (0x0403)

#include <stdint.h>
typedef struct{
	uint8_t r;
	uint8_t g;
	uint8_t b;
}RGB888Pixel;

class SimpleViewer
{
public:

	enum StreamTypes
	{
		STREAM_TYPE_IR_VGA,
		STREAM_TYPE_DEPTH_VGA,
		STREAM_TYPE_RGB_VGA,
		STREAM_TYPE_UNKNOWN
	};


	SimpleViewer(int w,int h);
	int init(int argc, char** argv);
	virtual ~SimpleViewer();

    void loadCOLORFrame();	

    int run();
	bool changeStringValueFlag;
	V4L2Camera *cam;
	unsigned short* ref;
	unsigned short* dst;
protected:
    virtual void display();
    virtual void onKey(unsigned char key, int x, int y);

    virtual int initOpenGL(int argc, char** agrv);
    void    initOpenGLHooks();

    

private:
    static SimpleViewer*           m_self;
    static void                 glutIdle();
    static void                 glutDisplay();
    static void                 glutKeyboard(unsigned char key, int x, int y);
    bool 			colorstop ;
    int 			v4l2flag;
    int                         m_width;
    int                         m_height;
    unsigned int                m_nTexMapX;
    unsigned int                m_nTexMapY;
	RGB888Pixel*				m_pTexMap;


   float			m_pDepthHist[MAX_DEPTH];
};

#endif



