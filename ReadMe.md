# MIL STD 1553 Bus Monitor, Bus Controller and RT Emulator GUI for [DDC](https://www.ddc-web.com/en/connectivity/databus/milstd1553-1)

![image](https://github.com/user-attachments/assets/534a3791-a22b-4e0d-b64c-29fdd7b285ba)

## How to use

### Install dependencies

``` bash
sudo apt install -y build-essential libboost-all-dev cmake libspdlog-dev nlohmann-json3-dev git libqt5gui5 libtiff5-dev
sudo ln -s /usr/lib/x86_64-linux-gnu/libtiff.so.6 /usr/lib/x86_64-linux-gnu/libtiff.so.5
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
./scripts/bm.sh
```

#### RT Emulator

``` bash
./scripts/rt.sh
```

#### ❗ Don't forget to have the DDC drivers installed on your machine and have a valid DDC device connected to your machine. You should also make sure the DDC device is connected to a functioning MIL STD 1553 bus ❗
