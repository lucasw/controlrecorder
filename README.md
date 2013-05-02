controlrecorder
===============

Record joystick and mouse and possibly keyboard inputs in linux for later playback.

Variations on this probably already exist, but want custom record format (yaml based?),
and ease of interoperation with vimjay.  

Spoofing inputs with uinput is a possibility.

Timestamping and interpolation are interesting issues, first program will will ignore and simply sample at some rate and playback at the rate the samples are loaded.

---
```
$ dmesg

[   35.102411] usb 2-1.4: New USB device found, idVendor=046d, idProduct=c216
[   35.102416] usb 2-1.4: New USB device strings: Mfr=1, Product=2, SerialNumber=0
[   35.102419] usb 2-1.4: Product: Logitech Dual Action
[   35.102421] usb 2-1.4: Manufacturer: Logitech
[   35.824994] usbcore: registered new interface driver usbhid
[   35.825000] usbhid: USB HID core driver
[   35.853258] input: Logitech Logitech Dual Action as /devices/pci0000:00/0000:00:1d.0/usb2/2-1/2-1.4/2-1.4:1.0/input/input13
[   35.853401] hid-generic 0003:046D:C216.0001: input,hidraw0: USB HID v1.10 Joystick [Logitech Logitech Dual Action] on usb-0000:00:1d.0-1.4/input0
```
