#include "opencv2/opencv.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

#include "BackgroundRemover.h"
#include "SkinDetector.h"
#include "FingerCount.h"

using namespace std;
using namespace cv;
 
Mat make_mask_image(Mat img_bgr);
int getMaxAreaContourId(vector <vector<cv::Point>> contours);
void getFingerPosition(vector<Point> max_contour, Mat img_result, bool debug);

void FingerTracker() {
	VideoCapture videoCapture(0);
	videoCapture.set(CAP_PROP_SETTINGS, 1);
	
	if (!videoCapture.isOpened())
	{
		cout << "Can't open!" << endl;
		return;
	}
	
	videoCapture.set(CAP_PROP_FRAME_WIDTH, 980);
	videoCapture.set(CAP_PROP_FRAME_HEIGHT, 640);
	
	Mat frame, frameOut, handMask, foreground, fingerCountDebug;

	BackgroundRemover backgroundRemover;
	SkinDetector skinDetector;
	//FaceDetector faceDetector;
	FingerCount fingerCount;
		
	while (true) {
		videoCapture >> frame;
		frameOut = frame.clone();

		skinDetector.drawSkinColorSampler(frameOut);
		foreground = backgroundRemover.getForeground(frame);
		handMask = skinDetector.getSkinMask(foreground);
		fingerCountDebug = fingerCount.findFingersCount(handMask, frameOut);

		imshow("output", frameOut);
		imshow("foreground", foreground);
		imshow("handMask", handMask);
		imshow("handDetection", fingerCountDebug);
		
		int key = waitKey(1);

		if (key == 27) // esc
			break;
		else if (key == 98) // b
			backgroundRemover.calibrate(frame);
		else if (key == 115) // s
			skinDetector.calibrate(frame);
	}

	
	videoCapture.release();

	return;
}