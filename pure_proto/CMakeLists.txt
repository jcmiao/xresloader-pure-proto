set(app_name "pure_proto")

add_compile_options(-std=c++11)

include_directories(
    "D:\\src\\protobuf-3.15.1\\src"
    ${PROJECT_SOURCE_DIR}/pure_proto/pb
)
link_directories(
    "D:\\src\\protobuf-3.15.1\\cmake"   
)

file(GLOB SRCS "./pb/*.cc" "./pb/google/protobuf/*.cc" "./pb/v3/*.cc" "./*.cpp")
add_executable(${app_name} ${SRCS})

target_link_libraries(${app_name} protobuf)