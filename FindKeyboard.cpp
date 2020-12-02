#include "Header_Init.h"
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

using namespace cv;
using namespace std;

extern map<int, string> label_to_tone;
extern Mat labels;
extern vector<int> sorted_by_x_labels;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	MiddleResult *temp = (MiddleResult*)userdata;

	if (event == EVENT_LBUTTONDOWN)
	{
		cout << "왼쪽 마우스 버튼 클릭.. 좌표 = (" << x << ", " << y << ")" << endl;
		temp->GiveMe_Label(x, y);
	}
}

MiddleResult FindKeyboard()
{
	Mat origin_image;
	origin_image = imread("/home/pi/darknet/data/piano_.jpg", IMREAD_COLOR);
	if (origin_image.empty()) {
		cout << "Could not open or find the image" << endl;
	}

	PianoBoundary test("piano");    //선언시 클래스 이름 입력.

	int x1, y1, w1, h1;

	x1 = test.getX_center();
	y1 = test.getY_center();
	w1 = test.getWidth();
	h1 = test.getHeight();
	
	if (origin_image.cols < x1 + w1) {
		w1 = origin_image.cols - x1;
	}
	if (origin_image.rows < y1 + h1) {
		h1 = origin_image.rows - y1 - 1;
	}

	Rect bounds(0, 0, origin_image.cols, origin_image.rows);
	Rect rect(x1, y1, w1, h1);
	Mat piano = origin_image(rect & bounds);
	
	Mat gray_for_labels;

	cvtColor(piano, piano, COLOR_BGRA2GRAY);    //그레이스케일로 변환

	Mat canny_image;
	Canny(piano, canny_image, 30, 90, 3);    //50, 150 케니로 선땀.

	vector<Vec4i> lines;
	HoughLinesP(canny_image, lines, 1, CV_PI / 180, 10, 0, 0);

	for (int i = 0; i < lines.size(); i++)
	{
		Vec4i L = lines[i];
		line(piano,
			Point(L[0], L[1]),
			Point(L[2], L[3]),
			Scalar(0, 0, 255),
			2,
			LINE_8);
	}
	
	medianBlur(piano, piano, 3);
	
	Mat canny_2;
	Canny(piano, canny_2, 30, 90, 3);
	
	Mat binary_image;
	threshold(piano, binary_image, 140, 230, THRESH_BINARY);
	
	Mat opening;
	Mat element(4, 4, CV_8U, cv::Scalar(1));
	morphologyEx(binary_image, opening, MORPH_OPEN, element);
	
	gray_for_labels = opening;

	vector<vector<Point> > contours;

	findContours(opening, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	Mat drawing = Mat::zeros(opening.size(), CV_8UC3);

	for (int i = 0; i < contours.size(); i++)
	{
		Scalar color = Scalar(255, 255, 255);
		drawContours(drawing, contours, i, color, 2);
	}

	vector<Point> ConvexHullPoints = contoursConvexHull(contours);

	polylines(drawing, ConvexHullPoints, true, Scalar(255, 255, 255), 3);    //흰색 선 그려짐


	bitwise_not(drawing, drawing);

	medianBlur(drawing, drawing, 3);

	imshow("drawing", drawing);
	
	cvtColor(drawing, drawing, COLOR_RGB2GRAY);

	Mat stats, centroids;

	Mat image_color;
	cvtColor(drawing, image_color, COLOR_GRAY2BGR);

	int nlabels = connectedComponentsWithStats(drawing, labels, stats, centroids);

	vector<int> label_number;    //넓이 400이상인 라벨번호만 들어가는 벡터

	for(int i = 0 ; i < nlabels ; i++) {
		if (i < 2) {
			continue;
		}

		int area = stats.at<int>(i, CC_STAT_AREA);
		int center_x = centroids.at<double>(i, 0);
		int center_y = centroids.at<double>(i, 1);
		int left = stats.at<int>(i, CC_STAT_LEFT);
		int top = stats.at<int>(i, CC_STAT_TOP);
		int width = stats.at<int>(i, CC_STAT_WIDTH);
		int height = stats.at<int>(i, CC_STAT_HEIGHT);
		//이 밑에 area 비교값을 카메라 부착 후 바꿔줘야함
		if(area > 180) {
			putText(drawing, to_string(i), Point(center_x, center_y), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 2);
			label_number.push_back(i);
		}
	}

	vector<int>::iterator iter_for_test;
	for (int i = 0; i < label_number.size(); i++) {
		iter_for_test = find(label_number.begin(), label_number.end(), labels.at<int>(0, 0));
		if (iter_for_test != label_number.end()) {
			iter_for_test = label_number.erase(iter_for_test);
		}
	}
	for (int i = 0; i < label_number.size(); i++) {
		iter_for_test = find(label_number.begin(), label_number.end(), labels.at<int>(labels.rows - 1, 0));
		if (iter_for_test != label_number.end()) {
			iter_for_test = label_number.erase(iter_for_test);
		}
	}
	for (int i = 0; i < label_number.size(); i++) {
		iter_for_test = find(label_number.begin(), label_number.end(), labels.at<int>(labels.rows - 1, labels.cols - 1));
		if (iter_for_test != label_number.end()) {
			iter_for_test = label_number.erase(iter_for_test);
		}
	}
	for (int i = 0; i < label_number.size(); i++) {
		iter_for_test = find(label_number.begin(), label_number.end(), labels.at<int>(0, labels.cols - 1));
		if (iter_for_test != label_number.end()) {
			iter_for_test = label_number.erase(iter_for_test);
		}
	} //끝점들을 제외시켜줌 -> 확인해야함.
	
	double y_mean = 0;
	for (int i = 0; i < label_number.size(); i++) {
		y_mean += centroids.at<double>(i, 1);
	}
	y_mean = y_mean / label_number.size();
	//y좌표의 평균값을 구해 저장하였다.

	vector<int> temp_label(label_number);
	for (int i = 0; i < label_number.size(); i++) {
		double max = 0.0;
		int index = 0;
		int temp_j = 0;
		for (int j = 0; j < label_number.size(); j++) {
			double x_value = centroids.at<double>(label_number.at(j), 0);
			if (x_value > max && temp_label.at(j) != -1) {
				max = x_value;
				index = label_number.at(j);
				temp_j = j;
			}
		}
		sorted_by_x_labels.push_back(index);
		temp_label.at(temp_j) = -1;
	}

	reverse(sorted_by_x_labels.begin(), sorted_by_x_labels.end());
	//이 코드에서 sorted_by_x_labels에 인덱스가 왼쪽 건반부터 차례대로 들어가있다.

	sorted_by_x_labels.erase(sorted_by_x_labels.begin() + 58);
	sorted_by_x_labels.erase(sorted_by_x_labels.begin() + 52);
	sorted_by_x_labels.erase(sorted_by_x_labels.begin() + 32);
	sorted_by_x_labels.erase(sorted_by_x_labels.begin() + 26);
	sorted_by_x_labels.erase(sorted_by_x_labels.begin() + 13);
	sorted_by_x_labels.erase(sorted_by_x_labels.begin() + 5);
	//sorted_by_x_labels.erase(sorted_by_x_labels.begin() + 18);
	//sorted_by_x_labels.erase(sorted_by_x_labels.begin() + 12);
		
	vector<double> y_compare;
	for (int i = 0; i < sorted_by_x_labels.size(); i++) {
		double j = centroids.at<double>(sorted_by_x_labels.at(i), 1);
		y_compare.push_back(j);
	}

	vector<int> y_index;
	for (int i = 0; i < y_compare.size() - 1; i++) {
		if (y_compare.at(i) > y_mean && y_compare.at(i + 1) > y_mean) {
			y_index.push_back(i);
		}
	}

	int term = 8;
	int i = 0;
	bool me = false;

	cout << "checkpoint_y_index" << endl;
	
	if (y_index.at(0) + 5 == y_index.at(1)) {
		term = 5;
		me = false;
	}
	else if (y_index.at(0) + 7 == y_index.at(1)) {
		term = 7;
		me = true;
	}
	else {
		cout << "error262" << endl;
	}

	while (i < y_index.size() - 1) {
		if (y_index.at(i) + term != y_index.at(i + 1)) {
			cout << "error266 : " << i << endl;
			break;
		}
		else {
			term = term == 5 ? 7 : 5;
		}
		i++;
	}

	if (me) {
		//인덱스0번에 들어가있는 놈이 미의 인덱스이다.
	   cout << y_index.at(0) << "is 미" << endl;
		cout << sorted_by_x_labels[y_index.at(0)] << "is 미" << endl;
	}
	else {
		//저놈이 시이다. 에러일수도 있다. 에러262면 뭔가 잘못된거다.
	   cout << y_index.at(0) << "is 시" << endl;
	}

	vector<int> black;
	vector<int> white;

	int me_4_si_11 = me ? 4 : 11;

	for (int i = 0; i < sorted_by_x_labels.size(); i++) {

		int num;
		string tone;

		if (i < y_index.at(0) - me_4_si_11) {
			num = 0;
		}
		else {
			num = (i - (y_index.at(0) - me_4_si_11)) / 12 + 1;
		}

		switch ((i - (y_index.at(0) - me_4_si_11)) % 12) {
		case 0:
			tone = "C" + to_string(num); break;
		case 1:
			tone = "C" + to_string(num) + "#"; black.push_back(sorted_by_x_labels[i]); break;
		case 2:
			tone = "D" + to_string(num); white.push_back(sorted_by_x_labels[i]); break;
		case 3:
			tone = "D" + to_string(num) + "#"; black.push_back(sorted_by_x_labels[i]); break;
		case 4:
			tone = "E" + to_string(num); white.push_back(sorted_by_x_labels[i]); break;
		case 5:
			tone = "F" + to_string(num); white.push_back(sorted_by_x_labels[i]); break;
		case 6:
			tone = "F" + to_string(num) + "#"; black.push_back(sorted_by_x_labels[i]); break;
		case 7:
			tone = "G" + to_string(num); white.push_back(sorted_by_x_labels[i]); break;
		case 8:
			tone = "G" + to_string(num) + "#"; black.push_back(sorted_by_x_labels[i]); break;
		case 9:
			tone = "A" + to_string(num); white.push_back(sorted_by_x_labels[i]); break;
		case 10:
			tone = "A" + to_string(num) + "#"; black.push_back(sorted_by_x_labels[i]); break;
		case 11:
			tone = "B" + to_string(num); white.push_back(sorted_by_x_labels[i]); break;
		}

		label_to_tone.insert(make_pair(sorted_by_x_labels[i], tone));

	}
	
	int flag = 0;
	
	for (int i = 0; i < sorted_by_x_labels.size(); i++) {

		for (int y = 0; y < labels.rows; ++y) {

			int* label = labels.ptr<int>(y);
			Vec3b* pixel = image_color.ptr<Vec3b>(y);


			for (int x = 0; x < labels.cols; ++x) {

				if (label[x] == sorted_by_x_labels.at(i) && flag == 0) {
					pixel[x][2] = 0;
					pixel[x][1] = 255;
					pixel[x][0] = 0;
				}
				else if (label[x] == sorted_by_x_labels.at(i) && flag == 1) {
					pixel[x][2] = 255;
					pixel[x][1] = 0;
					pixel[x][0] = 0;
				}
				else if (label[x] == sorted_by_x_labels.at(i) && flag == 2) {
					pixel[x][2] = 0;
					pixel[x][1] = 0;
					pixel[x][0] = 255;
				}
			}
		}
		flag++;
		if (flag == 3) {
			flag = 0;
		}
	}
	
	int result = -1;
	
	vector<int>::iterator iter;
	for (int i = 0; i < labels.cols; i++) {
		for (int j = 0; j < labels.rows; j++) {
			int tmp_num = labels.at<int>(j, i);
			iter = find(sorted_by_x_labels.begin(), sorted_by_x_labels.end(), tmp_num);
			if (iter == sorted_by_x_labels.end()) {
				labels.at<int>(j, i) = 0;
			}
		}
	} //sorted_by_x에 없는 애들은 다 0으로 만들었다. => 얘네를 바꿀거다.
	
	vector<int> null_cols;

	for (int i = 0; i < labels.cols; i++) {
		for (int j = 1; j < labels.rows - 1; j++) {
			if (labels.at<int>(j, i) == 0) {
				int up_count = 0;
				int down_count = 0;
				bool is_up = false;
				bool is_down = false;

				int k = j;
				for (; k > 0; k--) {
					if (labels.at<int>(k, i) != 0) {
						is_up = true;
						break;
					}
					up_count++;
				}

				int h = j;
				for (; h < labels.rows; h++) {
					if (labels.at<int>(h, i) != 0) {
						is_down = true;
						break;
					}
					down_count++;
				}

				if (is_up && is_down) {
					if (up_count >= down_count) {
						labels.at<int>(j, i) = labels.at<int>(h, i);
					}
					else {
						labels.at<int>(j, i) = labels.at<int>(k, i);
					}
				}
				else if (is_up) {
					labels.at<int>(j, i) = labels.at<int>(k, i);
				}
				else if (is_down) {
					labels.at<int>(j, i) = labels.at<int>(h, i);
				}
				else {
					//한 줄이 전체가 0인 경우이다. 코드 작성 안해놓음
					null_cols.push_back(i);
					break;
				}

			}
		}
	}
	
	for (int i = 0; i < null_cols.size() - 1; i++) {
		if (null_cols.at(i + 1) != null_cols.at(i) + 1) {
			null_cols.erase(null_cols.begin(), null_cols.begin() + i + 1);
			break;
		}
	}
	for (int i = null_cols.size() - 1; i > 1; i--) {
		if (null_cols.at(i - 1) != null_cols.at(i) - 1) {
			null_cols.erase(null_cols.begin() + i, null_cols.end());
			break;
		}
	}

	for (int k = 0; k < null_cols.size() - 1; k++) {
		for (int j = 0; j < labels.rows; j++) {
			labels.at<int>(j, null_cols.at(k)) = labels.at<int>(j, null_cols.at(k) - 1);
		}
	} // 이제 모든 픽셀들은 내가 원하는 라벨 번호만 가지고 있다. 문제는 검정 건반이 밑으로 쭉 내려온 부분을 어떻게 해결하는지 이다.

	Mat test_for_canny;
	Canny(gray_for_labels, test_for_canny, 30, 90, 3);

	Mat hough_result;
	cvtColor(test_for_canny, hough_result, COLOR_GRAY2BGR);

	vector<Vec2f> lines_1;
	vector<Vec2f> lines_2;
	HoughLines(test_for_canny, lines_1, 1.2, CV_PI / 180, 150, 0, 3 * CV_PI / 180);
	//HoughLines(test_for_canny, lines_2, 1, CV_PI / 180, 150, 150 * CV_PI / 180, CV_PI);

	vector<Vec2f> hough_result_lines;

	for (size_t i = 0; i < lines_1.size(); i++)
	{
		float rho = lines_1[i][0], theta = lines_1[i][1];
		if (rho < labels.rows * 0.7 && rho > labels.rows * 0.2) {
			hough_result_lines.push_back(lines_1[i]);
			Point pt1, pt2;
			double a = cos(theta), b = sin(theta);
			double x0 = a * rho, y0 = b * rho;
			pt1.x = cvRound(x0 + 1000 * (-b));
			pt1.y = cvRound(y0 + 1000 * (a));
			pt2.x = cvRound(x0 - 1000 * (-b));
			pt2.y = cvRound(y0 - 1000 * (a));
			cout << y0 << ", " << pt1.y << endl;
			line(hough_result, pt1, pt2, Scalar(0, 255, (255*(i-1))), 3, LINE_AA);
		}
	}
	
	cout << "black " << endl;
	for (int i = 0; i < black.size(); i++) {
		cout << black.at(i) << ", ";
	}
	cout << endl;
	cout << "white" << endl;
	for (int i = 0; i < white.size(); i++) {
		cout << white.at(i) << ", ";
	}
	cout << endl;
	
	if (hough_result_lines.size() != 1) {
		// 조건에 맞는 선분이 하나가 아니다. => 하나로 바꿔줘야한다. + 없는 경우도 코딩해놔야한다.
		//라즈베리파이로 돌려보고 고쳐야함.
		hough_result_lines.erase(hough_result_lines.begin() + 1);
		
	}
	if (hough_result_lines.size() == 1) {
		float rho = hough_result_lines[0][0], theta = hough_result_lines[0][1];
		double a = cos(theta), b = sin(theta);
		//cout << rho / b << " is y절편, " << rho * b << "is y0 " << endl;
		for (int i = 0; i < labels.cols; i++) {
			for (int j = 0; j < labels.rows; j++) {
				iter = find(black.begin(), black.end(), labels.at<int>(j, i));
				if (iter != black.end()) {
					double what_is_value = -(a / b) * i + (double)rho / b;
					if (j > what_is_value) {
						//직선 아래 검은 건반 상태
						if(i < labels.cols / 2) {
							//왼쪽을 가져온다.
							for(int tmp_num = i ; tmp_num > 0 ; tmp_num--) {
								vector<int>::iterator iter2;
								iter2 = find(white.begin(), white.end(), labels.at<int>(j, tmp_num));
								if (iter2 != white.end()) {
									//cout << labels.at<int>(j, i) << ", " << labels.at<int>(j, tmp_num) << endl;
									labels.at<int>(j, i) = labels.at<int>(j, tmp_num);
									break;
								}

							}
						}
						else {
							//오른쪽을 가져온다.
							for(int tmp_num = i ; tmp_num < labels.cols ; tmp_num++) {
								vector<int>::iterator iter2;
								iter2 = find(white.begin(), white.end(), labels.at<int>(j, tmp_num));
								if (iter2 != white.end()) {
									//cout << labels.at<int>(j, i) << ", " << labels.at<int>(j, tmp_num) << endl;
									labels.at<int>(j, i) = labels.at<int>(j, tmp_num);
									break;
								}
							}
						}
					}
				}
			}
		}
	}

	
	//imshow("hough_result", hough_result);
	//imshow("gray", gray_for_labels);
	//imshow("Contours", drawing);
	//imshow("image_color", image_color);
	
	MiddleResult middle(origin_image, labels, x1, y1, w1, h1);
	namedWindow("click", WINDOW_AUTOSIZE);
	
	Mat real_image;
	real_image = imread("/home/pi/darknet/data/piano.jpg", IMREAD_COLOR);
	
	//imshow("click", real_image);
	MiddleResult *p = &middle;
	//setMouseCallback("click", CallBackFunc, (void*)p);

	//waitKey(0);

	return middle;
}


