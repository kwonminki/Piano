#include <stdlib.h>
#include "Header_Init.h"
#include <wiringPi.h>
#include <iostream>
#include <fstream>

using namespace std;

map<int, string> label_to_tone;
Mat labels;
int Picturenum;
vector<Result> result_of_process;
VideoCapture* cap;
clock_t before;
vector<int> sorted_by_x_labels;


int main()
{
	Picturenum = 0;
	//TakePicture();
	MiddleResult middleresult = FindKeyboard();
	before = clock();
	InterruptInit();
	VideoCapture cap2(0);
	cap = &cap2;
	double width = cap2.get(CAP_PROP_FRAME_WIDTH);
	double height = cap2.get(CAP_PROP_FRAME_HEIGHT);
	
	namedWindow("screen", 0);
	resizeWindow("screen", width, height);
	
	if (!cap2.isOpened())
	{
		cout << "Can't open!" << endl;
	}
	Mat frame;
	
	
	while (char(waitKey(1)) != 27 && cap2.isOpened()) {
		cap2 >> frame;
		imshow("screen", frame);			
	}
	cap2.release();
	for (int i = 0; i < Picturenum; i++) {
//		Finger(i, &middleresult);
		ofstream output("/home/pi/Piano/fingerPicturse/output/output.txt", ios::app);
		output <<  result_of_process.at(i).key << ", " << result_of_process.at(i).LorR << ", " << result_of_process.at(i).key_num << ", " << result_of_process.at(i).start << ", " << result_of_process.at(i).end << ", " << result_of_process.at(i).result << ", " << result_of_process.at(i).finger_number << endl;
		output.close();
	}
	
	cout << "done.";
}