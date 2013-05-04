/*
  
  Copyright 2012 Lucas Walter

     This file is part of controlrecorder.

    controlrecorder is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    controlrecorder is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with controlrecorder.  If not, see <http://www.gnu.org/licenses/>.


  Joystick reading part of this code from
  jstest.c  Version 1.2
  Copyright (c) 1996-1999 Vojtech Pavlik
*/

#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>

#include <iostream>
#include <sstream>
#include <deque>

#include <boost/lexical_cast.hpp>
#include <boost/timer.hpp>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <glog/logging.h>
#include <gflags/gflags.h>
#include <linux/joystick.h>


#define NAME_LENGTH 128

/**
  *
  */
int main( int argc, char* argv[] )
{
  google::InitGoogleLogging(argv[0]);
  google::LogToStderr();
  google::ParseCommandLineFlags(&argc, &argv, false);


  int fd;
  unsigned char axes = 2;
  unsigned char buttons = 2;
  int version = 0x000800;
  char name[NAME_LENGTH] = "Unknown";

  if ((fd = open("/dev/input/js0", O_RDONLY)) < 0) {
    perror("controlrecorder");
    exit(1);
  }

  ioctl(fd, JSIOCGVERSION, &version);
  ioctl(fd, JSIOCGAXES, &axes);
  ioctl(fd, JSIOCGBUTTONS, &buttons);
  ioctl(fd, JSIOCGNAME(NAME_LENGTH), name);



  struct js_event js;

  fcntl(fd, F_SETFL, O_NONBLOCK);

  cv::Mat im = cv::Mat(600, 800, CV_8UC3, cv::Scalar::all(0));

  std::vector<int> axis;
  std::vector<int> button;
  
  axis.resize(axes);
  button.resize(buttons);

  const bool write_output = false;

  cv::FileStorage fs;
  if (write_output) {
  fs.open("joystick.yml", cv::FileStorage::WRITE);

  time_t rawtime; time(&rawtime);
  fs << "date" << asctime(localtime(&rawtime));

  LOG(INFO) << "Joystick (" << name << ") has " 
      << axes << " axes and " << buttons << " buttons. Driver version is "
      << (version >> 16) << "."  << ((version >> 8) & 0xff) << "."
      << (version & 0xff);

  fs << "name" << name;
  fs << "axes" << axes;
  fs << "buttons" << buttons;
  fs << "version" << version;

  fs << "events" << "[";
  }

  cv::Point2f old;
  cv::Point2f pos = cv::Point2f(im.cols/2, im.rows/2);
  cv::Point2f vel;

  int ind = 1000000;
  bool do_loop = true;
  while (do_loop) {

    old = pos;
    int i = 0;
    while ((read(fd, &js, sizeof(struct js_event)) == sizeof(struct js_event)) 
        && (i < 25))  {
      //LOG(INFO) << "Event: type " << js.type << ", time " << js.time
      //    << ", number " << js.number << ", value " << js.value;

      if (errno != EAGAIN) {
        perror("\ncontrolrecorder: error reading");
        //do_loop = false;
        //break;
      }

      if (write_output) {
        fs << "{:";
        fs << "ind" << ind;
        fs << "time" << (int)js.time;
        fs << "type" << (int)js.type;
        fs << "number" << (int)js.number;
        fs << "value" << (int) js.value;
        fs << "}";
      }
      ind++;
      i++;
      switch(js.type & ~JS_EVENT_INIT) {
          case JS_EVENT_BUTTON:
          button[js.number] = js.value;
          break;
          case JS_EVENT_AXIS:
          axis[js.number] = js.value;
          break;
      }

    }

    {
      float fr = 2.25;
      float raw_x = ((float)axis[0])/(32768.0);
      float sign_x = (raw_x > 0) * 2.0 - 1.0;
      raw_x *= fr;
      raw_x *= raw_x * sign_x;

      vel.x += raw_x;

      float raw_y = ((float)axis[1])/(32768.0);
      float sign_y = (raw_y > 0) * 2.0 - 1.0;
      raw_y *= fr;
      raw_y *= raw_y;
      vel.y += raw_y * sign_y;

      fr = 0.65;
      pos.x += vel.x;
      pos.y += vel.y;
      vel.x *= fr;
      vel.y *= fr;
      if (pos.x > im.cols) {
        pos.x = im.cols;
        vel.x = 0;
      }
      if (pos.y > im.rows) {
        pos.y = im.rows;
        vel.y = 0;
      }
      if (pos.x < 0) {
        pos.x = 0;
        vel.x = 0;
      }
      if (pos.y < 0) {
        pos.y = 0;
        vel.y = 0;
      }
          //LOG(INFO) << pt.x << " " << pt.y;

          cv::Scalar col = cv::Scalar(
              (axis[2] + 32768)/256,
              (axis[3] + 32768)/256,
              230
              );

          cv::line(im,
              old,
              pos,
              col,
              4,4
              );   
    }


    cv::imshow("im", im);
    char d = cv::waitKey(100);
    if (d == 'q') { do_loop = false; };
  }
  
  fs << "]";
  fs.release();
}
