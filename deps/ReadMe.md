# MIL-STD-1553-Bus-Analyzer Project Dependencies

## How I created deps/wxwidgets

1. Run `sudo apt install libgtk-3-dev`
1. Download and unzip https://github.com/wxWidgets/wxWidgets/releasesdownload/v3.2.6/wxWidgets-3.2.6.tar.bz2
1. cd into this downloaded dir
1. Create a dir called `gtk-build` and cd into it
1. Run `../configure`
1. Run `make -j3`
1. Delete the links in the generated `lib/` dir
1. Rename all the .so files in this lib dir from ...`.so.0.3.0` to ...`.so.0`
1. Copy this `lib` dir into the projects `deps` dir
1. Copy the `include` dir from the zip downloaded earlier into the projects `deps` dir
1. Run `./wx-config --cxxflags` (`wx-config` was generated somewhere inside `gtk-build`) and use the outputs of this command to determine include directories, link libraries and compile definitions
1. I know this is hideous but it works lol :P