#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include<iostream>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
using namespace std;
using namespace cv;
int fd;
void settings(const char *abc)
{
      fd = open(abc,O_RDWR | O_NOCTTY); /* ttyUSB0 is the FT232 based USB2SERIAL Converter   */
      usleep(3500000);
                                    /* O_RDWR Read/Write access to serial port           */
                                    /* O_NOCTTY - No terminal will control the process   */
                                    /* O_NDELAY -Non Blocking Mode,Does not care about-  */
                                    /* -the status of DCD line,Open() returns immediatly */                                        
                                    
            if(fd == -1)                        /* Error Checking */
                   printf("\n  Error! in Opening ttyUSB0  ");
            else
                   printf("\n  ttyUSB0 Opened Successfully ");
       struct termios toptions;         /* get current serial port settings */
       tcgetattr(fd, &toptions);        /* set 9600 baud both ways */
       cfsetispeed(&toptions, B9600);
       cfsetospeed(&toptions, B9600);   /* 8 bits, no parity, no stop bits */
       toptions.c_cflag &= ~PARENB;
       toptions.c_cflag &= ~CSTOPB;
       toptions.c_cflag &= ~CSIZE;
       toptions.c_cflag |= CS8;         /* Canonical mode */
       toptions.c_lflag |= ICANON;       /* commit the serial port settings */
       tcsetattr(fd, TCSANOW, &toptions);
}
void sendCommand(const char *abc)
{
   write(fd, abc, 1);
}

int barcode(Mat *img){
	//BARCODE MISSING
}
char circ_col(Mat *img, int rmin, int rmax, int gmax, int gmin, int bmin, int bmax){
	Mat img3;
	cvtColor(img,img3,CV_RGB@GRAY,0);
	vector<Vec3f>circles;
	HoughCircles(img3,circles,CV_HOUGH_GRADIENT,2,img.rows/8,200,100,0,0);
	if(circles.size()>0){
		int x = circle[0][0], y = circle[0][1];
		char color;
		if(img->at<uchar>(x,y)[0]>bmin && img->at<uchar>(x,y)[0]<bmax && (img->at<uchar>(x,y)[1]>gmax || img->at<uchar>(x,y)[1]<gmin) && (img->at<uchar>(x,y)[2]>rmax || img->at<uchar>(x,y)[2]<rmin)){
			color='B';
			return color;
		} 
		if(img->at<uchar>(x,y)[2]>rmin && img->at<uchar>(x,y)[2]<rmax && (img->at<uchar>(x,y)[1]>gmax || img->at<uchar>(x,y)[1]<gmin) && (img->at<uchar>(x,y)[0]>bmax || img->at<uchar>(x,y)[0]<bmin)){
			color='R';
			return color;
		}
		if(img->at<uchar>(x,y)[1]>gmin && img->at<uchar>(x,y)[1]<gmax && (img->at<uchar>(x,y)[0]>bmax || img->at<uchar>(x,y)[0]<bmin) && (img->at<uchar>(x,y)[2]>rmax || img->at<uchar>(x,y)[2]<rmin)){
			color='G';
			return color;
		}
	}
}
int main()
{	
	settings("/dev/ttyACMO");
	char dir='W',col;
	VideoCapture cap(0);
	if(!cap.isOpened())
		return -1;
	namedWindow("window",WINDOW_NORMAL);
	int thre=83;
	createTrackbar("thre","window",&thre,255);
	int rmin=0,rmax=0,bmin=0,bmax=0,gmin=0,gmax=0;
	createTrackbar("rmin","window",&rmin,255);
	createTrackbar("rmax","window",&rmax,255);
	createTrackbar("bmin","window",&bmin,255);
	createTrackbar("bmax","window",&bmax,255);
	createTrackbar("gmin","window",&gmin,255);
	createTrackbar("gmax","window",&gmax,255);
	while(1)
	{
		Mat img;
		cap>>img;
		Mat img1(img.rows,img.cols,CV_8UC1,Scalar(0));
		Mat img3(img.rows,img.cols,CV_8UC1,Scalar(0));
		int i,j,avg=0;
		cvtColor(img,img3,CV_RGB2GRAY,0);
		vector<Vec3f>circles;
		HoughCircles(img3,circles,CV_HOUGH_GRADIENT,2,img.rows/8,200,100,0,0);
		cout<<circles.size()<<endl;
		int count=0;
		if(circles.size()==1){
			int x = circles[0][0];
			if(x < img.cols/3){
				sendCommand('A');
			}
			else if(x > 2*img.cols/3){
				sendCommand('D');
			}
			else{
				sendCommand('W');
			}
		}
		else if(circles.size()==0 && count<2){
			int ang = barcode(&img);
			avg+=ang;
			count++;
			if(arg==0||arg==3||arg==6||arg==9||arg==12||arg==15){
				col='G';
			}
			else if(arg==1||arg==4||arg==7||arg==10||arg==13){
				col='R';
			}
			else
				col='B';
			sendCommand('W');
			cout<<"spin start"<<endl;
			if(circ_col(&img3)==col){
				sendCommand('S');
			}
			sendCommand('W');
		}
		else if(circles.size()==0 && count==2){
			int ang = avg/2;
			count++;
			if(arg==0||arg==3||arg==6||arg==9||arg==12||arg==15){
				col='G';
			}
			else if(arg==1||arg==4||arg==7||arg==10||arg==13){
				col='R';
			}
			else
				col='B';
			sendCommand('W');
			while(circ_col(&img)==col){
				sendCommand('S');
			}
			sendCommand('W');
		}
		else{
			sendCommand(dir);
		}
		namedWindow("window1",WINDOW_NORMAL);
		imshow("window",img1);
		waitKey(1);
		imshow("window1",img);
		waitKey(1);
		namedWindow("window3",WINDOW_NORMAL);
		imshow("window3",img3);
		waitKey(1);
	}
	return 0;
}