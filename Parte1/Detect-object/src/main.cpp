/*
 * Title: Window Click Example
 * Class: Vision para Robot
 * Instructor: Dr. Jose Luis Gordillo (http://robvis.mty.itesm.mx/~gordillo/)
 * Code: Manlio Barajas (manlito@gmail.com)
 * Institution: Tec de Monterrey, Campus Monterrey
 * Date: January 28, 2013
 *
 * Description: Shows the most basic interaction with OpenCV HighGui window.
 *
 * This programs uses OpenCV http://www.opencv.org/
 */

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/gapi/render/render.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// Here we will store points
Point point;
uchar yi;
uchar i;
uchar q;
Mat currentImage;
Mat filtered;

/* This is the callback that will only display mouse coordinates */
void mouseCoordinatesExampleCallback(int event, int x, int y, int flags, void* param);
void RGBToYIQ(const Mat &original, Mat &converted);
void filterObject(const Mat &original, Mat &converted);

int main(int argc, char *argv[])
{
	/* First, open camera device */
	VideoCapture camera;
    camera.open(0);

	/* Create images where captured and transformed frames are going to be stored */

    /* Create main OpenCV window to attach callbacks */
    namedWindow("Image");
    setMouseCallback("Image", mouseCoordinatesExampleCallback);

    while (true)
	{
		camera >> currentImage;

		if (currentImage.data) 
		{
            //RGBToYIQ(currentImage, currentImage);
            //cvtColor(currentImage, currentImage, COLOR_BGR2HSV);
            filterObject(currentImage, filtered);
            /* Show image */
            imshow("Image", currentImage);
            imshow("Filtered", filtered);

			/* If 'x' is pressed, exit program */
			if (waitKey(3) == 'x')
				break;
		}
		else
		{
			cout << "No image data.. " << endl;
		}
	}
}


void mouseCoordinatesExampleCallback(int event, int x, int y, int flags, void* param)
{
    switch (event)
    {
        case EVENT_LBUTTONDOWN:
            cout << "Detected" << endl;
                q = currentImage.at<Vec3b>(y,x)[0];
                i = currentImage.at<Vec3b>(y,x)[1];
                yi = currentImage.at<Vec3b>(y,x)[2];
            break;
        case EVENT_MOUSEMOVE:
            break;
        case EVENT_LBUTTONUP:
            break;
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

void filterObject(const Mat &original, Mat &converted) {
  	if (converted.empty()) {
		  converted = Mat(original.rows, original.cols, original.type());
    }

  for(int row = 0; row < original.rows; row++) {
    for (int col = 0; col < original.cols; col++) {
      uchar qq = original.at<Vec3b>(row,col)[0];
      uchar ii = original.at<Vec3b>(row,col)[1];
      uchar yy = original.at<Vec3b>(row,col)[2];
      bool allPass = qq > q - 25 && qq < q + 25 && ii > i - 25 && ii < i + 25 && yy > yi - 25 && yy < yi + 25;
      converted.at<Vec3b>(row,col)[0] = allPass ? qq : 0;
      converted.at<Vec3b>(row,col)[1] = allPass ? ii : 0;
      converted.at<Vec3b>(row,col)[2] = allPass ? yy : 0;
    }
  }
}