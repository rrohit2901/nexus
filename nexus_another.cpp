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
	settings("/dev/ttyACM0");
	VideoCapture cap(1);
	if(!cap.isOpened())
		return -1;
	namedWindow("original",WINDOW_NORMAL);
	int i,j,thre=152;
	createTrackbar("thre","original",&thre,255);
	while(1){
		Mat img;
		cap>>img;
		int flag=0;
		cvtColor(img,img,CV_BGR2GRAY,0);
		Mat img1 = img.clone();
		for(i=0;i<img.rows;i++){
			for(j=0;j<img.cols;j++){
				if(img.at<uchar>(i,j)<thre){
					img1.at<uchar>(i,j)=0;
				}
				else
					img1.at<uchar>(i,j)=255;
			}
		}
		Mat img2(img.rows,img.cols,CV_8UC1,Scalar(0));
		Mat img3 = img.clone();
		Canny(img1, img2, 50, 100, 3);
		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		findContours( img1, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
		drawContours( img3, contours, -1, Scalar(255,0,0),2,8,hierarchy,0,Point());
		/*namedWindow("canny",WINDOW_NORMAL);
		imshow("canny",img2);
		waitKey(1);*/
		/*namedWindow("contour", WINDOW_NORMAL)
		imshow("contour",img3);
		waitKey(1);*/
		imshow("original",img);
		waitKey(1);
		if(contours.empty()){
			continue;
		}
		int min_dist=INT_MAX, index;
		Moments m;
		int centerx0=img.cols/2, centery0=img.rows-1, centerx, centery;
		for(i=0;i<contours.size();i++){
			m=moments(contours[i],true);
			centerx=(int)(m.m10/m.m00);
			centery=(int)(m.m01/m.m00);
			int dis = (int)sqrt((centerx-centerx0)*(centerx-centerx0)+(centery-centery0)*(centery-centery0));
			if(dis<min_dist){
				min_dist = dis;
				index= i;
			}
		}
		vector<Vec4i> lines;
		HoughLinesP(img3, lines, 1, CV_PI/180, 150, 50, 10 );
		
			i=index;
			m=moments(contours[i],true);
			centerx=(int)(m.m10/m.m00);
			centery=(int)(m.m01/m.m00);
			if(centerx<2*img.cols/7){
				sendCommand("A");
			}
			else if(centerx>5*img.cols/7){
				sendCommand("D");
			}
			else{
				sendCommand("W");
			}
		
	}
	return 0;
}