# MIL STD 1553 Bus Monitor, Bus Controller and RT Emulator with GUI using the [wxWidgets framework](https://wxwidgets.org/) and [DDC drivers](https://www.ddc-web.com/en/connectivity/databus/milstd1553-1)

![image](https://github.com/user-attachments/assets/534a3791-a22b-4e0d-b64c-29fdd7b285ba)

## How to use

### Install dependencies
``` bash
sudo apt install build-essential libboost-all-dev cmake libspdlog-dev nlohmann-json3-dev git libqt5gui5
```

### Clone the project
``` bash
git clone https://github.com/rendayigit/MIL-STD-1553-Bus-Analyzer.git
```

### Build the project
``` bash 
./scripts/build.sh
```

### Run the project
#### Bus Controller
``` bash
./scripts/bc.sh
```

#### Bus Monitor
``` bash
./scripts/bc.sh
```

#### RT Emulator
``` bash
./scripts/rt.sh
```

#### ❗ Don't forget to have the DDC drivers installed on your machine and have a valid DDC device connected to your machine. You should also make sure the DDC device is connected to a functioning MIL STD 1553 bus ❗

## For Developers

### How I created deps/wxwidgets

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
