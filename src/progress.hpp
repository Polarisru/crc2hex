#pragma once

#include <string>
#include <iostream>
#include <iomanip>

using namespace std;

class Progress {
public:
  Progress(const string& prefix, int top) {
    top_ = top;
    pos_ = 0;
    prefix_ = prefix;
    show();
  }
  ~Progress() {
    cout << endl;
  }
  void inc() {
    pos_++;
    show();
  }
private:
  int top_;
  int pos_;
  string prefix_;
  void show() {
    double f_pos = static_cast<double>(pos_) / top_ * 100;
    cout << fixed;
    cout.precision(1);
    cout << "\r" << prefix_ << ": \t[" << setw(4) << setfill(' ') << f_pos << "%]" << flush;
  }
};
