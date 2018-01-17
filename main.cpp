#include <opencv2\opencv.hpp>
#include <iostream>
#include <math.h>
#include <Windows.h>
#include<stdio.h>
#include<stdlib.h>
using namespace cv;
using namespace std;
void get_screenshot();//获取手机上的图像
Point get_Character_Loc(Mat& srcImage, Mat& Tem_img);//获取模板的坐标
Point GetNextPoint(Mat& srcImage);//获取下个要跳的点
int get_distance(Point& finst_point, Point& next_point);//计算距离
void jump(int& g_distance);//跳
void end();//to do
Mat srcImage;
Mat dstImage;
Mat Character;
//Mat ROI_img;
//Mat end_img;//结束模板
//Mat dst_end;//结束匹配结果图片

int num = 500;//跳的次数
//float res_end;
int main()
{
	//end_img = imread("end.png");//结束模板图片
	
	while (num > 0)//经过num次结束
	{
		get_screenshot();
		srcImage = imread("d:/ADB/autojump.png");//读取源图片
		dstImage = srcImage.clone();
		Character = imread("object.png");//读取棋子模板图片
		Point next_p = get_Character_Loc(srcImage, Character);//获取棋子的坐标
		//-------------------------------------------------------------------
		//检测图像的尺寸
		//int colss, rowss;
		//colss = srcImage.cols;
		//rowss = srcImage.rows;
		//cout << colss << "   " << rowss << " " << endl;
		//1080    1920
		//-------------------------------------------------------------------
		//--------------------------------------------------------------------
		//检查目标检测情况
		//Mat ROI_img;
		//ROI_img = srcImage(Rect(srcImage.cols/5, srcImage.rows/3, (srcImage.cols/3)+50,(srcImage.rows/3)+50));
		//imshow(" ", ROI_img);
		//--------------------------------------------------------------------
		Point get_next = GetNextPoint(srcImage);//检测下一个点的坐标
		cout << "下一个点的坐标：" << get_next << endl << endl;//输出下个位置坐标
		int g_distance = get_distance(next_p, get_next);//计算距离
		jump(g_distance);//跳
		num--;
		Sleep(500);
	}
	waitKey(0);
	return 0;
}
void get_screenshot()//获取屏幕图像函数，应用ADB命令
{
	system("adb shell /system/bin/screencap -p /sdcard/autojump.png");
	system("adb pull /sdcard/autojump.png d:/ADB/autojump.png");

}
/*
void end()//to do
{
	
	matchTemplate(srcImage, end_img, dst_end, CV_TM_SQDIFF);
	double minVal, maxVal;
	Point minLoc, maxLoc, matchLoc;
	minMaxLoc(dstImage, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
}
*/
Point get_Character_Loc(Mat& srcImage, Mat& Tem_img)
{
	matchTemplate(srcImage, Tem_img, dstImage, CV_TM_SQDIFF);
	double minVal, maxVal;
	Point minLoc, maxLoc, matchLoc;
	minMaxLoc(dstImage, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
	matchLoc = minLoc;
	//cout << "maxVal:" << (int)maxVal << endl;
	rectangle(srcImage, Rect(matchLoc, Size(Character.cols, Character.rows)), Scalar(255, 255, 0), 1, 8, 0);
	putText(srcImage, "object", Point(matchLoc.x + Character.cols*0.5, matchLoc.y + Character.rows), 1, 2, Scalar(0, 0, 255));

	return Point(matchLoc.x + Character.cols*0.5, matchLoc.y + Character.rows);
}

int get_distance(Point& first_point, Point& next_point)
{
	int A = first_point.x - next_point.x;
	int B = first_point.y - (next_point.y + 50);
	return int(pow(pow(A, 2) + pow(B, 2), 0.5));
}

void jump(int&g_distance)
{
	char AA[50];
	int distance_ = g_distance * 1.35;
	if (distance_ > 1000)
		distance_ = 300;

	int rand_x = int(320 + rand() % 50); //加上随机数使得每次按压都是在点（320,410）-（370,460）之间
	int rand_y = int(410 + rand() % 50);
	sprintf_s(AA, "adb shell input swipe %d %d %d %d %d", rand_x, rand_y, rand_x, rand_y, distance_);
	//cout << AA << endl;
	cout << distance_ << endl<<endl;
	system(AA);
}

Point GetNextPoint(Mat& srcImage)
{
	Point point1;
	Point point2;
	cvtColor(srcImage, srcImage, CV_BGR2GRAY);
	GaussianBlur(srcImage, srcImage,Size(3, 3), 0);  //高斯滤波，降低噪声
	Canny(srcImage, srcImage, 10, 15);//Canny(srcImage, srcImage, 20, 30);      //进行边缘检测
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(srcImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point()); //找到关键的角点
																								//遍历每一个轮廓，把多余的轮廓去掉
	vector<vector<Point> >::const_iterator it = contours.begin();
	while (it != contours.end()) {
		if (it->size()<175)//150
			it = contours.erase(it);
		else
			++it;
	}
	int nYMin = srcImage.rows;
	int nXMin = srcImage.cols;
	int nYMax = 0;
	int nXMax = 0;
	int nIdY = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		//contours[i]代表的是第i个轮廓，contours[i].size()代表的是第i个轮廓上所有的像素点数  
		for (int j = 0; j < contours[i].size(); j++)
		{
			if (contours[i][j].y < nYMin)
			{
				nYMin = contours[i][j].y;   //找到最低的y值
				point1 = contours[i][j];    //记录  y值最低点坐标
				nIdY = i;                   //记录哪个区域内的
			}
		}
	}
	int minY = srcImage.cols;

	for (int j = 0; j < contours[nIdY].size(); j++) //在哪个区域内继续变量 找到x最大值
	{
		if (contours[nIdY][j].x >nXMax)
		{
			nXMax = contours[nIdY][j].x;
		}
	}
	for (int j = 0; j < contours[nIdY].size(); j++)
	{//找到x中最大值上的最小值
		if (contours[nIdY][j].x == nXMax && contours[nIdY][j].y < minY)
		{
			point2 = contours[nIdY][j];
			minY = contours[nIdY][j].y;     //记录X点的最大值
		}
	}
	return Point(point1.x, point2.y);       //返回中点坐标
}