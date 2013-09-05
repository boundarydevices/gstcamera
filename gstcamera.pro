QT += core 
SOURCES += gstcamera.cpp
QMAKE_CXXFLAGS += `pkg-config \
    --cflags \
    gstreamer-0.10 `
QMAKE_LIBS += `pkg-config \
    --libs \
    gstreamer-0.10`
target.path = /usr/bin
INSTALLS += target
