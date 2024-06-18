idf_component_register(SRCS "main.cpp" "${CMAKE_SOURCE_DIR}/utils/Src/GPIO.cpp" 
                            "${CMAKE_SOURCE_DIR}/utils/Src/Sensor.cpp" 
                            "${CMAKE_SOURCE_DIR}/utils/Src/Timer.cpp" 
                            "${CMAKE_SOURCE_DIR}/utils/Src/Motor.cpp"
                            "${CMAKE_SOURCE_DIR}/utils/Src/Navigator.cpp"
                            "${CMAKE_SOURCE_DIR}/utils/Src/Compass.cpp"
                            "${CMAKE_SOURCE_DIR}/utils/Src/comm.cpp"
                    INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/utils/Headers")
