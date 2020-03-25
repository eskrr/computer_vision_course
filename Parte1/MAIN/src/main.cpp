#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_VALUE 255

int binaryThreshold = 127;
cv::Vec3b hoverPixel(0, 0, 0);
std::vector<int> clickCoordinates;
int iSliderValue = 127;

void mouseEvent(int event, int x, int y, int flags, void* param) {
  cv::Mat* image = (cv::Mat*)param;
  cv::Vec3b pixel = image->at<cv::Vec3b>(y,x);

  switch (event)
  {
      case cv::EVENT_LBUTTONDOWN:
           clickCoordinates.push_back(y);
           clickCoordinates.push_back(x);
           std::cout << "clickCoordinates Size: " << clickCoordinates.size() << std::endl;
          break;
      case cv::EVENT_MOUSEMOVE:
          std::cout << "  Mouse X, Y: " << x << ", " << y << "\t[B, G, R] " << pixel << std::endl;
          hoverPixel = pixel;
          break;
      case cv::EVENT_LBUTTONUP:
          break;
  }
}

void rgbImagetoGrayScale(const cv::Mat& rgbImage, cv::Mat& bwImage) {
  bwImage = rgbImage.clone();
  for (int i = 0; i < bwImage.rows; i++) {
    for (int j = 0; j < bwImage.cols; j++) {
      cv::Vec3b pixel = bwImage.at<cv::Vec3b>(i, j);
      double grayValue = (pixel[2] + pixel[1] + pixel[0]) / 3;
      bwImage.at<cv::Vec3b>(i, j)[0] = grayValue;
      bwImage.at<cv::Vec3b>(i, j)[1] = grayValue;
      bwImage.at<cv::Vec3b>(i, j)[2] = grayValue;
    }
  }
}

void rgbImagetoHSV(const cv::Mat& rgbImage, cv::Mat& hsvImage) {
  hsvImage = rgbImage.clone();
  for (int i = 0; i < rgbImage.rows; i++) {
    for (int j = 0; j < rgbImage.cols; j++) {
      cv::Vec3b pixel = rgbImage.at<cv::Vec3b>(i, j);

      double red = pixel[2];
      double green = pixel[1];
      double blue = pixel[0];

      double max = std::max(red, std::max(green, blue));
      double min = std::min(red, std::max(green, blue));

      double delta = max - min;
      double hue, saturation, value;

      if (max != min) {
        if (max == red && green >= blue) {
          hue = 60 * M_PI / 180 * (green - blue) / (max - min) + 0 * M_PI / 180;
        } else if (max == red && green < blue) {
          hue = 60 * M_PI / 180 * (green - blue) / (max - min) + 360 * M_PI / 180;
        } else if (max == green) {
          hue = 60 * M_PI / 180 * (blue - red) / (max - min) + 120 * M_PI / 180;
        } else if (max == blue) {
          hue = 60 * M_PI / 180 * (red - green) / (max - min) + 240 * M_PI / 180;
        }
      }

      if (max == 0) {
        saturation = 0;
      } else {
        saturation = 1- min / max;
      }

      value = max;

      hsvImage.at<cv::Vec3b>(i, j)[0] = hue / (2 * M_PI) * 255 ;
      hsvImage.at<cv::Vec3b>(i, j)[1] = saturation * 255;
      hsvImage.at<cv::Vec3b>(i, j)[2] = value * 255;

    }
  }
}

void rgbImagetoYIQ(const cv::Mat& original, cv::Mat& converted) {
  	if (converted.empty()) {
		  converted = cv::Mat(original.rows, original.cols, original.type());
    }

  uchar r, g, b;
  double y, i, q;


  for(int row = 0; row < original.rows; row++) {
    for (int col = 0; col < original.cols; col++) {
      b = original.at<cv::Vec3b>(row,col)[0];
      g = original.at<cv::Vec3b>(row,col)[1];
      r = original.at<cv::Vec3b>(row,col)[2];
      y = 0.299*r + 0.587*g + 0.114*b;
			i = 0.596*r - 0.275*g - 0.321*b;
			q = 0.212*r - 0.523*g + 0.311*b;
      converted.at<cv::Vec3b>(row,col)[2] = y;
      converted.at<cv::Vec3b>(row,col)[1] = i + 128;
      converted.at<cv::Vec3b>(row,col)[0] = q + 128;
    }
  }
}

void grayToBinaryScale(const cv::Mat& grayImage, cv::Mat& binaryImage) {
  binaryImage = grayImage.clone();
  for (int i = 0; i < binaryImage.rows; i++) {
    for (int j = 0; j < binaryImage.cols; j++) {
      cv::Vec3b pixel = binaryImage.at<cv::Vec3b>(i, j);
      if (pixel[0] < binaryThreshold) {
        binaryImage.at<cv::Vec3b>(i, j)[0] = 0;
        binaryImage.at<cv::Vec3b>(i, j)[1] = 0;
        binaryImage.at<cv::Vec3b>(i, j)[2] = 0;
      } else if (pixel[0] >= binaryThreshold) {
        binaryImage.at<cv::Vec3b>(i, j)[0] = 255;
        binaryImage.at<cv::Vec3b>(i, j)[1] = 255;
        binaryImage.at<cv::Vec3b>(i, j)[2] = 255;
      }
    }
  }
}

void calCulateHist(const cv::Mat& srcImage, cv::Mat& redHistogram, cv::Mat& greenHistogram, cv::Mat& blueHistogram) {
  std::vector<cv::Mat> bgrImage;
  cv::split(srcImage, bgrImage);

  /// Establish the number of bins
  int histSize = 256;

  /// Set the ranges ( for B,G,R) )
  float range[] = { 0, 256 } ;
  const float* histRange = {range};

  bool uniform = true;
  bool accumulate = false;

  cv::Mat bHist, gHist, rHist;

  /// Compute the histograms:
  cv::calcHist(&bgrImage[0], 1, 0, cv::Mat(), bHist, 1, &histSize, &histRange, uniform, accumulate);
  cv::calcHist(&bgrImage[1], 1, 0, cv::Mat(), gHist, 1, &histSize, &histRange, uniform, accumulate);
  cv::calcHist(&bgrImage[2], 1, 0, cv::Mat(), rHist, 1, &histSize, &histRange, uniform, accumulate);

  // Draw the histograms for B, G and R
  int histW = 512; 
  int histH = 400;
  int binW = cvRound((double) histW/histSize);

  redHistogram = cv::Mat(histH, histW, CV_8UC3, cv::Scalar( 0,0,0));
  greenHistogram = cv::Mat(histH, histW, CV_8UC3, cv::Scalar( 0,0,0));
  blueHistogram = cv::Mat(histH, histW, CV_8UC3, cv::Scalar( 0,0,0));

  /// Normalize the result to [ 0, hist.rows ]
  cv::normalize(bHist, bHist, 0, blueHistogram.rows, cv::NORM_MINMAX, -1, cv::Mat());
  cv::normalize(gHist, gHist, 0, greenHistogram.rows, cv::NORM_MINMAX, -1, cv::Mat());
  cv::normalize(rHist, rHist, 0, redHistogram.rows, cv::NORM_MINMAX, -1, cv::Mat());
  
  /// Draw for each channel
  for( int i = 1; i < histSize; i++ ) {
      line(blueHistogram, cv::Point(binW*(i-1), histH - cvRound(bHist.at<float>(i-1))),
                      cv::Point(binW*(i), histH - cvRound(bHist.at<float>(i))),
                      cv::Scalar(255, 0, 0), 2, 8, 0);
      line(greenHistogram, cv::Point(binW*(i-1), histH - cvRound(gHist.at<float>(i-1))),
                      cv::Point(binW*(i), histH - cvRound(gHist.at<float>(i))),
                      cv::Scalar(0, 255, 0), 2, 8, 0);
      line(redHistogram, cv::Point(binW*(i-1), histH - cvRound(rHist.at<float>(i-1))),
                      cv::Point(binW*(i), histH - cvRound(rHist.at<float>(i))),
                      cv::Scalar(0, 0, 255), 2, 8, 0);
  }
}

void drawLinesInHistogram(int numCols, int numRows, cv::Mat& redHistogram, cv::Mat& greenHistogram, cv::Mat& blueHistogram) {
  int redCol = hoverPixel[2] * numCols / MAX_VALUE;
  int greenCol = hoverPixel[1] * numCols / MAX_VALUE;
  int blueCol = hoverPixel[0] * numCols / MAX_VALUE;
  cv::line(greenHistogram, cv::Point(greenCol, 0), cv::Point(greenCol, numRows), cv::Scalar(0, MAX_VALUE, 0), 1, 8, 0);
  cv::line(blueHistogram, cv::Point(blueCol, 0), cv::Point(blueCol, numRows), cv::Scalar(MAX_VALUE, 0, 0), 1, 8, 0);
  cv::line(redHistogram, cv::Point(redCol, 0), cv::Point(redCol, numRows), cv::Scalar(0, 0, MAX_VALUE), 1, 8, 0);
}

void getMaxAndMins(const cv::Mat& image, int& redMax,
 int& redMin, int& greenMax, int& greenMin, int& blueMax, int& blueMin) {
  clickCoordinates.clear();
  while (clickCoordinates.size() < 4) {
    char key = cv::waitKey(1);
    if (key == 'x') {
      break;
    }
  }
  if (clickCoordinates.size() == 4) {
    cv::Mat croppedImage = image(cv::Rect(clickCoordinates[1], clickCoordinates[0], 
     clickCoordinates[3] - clickCoordinates[1], clickCoordinates[2] - clickCoordinates[0]));
    cv::imshow("Cropped", croppedImage);
    
    redMax = -1;
    redMin = 256;
    greenMax = -1;
    greenMin = 256;
    blueMax = -1;
    blueMin = 256;

    for (int i = 0; i < croppedImage.rows; i++) {
      for (int j = 0; j < croppedImage.cols; j++) {
        if (croppedImage.at<cv::Vec3b>(i, j)[0] > blueMax) {
          blueMax = croppedImage.at<cv::Vec3b>(i, j)[0];
        }
        if (croppedImage.at<cv::Vec3b>(i, j)[0] < blueMin) {
          blueMin = croppedImage.at<cv::Vec3b>(i, j)[0];
        }
        if (croppedImage.at<cv::Vec3b>(i, j)[1] > greenMax) {
          greenMax = croppedImage.at<cv::Vec3b>(i, j)[1];
        }
        if (croppedImage.at<cv::Vec3b>(i, j)[1] < greenMin) {
          greenMin = croppedImage.at<cv::Vec3b>(i, j)[1];
        }
        if (croppedImage.at<cv::Vec3b>(i, j)[2] > redMax) {
          redMax = croppedImage.at<cv::Vec3b>(i, j)[2];
        }
        if (croppedImage.at<cv::Vec3b>(i, j)[2] < redMin) {
          redMin = croppedImage.at<cv::Vec3b>(i, j)[2];
        }
      }
    }
  }
}

int main(int argc, char *argv[]) {
  // Open camera device
  cv::VideoCapture camera = cv::VideoCapture(0);

  // Clean the terminal
  std::cout << "\033[2J\033[1;1H";
  std::cout << "Basic Show Image \t|\tUse 'x' or 'Esc' to terminate execution\n";

  char model = 'y';
  bool showFilteredImage = false;
  while (true && camera.isOpened()) {
    cv::Mat image;
    cv::Mat redHistogram;
    cv::Mat greenHistogram;
    cv::Mat blueHistogram;
    camera.read(image); 

    if (model == 'g') {
      rgbImagetoGrayScale(image, image);
    } else if (model == 'b') {
      rgbImagetoGrayScale(image, image);
      grayToBinaryScale(image, image);
    } else if (model == 'h') { 
      rgbImagetoHSV(image, image);
    } else if (model == 'y') {
      rgbImagetoYIQ(image, image);
    }

    char key = cv::waitKey(1);
    if (key == 'r') {
      cv::destroyAllWindows();
      showFilteredImage = false;
      model = 'r';
    } else if (key == 'g') {
      cv::destroyAllWindows();
      showFilteredImage = false;
      model = 'g';
    } else if (key == 'b') {
      cv::destroyAllWindows();
      showFilteredImage = false;

      rgbImagetoGrayScale(image, image);
      grayToBinaryScale(image, image);
      model = 'b';
    } else if (key == 'h') {
      cv::destroyAllWindows();
      showFilteredImage = false;
      model = 'h';
    } else if (key == 'y') {
      cv::destroyAllWindows();
      showFilteredImage = false;
      model = 'y';
    } else if(key == 'x' || key == 27 ) {  // 27 = ESC
      cv::destroyAllWindows();
      showFilteredImage = false;
      break;
    } else if (key == 'c') {
      cv::destroyAllWindows();
      cv::imshow("Image", image);
      cv::setMouseCallback("Image", mouseEvent, &image);
      while (true) {
        calCulateHist(image, redHistogram, greenHistogram, blueHistogram);
        drawLinesInHistogram(image.cols, image.rows, redHistogram, greenHistogram, blueHistogram);
        cv::imshow("Red Histogram", redHistogram);
        cv::imshow("Green Histogram", greenHistogram);
        cv::imshow("Blue Histogram", blueHistogram);
        key = cv::waitKey(1);
        if (key == 'x') {
          cv::destroyAllWindows();
          break;
        }
      }
    }


    cv::imshow("Image", image);
    cv::setMouseCallback("Image", mouseEvent, &image);
    calCulateHist(image, redHistogram, greenHistogram, blueHistogram);
    drawLinesInHistogram(image.cols, image.rows, redHistogram, greenHistogram, blueHistogram);

    cv::Mat rowRed = cv::Mat::ones(10, greenHistogram.cols, greenHistogram.type());
    for (int i = 0; i < rowRed.rows; i++) {
      for (int j = 0; j < rowRed.cols; j++) {
        rowRed.at<cv::Vec3b>(i, j)[0] = 0;
        rowRed.at<cv::Vec3b>(i, j)[1] = 0;
        rowRed.at<cv::Vec3b>(i, j)[2] = j / 2;
      }
    }
    redHistogram.push_back(rowRed);
    cv::imshow(model == 'y' ? "Y Histogram" : "Red Histogram", redHistogram);
    
    cv::Mat row = cv::Mat::ones(10, greenHistogram.cols, greenHistogram.type());
    for (int i = 0; i < row.rows; i++) {
      for (int j = 0; j < row.cols; j++) {
        row.at<cv::Vec3b>(i, j)[0] = 0;
        row.at<cv::Vec3b>(i, j)[1] = j / 2;
        row.at<cv::Vec3b>(i, j)[2] = 0;
      }
    }
    greenHistogram.push_back(row);
    cv::imshow(model == 'y' ? "I Histogram" : "Green Histogram", greenHistogram);
    
    cv::Mat rowBlue = cv::Mat::ones(10, greenHistogram.cols, greenHistogram.type());
    for (int i = 0; i < rowBlue.rows; i++) {
      for (int j = 0; j < rowBlue.cols; j++) {
        rowBlue.at<cv::Vec3b>(i, j)[0] = j / 2;
        rowBlue.at<cv::Vec3b>(i, j)[1] = 0;
        rowBlue.at<cv::Vec3b>(i, j)[2] = 0;
      }
    }
    blueHistogram.push_back(rowBlue);
    cv::imshow(model == 'y' ? "Q Histogram" : "Blue Histogram", blueHistogram);

    if (model == 'b') {
      cv::createTrackbar("Threshold", "Image", &iSliderValue, 255);
      binaryThreshold  = iSliderValue;
    }
    
  }
}
