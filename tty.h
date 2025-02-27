#ifndef TTY_H
#define TTY_H

// #define NOMINMAX
#include <windows.h>

#include <string>
#include <vector>

struct TTY {
  TTY();
  virtual ~TTY();

  bool IsOK() const;

  void Connect(const std::string& port, int baudrate);
  void Disconnect();

  virtual void Write(const float& data);

  virtual void Write(const float data[4]);

  HANDLE m_Handle;
};

struct TTYException {};

#endif