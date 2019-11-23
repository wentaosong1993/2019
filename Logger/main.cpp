#include <iostream>
#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>
using namespace std;


int main()
{
    static plog::RollingFileAppender<plog::CsvFormatter> fileAppender("MultiAppender.csv", 8000, 3); // Create the 1st appender.
    static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender; // Create the 2nd appender.
    plog::init(plog::debug, &fileAppender).addAppender(&consoleAppender); // Initialize the logger with the both appenders.

    LOGW << "Hello World!\n";

    return 0;
}

