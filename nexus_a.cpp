#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include<bits/stdc++.h>
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
int main(){
	settings("/dev/ttyACMO");
	char dir='W';
	VideoCapture cap(1);
	if(!cap.isOpened())
		return -1;
	namedWindow("window",WINDOW_AUTOSIZE);
	int thre=152;
	createTrackbar("thre","window",&thre,255);
	while(1)
	{
		Mat img;
		cap>>img;
		Mat img1(img.rows,img.cols,CV_8UC1,Scalar(0));
		Mat img3(img.rows,img.cols,CV_8UC1,Scalar(0));
		int i,j;
		cvtColor(img,img3,CV_RGB2GRAY,0);
		vector<Vec3f>circles;
		HoughCircles(img3,circles,CV_HOUGH_GRADIENT,1,img.rows/8,200,100,0,0);
		cout<<circles.size()<<endl;
		if(circles.size()==1){
			int x = circles[0][0];
			if(x < img.cols/3){
				sendCommand("A");
			}
			else if(x > 2*img.cols/3){
				sendCommand("D");
			}
			else{
				sendCommand("W");
			}
		}
		else if(circles.size()==0){
			for(i=0;i<img.rows;i++){
				for(j=0;j<img.cols;j++){
					if(img.at<uchar>(i,j)<thre){
						img1.at<uchar>(i,j)=255;
					}
					else{
						img1.at<uchar>(i,j)=0;
					}
				}
			}
			vector<Vec2f> lines;
			HoughLines(img3, lines, 1, CV_PI/180, 100, 0, 0 );
			cout<<lines.size()<<endl;
			if(lines.size()==3){
				dir='A';
				sendCommand("W");
			}
			else if(lines.size()==4){
				dir='D';
				sendCommand("W");
			}
		}
		else{
			if(dir=='A')
				sendCommand("A");
			else if(dir=='D')
				sendCommand("D");
			else
				sendCommand("W");
		}
		namedWindow("window1",WINDOW_AUTOSIZE);
		imshow("window",img1);
		waitKey(1);
		imshow("window1",img);
		waitKey(1);
		namedWindow("window3",WINDOW_AUTOSIZE);
		imshow("window3",img3);
		waitKey(1);
	}
	return 0;
}