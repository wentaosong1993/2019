DISTFILES += \
    samples/Android/jni/Android.mk \
    samples/Android/jni/Application.mk \
    samples/CMakeLists.txt

HEADERS += \
    plog/Appenders/AndroidAppender.h \
    plog/Appenders/ColorConsoleAppender.h \
    plog/Appenders/ConsoleAppender.h \
    plog/Appenders/DebugOutputAppender.h \
    plog/Appenders/EventLogAppender.h \
    plog/Appenders/IAppender.h \
    plog/Appenders/RollingFileAppender.h \
    plog/Converters/NativeEOLConverter.h \
    plog/Converters/UTF8Converter.h \
    plog/Formatters/CsvFormatter.h \
    plog/Formatters/FuncMessageFormatter.h \
    plog/Formatters/MessageOnlyFormatter.h \
    plog/Formatters/TxtFormatter.h \
    plog/Init.h \
    plog/Log.h \
    plog/Logger.h \
    plog/Record.h \
    plog/Severity.h \
    plog/Util.h \
    plog/WinApi.h \
    samples/Demo/Customer.h \
    samples/Demo/MyClass.h

SOURCES += \
#    samples/Android/jni/Sample.cpp \
#    samples/Chained/ChainedApp/Main.cpp \
#    samples/Chained/ChainedLib/Main.cpp \
#    samples/ColorConsole/Main.cpp \
#    samples/CustomAppender/Main.cpp \
#    samples/CustomConverter/Main.cpp \
#    samples/CustomFormatter/Main.cpp \
#    samples/CustomType/Main.cpp \
#    samples/DebugOutput/Main.cpp \
    samples/Demo/Main.cpp \
    samples/Demo/MyClass.cpp \
#    samples/EventLog/Main.cpp \
#    samples/Facilities/Main.cpp \
#    samples/Hello/Main.cpp \
#    samples/Library/LibraryApp/Main.cpp \
#    samples/Library/LibraryLib/Lib.cpp \
#    samples/MultiAppender/Main.cpp \
#    samples/MultiInstance/Main.cpp \
#    samples/NativeEOL/Main.cpp \
#    samples/Performance/Main.cpp \
#    samples/ObjectiveC/Main.mm
