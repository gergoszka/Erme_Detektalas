#include <iostream>
#include <opencv2/core.hpp>    
#include <opencv2/highgui.hpp> 
#include <opencv2/imgproc.hpp> 

using namespace std;
using namespace cv;


	/*
	blur(gray, detected_edges, Size(3, 3));
	Canny(detected_edges, detected_edges, 40, 40 * 3, 3);
	dst = Scalar::all(0);
	img.copyTo(dst, detected_edges);
	imshow(window_name, dst);

	edge_temp = imread("huszas.png", 1);

	resize(dst,dst,edge_temp.size());
	cv::compare(dst, edge_temp, result, cv::CMP_EQ);
	cvtColor(result, result, COLOR_BGR2GRAY);
	int percentage = countNonZero(result);

	cout << percentage << "%" << endl;
	//imwrite("huszas.png", dst);
	*/



int main() {
	
	Mat img, gray, edge_temp, result;
	Mat dst, detected_edges;
	
	vector<vector<Point>> cont;

	///Beolvassuk a képeket
	for (int i = 7; i < 8; i++) {

		Mat temp, mask_obj, hsv;

		string sorszam = to_string(i);
		img = imread(sorszam + ".png", IMREAD_COLOR);


		if (img.empty()) {
			cout << "hiba" << endl;
			exit(-1);
		}

		
		//Edge detection
		int lowThreshold = 35;

		dst.create(img.size(), img.type());
		cvtColor(img, gray, COLOR_BGR2GRAY);
		namedWindow("edge", WINDOW_AUTOSIZE);
		
		blur(gray, detected_edges, Size(3, 3));
		Canny(detected_edges, detected_edges, lowThreshold, lowThreshold * 3, 3);
		dst = Scalar::all(0);
		img.copyTo(dst, detected_edges);
		imshow("edge", dst);

		waitKey();
		/*
		edge_temp = imread("huszas.png", 1);

		resize(dst, dst, edge_temp.size());
		cv::compare(dst, edge_temp, result, cv::CMP_EQ);
		cvtColor(result, result, COLOR_BGR2GRAY);
		int percentage = countNonZero(result);

		cout << percentage << "%" << endl;
		*/


		
		cvtColor(img, hsv, COLOR_BGR2HSV);

		inRange(hsv, Scalar(0, 0, 0), Scalar(248, 254, 255), mask_obj);

		medianBlur(mask_obj, mask_obj, 5);

		///Eltüntetjük a lyukakat
		//morphologyEx(mask_obj, mask_obj, MORPH_CLOSE, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)));

		imshow("maszk", mask_obj);
		moveWindow("maszk", 0, 0);

		int count = 0;
		
		/*
		findContours(mask_obj, cont, RETR_EXTERNAL, CHAIN_APPROX_NONE);
		drawContours(img, cont, -1, Scalar(0, 0, 255), 2);
		imshow("forint" + sorszam, img);
		moveWindow("forint" + sorszam, 50+mask_obj.cols, 0);
		cout << "Az adott forint a: forint_" << sorszam << endl;
		*/
		
		
		//Megkeressük a forintokat a képen
		for (auto c : cont) {

			///Átmentjük egy új képre az adott forintot	
			Rect r = boundingRect(c);
			temp = img(r);

			///Növeljük a darabszámot
			count++;

			imshow(to_string(count), temp);
			moveWindow(to_string(count), img.cols+100+mask_obj.cols, 0);
			waitKey(0);
				
			destroyWindow(to_string(count));
			
		}
		destroyWindow("maszk");
		destroyWindow("forint" + sorszam);
		
	}
	waitKey(0);
	return 0;
}
