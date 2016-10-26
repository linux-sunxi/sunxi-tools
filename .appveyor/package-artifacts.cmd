REM Create an archive from the built binaries and required libraries (DLLs).
REM For users' convenience, we also include the libusb "listdevs.exe" and our
REM markdown documentation/license.
REM AppVeyor will run this as "after_build" step.

mkdir dist
set FILES=*.exe *.md
set FILES=%FILES% .\libusb\MinGW32\dll\*.dll
set FILES=%FILES% .\libusb\examples\bin32\listdevs.exe
7z a dist\sunxi-tools-win32.7z %FILES%
