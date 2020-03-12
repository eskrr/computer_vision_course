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
#include <unistd.h>

using namespace std;
using namespace cv;

#define BLUE_INDEX 0
#define GREEN_INDEX 1
#define RED_INDEX 2

#define HUE_INDEX 0
#define LIGHTNESS_INDEX 1
#define SATURATION_INDEX 2

void castToHSL(const Mat &sourceImage, Mat &destinationImage) {
  if (destinationImage.empty())
    destinationImage = Mat(sourceImage.rows, sourceImage.cols, sourceImage.type());

  for (int y = 0; y < sourceImage.rows; ++y) {
    for (int x = 0; x < sourceImage.cols; ++x) {
      Vec3b pixel = sourceImage.at<Vec3b>(y, x), new_pixel;
      int red = (int)pixel[RED_INDEX];
      int blue = (int)pixel[BLUE_INDEX];
      int green = (int)pixel[GREEN_INDEX];
      int max_value, min_value;
      float hue, saturation, lightness;

      // Get minimum value
      min_value = min(red, blue);
      min_value = min(min_value, green);

      // We need to find which is the maximum value.
      if (red > blue && red > green) {
        // Max is red.
        max_value = red;
        hue = fmod(60 * (float(green - blue)/float(max_value - min_value)) + 360, 360);
      } else if (green > red && green > blue) {
        // Max is green.
        max_value = green;
        hue = 60 * (float(blue - red)/float(max_value - min_value)) + 120;
      } else if (blue > red && blue > green) {
        // Max is blue.
        max_value = blue;
        hue = 60 * (float(red - green)/float(max_value - min_value)) + 240;
      } else {
        max_value = min_value;
        hue = 0;
      }
      lightness = float(max_value + min_value)/2.0;
      if (max_value == min_value) {
        saturation = 0.0;
      } else if (lightness <= 0.5) {
        saturation = float(max_value-min_value)/(2*lightness);
      } else {
        saturation = float(max_value-min_value)/(2-2*lightness);
      }

      new_pixel[HUE_INDEX] = hue;
      new_pixel[LIGHTNESS_INDEX] = lightness;
      new_pixel[SATURATION_INDEX] = saturation;

      destinationImage.at<Vec3b>(y, x) = new_pixel;
    }
  }
}

int main(int argc, char *argv[]) {
  /* First, open camera device */
  VideoCapture camera = VideoCapture(0);
  bool isCameraAvailable = camera.isOpened();

  /* Create images where captured and transformed frames are going to be stored
   */
  Mat currentImage;
  Mat hslImage;
  Mat recomposedImage;

  /* Clean the terminal */
  cout << "\033[2J\033[1;1H";
  cout << "Basic Show Image \t|\tUse 'x' or 'Esc' to terminate execution\n";
  while (true) {
    /* Obtain a new frame from camera */
    if (isCameraAvailable) {
      camera.read(currentImage);
    } else {
      currentImage = imread("PlaceholderImage.jpg", IMREAD_COLOR);
    }

    if (currentImage.size().width <= 0 && currentImage.size().height <= 0) {
      cout << "ERROR: Camera returned blank image, check connection\n";
      break;
    }
    
    castToHSL(currentImage, hslImage);

    /* Show images */
    // imshow("Original", currentImage);
    imshow("HSL", hslImage);
    usleep(1000000);

    cvtColor(hslImage, recomposedImage, COLOR_HLS2BGR);
    imshow("Recomposed", recomposedImage);

    /* If 'x' is pressed, exit program */
    char key = waitKey(1);
    if(key == 'x' || key == 27 ){ // 27 = ESC
      break;
    }
  }
}
