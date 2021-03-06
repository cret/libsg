cmake_minimum_required(VERSION 2.8)

## 路径里含有空格会导致编译失败，目前没有好的办法处理，所以先以检查并提示而代之。
if((${CMAKE_CURRENT_SOURCE_DIR} MATCHES " ") OR (${CMAKE_CURRENT_BINARY_DIR} MATCHES " "))
    message(FATAL_ERROR "'source path': ${CMAKE_CURRENT_SOURCE_DIR}\n'build path': ${CMAKE_CURRENT_BINARY_DIR}\nError: Path invalid, might contains with <spaces>")
endif()

## 导入第三方依赖库的 CMake 规则，直接把需要的库编译进来，而不用再去单独编译安装。
## 以简化安装和使用，这样主要是针对像 Windows 这种没有包管理的系统。
add_subdirectory(3rdparty)


# project
#########
project(libsg)

## 库的版本号对于 *nix 一系的操作系统的动态库（so, dylib等）来说都很重要，
## 通常是辨别 API 接品兼容性的重要属性，最好在这里指定。
set(major_version 1)
set(minor_version 0)
set(patch_version 0)
set(build_version 0)
set(version ${major_version}.${minor_version}.${patch_version})

set(lib_name sg)
set(lib_name_static ${lib_name})
## 为 Windows 和静态库和动态库取不同的名字，否则它们的 .lib 文件会有名字冲突，并相互覆盖。
if(WIN32)
    set(lib_name_static lib${lib_name_static})
endif()
set(target_static lib${lib_name}-static)


# files
#######
file(GLOB src_b64 3rdparty/b64.c/*.c)
file(GLOB src_cjson 3rdparty/cjson/*.c)
file(GLOB src_vtd-xml 3rdparty/vtd-xml/*.c)
## 排除不必要的文件 "lex.yy (2).c", "lex.yy_old.c" 以免编译失败
list(REMOVE_ITEM src_vtd-xml
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/vtd-xml/lex.yy (2).c"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/vtd-xml/lex.yy_old.c")
file(GLOB src_dablooms 3rdparty/dablooms/src/*.c)
list(REMOVE_ITEM src_dablooms "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/dablooms/src/test_dablooms.c")
file(GLOB src_str src/str/*.c)
file(GLOB src_compress src/compress/*.c)
file(GLOB src_container src/container/*.c)
file(GLOB src_db src/db/*.c)
file(GLOB src_math src/math/*.c)
file(GLOB src_net src/net/*.c)
file(GLOB src_sys src/sys/*.c)
file(GLOB src_util src/util/*.c)
file(GLOB src_crypto src/crypto/*.c)
file(GLOB src_hash src/hash/*.c)

set(src_lib
    ${src_b64}
    ${src_cjson}
    3rdparty/mongoose_5.6/mongoose.c
    3rdparty/json_checker/JSON_checker.c
    ${src_str}
    ${src_compress}
    ${src_container}
    ${src_db}
    ${src_math}
    ${src_sys}
    ${src_util}
    ${src_crypto}
    ${src_hash}
    src/net/net_card.c
    )

## 有些文件在 Linux 上不需要包含进来编译，直接引用安装好的库
if(NOT "${CMAKE_SYSTEM}" MATCHES "Linux")
    set(src_lib ${src_lib} 
        ${src_vtd-xml}
        )
endif()


# build options
###############
## 添加 3rdparty, include 到头文件搜索目录
include_directories(3rdparty)
include_directories(include)
include_directories(3rdparty/pcre)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/3rdparty/pcre)
include_directories(3rdparty/libxml2/include)
include_directories(3rdparty/openssl/include)
include_directories(3rdparty/dablooms/src)
include_directories(3rdparty/tbox/build)
include_directories(3rdparty/tbox/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/mpir)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/mpir/include)
## Windows 下还需要添加直接包含的依赖库
if(WIN32)
    include_directories(3rdparty/libiconv/include)
    include_directories(3rdparty/zlib)
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/3rdparty/zlib)
elseif(APPLE)
elseif(UNIX)
    include_directories(3rdparty/libuuid/include)
    include_directories(3rdparty/libiconv-1.14/local/include)
    include_directories(3rdparty/zlib)
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/3rdparty/zlib)
endif()

## Linux 下由于用了一些 GNU C 特有方法及标准 C99语法，必须设置为 gnu99 或更新的标准。
if("${CMAKE_SYSTEM}" MATCHES "Linux")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
endif()

## 部分Linux 下需要加 PIC 以通过链接
if("${CMAKE_SYSTEM}" MATCHES "Linux")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif()

## 为了防止后面调用 set_target_properties 时出现参数错误，这里故意设置一个空的默认值给 link_flags，
set(link_flags " ")

# Windows 下由于直接包含依赖库所需要设置的几个预处理器
if(WIN32)
    add_definitions(-DUSING_STATIC_LIBICONV -DPCRE_STATIC)
else(NOT APPLE)
    add_definitions(-DUSING_STATIC_LIBICONV)
endif()


# targets
#########
## 分别设置静态库和动态库目标
add_library(${target_static} STATIC ${src_lib})

set_target_properties(${target_static} PROPERTIES
    OUTPUT_NAME ${lib_name_static}_singular)

set(common_deps tbox libmpir openssl pcre)
if(WIN32)
    set(link_archives_release
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/mpir/lib/${CMAKE_VS_PLATFORM_NAME}/Release/mpir.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/tbox/build/tbox.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/openssl/libcrypto.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/openssl/libssl.lib
        ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/libiconv/Release/libiconv.lib
        ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/pcre/Release/pcre.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libxml2/win32/bin.msvc/libxml2_a.lib
        ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/c99-snprintf/Release/libc99-snprintf.lib
        ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/zlib/Release/zlibstatic.lib
        )
    set(link_archives_debug
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/mpir/lib/${CMAKE_VS_PLATFORM_NAME}/Release/mpir.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/tbox/build/tbox.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/openssl/libcrypto.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/openssl/libssl.lib
        ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/libiconv/Debug/libiconv.lib
        ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/pcre/Debug/pcred.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libxml2/win32/bin.msvc/libxml2_a.lib
        ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/c99-snprintf/Debug/libc99-snprintf.lib
        ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/zlib/Debug/zlibstaticd.lib
        )
    foreach(archive ${link_archives_release})
        set(link_libs ${link_libs} optimized ${archive})
    endforeach()
    foreach(archive ${link_archives_debug})
        set(link_libs ${link_libs} debug ${archive})
    endforeach()
    set(link_libs ${link_libs}
        Rasapi32.lib
        ws2_32.lib
        Crypt32.lib
        )
    add_dependencies(${target_static} libxml2 zlibstatic ${common_deps})
elseif(APPLE)
    set(link_archives
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/openssl/libcrypto.a
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/dablooms/build/libdablooms.a
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/mpir/lib/libmpir.a
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/tbox/build/libtbox.a
        ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/pcre/libpcre.a
        ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/zlib/libz.a
        )
    set(link_libs ${link_libs}
        ${link_archives}
        iconv dl xml2 curl pthread
        )
    add_dependencies(${target_static} openssl dablooms zlibstatic ${common_deps})
elseif(UNIX)
    set(link_archives
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libxml2/lib/libxml2.a
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libuuid/lib/libuuid.a
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libiconv-1.14/local/lib/libiconv.a
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/mpir/lib/libmpir.a
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/openssl/libcrypto.a
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/dablooms/build/libdablooms.a
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/tbox/build/libtbox.a
        ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/pcre/libpcre.a
        )
    set(link_libs ${link_libs}
        ${link_archives}
        pthread
        rt # we would better put 'rt', 'dl' to rear, it's sensitive for gcc
        dl
        )
    add_dependencies(${target_static} libxml2 libuuid libiconv openssl dablooms ${common_deps})
endif()

target_link_libraries(${target_static} ${link_libs})

## 把静态库合并，这样会生成独立的静态库文件，方便使用
if(WIN32)
    add_custom_target(combined_debug ALL
        COMMAND lib.exe /OUT:${CMAKE_CURRENT_BINARY_DIR}/Debug/${lib_name_static}.lib ${CMAKE_CURRENT_BINARY_DIR}/Debug/${lib_name_static}_singular.lib ${link_archives_debug}
        DEPENDS ${target_static}
        )
    add_custom_target(combined_release ALL
        COMMAND lib.exe /OUT:${CMAKE_CURRENT_BINARY_DIR}/Release/${lib_name_static}.lib ${CMAKE_CURRENT_BINARY_DIR}/Release/${lib_name_static}_singular.lib ${link_archives_release}
        DEPENDS ${target_static}
        )
elseif(APPLE)
    add_custom_target(combined ALL
        COMMAND libtool -static -o libsg.a ${CMAKE_CURRENT_BINARY_DIR}/lib${lib_name_static}_singular.a ${link_archives}
        DEPENDS ${target_static}
        )
else()
    add_custom_target(combined ALL
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/combine_libs.sh ${CMAKE_CURRENT_BINARY_DIR}/lib${lib_name_static}_singular.a ${link_archives}
        DEPENDS ${target_static}
        )
endif()

## 最后再调用测试的 CMake，之所以放到最后是因为，测试程序依赖 sg 库，需要链接 sg 库，放到最后这样可以方便调用生成好的文件和变量。
set(sg_inc_dir ${CMAKE_CURRENT_SOURCE_DIR})
set(sg_lib_dir ${CMAKE_CURRENT_BINARY_DIR})
enable_testing()
add_subdirectory(test/unit)
