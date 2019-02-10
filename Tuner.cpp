////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <pthread.h>
#include <fstream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#define CONTROLYUV "Control YUV"
#define CONTROLHSV "Control HSV"

using namespace cv;
using namespace std;

enum Colours { Red2, Red, Green, Blue, Yellow, None };

//int lowH[5] = {   0, 150, 51,  75, 20 };
//int highH[5] = { 10, 230, 75, 107, 35 };

//int lowS[5] = {  158, 128, 127, 127,  85 };
//int highS[5] = { 255, 255, 255, 255, 255 };

//int lowV[5] = {  158,   0, 127, 127, 150 };
//int highV[5] = { 255, 255, 255, 255, 255 };

//int lowH[5] = {   0, 150, 90,  90, 20 };
//int highH[5] = { 10, 230, 119, 119, 35 };

//int lowS[5] = {  158, 128, 96, 200,    0 };
//int highS[5] = { 255, 255, 191, 255, 117 };

//int lowV[5] = {  158,   0,   0,   0, 150 };
//int highV[5] = { 255, 255, 255, 255, 255 };

int lowH[5] =    {   0, 150, 44,  90, 22 };
int highH[5] =   {  10, 230, 89, 119, 46 };

int lowS[5] =    {  71, 128,  30,  200,   0 };
int highS[5] =   { 255, 255, 166,  255, 132 };

int lowV[5] =    { 158,   0,  90,   0, 209 };
int highV[5] =   { 255, 255, 143, 255, 255 };

int Hue = 0;
int Saturation = 0;
int Value = 0;

typedef struct colour_store {
	string name;
	int hue;
	int sat;
	int value;
} colour_store_t;

typedef struct custom_data
{
    int state;
	colour_store_t red;
	colour_store_t red2;
	colour_store_t blue;
	colour_store_t green;
	colour_store_t yellow;
    pthread_mutex_t mtx;
} custom_data_t;


void Write(string filename, custom_data_t data) {
	ofstream myfile;
	myfile.open(filename);
	// iterate data
	myfile << '[{"colorName": "' + data.red.name + '", "hue": "' + data.red.hue + '", "saturation": "' + data.red.sat + '", "value": "' + data.red.value + '"},\n';
	myfile << '{"colorName": "' + data.red2.name + '", "hue": "' + data.red2.hue + '", "saturation": "' + data.red2.sat + '", "value": "' + data.red2.value + '"},\n';
	myfile << '[{"colorName": "' + data.green.name + '", "hue": "' + data.green.hue + '", "saturation": "' + data.green.sat + '", "value": "' + data.green.value + '"},\n';
	myfile << '[{"colorName": "' + data.yellow.name + '", "hue": "' + data.yellow.hue + '", "saturation": "' + data.yellow.sat + '", "value": "' + data.yellow.value + '"},\n';
	myfile << '[{"colorName": "' + data.blue.name + '", "hue": "' + data.blue.hue + '", "saturation": "' + data.blue.sat + '", "value": "' + data.blue.value + '"}]\n';
	myfile.close();
}

void StoreToPtr(void* userdata) {
	custom_data_t* ptr = (custom_data_t*)userdata;
    if (!ptr)
    {
        std::cout << "@Save userdata is empty" << std::endl;
        return;
    }
	pthread_mutex_lock(&ptr->mtx);
	char[] stateName;
    switch(ptr->state) {
		case Red:
		ptr.red = colour_store_t{
			name: string("red"),
			hue: Hue,
			sat: Saturation,
			value: Value,
		};
		break;
		case Blue:
		ptr.blue = colour_store_t{
			name: string("blue"),
			hue: Hue,
			sat: Saturation,
			value: Value,
		};
		break;
		case Yellow:
		ptr.green = colour_store_t{
			name: string("green"),
			hue: Hue,
			sat: Saturation,
			value: Value,
		};
		break;
		case Red2:
		ptr.red = colour_store_t{
			name: string("red2"),
			hue: Hue,
			sat: Saturation,
			value: Value,
		};
		break;
		case Green:
		ptr.green = colour_store_t{
			name: string("green"),
			hue: Hue,
			sat: Saturation,
			value: Value,
		};
		break;
	}
}

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
    Rect saveRect = Rect(0,100,150,50);
    Rect exitRect = Rect(150,100, 150, 50);

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
		else if(saveRect.contains(Point(x, y))) {
			StoreToPtr(userdata);
			ptr->state = 98;
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

void onMouse(int event, int x, int y, int flags, void* param) // now it's in param
{
    Mat &img = *((Mat*)param); //cast and deref the param

    if (event == EVENT_LBUTTONDOWN)
    {
        Vec3b val = img.at<Vec3b>(y,x); // opencv is row-major! 
        cout << "x= " << x << " y= " << y << "val= "<<val<< endl;
        
        Hue = val[0];
        Saturation = val[1];
        Value = val[2];
    }
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

	
	namedWindow(CONTROLYUV, CV_WINDOW_AUTOSIZE); //create a window called CONTROLYUV

	Mat imgControl(150,300, CV_8UC3, Scalar(200, 200, 200));
	
	custom_data_t my_data = { -1 };
	cvSetMouseCallback(CONTROLYUV, my_button_cb, &my_data ); 
		
	int iLowH = 0;
	int iHighH = 255;

	int iLowS = 0; 
	int iHighS = 255;

	int iLowV = 0;
	int iHighV = 255;

	//Create trackbars in CONTROLYUV window
	
	cvCreateTrackbar("LowY", CONTROLYUV, &iLowH, 255); //Hue (0 - 179)
	cvCreateTrackbar("HighY", CONTROLYUV, &iHighH, 255);
	cvCreateTrackbar("LowU", CONTROLYUV, &iLowS, 255); //Saturation (0 - 255)
	cvCreateTrackbar("HighU", CONTROLYUV, &iHighS, 255);
	cvCreateTrackbar("LowV", CONTROLYUV, &iLowV, 255); //Value (0 - 255)
	cvCreateTrackbar("HighV", CONTROLYUV, &iHighV, 255);
	
	string buttonText("Exit");
	putText(imgControl, buttonText, Point(130, 130), FONT_HERSHEY_PLAIN, 1, Scalar(0,0,0));
	
	rectangle(imgControl,Point(0,0),Point(75,50),Scalar(0,0,128),CV_FILLED);
	rectangle(imgControl,Point(75,0),Point(150,50),Scalar(0,0,255),CV_FILLED);
	rectangle(imgControl,Point(150,0),Point(300,50),Scalar(255,0,0),CV_FILLED);
	rectangle(imgControl,Point(0,50),Point(150,100),Scalar(0,255,0),CV_FILLED);
	rectangle(imgControl,Point(150,50),Point(300,100),Scalar(0,255,255),CV_FILLED);
	
	
	imshow(CONTROLYUV, imgControl);

	Mat imgOriginal;
    
    Mat imgROI;
    
    Mat imgHSV;
    
    namedWindow("Colour Space", CV_WINDOW_AUTOSIZE);    
    setMouseCallback("Colour Space", onMouse, &imgHSV); // pass the address
    
	while (true)
	{
		bool bSuccess = cap.read(imgOriginal); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			 cout << "Cannot read a frame from video stream" << endl;
			 break;
		}

		//Extract a region of interest from the grey scale frame
		Rect roi(0,0,640,80);  
		imgOriginal(roi).copyTo(imgROI);

		//cvtColor(imgROI, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
		cvtColor(imgROI, imgHSV, COLOR_BGR2YUV); //Convert the captured frame from BGR to YUV

		Mat imgThresholded;
		inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image
		  
		//morphological opening (remove small objects from the foreground)
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
		dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

		//morphological closing (fill small holes in the foreground)
		dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

		Moments oMoments = moments(imgThresholded);
		Point2f center(oMoments.m10 / oMoments.m00, 320);
		circle( imgThresholded, center, 5, Scalar(128, 128, 128), -1, 8, 0);

		imshow("Thresholded Image", imgThresholded); //show the thresholded image
		imshow("Colour Space", imgHSV);
		imshow("Original", imgROI); //show the original image

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
		
		if( my_data.state == 98 ) {
			Write(string("config.json"), my_data)
		}
		if( my_data.state >= Red2 && my_data.state <= Yellow ) {
			/*
			cvSetTrackbarPos("LowH", CONTROLHSV,  lowH[my_data.state]);
			cvSetTrackbarPos("HighH", CONTROLHSV, highH[my_data.state]);
			cvSetTrackbarPos("LowS", CONTROLHSV,  lowS[my_data.state]);
			cvSetTrackbarPos("HighS", CONTROLHSV, highS[my_data.state]);
			cvSetTrackbarPos("LowV", CONTROLHSV,  lowV[my_data.state]);
			cvSetTrackbarPos("HighV", CONTROLHSV, highV[my_data.state]);		
			*/
			
			cvSetTrackbarPos("LowY", CONTROLYUV,  lowH[my_data.state]);
			cvSetTrackbarPos("HighY", CONTROLYUV, highH[my_data.state]);
			cvSetTrackbarPos("LowU", CONTROLYUV,  lowS[my_data.state]);
			cvSetTrackbarPos("HighU", CONTROLYUV, highS[my_data.state]);
			cvSetTrackbarPos("LowV", CONTROLYUV,  lowV[my_data.state]);
			cvSetTrackbarPos("HighV", CONTROLYUV, highV[my_data.state]);
			pthread_mutex_lock(&my_data.mtx);
			my_data.state = None;
			pthread_mutex_unlock(&my_data.mtx);		
		} 
		else if( Hue > 0) {
			cvSetTrackbarPos("LowY", CONTROLYUV,  Hue - 15);
			cvSetTrackbarPos("HighY", CONTROLYUV, Hue + 15);
			cvSetTrackbarPos("LowU", CONTROLYUV,  Saturation - 15);
			cvSetTrackbarPos("HighU", CONTROLYUV, Saturation + 15);
			cvSetTrackbarPos("LowV", CONTROLYUV,  Value - 15);
			cvSetTrackbarPos("HighV", CONTROLYUV, Value + 15);
			Hue = 0;
		}
		
    
   }
   return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
