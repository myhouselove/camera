#include<iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "include/OrbbecCAPI.h"
#include <unistd.h>
#include <time.h>
#include <GL/glut.h>
#include "SimpleViewer.h"

using namespace cv;
using namespace std;

#define GL_WIN_SIZE_X   800
#define GL_WIN_SIZE_Y   600

#include "V4L2Camera.h"

int w=640;
int h=400;
unsigned char *rgb888 = NULL;
#define BUFFERSIZE 256000
unsigned char * getDepth2RGBData(unsigned short *ptr)
{
	
	//unsigned char* ptr = (unsigned char*)mDepth.getData();
	if (rgb888 == NULL) {
		rgb888 = new unsigned char[BUFFERSIZE*3];
	}
	for (int i = 0, j = 0; i < BUFFERSIZE * 3 && j < BUFFERSIZE; j++) {
		int value = (int)((float)(*((unsigned short*)ptr + j)) / 1500 * 255);
		rgb888[i] = 0x0;
		rgb888[i + 1] = 0xff000000 | value << 16 | value << 8 | value;
		rgb888[i + 2] = 0xff000000 | value << 16 | value << 8 | value;
		i += 3;
	}
	return rgb888;
}
int test(int argc, char** argv)
{
	
	namedWindow("girl");
	Mat girl = imread("timg.jpg");
	imshow("girl",girl);
	V4L2Camera *cam = new V4L2Camera();
	int ret = orbbec_Init();
	if(ret == 0 ){
		cout<<"init success"<<endl;
	}
	ret = orbbec_RefreshWorldConversionCache(1280,800,1);
	if(ret == 0 ){
		cout<<"set resolution success"<<endl;
	}
unsigned short* ref = new unsigned short[640*400];
	unsigned short* dst = new unsigned short[640*400];
	
	cam->Open("/dev/video0");
	cam->Init();
	cout<<"Init: ok"<<endl;
	//cam->setParameters(640,480,V4L2_PIX_FMT_MJPEG);
	cam->StartStreaming ();
	cout<<"StartStreaming: ok"<<endl;
	cam->start();
	//cam->GrabPreviewFrame();
	int i = 0;
	char name[15];
	memset(name,'\0',15);
	while((char)cvWaitKey(1) != (char)27){
	 cam->getUVCData(ref);
		
	Mat show(400,640,CV_16SC1,ref);
		sprintf(name,"test/p_%d.jpg",i);
		imwrite(name,show);
		cout<<name<<endl;
		i++;
		imshow("show",show);
	//ret = orbbec_Shift_2_Depth(ref,dst,640,400);
		//Mat ds(400,640,CV_8UC3,getDepth2RGBData(dst));
		//imshow("dst",ds);
	
	// usleep(30000);
	}
	orbbec_Release();
	//waitKey(0);
	delete ref;
	delete dst;
	cout<<"hello world"<<endl;
	return 0;
}


int main(int argc, char** argv){
	SimpleViewer *handle = new SimpleViewer(640,400);
	handle->init(argc,argv);
	handle->run();
	

	return 0;
}
