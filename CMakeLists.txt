# Specify the include directory for univalue
include_directories(C:/Users/pc/Desktop/Vectorium/univalue/include)

# Add the source files (including the univalue library files if needed)
add_executable(Vectorium main.cpp)  # Update with correct source files

# Link the univalue library
target_link_libraries(Vectorium C:/Users/pc/Desktop/Vectorium/univalue/lib/univalue.a)  # Adjust the library path as needed
