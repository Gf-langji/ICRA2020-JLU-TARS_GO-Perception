#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <string>
#include <sstream>
// OpenCV includes
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/calib3d.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/gpu/gpu.hpp>

using namespace cv;
using namespace std;


	//ȫ�ֱ���

	//�任�����������
	Mat mask(3, 3, CV_64FC1);
	Mat org(3, 1, CV_64FC1);
	Mat res(3, 1, CV_64FC1);

	Mat org1(3, 1, CV_64FC1);
	Mat res1(3, 1, CV_64FC1);

	Mat mask2(3, 3, CV_64FC1);
	Mat org2(3, 1, CV_64FC1);
	Mat res2(3, 1, CV_64FC1);

	Mat org3(3, 1, CV_64FC1);
	Mat res3(3, 1, CV_64FC1);

	//�궨�㣬ʱ���
	int k = 0;
	static Point edge;
	Point a, b, c, d;
	double t, tc;

	Mat dst;

	/*double position_last_x = 0;
	double position_last_x1 = 0;
	double position_last_y = 0;
	double position_last_y1 = 0;
	double position_last_x2 = 0;
	double position_last_x3 = 0;
	double position_last_y2 = 0;
	double position_last_y3 = 0;*/

	//A��B����Ԥ��������
	Point predict_pt_A_1;
	Point predict_pt_B_1;
	Point predict_pt_A_2;
	Point predict_pt_B_2;

	double final_x_1_A = 0;
	double final_y_1_A = 0;
	double final_x_1_B = 0;
	double final_y_1_B = 0;
	double final_x_2_A = 0;
	double final_y_2_A = 0;
	double final_x_2_B = 0;
	double final_y_2_B = 0;

	//��С������ֵ
	int min_detection_distance = 90;

	static void onMouse1(int event, int x, int y, int, void* userInput)
	{
		Mat src, dst;
		dst.copyTo(src);
		if (event != EVENT_LBUTTONDOWN)
			return;
		// Get the pointer input image
		Mat* img = (Mat*)userInput;
		// Draw circle
		circle(*img, Point(x, y), 5, Scalar(0, 255, 0), 3);

		src.copyTo(dst);
		edge = Point(x * 1.25, y * 1.25);
		if (k >= 0 && k <= 3) {
			std::cout << "x:" << x << "y:" << y << endl;
		}

		src.copyTo(dst);//ȷ�����߲�������src�Ͻ���

		k = k + 1;
		if (k > 0) {
			if (k == 1) {
				a.x = edge.x;
				a.y = edge.y;
			}
			if (k == 2) {
				b.x = edge.x;
				b.y = edge.y;
			}
			if (k == 3) {
				c.x = edge.x;
				c.y = edge.y;
			}
			if (k == 4) {
				d.x = edge.x;
				d.y = edge.y;
			}
			if (k == 5) {
				//CvMat* mask = cvCreateMat(3, 3, CV_32FC1);
				Point2f camera_view[] = { a,b,c,d };
				Point2f god_view[] = { Point2f(808,448),Point2f(808,0),Point2f(0,448),Point2f(0,0) };
				//����任����
				mask = getPerspectiveTransform(camera_view, god_view);
				std::cout << mask << endl;
			}
		}
	}

	
	void main()
	{
		Mat dst;
		Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
		Mat distCoeffs = Mat::zeros(5, 1, CV_64F);
		Mat view, rview, map1, map2;
		Size imageSize;

		while (1) {
			//ͼ����
			cv::Mat src = imread("pics/test_image.jpg");
			resize(src, src, Size(1024,1024));
			//����������
			Mat frameCalibration;
			cameraMatrix.at<double>(0, 0) = 6.355809472341207e+02;
			cameraMatrix.at<double>(0, 1) = 0;
			cameraMatrix.at<double>(0, 2) = 5.048749733034192e+02;
			cameraMatrix.at<double>(1, 1) = 6.356741995280925e+02;
			cameraMatrix.at<double>(1, 2) = 4.825381259048260e+02;
			cameraMatrix.at<double>(2, 2) = 1;

			distCoeffs.at<double>(0, 0) = -0.264788000153760;
			distCoeffs.at<double>(1, 0) = 0.046394008673741;
			distCoeffs.at<double>(2, 0) = 0.004060476522896;
			distCoeffs.at<double>(3, 0) = 3.429612389369573e-04;
			distCoeffs.at<double>(4, 0) = 0;
			imageSize = Size(1024, 1024);
			initUndistortRectifyMap(cameraMatrix, distCoeffs, Mat(),
				getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 1, imageSize, 0),
				imageSize, CV_16SC2, map1, map2);
			remap(src, frameCalibration, map1, map2, INTER_LINEAR);
			resize(frameCalibration, frameCalibration, Size(src.rows*0.8, src.cols*0.8));
			frameCalibration.copyTo(dst);

			imshow("src", frameCalibration);

			setMouseCallback("src", onMouse1, &frameCalibration);
			//waitKey(30);
			char c = waitKey();
			//�궨��ɣ����������
			if (c == 27) {
				destroyWindow("src");
				break;
			};



		}
		//�������˲���ʼ��
		//RNG rng;
		////1.kalman filter setup
		//const int stateNum = 4;                                      //״ֵ̬4��1����(x,y,��x,��y)
		//const int measureNum = 2;                                    //����ֵ2��1����(x,y)	
		//KalmanFilter KF_A(stateNum, measureNum, 0);
		//KalmanFilter KF_B(stateNum, measureNum, 0);

		//KF_A.transitionMatrix = (Mat_<float>(4, 4) << 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1);  //ת�ƾ���A
		//setIdentity(KF_A.measurementMatrix);                                             //��������H
		//setIdentity(KF_A.processNoiseCov, Scalar::all(1e-5));                            //ϵͳ�����������Q
		//setIdentity(KF_A.measurementNoiseCov, Scalar::all(1e-1));                        //���������������R
		//setIdentity(KF_A.errorCovPost, Scalar::all(1));                                  //����������Э�������P
		//rng.fill(KF_A.statePost, RNG::UNIFORM, 0, 1024 > 1024 ? 1024 : 1024);   //��ʼ״ֵ̬x(0)
		//Mat measurementA = Mat::zeros(measureNum, 1, CV_32F);     //��ʼ����ֵx'(0)����Ϊ����Ҫ�������ֵ�����Ա����ȶ���

		//size_t m = 0;
		//��һλ�ü�¼
		/*double position_last_x = 10, position_last_x1 = 10;
		double position_last_y = 10, position_last_y1 = 10;*/
		final_x_1_A = 0;
		final_y_1_A = 0;
		final_x_1_B = 0;
		final_y_1_B = 0;

		double final_x_all = 0;
		double final_y_all = 0;
		double final_x_all1 = 0;
		double final_y_all1 = 0;

		int times = 0;
		int times1 = 0;

		//�������
		Mat frameCalibration1, one;
		cv::Mat src1 = imread("pics/test_image.jpg");
		resize(src1, src1, Size(1024, 1024));
		//resize(src1, src1, Size(1024, 1024));
		t = getTickCount();
		remap(src1, frameCalibration1, map1, map2, INTER_NEAREST);

		//imshow("�ӽ�1", frameCalibration1);
			//waitKey(30);
		/*vector<Mat> channels;
		split(frameCalibration1, channels);
		one = channels[2] - channels[0];*/
		//one = DistillationColor(frameCalibration1, 1);

		//threshold(one, one, 200, 255, THRESH_BINARY);

		//RGBͨ����ȡ
		inRange(frameCalibration1, Scalar(200, 200, 90), Scalar(255, 255, 150), one);
		dilate(one, dst, Mat());
		//dilate(dst, dst, Mat());
		//imshow("dst", dst);
		//imshow("fram", frameCalibration1);
		RotatedRect rect;
		RotatedRect rect1;
		vector<vector<Point>> contours;
		findContours(dst, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
		//int temp = 0;
		/*for (m; m < contours.size(); m++) {
			if (contourArea(contours[m]) > 10 && contourArea(contours[m]) < 100) {
				cout << "area:" << contourArea(contours[m])<< endl;
				cout << "ID:" << m << endl;
				break;
			}
		}*/
		for (size_t i = 0; i < contours.size(); i++) {
			double area = contourArea(contours[i]);

			//cout << "����ID:" << i << "   �����" << area << endl;
			//if (area < 10||area>100) continue;
			rect = minAreaRect(contours[i]);
			org = (Mat_<double>(3, 1) << rect.center.x, rect.center.y, 1);
			res = mask * org;
			double *x = res.ptr<double>(0, 0);
			double *y = res.ptr<double>(1, 0);
			double *z = res.ptr<double>(2, 0);
			//�任���������������
			double x_1 = *x / *z;
			double y_1 = *y / *z;
			if ((x_1 < 0) || (x_1 > 808) || (y_1 < 0) || (y_1 > 448)) continue;

			rect1 = minAreaRect(contours[0]);

			org1 = (Mat_<double>(3, 1) << rect1.center.x, rect1.center.y, 1);
			res1 = mask * org1;
			double *x2 = res1.ptr<double>(0, 0);
			double *y2 = res1.ptr<double>(1, 0);
			double *z2 = res1.ptr<double>(2, 0);
			double x_2 = *x2 / *z2;
			double y_2 = *y2 / *z2;
			//if ((x_2 < 0) || (x_2 > 808) || (y_2 < 0) || (y_2 > 448)) continue;
			//cout << x_2 << "  " << y_2 << endl;

			//����������
			double distance = sqrt((x_1 - x_2)*(x_1 - x_2) +
				(y_1 - y_2)*(y_1 - y_2));
			//cout << "������ࣺ" << distance << endl;

			//�������ࣨA����
			if (distance <= min_detection_distance) {
				//final_x_all_src = final_x_all_src + rect.center.x;
				//final_y_all_src = final_y_all_src + rect.center.y;
				final_x_all = final_x_all + x_1;
				final_y_all = final_y_all + y_1;
				times++;

			}
			//�������ࣨB����
			else {
				//final_x_all_src1 = final_x_all_src1 + rect.center.x;
				//final_y_all_src1 = final_y_all_src1 + rect.center.y;
				final_x_all1 = final_x_all1 + x_1;
				final_y_all1 = final_y_all1 + y_1;
				times1++;
			}

		}
		//A������
		if (times > 0) {

			//final_x_src = final_x_all_src / times;
			//final_y_src = final_y_all_src / times;
			final_x_1_A = final_x_all / times;
			final_y_1_A = final_y_all / times;

			//circle(src, Point(final_x_src, final_y_src), 5, Scalar(0, 255, 0));
			//cout << "-----------------------------------------" << endl;
			//cout << "A���� x:" << final_x << " y:" << final_y << endl;
			//imshow("src", src);

		}
		//B������
		if (times1 > 0) {

			//final_x_src1 = final_x_all_src1 / times1;
			//final_y_src1 = final_y_all_src1 / times1;
			final_x_1_B = final_x_all1 / times1;
			final_y_1_B = final_y_all1 / times1;

			//circle(src, Point(final_x_src1, final_y_src1), 5, Scalar(0, 255, 0));
			//cout << "-----------------------------------------" << endl;
			//cout << "B���� x:" << final_x1<< " y:" << final_y1 << endl;
			//imshow("src", src);
		}

		/*if (abs(position_last_x - final_x_1_A) > 10) {
			final_x_1_A = final_x_1_B;
			final_y_1_A = final_y_1_B;
			final_x_1_B = position_last_x;
			final_y_1_B = position_last_y;
		}*/

		//��ֹA,Bλ������������������
		if (final_x_1_A > 0 || final_x_1_B > 0) {

			//double two_car_distance = sqrt((final_x_1_A - final_x_1_B)*(final_x_1_A - final_x_1_B) +
				//(final_y_1_A - final_y_1_B));
			//cout << "two_car_distance:" << two_car_distance << endl;
			//if (position_last_x != 10 && position_last_y != 10) {
			//	double judgeDist = sqrt((position_last_x - final_x_1_A)*(position_last_x - final_x_1_A) +
			//		(position_last_y - final_y_1_A)*(position_last_y - final_y_1_A));
			//	if (judgeDist > 10) {
			//		final_x_1_A = final_x_1_B;
			//		final_y_1_A = final_y_1_B;
			//		final_x_1_B = position_last_x1;
			//		final_y_1_B = position_last_y1;
			//		//cout << "�ڸ�1��A�����꣺" << final_x_1_A << " " << final_y_1_A << endl;
			//		//cout << "�ڸ�1��B�����꣺" << final_x_1_B << " " << final_y_1_B << endl;
			//		cout << "Change successully" << endl;
			//	}

			//}
			////��һλ�ü�¼
			//position_last_x = final_x_1_A;
			//position_last_x1 = final_x_1_B;
			//position_last_y = final_y_1_A;
			//position_last_y1 = final_y_1_B;

			//kalman prediction
			//Mat prediction_A = KF_A.predict();
			//predict_pt_A_1 = Point(prediction_A.at<float>(0), prediction_A.at<float>(1));   //Ԥ��ֵ(x',y')

			//update measurement
			//measurementA.at<float>(0) = (float)final_x_1_A;
			//measurementA.at<float>(1) = (float)final_y_1_A;
			////update
			//KF_A.correct(measurementA);
				
			double timeconsume = (getTickCount() - t) / getTickFrequency();
			printf("�ڸ�1֡���� %.2f\n", 1 / timeconsume);
			Mat out;
			warpPerspective(frameCalibration1, out, mask, Size(808, 448));
			if (final_x_1_A > 0&& final_x_1_A <808) {
				circle(out, Point(final_x_1_A, final_y_1_A), 5, Scalar(0, 0, 255), 3);
				//circle(out, predict_pt_A_1, 5, Scalar(255, 0, 0), 3);
				cout << "�ڸ�1��A�����꣺" << final_x_1_A << " " << final_y_1_A << endl;
			}
			if (final_x_1_B > 0&& final_x_1_B<808) {
				circle(out, Point(final_x_1_B, final_y_1_B), 5, Scalar(0, 255, 0), 3);
				//circle(out, predict_pt_B_1, 5, Scalar(0, 255, 0), 3);
				cout << "�ڸ�1��B�����꣺" << final_x_1_B << " " << final_y_1_B << endl;
			}

			imshow("ȫ���ӽ�1", out);
			waitKey(0);
		}
	}


	
