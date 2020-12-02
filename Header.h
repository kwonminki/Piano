#pragma once
#include <string>
#include <opencv2/imgproc.hpp>

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
};