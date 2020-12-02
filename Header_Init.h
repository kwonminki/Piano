#pragma once
#include <stdlib.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <string>
#include <iostream>
#include <list>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

using namespace std;
using namespace cv;

class PianoBoundary {
private:
	string class_name;
	int x_point;
	int y_point;
	int width;
	int height;
public:
	PianoBoundary(string className);
	string getClass_name();
	int getX_center();
	int getY_center();
	int getWidth();
	int getHeight();
};

class MiddleResult {
private:
	int detected_x_point;
	int detected_y_point;
	int detected_w;
	int detected_h;
	int origin_w;
	int origin_h;
	Mat labeled_image;
	int label_x, label_y;
public:
	MiddleResult(Mat origin, Mat label, int x, int y, int w, int h);
	int GiveMe_Label(int x, int y);
};

class Result
{
public:
	int finger_number;
	int key_num;
	string key;
	clock_t start;
	clock_t end;
	double result;
	int LorR;
	Result(int keynum, clock_t startclock);
	void End(clock_t endclock);
};

MiddleResult FindKeyboard();

vector<Point> contoursConvexHull(vector<vector<Point> > contours);

void FourNeighbor(int x, int y, Mat* gray, Mat* label, int* num);

void TakePicture();

void FingerTracker();

void Finger(int image_num, MiddleResult* middleresult);

void InterruptInit();

void WhenRising(int key_num);

void WhenFalling(int key_num);