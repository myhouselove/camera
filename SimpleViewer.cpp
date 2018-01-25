
#include "SimpleViewer.h"
#include "include/OrbbecCAPI.h"
#include <GL/glut.h>
#include <sstream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#define RES_COLOR_WIDTH    640
#define RES_COLOR_HEIGHT   480
#define GL_WIN_SIZE_X   800
#define GL_WIN_SIZE_Y   600
#define TEXTURE_SIZE    512

#define MIN_NUM_CHUNKS(data_size, chunk_size)	((((data_size)-1) / (chunk_size) + 1))
#define MIN_CHUNKS_SIZE(data_size, chunk_size)	(MIN_NUM_CHUNKS(data_size, chunk_size) * (chunk_size))


SimpleViewer* SimpleViewer::m_self = NULL;
uint32_t m_exposureReg, m_exposureRegSet;

void SimpleViewer::glutIdle()
{
    glutPostRedisplay();
}

void SimpleViewer::glutDisplay()
{
	SimpleViewer::m_self->display();
}

void SimpleViewer::glutKeyboard(unsigned char key, int x, int y)
{
	SimpleViewer::m_self->onKey(key, x, y);
}

SimpleViewer::SimpleViewer(int w,int h)
{
	m_self = this;
	m_width = w;
	m_height = h;
	
	//m_pTexMap = new unsigned char[w*h*3];
	colorstop = false;
	
	

}


SimpleViewer::~SimpleViewer()
{

    m_self = NULL;
    if (m_self != NULL)
    {
        delete[] m_self;
    }

}

int SimpleViewer::init(int argc, char** argv)
{
	// Default mode: 
	// 1. VGA IR stream is on
	// 2. laser is off
	cam = new V4L2Camera();
	int ret = orbbec_Init();
	if(ret == 0 ){
		//cout<<"init success"<<endl;
	}
	ret = orbbec_RefreshWorldConversionCache(1280,800,1);
	if(ret == 0 ){
		//cout<<"set resolution success"<<endl;
	}
	ref = new unsigned short[m_width*m_height];
	dst = new unsigned short[m_width*m_height];
	
	cam->Open("/dev/video0");
	cam->Init();
	//cout<<"Init: ok"<<endl;
	//cam->setParameters(640,480,V4L2_PIX_FMT_MJPEG);
	
	//cout<<"StartStreaming: ok"<<endl;
	//cam->start();	

    return initOpenGL(argc, argv);
}


void SimpleViewer::loadCOLORFrame()
{
	
	unsigned int textMapX = m_nTexMapX;
	unsigned int textMapY = m_nTexMapY;
	m_nTexMapX = MIN_CHUNKS_SIZE(m_width, TEXTURE_SIZE);
	m_nTexMapY = MIN_CHUNKS_SIZE(m_height, TEXTURE_SIZE);

	if (m_pTexMap== NULL)
	{
printf("x=%d,y=%d\t",m_nTexMapX,m_nTexMapY);
		m_pTexMap = new RGB888Pixel[m_nTexMapX * m_nTexMapY];
memset(m_pTexMap,0,m_nTexMapX * m_nTexMapY*3);
printf("32323");
fflush(stdout);
	}
	
		cam->getUVCData(ref);
		//int ret = orbbec_Shift_2_Depth(ref,dst,m_width,m_height);
		cv::Mat ds(m_height,m_width,CV_16SC1,ref);
		cv::imshow("dst",ds);	
		//cv::waitKey(0);
		unsigned short* pDepData = ref;
		RGB888Pixel* pTexRow = m_pTexMap;
		for (int y = 0; y < m_height; ++y)
		{
			const unsigned short* pDepth = pDepData;
			RGB888Pixel* pTex = pTexRow;
			for (int x = 0; x < m_width; ++x,++pTex)
			{
				
					int nHistValue = (int)((float)(*((unsigned short*)pDepth + y*640+x)) / 1500 * 255);
					pTex->r = 0xff000000 | nHistValue << 16 | nHistValue << 8 | nHistValue;
					pTex->g = 0xff000000 | nHistValue << 16 | nHistValue << 8 | nHistValue;
					pTex->b = 0;
				
			
			}

			//pDepData += 640;
			pTexRow += m_nTexMapX;
		}

/*
		for (int i = 0, j = 0; i < m_height * m_width * 3 && j < m_height * m_width; j++) 
		{
			int value = (int)((float)(*((unsigned short*)dst + j)) / 1500 * 255);
			rgb888[i] = 0x0;
			rgb888[i + 1] = 0xff000000 | value << 16 | value << 8 | value;
			rgb888[i + 2] = 0xff000000 | value << 16 | value << 8 | value;
			i += 3;
		}*/
		//cv::Mat gl(400,640,CV_8UC3,m_pTexMap);
		//cv::imshow("gl",gl);
	

}





int SimpleViewer::run()
{
	cam->StartStreaming ();
    glutMainLoop();

    return 0;
}


void SimpleViewer::display()
{
    int changedIndex = -1;


    loadCOLORFrame();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, GL_WIN_SIZE_X, GL_WIN_SIZE_Y, 0, -1.0, 1.0);

    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_nTexMapX, m_nTexMapY, 0, GL_RGB, GL_UNSIGNED_BYTE, m_pTexMap);
    // Display the OpenGL texture map
    glColor4f(1, 1, 1, 1);

    glBegin(GL_QUADS);

    int nXRes = m_width;
    int nYRes = m_height;

    // upper left
    glTexCoord2f(0, 0);
    glVertex2f(0, 0);
    // upper right
    glTexCoord2f((float)nXRes / (float)m_nTexMapX, 0);
    glVertex2f(GL_WIN_SIZE_X, 0);
    // bottom right
    glTexCoord2f((float)nXRes / (float)m_nTexMapX, (float)nYRes / (float)m_nTexMapY);
    glVertex2f(GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
    // bottom left
    glTexCoord2f(0, (float)nYRes / (float)m_nTexMapY);
    glVertex2f(0, GL_WIN_SIZE_Y);

    glEnd();

	
    // Swap the OpenGL display buffers
    glutSwapBuffers();
  
}


void SimpleViewer::onKey(unsigned char key, int x, int y)
{
	
}

int SimpleViewer::initOpenGL(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH |GLUT_RGBA);
    glutInitWindowSize(GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
    glutCreateWindow("depth");

    glutSetCursor(GLUT_CURSOR_NONE);

    initOpenGLHooks();

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    return 0;
}


void SimpleViewer::initOpenGLHooks()
{
    glutKeyboardFunc(glutKeyboard);
    glutDisplayFunc(glutDisplay);
    glutIdleFunc(glutIdle);
}




