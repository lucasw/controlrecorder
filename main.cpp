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
    perror("jstest");
    exit(1);
  }

  ioctl(fd, JSIOCGVERSION, &version);
  ioctl(fd, JSIOCGAXES, &axes);
  ioctl(fd, JSIOCGBUTTONS, &buttons);
  ioctl(fd, JSIOCGNAME(NAME_LENGTH), name);


  LOG(INFO) << "Joystick (" << name << ") has " 
      << axes << " axes and " << buttons << " buttons. Driver version is "
      << (version >> 16) << "."  << ((version >> 8) & 0xff) << "."
      << (version & 0xff);

  struct js_event js;

  fcntl(fd, F_SETFL, O_NONBLOCK);

  cv::Mat im = cv::Mat(600, 800, CV_8UC3, cv::Scalar::all(0));

  std::vector<int> axis;
  std::vector<int> button;
  
  axis.resize(axes);
  button.resize(buttons);

  while (1) {

    int i = 0;
    while ((read(fd, &js, sizeof(struct js_event)) == sizeof(struct js_event)) && (i< 50))  {
      //LOG(INFO) << "Event: type " << js.type << ", time " << js.time
      //    << ", number " << js.number << ", value " << js.value;
      
      i++;
      switch(js.type & ~JS_EVENT_INIT) {
          case JS_EVENT_BUTTON:
          button[js.number] = js.value;
          break;
          case JS_EVENT_AXIS:
          axis[js.number] = js.value;
          break;
      }
 
          cv::Point2f pt = cv::Point2f( 
              im.cols*((float)axis[0] + 32768.0)/(65536.0),
              im.rows*((float)axis[1] + 32768.0)/(65536.0));

          //LOG(INFO) << pt.x << " " << pt.y;

          cv::Scalar col = cv::Scalar(
              (axis[2] + 32768)/256,
              (axis[3] + 32768)/256,
              230
              );

          cv::circle(im,
              pt,
              3,
              col
              );   
    }

    if (errno != EAGAIN) {
      perror("\njstest: error reading");
      exit (1);
    }


    cv::imshow("im", im);
    char d = cv::waitKey(100);
    if (d == 'q') exit(0);
  }

}
