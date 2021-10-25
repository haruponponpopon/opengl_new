
#include <cstring>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;


void convertColorToGray(cv::Mat &input, cv::Mat &processed);

int main(int argc, char *argv[])
{
    // 1. prepare Mat objects for input−image and output−image
    cv::Mat input, processed;

    input = cv::imread("abc.png", 1);
    if(input.empty()){
        fprintf(stderr, "cannot open the input file\n");
        exit(0);
    }
    int y_pos = 10;
    int y_len = 80;
    int x_len = 29.8;

    for (int i=0; i<26; i++){
        string num = to_string(i);
        processed =  cv::Mat(input, cv::Rect(16+x_len*i, y_pos, x_len,y_len));
        cv::imwrite("text_image/"+num+".jpg", processed);
    }
    // processed =  cv::Mat(input, cv::Rect(7, y_pos, 25,y_len));
    // cv::imwrite("a.jpg", processed);
    // processed =  cv::Mat(input, cv::Rect(50, y_pos, 30,y_len));
    // cv::imwrite("b.jpg", processed);
    // processed =  cv::Mat(input, cv::Rect(55, y_pos, 19,y_len));
    // cv::imwrite("c.jpg", processed);
    // processed =  cv::Mat(input, cv::Rect(74, y_pos, 19,y_len));
    // cv::imwrite("d.jpg", processed);
    // processed =  cv::Mat(input, cv::Rect(93, y_pos, 19,y_len));
    // string r = "c";
    // cv::imwrite("text_image/e.jpg", processed);
    // cv::imwrite("text_image/"+r+".jpg", processed);

    // 5. create windows
    // cv::namedWindow("original image", 1);
    // cv::namedWindow("processed image", 1);

    // 6. show images
    // cv::imshow("original image", input);
    // cv::imshow("processed image", processed);

    // 7. wait key input
    // cv::waitKey(0);

    // // 8. save the processed result
    // cv::imwrite("processed.jpg", processed);

    return 0;
}


void convertColorToGray(cv::Mat &input, cv::Mat &processed)
{

    // 4. convert color to gray
    cv::Mat temp;
    std::vector<cv::Mat> planes;
    cv::cvtColor(input, temp, CV_BGR2YCrCb);
    cv::split(temp, planes);
    processed = planes[0];
}
