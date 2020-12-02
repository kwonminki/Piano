#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <stdlib.h> 

using namespace std;
using namespace cv;
 
void TakePicture() {
	VideoCapture cap(0);
	if (!cap.isOpened())
	{
		cout << "Can't open!" << endl;
		return;
	}
	
	//cap.set(CAP_PROP_FRAME_WIDTH, 1280);
	//cap.set(CAP_PROP_FRAME_HEIGHT, 720);
	
	double width = cap.get(CAP_PROP_FRAME_WIDTH);
	double height = cap.get(CAP_PROP_FRAME_HEIGHT);
	printf("size = [%f, %f]\n", width, height);
	
	namedWindow("Video", 0);
	resizeWindow("Video", width, height);
	
	Mat frame;
	
	while (char(waitKey(1)) != 'q' && cap.isOpened())
	{
		cap >> frame;  // get a frame from captures
		if(frame.empty())
		{
			cout << "Video over!" << endl;
			break;
		}
		//Show frame
		imshow("Video", frame);
	}
	
	imwrite("/home/pi/darknet/data/piano.jpg", frame);
	
	cap.release();
	destroyWindow("Video");

	//int exe = system("raspistill -o /home/pi/darknet/data/test2.jpg -w 1280 -h 720 -t 1");
	
	cout << "done!" << endl;
	return;
}