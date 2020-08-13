# Get Started

| Component | Tested |
| --- | --- |
| Target Board | NUCLEO-F413ZH |
| Compiler | gcc-arm-none-eabi-9-2019-q4-major |
| Zephyr West | v0.7.2 |

## Step 1: Get the source code

~~~bash
git clone --recursive https://github.com/iSotEE/zephyr-nucleo-f413zh.git
~~~

## Step 2: Build the host image

1. Set `APP_DIR` in `workspace/make.sh` as follows

~~~bash
#APP_DIR=baseline-host
#APP_DIR=baseline-guest
#APP_DIR=guest-usb
#APP_DIR=guest-thread-metric
APP_DIR=host
#APP_DIR=host-thread-metric
...
~~~

2. Run `make.sh` and a image named `zephyr-host.elf` will be created.

## Step 3: Build the guest image

1. Set `APP_DIR` in `workspace/make.sh` as follows (use project `guest-usb` as an example)

~~~bash
#APP_DIR=baseline-host
#APP_DIR=baseline-guest
APP_DIR=guest-usb
#APP_DIR=guest-thread-metric
#APP_DIR=host
#APP_DIR=host-thread-metric
~~~

2. Run `make.sh` and a image named `zephyr-guest.elf` will be created.

## Step 4: Start debugging

You can then use a debugger (e.g. GDB in Eclipse Embedded CDT) to load and run these images.
An example of GDB script is as follows.

~~~bash
load ${workspace_loc}/zephyr-nucleo-f413zh/workspace/zephyr-guest.elf
load ${workspace_loc}/zephyr-nucleo-f413zh/workspace/zephyr-host.elf
add-symbol-file ${workspace_loc}/zephyr-nucleo-f413zh/workspace/zephyr-host.elf
add-symbol-file ${workspace_loc}/zephyr-nucleo-f413zh/workspace/zephyr-guest.elf
monitor arm semihosting enable
set backtrace limit 10
~~~
