PROJECT( run_detector )
FIND_PACKAGE( OpenCV REQUIRED )

ADD_EXECUTABLE(run_detector detect_identifier.cpp)
TARGET_LINK_LIBRARIES( run_detector ${OpenCV_LIBS} )
