#include <iostream>
#include <opencv2/core.hpp>    
#include <opencv2/highgui.hpp> 
#include <opencv2/imgproc.hpp> 
#include "opencv2/features2d.hpp"

using namespace std;
using namespace cv;


int main() {
	
	Mat img, edge_temp;
	Mat dst, detected_edges;
	
	vector<vector<Point>> cont;

	//Beolvassuk a képeket
	for (int i = 5; i < 12; i++) {

		Mat temp, mask_obj, gray, drawn_cont,closed;

		string sorszam = to_string(i);
		img = imread(sorszam + ".png", 1);
		
		//Hibakezelés
		if (img.empty()) {
			cout << "Nincs ilyen kép!" << endl;
			exit(-1);
		}
		
		cvtColor(img, gray, COLOR_BGR2GRAY);
		adaptiveThreshold(gray, gray, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 19, 0);

		Mat kernel = Mat::ones(Size(7, 7), CV_8UC1);
		morphologyEx(gray, closed, MORPH_CLOSE, kernel);
		medianBlur(gray, gray, 5);

		findContours(closed, cont, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		drawn_cont = img.clone();
		drawContours(drawn_cont, cont, -1, Scalar(0, 0, 255), 2);

		
		imshow("detected circles", drawn_cont);
		//waitKey();


		int count = 0;
		//Megkeressük a forintokat a képen
		for (auto c : cont) {

			///Átmentjük egy új képre az adott érmét	
			Rect r = boundingRect(c);
			temp = img(r);

			count++;
			imshow(to_string(count), temp);
			moveWindow(to_string(count), img.cols+100+mask_obj.cols, 0);
			
			cvtColor(temp, temp, COLOR_BGR2GRAY);
			cout << "Jelenlegi próba: " << count << endl;

			int avg_min = 500;
			int erme_sorszam = 0;
			Mat img_matches;

			for (int j = 0; j < 6; j++) {

				Mat compare = imread(to_string(j) + ".png", 0);
				resize(temp, temp, compare.size());

				// detecting keypoints
				Ptr<FeatureDetector> detector = FastFeatureDetector::create(15);
				vector<KeyPoint> keypoints1, keypoints2;
				detector->detect(temp, keypoints1);
				detector->detect(compare, keypoints2);

				// computing descriptors
				Ptr<ORB> extractor = ORB::create();
				Mat descriptors1, descriptors2;
				extractor->compute(temp, keypoints1, descriptors1);
				extractor->compute(compare, keypoints2, descriptors2);

				// matching descriptors
				BFMatcher matcher(NORM_L2);
				vector<DMatch> matches;
				matcher.match(descriptors1, descriptors2, matches);

				int sum=0;
				for (int k = 0; k < matches.size(); k++) {
					sum += matches[k].distance;
				}

				if (matches.size() != 0) {
					int avg = sum / matches.size();
					if (avg < avg_min) {
						avg_min = avg;
						erme_sorszam = j;
						drawMatches(temp, keypoints1, compare, keypoints2, matches, img_matches);
					}
				}
				
			}
			
			// drawing the results
			if (img_matches.empty() == false) {
				namedWindow("matches", 1);
				imshow("matches", img_matches);
			}
			else {
				cout << "Nincs megfelelő match" << endl;
			}
			

			cout << "Legjobb distance: " << avg_min << "  Match szama: " << erme_sorszam << endl;
			cv::waitKey(0);
				
			destroyWindow("edge");
			destroyWindow("matches");
			destroyWindow(to_string(count));
			
		}
		destroyWindow("maszk");
		destroyWindow("érme" + sorszam);
		
	}
	cv::waitKey(0);
	return 0;
}
