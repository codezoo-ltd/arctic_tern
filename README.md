### Initial screen

![initial](https://user-images.githubusercontent.com/22319034/62351096-95110b00-b53f-11e9-9db7-8093de83ee23.png)


# mbed-porting-to arctic-tern board using-ARM-GCC

First release v1.0 includes the implementation of GPS and Modem

# GCC Version
gcc-arm-none-eabi-5_4-2016q2 

# Quick Start
* Install STM32 STLink and gcc compiler [ http://www.wolinlabs.com/blog/linux.stm32.discovery.gcc.html ]
* Edit .bashrc in your home directory adn add the following line:

  export PATH="/path/stlink/build:/path/gcc-arm-none-eabi-5_4-2016q2/bin:$PATH
* make clean && make -j@ && make flash          @:core numbers 

# Have fun!!
