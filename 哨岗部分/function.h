#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <string>
#include <sstream>
// OpenCV includes
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include<opencv2/calib3d.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/gpu/gpu.hpp>

//���߳�
#include<thread>
//#include"Armor_detection.h"
#include "FlyCapture2.h"
#include<winsock2.h>
#pragma comment(lib, "ws2_32.lib")//���ؾ�̬�⣬socket

using namespace cv;
using namespace FlyCapture2;
using namespace std;


namespace watchdog {
	struct PoseForSend {
		double x_A, y_A, x_B, y_B;
	};
	//ȫ�ֱ���

	//�任�����������
	Camera *pCameras;

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
	Point predict_pt_A_1 ;
	Point predict_pt_B_1 ;
	Point predict_pt_A_2 ;
	Point predict_pt_B_2 ;

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
	//�����Ϣ
	void PrintBuildInfo()
	{
		FC2Version fc2Version;
		Utilities::GetLibraryVersion(&fc2Version);

		ostringstream version;
		version << "FlyCapture2 library version: " << fc2Version.major << "."
			<< fc2Version.minor << "." << fc2Version.type << "."
			<< fc2Version.build;
		std::cout << version.str() << endl;

		ostringstream timeStamp;
		timeStamp << "Application build date: " << __DATE__ << " " << __TIME__;
		std::cout << timeStamp.str() << endl << endl;
	}
	void PrintError(FlyCapture2::Error error) { error.PrintErrorTrace(); }
	//λ�÷���
	void send_param(const char * ip) {

		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);

		SOCKET servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		//FlyCapture2::Error error;
		sockaddr_in sockAddr;
		memset(&sockAddr, 0, sizeof(sockaddr_in));
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_addr.s_addr = inet_addr(ip);
		sockAddr.sin_port = htons(2000);
		std:cout << "bind below" << std::endl;
		while(SOCKET_ERROR == ::bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR))) {
			std::cout << "bind error." << std::endl;
			Sleep(1000);
		};	
		std::cout << "Listening " << std::endl;
		listen(servSock, 10);

		SOCKADDR clntAddr;
		int nsize = sizeof(clntAddr);
		SOCKET clntSock;
		fd_set writes;
		timeval timeout{1, 0};
		conn:	std::cout << "accepting here ..." << std::endl;
		clntSock = ::accept(servSock, (SOCKADDR*)&clntAddr, &nsize);
		std::cout << "connected " << std::endl;
		FD_ZERO(&writes);
		FD_SET(clntSock, &writes);

		PoseForSend pose;
		int errorCount = 0;
		pose.x_A = -1;
		pose.x_B = -1;
		pose.y_A = -1;
		pose.y_B = -1;

		while (1) {
			try {
			
				FD_ZERO(&writes);
				FD_SET(clntSock, &writes);

				//���ڸھ�˫��	
				if (final_x_1_A != 0 && final_x_2_A != 0&& final_x_1_B != 0&& final_x_2_B != 0) {
					pose.x_A = (predict_pt_A_1.x + predict_pt_A_2.x) / 2;
					pose.y_A = (predict_pt_A_1.y + predict_pt_A_2.y) / 2;
					pose.x_B = (predict_pt_B_1.x + predict_pt_B_2.x) / 2;
					pose.y_B = (predict_pt_B_1.y + predict_pt_B_2.y) / 2;
					cout << "���ڸھ�˫��" << endl;
				
				}
				//1�ڸ���˫����2�ڸ��е���
				if (final_x_1_A != 0 && final_x_2_A != 0&& final_x_1_B != 0&& final_x_2_B == 0) {
					
					double distA = sqrt((predict_pt_A_1.x - predict_pt_A_2.x)*(predict_pt_A_1.x - predict_pt_A_2.x)
						+ (predict_pt_A_1.y - predict_pt_A_2.y)*(predict_pt_A_1.y - predict_pt_A_2.y));
					
					double distB = sqrt((predict_pt_B_1.x - predict_pt_A_2.x)*(predict_pt_B_1.x - predict_pt_A_2.x)
						+ (predict_pt_B_1.y - predict_pt_A_2.y)*(predict_pt_B_1.y - predict_pt_A_2.y));
					
					if (distA > distB) {
						pose.x_A = predict_pt_A_1.x;
						pose.y_A = predict_pt_A_1.y;
						pose.x_B = (predict_pt_B_1.x + predict_pt_A_2.x) / 2;
						pose.y_B = (predict_pt_B_1.y + predict_pt_A_2.y) / 2;
					}
					else{
						pose.x_A = (predict_pt_A_1.x + predict_pt_A_2.x) / 2;
						pose.y_A = (predict_pt_A_1.y + predict_pt_A_2.y) / 2;
						pose.x_B = predict_pt_B_1.x ;
						pose.y_B = predict_pt_B_1.y ;
					}
					cout << "1�ڸ���˫����2�ڸ��е���" << endl;
				}
				//1�ڸ��е�����2�ڸ���˫��
				if (final_x_1_A != 0 && final_x_2_A != 0 && final_x_2_B != 0 && final_x_1_B == 0) {

					double distA = sqrt((predict_pt_A_1.x - predict_pt_A_2.x)*(predict_pt_A_1.x - predict_pt_A_2.x)
						+ (predict_pt_A_1.y - predict_pt_A_2.y)*(predict_pt_A_1.y - predict_pt_A_2.y));

					double distB = sqrt((predict_pt_A_1.x - predict_pt_B_2.x)*(predict_pt_A_1.x - predict_pt_B_2.x)
						+ (predict_pt_A_1.y - predict_pt_B_2.y)*(predict_pt_A_1.y - predict_pt_B_2.y));

					if (distA > distB) {
						pose.x_A = predict_pt_A_2.x;
						pose.y_A = predict_pt_A_2.y;
						pose.x_B = (predict_pt_A_1.x + predict_pt_B_2.x) / 2;
						pose.y_B = (predict_pt_A_1.y + predict_pt_B_2.y) / 2;
					}
					else {
						pose.x_A = (predict_pt_A_1.x + predict_pt_A_2.x) / 2;
						pose.y_A = (predict_pt_A_1.y + predict_pt_A_2.y) / 2;
						pose.x_B = predict_pt_B_2.x;
						pose.y_B = predict_pt_B_2.y;
					}
					cout << "1�ڸ��е�����2�ڸ���˫��" << endl;
				}
				//���ڸھ�����	
				if (final_x_1_A != 0 && final_x_2_A != 0&& final_x_1_B ==0&& final_x_2_B ==0) {
					double distA = sqrt((predict_pt_A_1.x - predict_pt_A_2.x)*(predict_pt_A_1.x - predict_pt_A_2.x)
						+ (predict_pt_A_1.y - predict_pt_A_2.y)*(predict_pt_A_1.y - predict_pt_A_2.y));
					if (distA < 100) {
						pose.x_A = (predict_pt_A_1.x + predict_pt_A_2.x) / 2;
						pose.y_A = (predict_pt_A_1.y + predict_pt_A_2.y) / 2;
						pose.x_B = -1;
						pose.y_B = -1;

					}
					else{
						pose.x_A = predict_pt_A_1.x;
						pose.y_A = predict_pt_A_1.y;
						pose.x_B = predict_pt_A_2.x;
						pose.y_B = predict_pt_A_2.y;

					}
					cout << "���ڸھ�����" << endl;
				}

				//��1�ڸ�	
				if (final_x_1_A != 0 && final_x_1_B != 0 && final_x_2_A == 0 && final_x_2_B == 0) {
					pose.x_A = predict_pt_A_1.x ;
					pose.y_A = predict_pt_A_1.y ;
					pose.x_B = predict_pt_B_1.x ;
					pose.y_B = predict_pt_B_1.y ;
					cout << "��1�ڸ�	" << endl;
				
				}
				//��2�ڸ�	
				if (final_x_2_A != 0 && final_x_2_B != 0 && final_x_1_A == 0 && final_x_1_B == 0) {
					pose.x_A = predict_pt_A_2.x;
					pose.y_A = predict_pt_A_2.y;
					pose.x_B = predict_pt_B_2.x;
					pose.y_B = predict_pt_B_2.y;
					cout << "��2�ڸ�	" << endl;
				}


				/*
				pose.x_A++;
				pose.y_A++;
				pose.x_B++;
				pose.y_B++;
				*/

				
				//std::cout << "accepting ..." << std::endl;
				///clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nsize);
				switch (::select(2, NULL, &writes, NULL, &timeout)) {
					case -1: std::cout << "select error" << std::endl;
						closesocket(clntSock);
						goto conn;
						//break;
					case 0:
						std::cout << 0 << std::endl;
						closesocket(clntSock);
						goto conn;
						continue;
					default:
						if (FD_ISSET(clntSock, &writes)) {
							std::cout << "x1:" << pose.x_A << "y1:" << pose.y_A << std::endl
								<< "x2:" << pose.x_B << "y2:" << pose.y_B << std::endl;
							Sleep(20);
							int r = ::send(clntSock, (char*)&pose, sizeof(PoseForSend), NULL);
							// std::cout << r << std::endl;
							if (r <= 0) {
								std::cout << "send error" << r << std::endl;
								goto conn;
							}
							// std::cout << "send once" << std::endl;
							//Sleep(2000);
 						}
				}
				//cout << "send successful" << endl;
			}
			catch (std::exception& e) {
				errorCount++;
				if (errorCount >= 10) {
					printf("error occurs while sending.\nnode exit.");
					errorCount = 0;
					break;
				}
				continue;
			}
		}

		closesocket(servSock);
		closesocket(clntSock);

		WSACleanup();
	}
	//�˹��궨���������ң������������α궨��
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
	static void onMouse2(int event, int x, int y, int, void* userInput)
	{
		Mat src, dst;
		dst.copyTo(src);
		if (event != EVENT_LBUTTONDOWN)
			return;
		// Get the pointer input image
		Mat* img = (Mat*)userInput;
		// Draw circle
		circle(*img, Point(x, y), 5, Scalar(0, 255, 0), 3);


		edge = Point(x * 1.25, y * 1.25);
		if (k >= 5 && k <= 8) {
			std::cout << "x:" << x << "y:" << y << endl;
		}
		//putText(src, temp_1, Point(x, y), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(150, 55, 245));
		//imshow("src", src);
		src.copyTo(dst);//ȷ�����߲�������src�Ͻ���

		k = k + 1;
		if (k > 0) {
			if (k == 6) {
				a.x = edge.x;
				a.y = edge.y;
			}
			if (k == 7) {
				b.x = edge.x;
				b.y = edge.y;
			}
			if (k == 8) {
				c.x = edge.x;
				c.y = edge.y;
			}
			if (k == 9) {
				d.x = edge.x;
				d.y = edge.y;
			}
			if (k == 10) {
				//CvMat* mask = cvCreateMat(3, 3, CV_32FC1);
				Point2f camera_view[] = { a,b,c,d };
				Point2f god_view[] = { Point2f(0,0),Point2f(0,448),Point2f(808,0),Point2f(808,448) };
				mask2 = getPerspectiveTransform(camera_view, god_view);
				std::cout << mask2 << endl;
			}
		}

	}

	//�߳�1���캯��
	void watchdog_1(void)
	{
		Mat dst;

		FlyCapture2::Error error;
		Image image;
		Image image2;
		Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
		Mat distCoeffs = Mat::zeros(5, 1, CV_64F);
		Mat view, rview, map1, map2;
		Size imageSize;
		
		while (1) {
				
				error = pCameras[0].RetrieveBuffer(&image);

				if (error != PGRERROR_OK)
				{
					PrintError(error);
					delete[] pCameras;
					std::cout << "Press Enter to exit." << endl;
					cin.ignore();
				}

				error = image.Convert(PIXEL_FORMAT_BGR, &image2);

				unsigned int rowBytes = (double)image2.GetDataSize() / (double)image2.GetRows();
				//ͼ����
				cv::Mat src = cv::Mat(image2.GetRows(), image2.GetCols(), CV_8UC3, image2.GetData(), rowBytes);
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
		RNG rng;
		//1.kalman filter setup
		const int stateNum = 4;                                      //״ֵ̬4��1����(x,y,��x,��y)
		const int measureNum = 2;                                    //����ֵ2��1����(x,y)	
		KalmanFilter KF_A(stateNum, measureNum, 0);
		KalmanFilter KF_B(stateNum, measureNum, 0);

		KF_A.transitionMatrix = (Mat_<float>(4, 4) << 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1);  //ת�ƾ���A
		setIdentity(KF_A.measurementMatrix);                                             //��������H
		setIdentity(KF_A.processNoiseCov, Scalar::all(1e-5));                            //ϵͳ�����������Q
		setIdentity(KF_A.measurementNoiseCov, Scalar::all(1e-1));                        //���������������R
		setIdentity(KF_A.errorCovPost, Scalar::all(1));                                  //����������Э�������P
		rng.fill(KF_A.statePost, RNG::UNIFORM, 0, 1024 > 1024 ? 1024 : 1024);   //��ʼ״ֵ̬x(0)
		Mat measurementA = Mat::zeros(measureNum, 1, CV_32F);     //��ʼ����ֵx'(0)����Ϊ����Ҫ�������ֵ�����Ա����ȶ���

		KF_B.transitionMatrix = (Mat_<float>(4, 4) << 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1);  //ת�ƾ���A
		setIdentity(KF_B.measurementMatrix);                                             //��������H
		setIdentity(KF_B.processNoiseCov, Scalar::all(1e-5));                            //ϵͳ�����������Q
		setIdentity(KF_B.measurementNoiseCov, Scalar::all(1e-1));                        //���������������R
		setIdentity(KF_B.errorCovPost, Scalar::all(1));                                  //����������Э�������P
		rng.fill(KF_B.statePost, RNG::UNIFORM, 0, 1024 > 1024 ? 1024 : 1024);   //��ʼ״ֵ̬x(0)	
		Mat measurementB = Mat::zeros(measureNum, 1, CV_32F);
		//double points_x[10];
		//double points_y[10];
		size_t m = 0;
		//��һλ�ü�¼
		double position_last_x = 10, position_last_x1 = 10;
		double position_last_y = 10, position_last_y1 = 10;
		
		while (1) {
			try {
				t = getTickCount();
				//���������
				error = pCameras[0].RetrieveBuffer(&image);

				if (error != PGRERROR_OK)
				{
					PrintError(error);
					delete[] pCameras;
					std::cout << "Press Enter to exit." << endl;
					cin.ignore();
				}

				error = image.Convert(PIXEL_FORMAT_BGR, &image2);
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
				unsigned int rowBytes = (double)image2.GetDataSize() / (double)image2.GetRows();
				error = image.Convert(PIXEL_FORMAT_BGR, &image2);
				
				//�������
				Mat frameCalibration1, one;
				cv::Mat src1 = cv::Mat(image2.GetRows(), image2.GetCols(), CV_8UC3, image2.GetData(), rowBytes);
				//resize(src1, src1, Size(1024, 1024));
				remap(src1, frameCalibration1, map1, map2, INTER_NEAREST);
			
				//imshow("�ӽ�1", frameCalibration1);
					//waitKey(30);
				/*vector<Mat> channels;
				split(frameCalibration1, channels);
				one = channels[2] - channels[0];*/
				//one = DistillationColor(frameCalibration1, 1);

				//threshold(one, one, 200, 255, THRESH_BINARY);

				//RGBͨ����ȡ
				inRange(frameCalibration1, Scalar(45, 55, 90), Scalar(67, 102, 255), one);
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
						times ++;

					}
					//�������ࣨB����
					else {
						//final_x_all_src1 = final_x_all_src1 + rect.center.x;
						//final_y_all_src1 = final_y_all_src1 + rect.center.y;
						final_x_all1 = final_x_all1 + x_1;
						final_y_all1 = final_y_all1 + y_1;
						times1 ++;
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
				if (final_x_1_A > 0 || final_x_1_B > 0 ) {

					//double two_car_distance = sqrt((final_x_1_A - final_x_1_B)*(final_x_1_A - final_x_1_B) +
						//(final_y_1_A - final_y_1_B));
					//cout << "two_car_distance:" << two_car_distance << endl;
					if (position_last_x!=10&& position_last_y!=10) {
						double judgeDist = sqrt((position_last_x - final_x_1_A)*(position_last_x - final_x_1_A) +
							(position_last_y - final_y_1_A)*(position_last_y - final_y_1_A));
						if (judgeDist > 10) {
							final_x_1_A = final_x_1_B;
							final_y_1_A = final_y_1_B;
							final_x_1_B = position_last_x1;
							final_y_1_B = position_last_y1;
							//cout << "�ڸ�1��A�����꣺" << final_x_1_A << " " << final_y_1_A << endl;
							//cout << "�ڸ�1��B�����꣺" << final_x_1_B << " " << final_y_1_B << endl;
							cout << "Change successully" << endl;
						}

					}
					//��һλ�ü�¼
					position_last_x = final_x_1_A;
					position_last_x1 = final_x_1_B;
					position_last_y = final_y_1_A;
					position_last_y1 = final_y_1_B;

					//kalman prediction
					Mat prediction_A = KF_A.predict();
					predict_pt_A_1 = Point(prediction_A.at<float>(0), prediction_A.at<float>(1));   //Ԥ��ֵ(x',y')

					//update measurement
					measurementA.at<float>(0) = (float)final_x_1_A;
					measurementA.at<float>(1) = (float)final_y_1_A;
					//update
					KF_A.correct(measurementA);

					Mat prediction_B = KF_B.predict();
					predict_pt_B_1 = Point(prediction_B.at<float>(0), prediction_B.at<float>(1));   //Ԥ��ֵ(x',y')

					//update measurement
					measurementB.at<float>(0) = (float)final_x_1_B;
					measurementB.at<float>(1) = (float)final_y_1_B;
					//update
					KF_B.correct(measurementB);
					double timeconsume = (getTickCount() - t) / getTickFrequency();
					printf("�ڸ�1֡���� %.2f\n", 1 / timeconsume);
					/*Mat out;
					warpPerspective(frameCalibration1, out, mask, Size(808, 448));
					if (final_x_1_A > 0&& final_x_1_A <808) {
						circle(out, Point(final_x_1_A, final_y_1_A), 5, Scalar(0, 0, 255), 3);
						circle(out, predict_pt_A_1, 5, Scalar(255, 0, 0), 3);
						cout << "11�ڸ�1��A�����꣺" << final_x_1_A << " " << final_y_1_A << endl;
					}
					if (final_x_1_B > 0&& final_x_1_B<808) {
						circle(out, Point(final_x_1_B, final_y_1_B), 5, Scalar(0, 0, 255), 3);
						circle(out, predict_pt_B_1, 5, Scalar(255, 0, 0), 3);
						cout << "11�ڸ�1��B�����꣺" << final_x_1_B << " " << final_y_1_B << endl;
					}
					
					imshow("ȫ���ӽ�1", out);
					waitKey(30);*/
				}
				

			}
			catch (std::exception& e) {
				continue;
			}
		}

		
	}
	//�߳�2���캯��
	void watchdog_2(void)
	{		
			Mat dst;
			FlyCapture2::Error error;
			Image image;
			Image image2;
			Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
			Mat distCoeffs = Mat::zeros(5, 1, CV_64F);
			Mat view, rview, map1, map2;
			Size imageSize;
		while (1) {
			
				
				error = pCameras[1].RetrieveBuffer(&image);
				if (error != PGRERROR_OK)
				{
					PrintError(error);
					delete[] pCameras;
					std::cout << "Press Enter to exit." << endl;
					cin.ignore();
				}

				error = image.Convert(PIXEL_FORMAT_BGR, &image2);
				unsigned int rowBytes = (double)image2.GetDataSize() / (double)image2.GetRows();


				cv::Mat src0 = cv::Mat(image2.GetRows(), image2.GetCols(), CV_8UC3, image2.GetData(), rowBytes);
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

				remap(src0, frameCalibration, map1, map2, INTER_NEAREST);
				resize(frameCalibration, frameCalibration, Size(frameCalibration.rows*0.8, frameCalibration.cols*0.8));

				frameCalibration.copyTo(dst);

				imshow("src0", frameCalibration);


				setMouseCallback("src0", onMouse2, &frameCalibration);
				char c = waitKey();
				if (c == 27) {
					destroyWindow("src0");
					break;
				};
		

		}

		RNG rng;
		//1.kalman filter setup
		const int stateNum = 4;                                      //״ֵ̬4��1����(x,y,��x,��y)
		const int measureNum = 2;                                    //����ֵ2��1����(x,y)	
		KalmanFilter KF_A(stateNum, measureNum, 0);
		KalmanFilter KF_B(stateNum, measureNum, 0);

		KF_A.transitionMatrix = (Mat_<float>(4, 4) << 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1);  //ת�ƾ���A
		setIdentity(KF_A.measurementMatrix);                                             //��������H
		setIdentity(KF_A.processNoiseCov, Scalar::all(1e-5));                            //ϵͳ�����������Q
		setIdentity(KF_A.measurementNoiseCov, Scalar::all(1e-1));                        //���������������R
		setIdentity(KF_A.errorCovPost, Scalar::all(1));                                  //����������Э�������P
		rng.fill(KF_A.statePost, RNG::UNIFORM, 0, 1024 > 1024 ? 1024 : 1024);   //��ʼ״ֵ̬x(0)
		Mat measurementA = Mat::zeros(measureNum, 1, CV_32F);     //��ʼ����ֵx'(0)����Ϊ����Ҫ�������ֵ�����Ա����ȶ���

		KF_B.transitionMatrix = (Mat_<float>(4, 4) << 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1);  //ת�ƾ���A
		setIdentity(KF_B.measurementMatrix);                                             //��������H
		setIdentity(KF_B.processNoiseCov, Scalar::all(1e-5));                            //ϵͳ�����������Q
		setIdentity(KF_B.measurementNoiseCov, Scalar::all(1e-1));                        //���������������R
		setIdentity(KF_B.errorCovPost, Scalar::all(1));                                  //����������Э�������P
		rng.fill(KF_B.statePost, RNG::UNIFORM, 0, 1024 > 1024 ? 1024 : 1024);   //��ʼ״ֵ̬x(0)	
		Mat measurementB = Mat::zeros(measureNum, 1, CV_32F);
		size_t m = 0;
		double position_last_x = 0;
		double position_last_x1 = 0;
		double position_last_y = 0;
		double position_last_y1 = 0;
	
		while (1) {
			try {
				//std::cout << "2" << endl;

				t = getTickCount();

				error = pCameras[1].RetrieveBuffer(&image);
				if (error != PGRERROR_OK)
				{
					PrintError(error);
					delete[] pCameras;
					std::cout << "Press Enter to exit." << endl;
					cin.ignore();
				}
				error = image.Convert(PIXEL_FORMAT_BGR, &image2);

			

				final_x_2_A = 0;
				final_y_2_A = 0;
				final_x_2_B = 0;
				final_y_2_B = 0;

				double final_x_all2 = 0;
				double final_y_all2 = 0;
				double final_x_all3 = 0;
				double final_y_all3 = 0;

				int times2 = 0;
				int times3 = 0;

				unsigned int rowBytes = (double)image2.GetDataSize() / (double)image2.GetRows();
				Mat frameCalibration2, one;
				cv::Mat src2 = cv::Mat(image2.GetRows(), image2.GetCols(), CV_8UC3, image2.GetData(), rowBytes);
				remap(src2, frameCalibration2, map1, map2, INTER_NEAREST);
				//imshow("�ӽ�2", frameCalibration2);
				//waitKey(30);
				//Mat dst2;
				/*vector<Mat> channels;
				split(frameCalibration2, channels);
				one = channels[2] - channels[0];*/
				//one = DistillationColor(frameCalibration2, 1);
				//threshold(one, one, 200, 255, THRESH_BINARY);
				inRange(frameCalibration2, Scalar(45, 55, 90), Scalar(67, 102, 255), one);
				dilate(one, dst, Mat());

				//dilate(dst, dst, Mat());
				//imshow("dst2", one);
				RotatedRect rect2;
				RotatedRect rect3;
				vector<vector<Point>> contours;
				findContours(dst, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
				/*for (m; m < contours.size(); m++) {
					if (contourArea(contours[m]) > 10 && contourArea(contours[m]) < 100) {
						cout << "area2:" << contourArea(contours[m]) << endl;
						cout << "ID2:" << m << endl;
						break;
					}
				}*/
				for (size_t i = 0; i < contours.size(); i++) {
					double area = contourArea(contours[i]);

					//if (area < 10 || area>100) continue;
					//cout << "����ID:" << i << "   �����" << area << endl;
					rect2 = minAreaRect(contours[i]);
					org2 = (Mat_<double>(3, 1) << rect2.center.x, rect2.center.y, 1);
					res2 = mask2 * org2;
					double *x = res2.ptr<double>(0, 0);
					double *y = res2.ptr<double>(1, 0);
					double *z = res2.ptr<double>(2, 0);
					double x_1 = *x / *z;
					double y_1 = *y / *z;
					if ((x_1 < 0) || (x_1 > 808) || (y_1 < 0) || (y_1 >448 )) continue;

					rect3 = minAreaRect(contours[0]);
					org3 = (Mat_<double>(3, 1) << rect3.center.x, rect3.center.y, 1);
					res3 = mask2 * org3;
					double *x2 = res3.ptr<double>(0, 0);
					double *y2 = res3.ptr<double>(1, 0);
					double *z2 = res3.ptr<double>(2, 0);
					double x_2 = *x2 / *z2;
					double y_2 = *y2 / *z2;
					//if ((x_2 < 0) || (x_2 > 808) || (y_2 < 0) || (y_2 > 448)) continue;
					//cout << x_2 << "  " << y_2 << endl;
					double distance = sqrt((x_1 - x_2)*(x_1 - x_2) +
						(y_1 - y_2)*(y_1 - y_2));
					//cout << "������ࣺ" << distance << endl;
					if (distance <= min_detection_distance ) {
						//final_x_all_src = final_x_all_src + rect.center.x;
						//final_y_all_src = final_y_all_src + rect.center.y;
						final_x_all2 = final_x_all2 + x_1;
						final_y_all2 = final_y_all2 + y_1;
						times2++;

					}

					else {
						//final_x_all_src1 = final_x_all_src1 + rect.center.x;
						//final_y_all_src1 = final_y_all_src1 + rect.center.y;
						final_x_all3 = final_x_all3 + x_1;
						final_y_all3 = final_y_all3 + y_1;
						times3++;
					}


					//std::cout << "�ڸ�1���:" << x_1 << "   " << y_1 << endl;

				}
				if (times2 > 0) {

					//final_x_src = final_x_all_src / times;
					//final_y_src = final_y_all_src / times;
					final_x_2_B = final_x_all2 / times2;
					final_y_2_B = final_y_all2 / times2;
					//
					//circle(src, Point(final_x_src, final_y_src), 5, Scalar(0, 255, 0));
					//cout << "-----------------------------------------" << endl;
					//cout << "A���� x:" << final_x << " y:" << final_y << endl;
					//imshow("src", src);
				}
				if (times3 > 0) {

					//final_x_src1 = final_x_all_src1 / times1;
					//final_y_src1 = final_y_all_src1 / times1;
					final_x_2_A = final_x_all3 / times3;
					final_y_2_A = final_y_all3 / times3;
					//times3 = 0;
					//circle(src, Point(final_x_src1, final_y_src1), 5, Scalar(0, 255, 0));
					//cout << "-----------------------------------------" << endl;
					//cout << "B���� x:" << final_x1<< " y:" << final_y1 << endl;
					//imshow("src", src);
				}



				/*if (position_last_x2 - final_x_2_A > 10) {
					final_x_2_A = final_x_2_B;
					final_y_2_A = final_y_2_B;
					final_x_2_B = position_last_x2;
					final_y_2_B = position_last_y2;
					//2.kalman prediction

				}*/
			if (final_x_2_A > 0 || final_x_2_B > 0) {
				if (position_last_x != 0 && position_last_y != 0) {
					double judgeDist = sqrt((position_last_x - final_x_2_A)*(position_last_x - final_x_2_A) +
						(position_last_y - final_y_2_A)*(position_last_y - final_y_2_A));
					if (judgeDist > 10) {
						final_x_2_A = final_x_2_B;
						final_y_2_A = final_y_2_B;
						final_x_2_B = position_last_x1;
						final_y_2_B = position_last_y1;
					
						cout << "Change successully" << endl;
					}

				}
				position_last_x = final_x_2_A;
				position_last_x1 = final_x_2_B;
				position_last_y = final_y_2_A;
				position_last_y1 = final_y_2_B;
				//cout << "after:"<<position_last_x << "  " << final_x_2_A << endl;

				Mat prediction_A = KF_A.predict();
				predict_pt_A_2 = Point(prediction_A.at<float>(0), prediction_A.at<float>(1));   //Ԥ��ֵ(x',y')

				//3.update measurement
				measurementA.at<float>(0) = final_x_2_A;
				measurementA.at<float>(1) = final_y_2_A;
				//4.update
				KF_A.correct(measurementA);

				Mat prediction_B = KF_B.predict();
				predict_pt_B_2 = Point(prediction_B.at<float>(0), prediction_B.at<float>(1));   //Ԥ��ֵ(x',y')

				//3.update measurement
				measurementB.at<float>(0) = final_x_2_B;
				measurementB.at<float>(1) = final_y_2_B;
				//4.update
				KF_B.correct(measurementB);

				double timeconsume = (getTickCount() - t) / getTickFrequency();
				printf("                     �ڸ�2֡���� %.2f\n", 1 / timeconsume);
				//���ӻ�
				/*Mat out;
				warpPerspective(frameCalibration2, out, mask2, Size(808, 448));
				//resize(out, out, Size(out.cols, out.rows));
				if (final_x_2_A > 0) {
					circle(out, Point(final_x_2_A, final_y_2_A), 5, Scalar(0, 0, 255), 3);
					circle(out, predict_pt_A_2, 5, Scalar(255, 0, 0), 3);
					//cout << "22�ڸ�2��A�����꣺" << predict_pt_A_2.x << " " << predict_pt_A_2.y << endl;
				}
				if (final_x_2_B > 0) {
					circle(out, Point(final_x_2_B, final_y_2_B), 5, Scalar(0, 0, 255), 3);
					circle(out, predict_pt_B_2, 5, Scalar(255, 0, 0), 3);
					//cout << "22�ڸ�2��B�����꣺" << predict_pt_B_2.x << " " << predict_pt_B_2.y << endl;
				}
				//cout << "�ڸ�2��A�����꣺" << predict_pt_A_2.x << " " << predict_pt_A_2.y << endl;
				//cout << "�ڸ�2��B�����꣺" << predict_pt_B_2.x << " " << predict_pt_B_2.y << endl;

				/*if (final_x_2_A > 0) {

				}
				if (final_x_2_B > 0) {

				}*/
				//resize(frameCalibration2, frameCalibration2, Size(500, 500));
				//imshow("�ڸ��ӽ�2", frameCalibration2);
				//imshow("ȫ���ӽ�2", out);
				//waitKey(30);
			}
		}
			catch (std::exception& e) {
				continue;
			}

		}
	}
	//�����ʼ��
	int cam_initialize() {

		PrintBuildInfo();

		const int k_numImages = 50;
		FlyCapture2::Error error;

		//
		// Initialize BusManager and retrieve number of cameras detected
		//
		BusManager busMgr;
		unsigned int numCameras;
		error = busMgr.GetNumOfCameras(&numCameras);
		if (error != PGRERROR_OK)
		{
			PrintError(error);
			return -1;
		}

		std::cout << "Number of cameras detected: " << numCameras << endl;

		//
		// Check to make sure at least two cameras are connected before
		// running example
		//
		if (numCameras < 2)
		{
			std::cout << "Insufficient number of cameras." << endl;
			std::cout << "Make sure at least two cameras are connected for example to "
				"run."
				<< endl;
			std::cout << "Press Enter to exit." << endl;
			cin.ignore();
			return -1;
		}

		//
		// Initialize an array of cameras
		//
		// *** NOTES ***
		// The size of the array is equal to the number of cameras detected.
		// The array of cameras will be used for connecting, configuring,
		// and capturing images.
		//
		pCameras = new Camera[numCameras];

		//
		// Prepare each camera to acquire images
		//
		// *** NOTES ***
		// For pseudo-simultaneous streaming, each camera is prepared as if it
		// were just one, but in a loop. Notice that cameras are selected with
		// an index. We demonstrate pseduo-simultaneous streaming because true
		// simultaneous streaming would require multiple process or threads,
		// which is too complex for an example.
		//
		for (unsigned int i = 0; i < numCameras; i++)
		{
			PGRGuid guid;
			error = busMgr.GetCameraFromIndex(i, &guid);
			if (error != PGRERROR_OK)
			{
				PrintError(error);
				std::cout << "Press Enter to exit." << endl;
				delete[] pCameras;
				cin.ignore();
				return -1;
			}

			// Connect to a camera
			error = pCameras[i].Connect(&guid);
			if (error != PGRERROR_OK)
			{
				PrintError(error);
				delete[] pCameras;
				std::cout << "Press Enter to exit." << endl;
				cin.ignore();
				return -1;
			}

			// Get the camera information
			CameraInfo camInfo;
			error = pCameras[i].GetCameraInfo(&camInfo);
			if (error != PGRERROR_OK)
			{
				PrintError(error);
				delete[] pCameras;
				std::cout << "Press Enter to exit." << endl;
				cin.ignore();
				return -1;
			}

			//PrintCameraInfo(&camInfo);

			// Turn trigger mode off
			TriggerMode trigMode;
			trigMode.onOff = false;
			error = pCameras[i].SetTriggerMode(&trigMode);
			if (error != PGRERROR_OK)
			{
				PrintError(error);
				delete[] pCameras;
				std::cout << "Press Enter to exit." << endl;
				cin.ignore();
				return -1;
			}

			// Turn Timestamp on
			EmbeddedImageInfo imageInfo;
			imageInfo.timestamp.onOff = true;
			error = pCameras[i].SetEmbeddedImageInfo(&imageInfo);
			if (error != PGRERROR_OK)
			{
				PrintError(error);
				delete[] pCameras;
				std::cout << "Press Enter to exit." << endl;
				cin.ignore();
				return -1;
			}

			// Start streaming on camera
			error = pCameras[i].StartCapture();
			if (error != PGRERROR_OK)
			{
				PrintError(error);
				delete[] pCameras;
				std::cout << "Press Enter to exit." << endl;
				cin.ignore();
				return -1;
			}
		}
	}
}