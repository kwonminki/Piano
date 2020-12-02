#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include "Header_Init.h"

using namespace std;
using namespace cv;
using namespace cv::dnn;

extern vector<Result> result_of_process;
extern Mat labels;
extern vector<int> sorted_by_x_labels;
extern map<int, string> label_to_tone;

const int POSE_PAIRS[20][2] =
{
	{ 0, 1 },
	{ 1, 2 },
	{ 2, 3 },
	{ 3, 4 },
	          // thumb
	{ 0, 5 },
	{ 5, 6 },
	{ 6, 7 },
	{ 7, 8 }, 
	         // index
	{ 0, 9 },
	{ 9, 10 },
	{ 10, 11 },
	{ 11, 12 },
	     // middle
	{ 0, 13 },
	{ 13, 14 },
	{ 14, 15 },
	{ 15, 16 }, 
	  // ring
	{ 0, 17 },
	{ 17, 18 },
	{ 18, 19 },
	{ 19, 20 }   // small
};

string protoFile = "/home/pi/Piano/pose_deploy.prototxt";
string weightsFile = "/home/pi/Piano/pose_iter_102000.caffemodel";

int nPoints = 22;

void Finger(int image_num, MiddleResult* middleresult)
{
	string imageFile = to_string(image_num) + ".jpg";
	
	string address = "/home/pi/Piano/fingerPicturse/output/" + imageFile;
	
	cout << "USAGE : ./handPoseImage <imageFile> " << endl;

	imageFile = "/home/pi/Piano/fingerPicturse/"+ imageFile;
	// Take arguments from commmand line

	float thresh = 0.01;

	Mat origin_image = imread(imageFile);
	int x_for_split = origin_image.cols / 2;
	
	vector <vector <Point>> Fingers_vector;
	vector <Point> left;
	vector <Point> right;

	for (int count = 0; count < 2; count++) {
		Mat frame;
		if (count == 0) {
			Rect rect(0, 0, x_for_split, origin_image.rows);
			frame = origin_image(rect);
		}
		else if (count == 1) {
			Rect rect(x_for_split, 0, origin_image.cols - x_for_split, origin_image.rows);
			frame = origin_image(rect);
		}
        
		Mat frameCopy = frame.clone();
		int frameWidth = frame.cols;
		int frameHeight = frame.rows;

		float aspect_ratio = frameWidth / (float)frameHeight;
		int inHeight = 368;
		int inWidth = (int(aspect_ratio * inHeight) * 8) / 8;

		cout << "inWidth = " << inWidth << " ; inHeight = " << inHeight << endl;

		double t = (double)cv::getTickCount();
		Net net = readNetFromCaffe(protoFile, weightsFile);

		Mat inpBlob = blobFromImage(frame, 1.0 / 255, Size(inWidth, inHeight), Scalar(0, 0, 0), false, false);

		net.setInput(inpBlob);

		Mat output = net.forward();

		int H = output.size[2];
		int W = output.size[3];

		// find the position of the body parts
		vector<Point> points(nPoints);
		for (int n = 0; n < nPoints; n++)
		{
			// Probability map of corresponding body's part.
			Mat probMap(H, W, CV_32F, output.ptr(0, n));
			resize(probMap, probMap, Size(frameWidth, frameHeight));

			Point maxLoc;
			double prob;
			minMaxLoc(probMap, 0, &prob, 0, &maxLoc);
			if (prob > thresh)
			{
				circle(frameCopy, cv::Point((int)maxLoc.x, (int)maxLoc.y), 8, Scalar(0, 255, 255), -1);
				cv::putText(frameCopy, cv::format("%d", n), cv::Point((int)maxLoc.x, (int)maxLoc.y), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 0, 255), 2);

			}
			points[n] = maxLoc;
		}

		int nPairs = sizeof(POSE_PAIRS) / sizeof(POSE_PAIRS[0]);

		for (int n = 0; n < nPairs; n++)
		{
			// lookup 2 connected body/hand parts
			Point2f partA = points[POSE_PAIRS[n][0]];
			Point2f partB = points[POSE_PAIRS[n][1]];

			if (partA.x <= 0 || partA.y <= 0 || partB.x <= 0 || partB.y <= 0)
				continue;

			line(frame, partA, partB, Scalar(0, 255, 255), 8);
			circle(frame, partA, 8, Scalar(0, 0, 255), -1);
			circle(frame, partB, 8, Scalar(0, 0, 255), -1);
		}

		if (count == 0) {
			for (int i = 0; i < 5; i++) {
				left.push_back(points[POSE_PAIRS[3 + (4 * i)][1]]);
			}
			int n = 5;
			for (int i = n - 1; i > 0; i--) {
				for (int j = 0; j < i; j++) {
					if (left.at(j).x < left.at(j + 1).x) {
						Point tmp = left.at(j);
						left.at(j) = left.at(j + 1);
						left.at(j + 1) = tmp;
					}
				}          
			}
            
			cout << "left" << endl;
			for (int i = 0; i < 5; i++) {
				circle(frame, left.at(i), 8, Scalar(i * 50, count * 255, 0), -1);
				cout << i << " is " << left.at(i) << endl;
			}

		}
		else {
			for (int i = 0; i < 5; i++) {
				right.push_back(points[POSE_PAIRS[3 + (4 * i)][1]]);
			}
			int n = 5;
			for (int i = n - 1; i > 0; i--) {
				for (int j = 0; j < i; j++) {
					if (right.at(j).x > right.at(j + 1).x) {
						Point tmp = right.at(j);
						right.at(j) = right.at(j + 1);
						right.at(j + 1) = tmp;
					}
				}
			}

			cout << "right" << endl;
			for (int i = 0; i < 5; i++) {
				circle(frame, right.at(i), 8, Scalar(i * 50, count * 255, 0), -1);
				right.at(i).x += x_for_split;
				cout << i << " is " << right.at(i) << endl;
			}
		}
		
		//imshow("Output-Keypoints" + to_string(count), frameCopy);
		//imshow("Output-Skeleton" + to_string(count), frame);
    		
		t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		cout << "Time Taken = " << t << endl;

	}
	
	/////here
bool flag_isfind = false;
	for (int i = 0; i < 5; i++)
	{
		if (middleresult->GiveMe_Label(right.at(i).x, right.at(i).y) == sorted_by_x_labels.at(result_of_process.at(image_num).key_num)) {
			result_of_process.at(image_num).finger_number = i+1;
			result_of_process.at(image_num).key = label_to_tone[sorted_by_x_labels.at(result_of_process.at(image_num).key_num)];
			result_of_process.at(image_num).LorR = 1;
			flag_isfind = true;
			break;
		}
	}
	if (!flag_isfind)
	{
		//없음.. 최대한 가까운거 오른손
		cout << "none right" << endl;
	}
	flag_isfind = false;
	for (int i = 0; i < 5; i++)
	{
		if (middleresult->GiveMe_Label(left.at(i).x, left.at(i).y) == sorted_by_x_labels.at(result_of_process.at(image_num).key_num)) {
			result_of_process.at(image_num).finger_number = i+1;
			result_of_process.at(image_num).key = label_to_tone[sorted_by_x_labels.at(result_of_process.at(image_num).key_num)];
			result_of_process.at(image_num).LorR = 0;
			flag_isfind = true;
			break;
		}
	}
	if (!flag_isfind)
	{
		//없음.. 최대한 가까운거 왼손
		cout << "none left" << endl;
	}

	imwrite(address, origin_image);

	//waitKey();

	return;
}
