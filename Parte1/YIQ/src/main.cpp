/*
 * Title: Flipped Image Sample (Cascaron)
 * Class: Vision para Robot
 * Instructor: Dr. Jose Luis Gordillo (http://LabRob.mty.itesm.mx/)
 * Code: Manlio Barajas (manlito@gmail.com), Alberto Jahuey (A01039835@itesm.mx)
 * Institution: Tec de Monterrey, Campus Monterrey
 * Date: January 10, 2012 Last Update: January 30, 2019
 *
 * Description: This program takes input from a camera (recognizable by
 * OpenCV) and it flips it horizontally.
 * "Basic" version uses frequently the "Cv::Mat::At" method which slows down
 * performance. This program has illustrative purposes, provided the existence
 * of cv::flip method.
 *
 *
 * This programs uses OpenCV http://opencv.willowgarage.com/wiki/
 */

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

void RGBToYIQ(const Mat &original, Mat &converted);
void YIQToRGB(const Mat &original, Mat &converted);

int main(int argc, char *argv[]) {
  /* First, open camera device */
  VideoCapture camera = VideoCapture(0);
  bool isCameraAvailable = camera.isOpened();

  /* Create images where captured and transformed frames are going to be stored
   */
  Mat currentImage;
  Mat YIQ;
  Mat reverted;

  /* Clean the terminal */
  cout << "\033[2J\033[1;1H";
  cout << "Basic Show Image \t|\tUse 'x' or 'Esc' to terminate execution\n";
  while (true) {
    camera.read(currentImage);

    if (currentImage.size().width <= 0 && currentImage.size().height <= 0) {
      cout << "ERROR: Camera returned blank image, check connection\n";
      break;
    }
    RGBToYIQ(currentImage, YIQ);

    /* Show images */
    imshow("Original", currentImage);
    imshow("YIQ", YIQ);

    /* If 'x' is pressed, exit program */
    char key = waitKey(1);
    if(key == 'x' || key == 27 ){ // 27 = ESC
      break;
    }
  }
}

void RGBToYIQ(const Mat &original, Mat &converted) {
  	if (converted.empty()) {
		  converted = Mat(original.rows, original.cols, original.type());
    }

  uchar r, g, b;
  double y, i, q;


  for(int row = 0; row < original.rows; row++) {
    for (int col = 0; col < original.cols; col++) {
      b = original.at<Vec3b>(row,col)[0];
      g = original.at<Vec3b>(row,col)[1];
      r = original.at<Vec3b>(row,col)[2];
      y = 0.299*r + 0.587*g + 0.114*b;
			i = 0.596*r - 0.275*g - 0.321*b;
			q = 0.212*r - 0.523*g + 0.311*b;
      converted.at<Vec3b>(row,col)[2] = y;
      converted.at<Vec3b>(row,col)[1] = i + 128;
      converted.at<Vec3b>(row,col)[0] = q + 128;
    }
  }
}

void YIQToRGB(const Mat &original, Mat &converted) {
  	if (converted.empty()) {
		  converted = Mat(original.rows, original.cols, original.type());
    }

  double y, i, q;
  uchar r, g, b;


  for(int row = 0; row < original.rows; row++) {
    for (int col = 0; col < original.cols; col++) {
      q = original.at<Vec3b>(row,col)[0] - 128;
      i = original.at<Vec3b>(row,col)[1] - 128;
      y = original.at<Vec3b>(row,col)[2];
      r = 1*y + 0.956*i + 0.621*q;
			g = 1*y - 0.272*i - 0.647*q;
			b = 1*y - 1.107*i + 1.705*q;
      converted.at<Vec3b>(row,col)[2] = r;
      converted.at<Vec3b>(row,col)[1] = g;
      converted.at<Vec3b>(row,col)[0] = b;
    }
  }
}