////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <pthread.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

enum Colours { Red2, Red, Green, Blue, Yellow };

int lowH[5] = {   0, 150, 51,  75, 20 };
int highH[5] = { 10, 230, 75, 107, 35 };

int lowS[5] = {  158, 128, 127, 127,  85 };
int highS[5] = { 255, 255, 255, 255, 255 };

int lowV[5] = {  158,   0, 127, 127, 150 };
int highV[5] = { 255, 255, 255, 255, 255 };

typedef struct custom_data
{
    int state;
    pthread_mutex_t mtx;
} custom_data_t;

void my_button_cb(int event, int x, int y, int flags, void* userdata)
{
    //std::cout << "@my_button_cb" << std::endl;   

    // convert userdata to the right type
    custom_data_t* ptr = (custom_data_t*)userdata;
    if (!ptr)
    {
        std::cout << "@my_button_cb userdata is empty" << std::endl;
        return;
    }

	Rect red2Rect   = Rect(0,0, 75, 50);
	Rect redRect    = Rect(75,0, 75, 50);
	Rect blueRect   = Rect(150,0, 150, 50);
	Rect greenRect  = Rect(0,50, 150, 50);
	Rect yellowRect = Rect(150,50, 150, 50);
    
    Rect exitRect = Rect(0,100, 300, 50);

	// lock mutex to protect data from being modified by the
	// main() thread
	pthread_mutex_lock(&ptr->mtx);
    ptr->state = -1;
    if (event == EVENT_LBUTTONUP)
    {
        std::cout << "Button Up" << std::endl;        

		if( exitRect.contains(Point(x, y)) ) {			
			ptr->state = 99;			
		}
		else if( red2Rect.contains(Point(x, y)) ) {			
			ptr->state = Red2;			
		}
		else if( redRect.contains(Point(x, y)) ) {			
			ptr->state = Red;			
		}
		else if( blueRect.contains(Point(x, y)) ) {			
			ptr->state = Blue;			
		}
		else if( greenRect.contains(Point(x, y)) ) {			
			ptr->state = Green;			
		}
		else if( yellowRect.contains(Point(x, y)) ) {			
			ptr->state = Yellow;			
		}
    }   
    // unlock mutex
	pthread_mutex_unlock(&ptr->mtx);
}    


 int main( int argc, char** argv )
 {
		
	int camNumber = 0;
	
	if( argc > 1 )
		camNumber = 1;
		
	VideoCapture cap(camNumber); //capture the video from web cam

	if ( !cap.isOpened() )  // if not success, exit program
	{
		 cout << "Cannot open the web cam" << endl;
		 return -1;
	}

	
	namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"

	Mat imgControl(150,300, CV_8UC3, Scalar(200, 200, 200));
	
	custom_data_t my_data = { -1 };
	cvSetMouseCallback("Control", my_button_cb, &my_data ); 
		
	int iLowH = 0;
	int iHighH = 179;

	int iLowS = 0; 
	int iHighS = 255;

	int iLowV = 0;
	int iHighV = 255;

	//Create trackbars in "Control" window
	
	cvCreateTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
	cvCreateTrackbar("HighH", "Control", &iHighH, 179);
	cvCreateTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	cvCreateTrackbar("HighS", "Control", &iHighS, 255);
	cvCreateTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
	cvCreateTrackbar("HighV", "Control", &iHighV, 255);
	
	string buttonText("Exit");
	putText(imgControl, buttonText, Point(130, 130), FONT_HERSHEY_PLAIN, 1, Scalar(0,0,0));
	
	rectangle(imgControl,Point(0,0),Point(75,50),Scalar(0,0,128),CV_FILLED);
	rectangle(imgControl,Point(75,0),Point(150,50),Scalar(0,0,255),CV_FILLED);
	rectangle(imgControl,Point(150,0),Point(300,50),Scalar(255,0,0),CV_FILLED);
	rectangle(imgControl,Point(0,50),Point(150,100),Scalar(0,255,0),CV_FILLED);
	rectangle(imgControl,Point(150,50),Point(300,100),Scalar(0,255,255),CV_FILLED);
	
	
	imshow("Control", imgControl);

    
  while (true)
  {
	Mat imgOriginal;

	bool bSuccess = cap.read(imgOriginal); // read a new frame from video

    if (!bSuccess) //if not success, break loop
	{
		 cout << "Cannot read a frame from video stream" << endl;
		 break;
	}

    Mat imgHSV;

    cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
 
    Mat imgThresholded;

    inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image
      
    //morphological opening (remove small objects from the foreground)
	erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
	dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

    //morphological closing (fill small holes in the foreground)
    dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
    erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

    imshow("Thresholded Image", imgThresholded); //show the thresholded image
    imshow("Original", imgOriginal); //show the original image

    if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
    {
		cout << "esc key is pressed by user" << endl;
		break; 
    }
    
    pthread_mutex_lock(&my_data.mtx);
    //std::cout << "The state retrieved by the callback is: " << my_data.state << std::endl;    
    pthread_mutex_unlock(&my_data.mtx);
    
    if( my_data.state == 99 )
		return 0;
		
	if( my_data.state >= Red2 && my_data.state <= Yellow ) {
		cvSetTrackbarPos("LowH", "Control",  lowH[my_data.state]);
		cvSetTrackbarPos("HighH", "Control", highH[my_data.state]);
		cvSetTrackbarPos("LowS", "Control",  lowS[my_data.state]);
		cvSetTrackbarPos("HighS", "Control", highS[my_data.state]);
		cvSetTrackbarPos("LowV", "Control",  lowV[my_data.state]);
		cvSetTrackbarPos("HighV", "Control", highV[my_data.state]);		
	}
    
   }
   return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
