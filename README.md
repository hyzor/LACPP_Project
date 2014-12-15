LACPP_Project
=============
Sequential edge detection algorithm using the Sobel operator.

####Todo
Implement parallel versions using:
- Threads and locks
- Tasks

####How to build

Since OpenCV is used to load the images, the libraries for OpenCV has to be linked when building.

Supply a link to the library path containing OpenCV. (**Example**: -L/usr/lib/i386-linux-gnu)

Then link to the required library files of OpenCV:

-lopencv_core -lopencv_imgproc -lopencv_highgui

Now build using g++.

#####Build command
The build command should look similar to the following:

g++ -L/usr/lib/i386-linux-gnu -o "LACPP_Project"  ./main.o   -lopencv_core -lopencv_imgproc -lopencv_highgui

#####Installing OpenCV
OpenCV can easily be installed using apt-get:

sudo apt-get install libopencv-dev
