/*
 * DisplayImage.cpp
 *
 *  Created on: Jul 1, 2014
 *      Author: ardillo
 */

#include <opencv2/opencv.hpp>
#include <algorithm>
#include <tesseract/baseapi.h>


using namespace cv;
using namespace std;

//#define DEBUG
//#define IMAGE_OUTPUT

int main( int argc, char** argv )
{
  Mat image;
  image = imread( argv[1], 1 );

  Mat output = image.clone();

  if( argc != 2 || !image.data ){
      cout << "No image data \n" << endl;
      return -1;
    }

  // check all G values and push them in an array.
  int lookup[image.rows];
  for (int i = 0; i < image.rows; i++){
    int value = image.at<Vec3b>(i,0)[1];
    lookup[i] = value;
#ifdef DEBUG
    cout << endl << "line: " << i << "-> " << lookup[i] << endl;
#endif
  }

  // sort the unsorted array with G values
  sort(lookup, lookup + image.rows);
  for (int i = 0; i < image.rows; i++){
#ifdef DEBUG
	    cout << endl << "line: " << i << "-> " << lookup[i] << endl;
#endif
  }

  // loop through image and check value in sorted array for correct line number.
  // return location in array and use that as a newline nr for the output.
  for (int i = 0; i < image.rows; i++){
	  int value = image.at<Vec3b>(i,0)[1];
	  int newline = -1;
	  for (int i = 0; i < image.rows; i++){
		  if (lookup[i] == value){
			  newline = i;
			  break;
		  }
#ifdef DEBUG
		  cout << "Value not found" << endl;
#endif
	  }
  	for (int j = 0; j < image.cols; j++){
  	  output.at<Vec3b>(newline, j)[0] = image.at<Vec3b>(i,j)[0];
  	  output.at<Vec3b>(newline, j)[1] = image.at<Vec3b>(i,j)[1];
  	  output.at<Vec3b>(newline, j)[2] = image.at<Vec3b>(i,j)[2];
  	}
  }

  // test output to show if the lines are correctly sorted to G values
  for (int i = 0; i < output.rows; i++){
	for (int j = 0; j < output.cols; j++){
		int B = output.at<Vec3b>(i,j)[0];
		int G = output.at<Vec3b>(i,j)[1];
		int R = output.at<Vec3b>(i,j)[2];

#ifdef DEBUG
		cout << " R:" << R << " G:" << G << " B:" << B << " |";
		if(j == 149) cout << endl;
#endif

	}
  }

  // convert to grayscale image
  Mat gray(output.rows, output.cols, CV_8UC1);
  cvtColor(output, gray, CV_BGR2GRAY);

  // pass to Tesseract for OCR
  tesseract::TessBaseAPI tess;
  tess.Init(NULL, "eng", tesseract::OEM_DEFAULT);
  tess.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
  tess.SetImage((uchar*)gray.data, gray.cols, gray.rows, 1, gray.cols);

  // extract text
  char* out = tess.GetUTF8Text();
#ifdef DEBUG
  cout << "Characters in picture ";
#endif
  cout << out << endl;

#ifdef IMAGE_OUTPUT
  // store image
  //imwrite( "output.png", output);

  namedWindow( "Display Image", WINDOW_AUTOSIZE );
  imshow( "Display Image", image );

  namedWindow( "Display output", WINDOW_AUTOSIZE );
  imshow( "Display output", output );

  namedWindow( "Display gray", WINDOW_AUTOSIZE );
  imshow( "Display gray", gray );

  waitKey(0);
#endif

  return 0;
}


