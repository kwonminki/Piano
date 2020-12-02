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
#include "Header_Init.h"

using namespace std;
using namespace cv;

extern map<int, string> label_to_tone;

PianoBoundary::PianoBoundary(string className) {
	//int ret = system("cd ~/darknet && ./darknet detector test data/obj.data yolo-obj.cfg backup/yolo-obj_4000.weights -ext_output data/piano.jpg > data/result2.txt");
	//cmd 명령어. tmp.txt에 사진경로 저장, result.txt에 해당 사진을 인식한 결과 출력.							-dont_show
	//darknet.exe detector test data/obj.data train/yolo-obj.cfg backup/yolo-obj_last.weights -dont_show -ext_output <data/tmp.txt> data/result.txt
	//darknet.exe detector train data/obj.data train/yolo-obj.cfg darknet53.conv.74 학습명령어
	string in_line;
	ifstream in2("/home/pi/darknet/data/result.txt");
	cout << "start" << endl;
	while (getline(in2, in_line)) {
		if (!in_line.find(className)) {
			string w;
			vector<string> word;
			for (stringstream sts(in_line); (sts >> w);) {
				word.push_back(w);
			}
			class_name = word.at(0).substr(0, word.at(0).length() - 1);
			x_point = stoi(word.at(3));
			y_point = stoi(word.at(5));
			width = stoi(word.at(7));
			height = stoi(word.at(9).substr(0, word.at(9).length() - 1));
		}
	}
}

string PianoBoundary::getClass_name() {
	return class_name;
}
int PianoBoundary::getX_center() {
	return x_point;
}
int PianoBoundary::getY_center() {
	return y_point;
}
int PianoBoundary::getWidth() {
	return width;
}
int PianoBoundary::getHeight() {
	return height;
}

MiddleResult::MiddleResult(Mat origin, Mat label, int x, int y, int w, int h) {
	origin_w = origin.cols;
	origin_h = origin.rows;
	labeled_image = label;
	detected_x_point = x;
	detected_y_point = y;
	detected_w = w;
	detected_h = h;
}
int MiddleResult::GiveMe_Label(int x, int y) {
	if (x<detected_x_point || x>detected_x_point + detected_w || y<detected_y_point || y>detected_y_point + detected_h) {
		cout << "error : point is not in labeled area!" << endl;
		return -1;
	}
	label_x = x - detected_x_point;
	label_y = y - detected_y_point;

	int result = labeled_image.at<int>(label_y, label_x);

	cout << "label is " << result << endl;
	if (label_to_tone.count(result)) {
		cout << "계이름은 " << label_to_tone.find(result)->second << endl;
	}
	else {
		cout << "계이름 안나옴" << endl;
		result = -1;
	}
	return result;
}

vector<Point> contoursConvexHull(vector<vector<Point> > contours)
{
	vector<Point> result;
	vector<Point> pts;
	for (size_t i = 0; i < contours.size(); i++)
		for (size_t j = 0; j < contours[i].size(); j++)
			pts.push_back(contours[i][j]);
	convexHull(pts, result);
	return result;
}

void FourNeighbor(int x, int y, Mat* gray, Mat* label, int* num) {

	if (label->at<int>(y, x) != label->at<int>(y + 1, x)) {
		if ((int)gray->at<uchar>(y + 1, x) != 0) {
			label->at<int>(y + 1, x) = label->at<int>(y, x);
			if (y == label->rows - 2) return;
			*num = *num + 1;
			FourNeighbor(x, y + 1, gray, label, num);
		}
	}

	if (label->at<int>(y, x) != label->at<int>(y - 1, x)) {
		if ((int)gray->at<uchar>(y - 1, x) != 0) {
			label->at<int>(y - 1, x) = label->at<int>(y, x);
			if (y == 1) return;
			*num = *num + 1;
			FourNeighbor(x, y - 1, gray, label, num);
		}
	}

	if (label->at<int>(y, x) != label->at<int>(y, x + 1)) {
		if ((int)gray->at<uchar>(y, x + 1) != 0) {
			label->at<int>(y, x + 1) = label->at<int>(y, x);
			if (x == label->cols - 2) return;
			*num = *num + 1;
			FourNeighbor(x + 1, y, gray, label, num);
		}
	}

	if (label->at<int>(y, x) != label->at<int>(y, x - 1)) {
		if ((int)gray->at<uchar>(y, x - 1) != 0) {
			label->at<int>(y, x - 1) = label->at<int>(y, x);
			if (x == 1) return;
			*num = *num + 1;
			FourNeighbor(x - 1, y, gray, label, num);
		}
	}

	return;
}