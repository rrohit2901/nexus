#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include<bits/stdc++.h>
using namespace std;
using namespace cv;
int main(){
	char dir='W';
	VideoCapture cap(0);
	if(!cap.isOpened())
		return -1;
	namedWindow("window",WINDOW_NORMAL);
	int thre=83;
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
		HoughCircles(img3,circles,CV_HOUGH_GRADIENT,2,img.rows/8,200,100,0,0);
		if(circles.size()>0)
			cout<<circles.size()<<endl;
		if(circles.size()==1){
			int x = circles[0][0];
			if(x < img.cols/3){
				cout<<"A"<<endl;
			}
			else if(x > 2*img.cols/3){
				cout<<"D"<<endl;
			}
			else{
				cout<<"W"<<endl;
			}
		}
		else if(circles.size()==0){
			for(i=0;i<img.rows;i++){
				for(j=0;j<img.cols;j++){
					if(img3.at<uchar>(i,j)<thre){
						img1.at<uchar>(i,j)=0;
					}
					else{
						img1.at<uchar>(i,j)=255;
					}
				}
			}
			Mat dst,cdst;
			Canny(img1,dst,50,100,3);
			cvtColor(dst, cdst, CV_GRAY2BGR);
			vector<Vec4i> lines;
			HoughLinesP(dst, lines, 1, CV_PI/180, 50, 50, 10 );
			cout<<lines.size()<<endl;
			for( size_t i = 0; i < lines.size(); i++ )
			{
			  Vec4i l = lines[i];
			  line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
			}
			if(lines.size()==3){
				dir='A';
				cout<<"W"<<endl;
			}
			else if(lines.size()==4){
				dir='D';
				cout<<"W"<<endl;
			}
		}
		else{
			cout<<dir<<endl;
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