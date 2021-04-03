
 
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;
int main() {

    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        cout << "Error opening video stream" << endl;
        return -1;
    }
    namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"

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

    
    
    
    
    while (cap.isOpened())
    {
        Mat hsv, skin_mask, mask, blur;
        Mat thresh, area_contour;
        cv::Mat frame, result;
        vector<vector<Point> > contour;
        vector<cv::Vec4i> hierarchy;
        Rect bounding_rect;

        cap >> frame;                                                          

      



        cv::cvtColor(frame, hsv, CV_BGR2HSV);  //Color conversion
       
        cv::inRange(hsv, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), skin_mask);//Skin tone segmentation
        
        imshow("Thresholded Image", skin_mask);
       


        int an = 5, blurSize = 7;
        cv::medianBlur(skin_mask, skin_mask, blurSize);  //Image smoothing

        Mat kernel = cv::getStructuringElement(MORPH_ELLIPSE, Size(an * 2 + 1, an * 2 + 1), Point(an, an));// Creating a structureing element kernel
        cv::erode(skin_mask, skin_mask, kernel);   //Erosion
        cv::dilate(skin_mask, skin_mask, kernel);  //Dilation
 

//        cv::namedWindow("skin_segment1",1);
        cv::imshow("skin_segment1", skin_mask);



        int a = frame.rows;
     
        Mat labelImage(frame.size(), CV_32S);
        for (int i = 0; i < 25; i++) {
            cv::line(skin_mask, { 0, a }, { frame.cols, a }, Scalar(0, 0, 0), 30, 8);
            a = a - 30;
            int label = connectedComponents(skin_mask, labelImage, 8);  //returns number of connected components

            if (label > 2) //i.e if there are more than one connected components with white pixels 
                break;
        }
        cv::namedWindow("skin_segment", 1);
        cv::imshow("skin_segment", skin_mask);

        

        cv::findContours(skin_mask, contour, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);//Finding the contour

        


        Point2f rect_points[4];
        for (size_t i = 0; i < contour.size(); i++) {
            double area = contourArea(contour[i]);
            if (area > 500) {
                RotatedRect box = minAreaRect(contour[i]);
                box.points(rect_points);
               
                Point2f center = box.center;
                double width = box.size.width;
                double height = box.size.height;
             

              
                if (height > width) { //reducing the size of rectangle
                    box.size.height = (float)(0.33) * box.size.height;
                    box.center = (rect_points[1] + rect_points[2]) / 2 + (rect_points[0] - rect_points[1]) / 6;
                    //box.center = (rect_points[1] + rect_points[2]) / 2 + (rect_points[0] - rect_points[1]) / 7;
                }
                else {
                    box.size.width = (float)(0.33) * box.size.width;
                    box.center = (rect_points[2] + rect_points[3]) / 2 + (rect_points[0] - rect_points[3]) / 6;
                    //box.center = (rect_points[2] + rect_points[3]) / 2 + (rect_points[0] - rect_points[3]) / 7;
                }
                box.points(rect_points);
                Point2f center1 = box.center;

                
                for (int j = 0; j < 4; j++) {
                    line(frame, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 255, 0), 2, 8); // Drawing lines around the rectangle
                }

               // drawContours( frame, contour, i, Scalar( 0, 255, 0 ), 1 ); // Draw the largest contour
            }
        }

        cv::namedWindow("ROI", 1);
        cv::imshow("ROI", frame);
        if (cv::waitKey(30) >= 0) break;
    }

    cap.release();
    waitKey(1);


}

