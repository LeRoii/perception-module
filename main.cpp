// Compiler: MSVC 19.29.30038.1
// C++ Standard: C++17
#include <iostream>
#include <unistd.h>
#include <thread>
#include <opencv2/opencv.hpp>
#include <X11/Xlib.h>
#include "top.h"
using namespace std;


// int main(){
//     unsigned char ch[2];
//     ch[0] = 0x01;
//     ch[1] = 0x2C;

//     int num = int(ch[0] << 8) + int(ch[1]);
//     std::cout << num;

//     return 0;
// }

int main(int argc, char *argv[]){

    // std::vector<cv::Point2f> ptSrc,ptDst;
    // ptSrc.push_back(cv::Point2f(426,411));
    // ptSrc.push_back(cv::Point2f(469,19));
    // ptSrc.push_back(cv::Point2f(558,14));
    // ptSrc.push_back(cv::Point2f(340,347));
    // ptSrc.push_back(cv::Point2f(339,294));
    // ptSrc.push_back(cv::Point2f(71,137));

    // ptDst.push_back(cv::Point2f(745,504));
    // ptDst.push_back(cv::Point2f(788,96));
    // ptDst.push_back(cv::Point2f(880,91));
    // ptDst.push_back(cv::Point2f(655,440));
    // ptDst.push_back(cv::Point2f(653,383));
    // ptDst.push_back(cv::Point2f(371,225));

    // cv::Mat h = cv::findHomography(ptSrc,ptDst);

    // std::cout<<h<<std::endl;
    // return 0;

    int opt = 0;
    bool open_vis = 0; bool open_ir = 0; bool open_fusion = 0; bool get_capture = 0;; bool kcf = 0;

    Top top;
    top.run();
    // thread visiRun(&Top::visiRun, &top);
    // thread thermelRun(&Top::thermelRun, &top);
    // thread capture(&Top::get_capture, &top);

    // capture.join();
    // visiRun.detach();
    // thermelRun.detach();

	return 0;
}
