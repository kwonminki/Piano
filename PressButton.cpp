#include "Header_Init.h"
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <stdlib.h> 

using namespace std;
using namespace cv;

extern int Picturenum;
extern vector<Result> result_of_process;
extern VideoCapture* cap;

void WhenRising(int keynum) //눌렀을 때
{
	Mat frame;
	/*
	VideoCapture capture = *cap;
	
	if (!capture.isOpened()) {
		//카메라가 개방 되지 않으면 종료
		cout << "Can not open capture!!" << endl;
		return;
	}
	
	capture >> frame;
	string filename = "/home/pi/Piano/fingerPicturse/" + to_string(Picturenum) + ".jpg";
	imwrite(filename, frame);
	*/
	Result tmp(keynum, clock());
	result_of_process.push_back(tmp);
	
	ofstream output("/home/pi/Piano/fingerPicturse/output/output.txt", ios::app);
	output << Picturenum << ", " << "0" << ", " << keynum << ", " << clock() << ", " << "=======================" << endl;
	output.close();
	
	Picturenum++;
}

void WhenFalling(int keynum)
{
	/*if (Picturenum == 0)return;
	for (int i = Picturenum; i > 0; i--)
	{
		if (result_of_process.at(i - 1).key_num == keynum)
		{
			result_of_process.at(i - 1).End(clock());
			
			ofstream output("/home/pi/Piano/fingerPicturse/output/output.txt", ios::app);
			output <<  result_of_process.at(i).key << ", " << result_of_process.at(i).LorR << ", " << result_of_process.at(i).key_num << ", " << result_of_process.at(i).start << ", " << result_of_process.at(i).end << ", " << result_of_process.at(i).result << ", " << result_of_process.at(i).finger_number << endl;
			output.close();
			
			break;
		}
	}
	*/
	ofstream output("/home/pi/Piano/fingerPicturse/output/output.txt", ios::app);
	output << Picturenum << ", " << clock() << ", " << "<<<<<fall>>>>>" << endl;
	output.close();
	
	
	//cout << "no result" << endl;
}