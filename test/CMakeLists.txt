add_executable(test test.cpp)
target_include_directories(test PRIVATE ${PROJECT_SOURCE_DIR}/XuTL)
target_compile_features(test PRIVATE cxx_std_11)