#include <opencv2\opencv.hpp>
#include <iostream>
#include <math.h>
#include <Windows.h>
#include<stdio.h>
#include<stdlib.h>
using namespace cv;
using namespace std;
void get_screenshot();//��ȡ�ֻ��ϵ�ͼ��
Point get_Character_Loc(Mat& srcImage, Mat& Tem_img);//��ȡģ�������
Point GetNextPoint(Mat& srcImage);//��ȡ�¸�Ҫ���ĵ�
int get_distance(Point& finst_point, Point& next_point);//�������
void jump(int& g_distance);//��
void end();//to do
Mat srcImage;
Mat dstImage;
Mat Character;
//Mat ROI_img;
//Mat end_img;//����ģ��
//Mat dst_end;//����ƥ����ͼƬ

int num = 500;//���Ĵ���
//float res_end;
int main()
{
	//end_img = imread("end.png");//����ģ��ͼƬ
	
	while (num > 0)//����num�ν���
	{
		get_screenshot();
		srcImage = imread("d:/ADB/autojump.png");//��ȡԴͼƬ
		dstImage = srcImage.clone();
		Character = imread("object.png");//��ȡ����ģ��ͼƬ
		Point next_p = get_Character_Loc(srcImage, Character);//��ȡ���ӵ�����
		//-------------------------------------------------------------------
		//���ͼ��ĳߴ�
		//int colss, rowss;
		//colss = srcImage.cols;
		//rowss = srcImage.rows;
		//cout << colss << "   " << rowss << " " << endl;
		//1080    1920
		//-------------------------------------------------------------------
		//--------------------------------------------------------------------
		//���Ŀ�������
		//Mat ROI_img;
		//ROI_img = srcImage(Rect(srcImage.cols/5, srcImage.rows/3, (srcImage.cols/3)+50,(srcImage.rows/3)+50));
		//imshow(" ", ROI_img);
		//--------------------------------------------------------------------
		Point get_next = GetNextPoint(srcImage);//�����һ���������
		cout << "��һ��������꣺" << get_next << endl << endl;//����¸�λ������
		int g_distance = get_distance(next_p, get_next);//�������
		jump(g_distance);//��
		num--;
		Sleep(500);
	}
	waitKey(0);
	return 0;
}
void get_screenshot()//��ȡ��Ļͼ������Ӧ��ADB����
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

	int rand_x = int(320 + rand() % 50); //���������ʹ��ÿ�ΰ�ѹ�����ڵ㣨320,410��-��370,460��֮��
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
	GaussianBlur(srcImage, srcImage,Size(3, 3), 0);  //��˹�˲�����������
	Canny(srcImage, srcImage, 10, 15);//Canny(srcImage, srcImage, 20, 30);      //���б�Ե���
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(srcImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point()); //�ҵ��ؼ��Ľǵ�
																								//����ÿһ���������Ѷ��������ȥ��
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
		//contours[i]������ǵ�i��������contours[i].size()������ǵ�i�����������е����ص���  
		for (int j = 0; j < contours[i].size(); j++)
		{
			if (contours[i][j].y < nYMin)
			{
				nYMin = contours[i][j].y;   //�ҵ���͵�yֵ
				point1 = contours[i][j];    //��¼  yֵ��͵�����
				nIdY = i;                   //��¼�ĸ������ڵ�
			}
		}
	}
	int minY = srcImage.cols;

	for (int j = 0; j < contours[nIdY].size(); j++) //���ĸ������ڼ������� �ҵ�x���ֵ
	{
		if (contours[nIdY][j].x >nXMax)
		{
			nXMax = contours[nIdY][j].x;
		}
	}
	for (int j = 0; j < contours[nIdY].size(); j++)
	{//�ҵ�x�����ֵ�ϵ���Сֵ
		if (contours[nIdY][j].x == nXMax && contours[nIdY][j].y < minY)
		{
			point2 = contours[nIdY][j];
			minY = contours[nIdY][j].y;     //��¼X������ֵ
		}
	}
	return Point(point1.x, point2.y);       //�����е�����
}