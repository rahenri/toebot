#include <cstring>
#include <iostream>
#include <unistd.h>
#include <poll.h>

#include "line_reader.h"

#include <sys/select.h>

using namespace std;

LineReader LineReaderSingleton;

LineReader::LineReader() {
  memset(fds, 0, sizeof(fds));
  fds[0].fd = 0;
  fds[0].events = POLLIN;
}
int LineReader::ReadLine(string* str, bool blocking) {
  str->clear();
  while (1) {
    int ret = readMoreData(blocking);
    if (ret == EAGAIN && !blocking) {
      return EAGAIN;
    }
    if (ret < 0) {
      return ret;
    }
    if (ret == 0) {
      return str->size() > 0 ? 1 : 0;
    }
    for (;buffer_start < buffer_end;) {
      char c = buffer[buffer_start++];
      if (c == '\n' || c =='\r') {
        // Done reading a line
        return 1;
      }
      *str += c;
    }
  }
}

bool LineReader::HasData() {
  if (buffer_end > buffer_start) {
    return true;
  }
  return this->poll(0) > 0;
}

int LineReader::poll(int timeout) {
  int ret = ::poll(fds, 1, timeout);
  if (ret < 0) {
    cerr << "Poll failed: " << errno << endl;
  }
  return ret;
}

int LineReader::readMoreData(bool blocking) {
  if (buffer_end - buffer_start > 0) {
    return buffer_end - buffer_start;
  }
  if (!blocking && this->poll(1) <= 0) {
    return EAGAIN;
  }
  buffer_start = 0;
  int ret = read(0, buffer, sizeof(buffer));
  if (ret >= 0) {
    buffer_end = ret;
  } else {
    buffer_end = 0;
  }
  return ret;
}
