//
// Created by valy on 10/31/24.
//

#ifndef LOG_H
#define LOG_H

//#define LOG_ENABLED

#define LOG_BAUDRATE 921600
#define LOG_QUEUE_SIZE 128
#define LOG_BUFFER_SIZE 1024


void LOG_init(void);
void LOG_SERIAL(const char *format, ...);

#endif //LOG_H
