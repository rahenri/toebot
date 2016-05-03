#ifndef LINE_READER_H
#define LINE_READER_H

#include <string>
#include <poll.h>

class LineReader {
  public:
    LineReader();
    int ReadLine(std::string* str);

    bool HasData();
  private:
    int readMoreData();
    char buffer[1024];
    int buffer_start = 0, buffer_end = 0;
    struct pollfd fds[1];
};

extern LineReader LineReaderSingleton;

#endif
