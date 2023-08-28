BUILD_MCU = atmega16
BUILD_CLK = 8000000UL
UPLOADER = ft232h-clone
# UPLOADER = usbtiny
DUDEPORT =
FUSES = -U lfuse:w:0xd4:m -U hfuse:w:0xd1:m

###################################################
# Tools & flags
###################################################

CC 	= avr-gcc
SIZE 	= avr-size
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
# Libs
AR 	= avr-ar
LNKLIBS = -lm -lc -lgcc


AVRDUDE = avrdude

DUDEFLAGS = -V -p $(BUILD_MCU) $(DUDEPORT) -c $(UPLOADER)

SIZEFLAGS =

PRJCFLAGS = -mmcu=$(BUILD_MCU) -DF_CPU=$(BUILD_CLK) -D__AVR__
LDFLAGS   = -Wl,-Map,$(PRG).map
# -u,vfprintf,
#  -Wl,-u,vfprintf,-Map,$(P).map
# # Wl,-Map,$(P).map -u,vfprintf

PRJCFLAGS := $(PRJCFLAGS) -g -Wall -Os
PRJCFLAGS := $(PRJCFLAGS) -ffunction-sections
PRJCFLAGS := $(PRJCFLAGS) -fdata-sections
PRJCFLAGS := $(PRJCFLAGS) -fmerge-all-constants
PRJCFLAGS := $(PRJCFLAGS) -fno-inline-small-functions
PRJCFLAGS := $(PRJCFLAGS) -fshort-enums
PRJCFLAGS := $(PRJCFLAGS) -fno-exceptions

LDFLAGS := $(LDFLAGS) -Wl,--gc-sections
LDFLAGS := $(LDFLAGS) -Wl,--relax

FORMAT = ihex


# programmer
# id = "ft232h-clone";
# desc = "FT232H based generic programmer";
# type = "avrftdi";
# prog_modes = PM_TPI | PM_ISP;
# connection_type = usb;
# usbvid = 0x0403;
# usbpid = 0x6014;
# usbdev = "A";
# # ISP-signals
# reset = 7; # AD3 (TMS)
# sck = 6; # AD0 (TCK)
# sdo = 4; # AD1 (TDI)
# sdi = 5; # AD2 (TDO)
# ;
