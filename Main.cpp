#include <iostream>
#include <opencv2/core.hpp>    
#include <opencv2/highgui.hpp> 
#include <opencv2/imgproc.hpp> 

using namespace std;
using namespace cv;


int main() {
	
	Mat img, gray, edge_temp;
	Mat dst, detected_edges;
	
	vector<vector<Point>> cont;

	///Beolvassuk a képeket
	for (int i = 0; i < 7; i++) {

		Mat temp, mask_obj, hsv, drawn_cont;

		string sorszam = to_string(i);
		img = imread(sorszam + ".png", 1);
		

		//Hibakezelés
		if (img.empty()) {
			cout << "Nincs ilyen kép!" << endl;
			exit(-1);
		}
		
		//Eredeti kép maszkolása
		cvtColor(img, hsv, COLOR_BGR2HSV);
		inRange(hsv, Scalar(0, 0, 0), Scalar(248, 254, 255), mask_obj);
		medianBlur(mask_obj, mask_obj, 5);

		imshow("maszk", mask_obj);
		moveWindow("maszk", 0, 0);

		int count = 0;

		//Maszkolt kép alapján megrajzolkuk a kontúrokat
		findContours(mask_obj, cont, RETR_EXTERNAL, CHAIN_APPROX_NONE);

		drawn_cont = img.clone();
		drawContours(drawn_cont, cont, -1, Scalar(0, 0, 255), 2);

		imshow("érme" + sorszam, drawn_cont);
		moveWindow("érme" + sorszam, 50+mask_obj.cols, 0);
		cout << "A jelenlegi penzerme: erme_" << sorszam << endl;
		
		
		//Megkeressük a forintokat a képen
		for (auto c : cont) {

			///Átmentjük egy új képre az adott érmét	
			Rect r = boundingRect(c);
			temp = img(r);

			//Edge detection
			int lowThreshold = 40;

			dst.create(temp.size(), temp.type());
			cvtColor(temp, gray, COLOR_BGR2GRAY);

			blur(gray, detected_edges, Size(3, 3));
			Canny(detected_edges, detected_edges, lowThreshold, lowThreshold * 3, 3);

			imshow("edge", detected_edges);
			moveWindow("edge", img.cols + 100 + mask_obj.cols, 0);
			//imwrite("edge_" + sorszam + ".png", temp);

			///Növeljük a darabszámot
			count++;

			imshow(to_string(count), temp);
			moveWindow(to_string(count), img.cols+100+mask_obj.cols, detected_edges.rows + 50);


			int max_match = 0;
			int erme_sorszam = 0;
			for (int j = 0; j < 6; j++) {

				Mat compare = imread("edge_" + to_string(j) + ".png", 0);
				Mat img_match;

				resize(detected_edges, detected_edges, compare.size());
				cv::compare(detected_edges, compare, img_match, cv::CMP_EQ);
				
				int match = countNonZero(img_match);
				cout << j << ".kep egyezes: " << match << endl;

				if (match > max_match) {
					max_match = match;
					erme_sorszam = j;
				}
			}

			string name;
			switch (erme_sorszam) {
				case 0:
					name = "10ft-os";
					break;
				case 1:
					name = "50ft-os";
					break;
				case 2:
					name = "5ft-os";
					break;
				case 3:
					name = "100ft-os";
					break;
				case 4:
					name = "20ft-os";
					break;
				case 5:
					name = "200ft-os";
					break;
				default:
					name = "valami nem jött össze";
					break;
			}

			cout <<"A kep legnagyobb eselyel egy:  " << name << endl;

			waitKey(0);
				
			destroyWindow("edge");
			destroyWindow(to_string(count));
			
		}
		destroyWindow("maszk");
		destroyWindow("érme" + sorszam);
		
	}
	waitKey(0);
	return 0;
}
