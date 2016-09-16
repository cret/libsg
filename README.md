# ![](res/image/logo.png)

[![Build Status](https://travis-ci.org/metalwood/libsg.svg?branch=master)](https://travis-ci.org/metalwood/libsg)

## Overview

Cross-platform C utility library focusing on efficient development and easy-to-use.

## Idea

Reuse, not rework or reinvent.

Using C to build important infrastructure, not everything.

## Feature

#### One key to build

DO NOT need to install any dependency library before building of libsg.

#### Efficient development

```
/* if you want to get md5 with normal md5 library, your code is like this */

FILE *fp;
struct md5_context *ctx;
char buf[256], md5_sum[33];
size_t size;

fp = fopen("/home/test_file", "rb");
if (!fp)
    return;
    
ctx = md5_start();
if (!ctx)
    return;
    
while (!feof(fp)) {
    size = fread(buf, 1, 256, fp);
    if (size > 0)
        md5_update(ctx, buf, size);
}

md5_final(ctx, md5_sum);
fclose(fp);
```

```
/* if you want to get md5 by libsg, the code is like this */

int err;
struct sg_md_sum md5_sum = {0};

err = sg_md_file("/home/test_file", SGMDTYPE_MD5, &md5_sum);
if (err != 0)
    printf("md5 calc error\n");

```

#### Easy-to-understand APIs

Header files of libsg include nothing but only pure API declaration, and APIs will not cause misunderstanding.

Common C API vs libsg API sample

#### Performance

Key modules are optimized, you can create high performance application with libsg.

## Supported Platforms

Linux ／ Windows ／ OS X

## Download

https://github.com/metalwood/libsg/archive/master.zip

## Build & install

Before building, make sure CMake / XMake and C compiler(gcc/clang/msvc) has been installed correctly.
Make sure the source path does not have space.

Linux / MacOS

        $ mkdir build
        $ cd build
        $ cmake ..
        $ make -j4

Windows

        $ md build
        $ cd build
        $ cmake ..
        open .sln with Visual Studio and compile the project

## API doc & demo

coming soon

## Modules

**sys** atom mutex rwlock spin_lock memory module os proc time fs flag shell sleep thread ntp

**str** charset regex sprintf vlstr(variable length string)

**net** tcp/tcp_server udp/udp_server http/http_server websocket ftp

etp/etp_server: a extreme transfer protocol based on KCP / LIBUV(UDP) and FEC, it's fast and reliable.

**db** sqlite ejdb berkeleydb

**container** bip_buf list queue xml msgpack json

**compress** zip gzip

**crypto**

md: message digest algorithm pack including MD2/MD4/MD5/SHA1/SHA224/SHA256/SHA512.

mac: message authentication code algorithm pack including HmacMD5...

sym: symmetric encryption algorithm pack including DES/3DES/AES.

**hash** uuid murmur

**media** capture_v capture_a rtsp player

**math** c_int c_float big_int big_float

**platform** windows/dial...

**util** assert dump log

## Code specification

English: [Linux kernel coding style](https://www.kernel.org/doc/Documentation/CodingStyle)            简体中文: [Linux内核编码风格](http://www.cnblogs.com/baochuan/archive/2013/04/08/3006615.html)

## Contact

email: metalwood@foxmail.com                   QQ群: 556925561

## Contributors

metalwood (metalwood@foxmail.com)

canmor (476010464@qq.com)

vincent (vin@misday.com)
