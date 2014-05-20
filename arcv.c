/*
author 		: Arunkrishnan Chettoor
Platform 	: Cubieboard 2/cubian OS
Description	: The code will detect 3 vertial points from the depth image pixel and if the points is <= 500mm destance it provides 
		feedback to the corresponding GPIO pins.
*/

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include "libfreenect_cv.h"
#include "libfreenect.h"
#include "forBlind.h"
#include <pthread.h>
#include <unistd.h>

#define frontsound 0
#define leftsound 1
#define rightsound 2
#define noise 3


//#define leftmotor 61
//#define rightmotor 62
//#define centermotor 60
 
//#define withscreen 
//freenect_context *f_ctx;

int getDist(IplImage *depth){
 	int x = depth->width/2;
	int y = depth->height/2;
	//printf("width= %d and height %d \n",x,y);
	int d = depth->imageData[x*2+y*640*2+1];
	//printf("1st value is %d \n",d);
	d= d << 8;
	d= d+depth->imageData[x*2+y*640*2];
	return d;
}

int getDista(IplImage *depth,int x,int y){
	//printf("width= %d and height %d \n",x,y);
	int d = depth->imageData[x*2+y*640*2+1];
	//printf("1st value is %d \n",d);
	d= d << 8;
	d= d+depth->imageData[x*2+y*640*2];
	return d;
}

int initMotor(){
	char command[] = "echo 60 > /sys/class/gpio/export";
	system(command);
	char command1[] = "echo 61 > /sys/class/gpio/export";
	system(command1);
	char command2[] = "echo 62 > /sys/class/gpio/export";
	system(command2);
	usleep(1000 * 1000);
	char command3[] = "echo out > /sys/class/gpio/gpio61_pi13/direction";
        system(command3);
	char command4[] = "echo out > /sys/class/gpio/gpio60_pi11/direction";
                system(command4);	
	char command5[] = "echo out > /sys/class/gpio/gpio62_pi10/direction";
                system(command5);
	usleep(1000 * 1000);
	printf("\n Motors Initialized..\n");
	return 0;
}

int genSound(int command){
	if(command == frontsound){
		char command[]="aplay /usr/share/sounds/alsa/Front_Center.wav";
		return(system(command));
	}
	else if(command == rightsound){
                char command[]="aplay /usr/share/sounds/alsa/Front_Right.wav";
                return(system(command));
        }	
	else if(command == leftsound){
                char command[]="aplay /usr/share/sounds/alsa/Front_Left.wav";
                return(system(command));
        }
	else if(command == noise){
                char command[]="aplay /usr/share/sounds/alsa/Noise.wav";
                return(system(command));
        }
	return 0;
}

int passdistMotor(int* value){
	if(value[0] <= 500){
		char command[] = "echo 1 > /sys/class/gpio/gpio61_pi13/value";
	        system(command);
	}
	else{
		char command[] = "echo 0 > /sys/class/gpio/gpio61_pi13/value";
                system(command);
	}
	if(value[1] <= 500){
                char command[] = "echo 1 > /sys/class/gpio/gpio60_pi11/value";             
                system(command);
        }       
        else{
                char command[] = "echo 0 > /sys/class/gpio/gpio60_pi11/value";             
                system(command);
        } 
	if(value[2] <= 500){
                char command[] = "echo 1 > /sys/class/gpio/gpio62_pi10/value";             
                system(command);
        }       
        else{
                char command[] = "echo 0 > /sys/class/gpio/gpio62_pi10/value";             
                system(command);
        } 
	printf("\n %d   %d   %d \n",value[0],value[1],value[2]);
	return 0;
}

IplImage *GlViewColor(IplImage *depth)
{
	static IplImage *image = 0;
	if (!image) image = cvCreateImage(cvSize(640,480), 8, 3);
	unsigned char *depth_mid = (unsigned char*)(image->imageData);
	int i;
	for (i = 0; i < 640*480; i++) {
		int lb = ((short *)depth->imageData)[i] % 256;
		int ub = ((short *)depth->imageData)[i] / 256;
		switch (ub) {
			case 0:
				depth_mid[3*i+2] = 255;
				depth_mid[3*i+1] = 255-lb;
				depth_mid[3*i+0] = 255-lb;
				break;
			case 1:
				depth_mid[3*i+2] = 255;
				depth_mid[3*i+1] = lb;
				depth_mid[3*i+0] = 0;
				break;
			case 2:
				depth_mid[3*i+2] = 255-lb;
				depth_mid[3*i+1] = 255;
				depth_mid[3*i+0] = 0;
				break;
			case 3:
				depth_mid[3*i+2] = 0;
				depth_mid[3*i+1] = 255;
				depth_mid[3*i+0] = lb;
				break;
			case 4:
				depth_mid[3*i+2] = 0;
				depth_mid[3*i+1] = 255-lb;
				depth_mid[3*i+0] = 255;
				break;
			case 5:
				depth_mid[3*i+2] = 0;
				depth_mid[3*i+1] = 0;
				depth_mid[3*i+0] = 255-lb;
				break;
			default:
				depth_mid[3*i+2] = 0;
				depth_mid[3*i+1] = 0;
				depth_mid[3*i+0] = 0;
				break;
		}
	}
	return image;
}

void* motorThread(void* value){
	
}

int main(int argc, char **argv)
{	
	genSound(frontsound);
	if(!initMotor())
		printf("\nInitialized the motor\n");
	int value[3];
	pthread_t thread1;//for motor running
	while (cvWaitKey(100) != 27) {
		IplImage *depth = freenect_sync_get_depth_cv_nw(0);
		if (!depth) {
		    printf("Error: Kinect not connected?\n");
		    return -1;
		}
	//printf("values are %d , %d , %d \n",getDista(depth,160,240),getDista(depth,320,240),getDista(depth,480,240));
		
		value[0] = getDista(depth,160,240);
		value[1] = getDista(depth,320,240);
		value[2] = getDista(depth,480,240);
		
		passdistMotor(value);

		#ifdef withscreen
			cvShowImage("Depth", GlViewColor(depth));//GlViewColor(depth)
		#endif
	}
	//cvDestroyWindow("RGB");
	#ifdef withscreen
		cvDestroyWindow("Depth");
	#endif
	stopKinect();
	return 0;
}
