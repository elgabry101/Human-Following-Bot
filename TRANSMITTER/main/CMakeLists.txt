idf_component_register(SRCS "main.cpp" "${CMAKE_SOURCE_DIR}/utils/Src/GPIO.cpp"
                            "${CMAKE_SOURCE_DIR}/utils/Src/Sensor.cpp"
                            "${CMAKE_SOURCE_DIR}/utils/Src/Timer.cpp"
                            "${CMAKE_SOURCE_DIR}/utils/Src/Comm.cpp"
                            "${CMAKE_SOURCE_DIR}/utils/Src/Compass.cpp"
                    INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/utils/Headers")
