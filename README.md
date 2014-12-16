LACPP_Project
=============
Sequential, threaded and task-based edge detection algorithm using the Sobel operator.

####How to build

Since OpenCV is used to load the images, the libraries for OpenCV has to be linked when building.

Supply a link to the library path containing OpenCV. (**Example**: -L/usr/lib/i386-linux-gnu)

Then link to the required library files of OpenCV:

-lopencv_core -lopencv_imgproc -lopencv_highgui

Since C++11 features are used, append -std=c++11 to the build command.

Now build using g++.

#####Build command
The build command should look similar to the following:

g++ -L/usr/lib/i386-linux-gnu -o "LACPP_Project"  ./EdgeDetection.o ./ImageProcessingUtil.o ./main.o   -lopencv_core -lopencv_imgproc -lopencv_highgui

Assuming that the necessary objects are previously built. For example, main.cpp:

g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"main.d" -MT"main.d" -o "main.o" "../main.cpp"

#####Installing OpenCV
OpenCV can easily be installed using apt-get:

sudo apt-get install libopencv-dev
