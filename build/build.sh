rm -rf $(ls|grep -v 'build.*.sh')
sleep 1
#arm
#cmake .. -DCROSS_COMPILE=arm-hisiv500-linux- -DPLATFORM=hi3516av200 -DHISI_PLATFORM=1
#x86_64
cmake .. -DPLATFORM=x86_64
