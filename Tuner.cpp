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

enum Colours { Red, Green, Blue, Yellow, None };

//int lowY[5] = {   0, 150, 51,  75, 20 };
//int highY[5] = { 10, 230, 75, 107, 35 };

//int lowU[5] = {  158, 128, 127, 127,  85 };
//int highU[5] = { 255, 255, 255, 255, 255 };

//int lowV[5] = {  158,   0, 127, 127, 150 };
//int highV[5] = { 255, 255, 255, 255, 255 };

//int lowY[5] = {   0, 150, 90,  90, 20 };
//int highY[5] = { 10, 230, 119, 119, 35 };

//int lowU[5] = {  158, 128, 96, 200,    0 };
//int highU[5] = { 255, 255, 191, 255, 117 };

//int lowV[5] = {  158,   0,   0,   0, 150 };
//int highV[5] = { 255, 255, 255, 255, 255 };

int lowY[5] =    {   0, 150, 44,  90, 22 };
int highY[5] =   {  10, 230, 89, 119, 46 };

int lowU[5] =    {  71, 128,  30,  200,   0 };
int highU[5] =   { 255, 255, 166,  255, 132 };

int lowV[5] =    { 158,   0,  90,   0, 209 };
int highV[5] =   { 255, 255, 143, 255, 255 };

int Hue = 0;
int Saturation = 0;
int Value = 0;

typedef struct custom_data
{
    int state;
    pthread_mutex_t mtx;
} custom_data_t;


void Write() {
	FILE* myfile;
	myfile = fopen("config.txt","w");	
	
	fprintf(myfile, "let red_lower = core::Scalar { \n");
    fprintf(myfile, "    data: [%df64, %df64, %df64, -1f64], \n", lowY[Red], lowU[Red], lowV[Red]);
    fprintf(myfile, "}; \n");
    fprintf(myfile, "let red_upper = core::Scalar {  \n");
    fprintf(myfile, "    data: [%df64, %df64, %df64, -1f64], \n", highY[Red], highU[Red], highV[Red]);
    fprintf(myfile, "}; \n");

    fprintf(myfile, "let blue_lower = core::Scalar { \n");
    fprintf(myfile, "    data: [%df64, %df64, %df64, -1f64], \n", lowY[Blue], lowU[Blue], lowV[Blue]);
    fprintf(myfile, "}; \n");
    fprintf(myfile, "let blue_upper = core::Scalar { \n" );
    fprintf(myfile, "    data: [%df64, %df64, %df64, -1f64], \n", highY[Blue], highU[Blue], highV[Blue]);
    fprintf(myfile, "}; \n");

    fprintf(myfile, "let yellow_lower = core::Scalar { \n");
    fprintf(myfile, "    data: [%df64, %df64, %df64, -1f64], \n", lowY[Yellow], lowU[Yellow], lowV[Yellow]);
    fprintf(myfile, "}; \n");
    fprintf(myfile, "let yellow_upper = core::Scalar { \n");
    fprintf(myfile, "    data: [%df64, %df64, %df64, -1f64], \n", highY[Yellow], highU[Yellow], highV[Yellow]);
    fprintf(myfile, "}; \n");

    fprintf(myfile, "let green_lower = core::Scalar { \n");
    fprintf(myfile, "    data: [%df64, %df64, %df64, -1f64], \n", lowY[Green], lowU[Green], lowV[Green]);
    fprintf(myfile, "}; \n");
    fprintf(myfile, "let green_upper = core::Scalar { \n");
    fprintf(myfile, "    data: [%df64, %df64, %df64, -1f64], \n", highY[Green], highU[Green], highV[Green]);
    fprintf(myfile, "}; \n");
    
	fclose(myfile);
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
	
	Rect redRect    = Rect(0,0, 150, 50);
	Rect blueRect   = Rect(150,0, 150, 50);
	Rect greenRect  = Rect(0,50, 150, 50);
	Rect yellowRect = Rect(150,50, 150, 50);
    Rect exitRect 	= Rect(0,100,150,50);
    Rect saveRect 	= Rect(150,100, 150, 50);

	// lock mutex to protect data from being modified by the
	// main() thread
	pthread_mutex_lock(&ptr->mtx);
    ptr->state = -1;
    if (event == EVENT_LBUTTONUP)
    {
        //std::cout << "Button Up" << std::endl;        
		if( exitRect.contains(Point(x, y)) ) {			
			ptr->state = 99;		
		}
		else if(saveRect.contains(Point(x, y))) {						
			ptr->state = 98;
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
		
	int ilowY = 0;
	int ihighY = 255;

	int ilowU = 0; 
	int ihighU = 255;

	int iLowV = 0;
	int iHighV = 255;

	//Create trackbars in CONTROLYUV window
	
	cvCreateTrackbar("LowY", CONTROLYUV, &ilowY, 255); //Hue (0 - 179)
	cvCreateTrackbar("HighY", CONTROLYUV, &ihighY, 255);
	cvCreateTrackbar("LowU", CONTROLYUV, &ilowU, 255); //Saturation (0 - 255)
	cvCreateTrackbar("HighU", CONTROLYUV, &ihighU, 255);
	cvCreateTrackbar("LowV", CONTROLYUV, &iLowV, 255); //Value (0 - 255)
	cvCreateTrackbar("HighV", CONTROLYUV, &iHighV, 255);
	
	string buttonText("Exit");
	string saveText("Start  ");
	putText(imgControl, buttonText, Point(50, 130), FONT_HERSHEY_PLAIN, 1, Scalar(0,0,0));
	putText(imgControl, saveText, Point(180, 130), FONT_HERSHEY_PLAIN, 1, Scalar(0,0,0));	
	rectangle(imgControl,Point(0,0),Point(150,50),Scalar(0,0,255),CV_FILLED);
	rectangle(imgControl,Point(150,0),Point(300,50),Scalar(255,0,0),CV_FILLED);
	rectangle(imgControl,Point(0,50),Point(150,100),Scalar(0,255,0),CV_FILLED);
	rectangle(imgControl,Point(150,50),Point(300,100),Scalar(0,255,255),CV_FILLED);	
	
	imshow(CONTROLYUV, imgControl);

	Mat imgOriginal;
    
    Mat imgROI;
    
    Mat imgHSV;
    
    namedWindow("Colour Space", CV_WINDOW_AUTOSIZE);    
    setMouseCallback("Colour Space", onMouse, &imgHSV); // pass the address
    
    bool saving = false;
    
    int current = None;
    
	while (true)
	{
		bool bSuccess = cap.read(imgOriginal); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			 cout << "Cannot reamy_data.stated a frame from video stream" << endl;
			 break;
		}
		//Extract a region of interest from the grey scale frame
		Rect roi(0,0,640,80);  
		imgOriginal(roi).copyTo(imgROI);

		//cvtColor(imgROI, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
		cvtColor(imgROI, imgHSV, COLOR_BGR2YUV); //Convert the captured frame from BGR to YUV

		Mat imgThresholded;
		inRange(imgHSV, Scalar(ilowY, ilowU, iLowV), Scalar(ihighY, ihighU, iHighV), imgThresholded); //Threshold the image
		  
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
		
		// Use state 98 to enable and disable saving values
		if( my_data.state == 98 ) {
		  rectangle(imgControl,Point(150,100),Point(300,150),Scalar(200,200,200),CV_FILLED);
		  if( saving ) {
			  string saveText("Start   ");	
			  putText(imgControl, saveText, Point(180, 130), FONT_HERSHEY_PLAIN, 1, Scalar(0,0,0));	
			  cout << "Finish saving and write out file" << endl;				
			  Write();
			  saving = false;
		  } else  {
			  string saveText("Save  ");	
			  putText(imgControl, saveText, Point(180, 130), FONT_HERSHEY_PLAIN, 1, Scalar(0,0,0));	
			  cout << "Start saving" << endl;
			  saving = true;
		  }		 
		  imshow(CONTROLYUV, imgControl);
		}
		
		if( my_data.state >= Red && my_data.state <= Yellow ) {
			current = my_data.state;
			if( saving ) {
				rectangle(imgControl,Point(150,100),Point(300,150),Scalar(200,200,200),CV_FILLED);			
				if( current == Red ) {
					string saveText("Saving Red");	
					putText(imgControl, saveText, Point(180, 130), FONT_HERSHEY_PLAIN, 1, Scalar(0,0,0));	
				}
				if( current == Blue ) {
					string saveText("Saving Blue");	
					putText(imgControl, saveText, Point(180, 130), FONT_HERSHEY_PLAIN, 1, Scalar(0,0,0));	
				}
				if( current == Green ) {
					string saveText("Saving Green");	
					putText(imgControl, saveText, Point(180, 130), FONT_HERSHEY_PLAIN, 1, Scalar(0,0,0));	
				}
				if( current == Yellow ) {
					string saveText("Saving Yellow");	
					putText(imgControl, saveText, Point(180, 130), FONT_HERSHEY_PLAIN, 1, Scalar(0,0,0));	
				}				
				imshow(CONTROLYUV, imgControl);
			}
		}
		
		if( saving && Hue > 0 ) {				
			lowY[current]  = Hue - 15;
			highY[current] = Hue + 15;
			lowU[current]  = Saturation - 15;
			highU[current] = Saturation + 15;
			lowV[current]  = Value - 15;
			highV[current] = Value + 15;
			Hue = 0;
		}
		
		if( current >= Red && current <= Yellow ) {			
			cvSetTrackbarPos("LowY", CONTROLYUV,  lowY[current]);
			cvSetTrackbarPos("HighY", CONTROLYUV, highY[current]);
			cvSetTrackbarPos("LowU", CONTROLYUV,  lowU[current]);
			cvSetTrackbarPos("HighU", CONTROLYUV, highU[current]);
			cvSetTrackbarPos("LowV", CONTROLYUV,  lowV[current]);
			cvSetTrackbarPos("HighV", CONTROLYUV, highV[current]);			
		} 		    		
		
		pthread_mutex_lock(&my_data.mtx);
		my_data.state = None;
		pthread_mutex_unlock(&my_data.mtx);	
   }
   return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
