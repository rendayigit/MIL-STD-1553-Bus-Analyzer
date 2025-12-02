# MIL STD 1553 Bus Monitor, Bus Controller and Remote Terminal GUI for [DDC](https://www.ddc-web.com/en/connectivity/databus/milstd1553-1)

![image](https://github.com/user-attachments/assets/534a3791-a22b-4e0d-b64c-29fdd7b285ba)

## How to use

### Clone the project

``` bash
git clone https://github.com/rendayigit/MIL-STD-1553-Bus-Analyzer.git
```

### Install dependencies

``` bash
sudo ./scripts/install_dependencies.sh
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

#### Remote Terminal

``` bash
./scripts/rt.sh
```

#### ❗ Don't forget to have the DDC drivers installed on your machine and have a valid DDC device connected to your machine. You should also make sure the DDC device is connected to a functioning MIL STD 1553 bus ❗
