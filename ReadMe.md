# MIL STD 1553 Bus Monitor with GUI using the [wxWidgets framework](https://wxwidgets.org/) and [DDC drivers](https://www.ddc-web.com/en/connectivity/databus/milstd1553-1)

![image](https://github.com/user-attachments/assets/6f846b98-4fcd-4fb0-85a4-964397017ee9)

## For Developers

### How I created deps/wxwidgets

1. run `sudo apt install libgtk-3-dev`
1. download and unzip https://github.com/wxWidgets/wxWidgets/releasesdownload/v3.2.6/wxWidgets-3.2.6.tar.bz2
1. cd into this downloaded dir
1. create a dir called `gtk-build` and cd into it
1. run `../configure`
1. run `make -j3`
1. delete the links in the generated `lib/` dir
1. rename all the .so files in this lib dir from ...`.so.0.3.0` to ...`.so.0`
1. copy this `lib` dir into the projects `deps` dir
1. copy the `include` dir from the zip downloaded earlier into the projects `deps` dir
1. run `./wx-config --cxxflags` (`wx-config` was generated somewhere inside `gtk-build`) and use the outputs of this command to determine include directories, link libraries and compile definitions
1. i know this is hideous but it works lol :P
