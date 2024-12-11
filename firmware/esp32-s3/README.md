## Configure the project

Install idf.py cmd [https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html]

```bash
mkdir ~/esp
cd ~/esp
git clone git@github.com:espressif/esp-idf.git
cd esp-idf
git submodule update --init --recursive
./esp-idf/install.sh
```

Then in your project `. ~/esp/esp-idf/export.sh`

Before project configuration and build, make sure to set the correct chip target using `idf.py set-target TARGET` command.

## Erase the NVRAM

Before flash it to the board, it is recommended to erase NVRAM if user doesn't want to keep the previous examples or other projects stored info 
using `idf.py -p PORT erase-flash`

## Build and Flash

Build the project, flash it to the board, and start the monitor tool to view the serial output by running `idf.py -p PORT flash monitor`.

(To exit the serial monitor, type `Ctrl-T + Ctrl-X`)
