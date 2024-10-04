cmake_minimum_required(VERSION 3.0)

file(GLOB_RECURSE unity_files ${CMAKE_BUILD_DIRECTORY} unity_**.cxx)
message(${unity_files})
foreach(unity_file ${unity_files})
    file(READ ${unity_file} unity_file_content)
    if(${unity_file_content} MATCHES "__JETBRAINS_IDE__")
        continue()
    endif()
    file(WRITE ${unity_file} "#ifndef __JETBRAINS_IDE__\n${unity_file_content}\n#endif")
endforeach()
