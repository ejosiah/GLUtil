#pragma once

#include <vector>
#include "gl/logger.h"


static ncl::ConsoleAppender CONSOLE_APPENDER;
static ncl::FileAppender FILE_APPENDER("log.txt");


std::vector<ncl::Appender*> ncl::Logger::appenders = { &CONSOLE_APPENDER,  &FILE_APPENDER };
const unsigned int ncl::Logger::level = LEVEL_DEBUG;