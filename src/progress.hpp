#pragma once

#include <string>
#include <iostream>
#include <iomanip>

class Progress {
public:
  Progress(const std::string& prefix, int top) {
    top_ = top;
    pos_ = 0;
    prefix_ = prefix;
    show();
  }
  ~Progress() {
    std::cout << std::endl;
  }
  void inc() {
    pos_++;
    show();
  }
private:
  int top_;
  int pos_;
  std::string prefix_;
  void show() {
    double f_pos = static_cast<double>(pos_) / top_ * 100;
    std::cout << std::fixed;
    std::cout.precision(1);
    std::cout << "\r" << prefix_ << ": \t[" << std::setw(4) << std::setfill(' ') << f_pos << "%]" << std::flush;
  }
};
