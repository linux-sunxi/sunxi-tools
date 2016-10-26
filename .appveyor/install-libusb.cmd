REM This is a batch file to help with setting up the libusb dependency.
REM It is intended to be run as "install" step from within AppVeyor.

REM desired version and download URL
set VER=libusb-1.0.20
set URL=https://sourceforge.net/projects/libusb/files/libusb-1.0/%VER%/%VER%.7z/download

mkdir libusb
cd libusb
curl -fLsS -o %VER%.7z %URL%
7z x %VER%.7z
cd ..
