#include <iostream>
#include <opencv2/core.hpp>    
#include <opencv2/highgui.hpp> 
#include <opencv2/imgproc.hpp> 
#include "opencv2/features2d.hpp"

using namespace std;
using namespace cv;


int main() {

	Mat img, detected_edges;
	vector<vector<Point>> cont;

	//Beolvassuk a képeket
	for (int i = 0; i < 11; i++) {

		Mat temp, gray, drawn_cont, closed;

		string sorszam = to_string(i);
		img = imread(sorszam + ".png", 1);

		//Hibakezelés
		if (img.empty()) {
			cout << "Nincs ilyen kep!" << endl;
			exit(-1);
		}

		//Gauss thereshold-al háttér elkülönítése
		cvtColor(img, gray, COLOR_BGR2GRAY);
		adaptiveThreshold(gray, gray, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 19, 0);

		//Lyukak eltűntetése a képről  
		Mat kernel = Mat::ones(Size(7, 7), CV_8UC1);
		morphologyEx(gray, closed, MORPH_CLOSE, kernel);
		medianBlur(closed, closed, 5);			

		//Megkeressük és megrajzoljuk a kontúrokat az eredeti képen
		findContours(closed, cont, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		drawn_cont = img.clone();
		drawContours(drawn_cont, cont, -1, Scalar(0, 0, 255), 2);

		cout << "\n#Jelenlegi kep: coin_" << i << endl;
		imshow("mask", closed);
		moveWindow("mask", 0, 20);

		imshow("Contour", drawn_cont);
		moveWindow("Contour", closed.cols + 100, 20);

		int count = 0;
		//Ha több object is van a képen megnézzük mindent
		for (auto c : cont) {

			///Átmentjük egy új képre az objectumot	
			Rect r = boundingRect(c);
			temp = img(r);

			count++;
			imshow(to_string(count), temp);
			moveWindow(to_string(count), closed.cols + 100 + drawn_cont.cols + 100, 20);

			cvtColor(temp, temp, COLOR_BGR2GRAY);

			int avg_min = 10000;
			int erme_sorszam = 0;
			Mat img_matches;

			for (int j = 0; j < 6; j++) {

				Mat compare = imread(to_string(j) + ".png", 0);
				resize(temp, temp, compare.size());

				//Lényeges pontok detektálása
				Ptr<FeatureDetector> detector = FastFeatureDetector::create(15);
				vector<KeyPoint> keypoints1, keypoints2;
				detector->detect(temp, keypoints1);
				detector->detect(compare, keypoints2);

				//Descriptopok kiszáítása
				Ptr<ORB> extractor = ORB::create();
				Mat descriptors1, descriptors2;
				extractor->compute(temp, keypoints1, descriptors1);
				extractor->compute(compare, keypoints2, descriptors2);

				//A két kép lényeges pontjainak összehasonlítása
				BFMatcher matcher(NORM_HAMMING,true);
				vector<DMatch> matches;
				matcher.match(descriptors1, descriptors2, matches);

				int sum = 0;
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

			//Legjobb match megmutatása
			if (img_matches.empty() == false) {
				imshow("matches", img_matches);
				moveWindow("matches", 0, closed.rows + 100);
			}
			else {
				cout << "Nincs megfelelo match " << endl ;
			}

			string name;
			switch (erme_sorszam) {
			case 0:
				name = "5ft-os";
				break;
			case 1:
				name = "10ft-os";
				break;
			case 2:
				name = "20ft-os";
				break;
			case 3:
				name = "50ft-os";
				break;
			case 4:
				name = "100ft-os";
				break;
			case 5:
				name = "200ft-os";
				break;
			default:
				name = "valami nem jött össze";
				break;
			}

			cout << "Legjobb distance: " << avg_min << "	";
			cout << "A kep legnagyobb eselyel egy: " << name << endl;
			cv::waitKey(0);

			destroyWindow("matches");
			destroyWindow(to_string(count));

		}
		destroyWindow("mask");
		destroyWindow("Contour");
	}
	cv::waitKey(0);
	return 0;
}
