# The project is discontinued, since the original sleeptracker watch isn't available for ages now and almost every modern watch does it without my help.

Napkin expects you to have sleeptracker port at /dev/sleeptracker

On linux you'd need to have 'ftdi_sio' kernel module loaded (and usb support,
of course) and something along these lines in your udev rules:

BUS=="usb", SYSFS{product}=="FT232R USB UART", SYSFS{serial}=="********", SYSFS{manufacturer}=="FTDI", KERNEL=="ttyUSB*", SYMLINK="sleeptracker", MODE="660", GROUP="usb"

Note, that the I've wiped out the serial number, so that you don't try to use
mine. Perhaps it will differ, although I have only one device.

You can also set the other port name using SLEEPTRACKER_PORT environment
variable, like

env SLEEPTRACKER_PORT=/dev/ttyUSB0 napkin
