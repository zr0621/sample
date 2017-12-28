MIT License

C99/ANSIC
C++11
POSIX



工程目录结构
|----|./--工程根目录
|----|----|./install--安装目录，存放拷贝过去的可执行文件、配置文件、数据库文件、日志文件目录
|----|----|./build--编译文件目录，编译脚本&makefile等文件，执行build.sh调用cmake一键生成makefile，然后make install
|----|----|./doc--文档目录，README、COPYRIGHT等文件
|----|----|./thirdparty--引用外部的源文件、头文件、静态库或动态库目录，如ffmpeg、opencv等
|----|----|./config--配置文件原始目录
|----|----|./source--本工程的源代码目录
|----|----|----|./source/main/--主程序
|----|----|----|./source/platform/--平台相关的头文件声明和接口定义、通用数据类型、返回值类型，会被其他模块引用
|----|----|----|----|./source/platform/inc--
|----|----|----|----|./source/platform/src--
|----|----|----|./source/utils/--小工具，比如获取时间、base64加解密、crc校验等零散小功能
|----|----|----|----|./source/utils/inc--头文件
|----|----|----|----|./source/utils/src--源文件
|----|----|----|./source/log/--日志模块，实现日志打印、日志读写、日志查询，会被其他模块引用
|----|----|----|----|./source/log/inc--日志系统的头文件
|----|----|----|----|./source/log/src--日志系统的源文件
|----|----|----|./source/dsa/--数据结构和算法Data Structures and Algorithms，双向链表、队列、向量
|----|----|----|----|./source/dsa/inc--DSA的头文件
|----|----|----|----|./source/dsa/src--DSA的源文件
|----|----|./CMakeLists.txt--顶层cmake文件，每个模块还有各自的cmake文件