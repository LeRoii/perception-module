#ifndef __IRCAPTURE__
#define __IRCAPTURE__

#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include<iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <ctime>
#include "nvrender.h"

#include<sys/socket.h>
#include<netinet/in.h>
#include<errno.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>

#include "serial.h"

#include "camera_v4l2_cuda.h"

#ifdef WIN32
#define UNIV_CALLBACK __stdcall
#else
#define UNIV_CALLBACK
#endif

#define _PORT_ 6665
#define _BACKLOG_ 10

enum enDispalyMode
{
  EN_DISPLAY_MODE_VIS = 1,
  EN_DISPLAY_MODE_IR = 2,
  EN_DISPLAY_MODE_FUS = 3
};

enum enCtrlMode
{
  EN_CTRL_MODE_FREE = 0,
  EN_CTRL_MODE_AUTOTRACK = 1,
  EN_CTRL_MODE_OB = 2,
  EN_CTRL_MODE_MANUALTRACK = 3
};

struct stServoData
{
  enCtrlMode mode;
  int yaw;
  int pitch;
  short offsetX;
  short offsetY;
};

const float AUTO_TRACK_SIZE_L = 128;
const float AUTO_TRACK_SIZE_M = 64;
const float AUTO_TRACK_SIZE_S = 32;


class Top{
public:
    //constructor
    Top();
    ~Top();

    // main
    int run();

    // getting frame from capture
    void visiRun();
    void thermelRun();

    // main function
    int vis_ori();
    int ir_ori();
    int fusion_ori();
    int only_vis();
    int only_ir();
    int only_fusion();
    int kcf_tracking();
    int tracking();
    int data_transfer();
    int serial_transfer();
    int razer_serial_rec();
    void set_focal(int f);

    // test using function
    void keyboard();
    void display();
    void get_capture();
    void yolotest();

public:

  Serial serial_viscam;
  Serial serial_razer;
  Serial serial_servo;
    
private:
    cv::Mat fusion(cv::Mat &vis, cv::Mat &ir, float gamma);
    cv::Mat search_asyn(std::vector<cv::Mat> &buffer, std::vector<long> &stamp_buf, long stamp_now);
    void find_asyn(std::vector<cv::Mat> &buffer_v, std::vector<cv::Mat> &buffer_t, std::vector<long> &stamp_v, std::vector<long> &stamp_t, cv::Mat &vis, cv::Mat &thermel); 
    void encode_tcp_data();
    void decode_tcp_data();
    void tcp_send();
    void tcp_rec();
    void init_buffSenData_razer();

private:
  // test visible capture
  std::string filename0;
  std::string filename1;
  cv::VideoCapture cap0;
  cv::VideoCapture cap1;

  // frame buffer
  std::vector<cv::Mat> buffer_visi;
  std::vector<cv::Mat> buffer_thermel;
  std::vector<long> stamp_visi;
  std::vector<long> stamp_thermel;
  std::vector<cv::Mat> show_buffer;

  // tracking
  int start_tracking;
  int is_initialised;

  //detection
  int is_detection;
  int object_count;
  
  std::vector<cv::Rect> bbox_id;
  int track_id;

  //result what shangweiji need
  int* detection_num;
  int* tb;

  // tcp
  // int client_sock;
  void* client_socket;
  int sock;
  struct sockaddr_in server_socket;
  struct sockaddr_in socket_value;
  unsigned char* param;   // the parameter we send to shangweiji
  unsigned char* rec_param; // the parameter we reieve from shangweiji
  

  // serial
  int is_focal;
  int focal_rec;
  int focal_razer_rec;
  

  // mode type
  // mode_frame = 1, 2, 3  vis/ir/fusion
  // mode_fun = 1, 2, 3  ori/detect/track
  int mode_frame;
  int mode_fun;  

  v4l2 *visCam;
  v4l2 *thermalCam;

  enDispalyMode m_enDispMode;
  enCtrlMode m_enCtrlMode;
};

int is_detec_distane;

#endif
