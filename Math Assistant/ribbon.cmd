call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"

set path=%path%;"C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x86"

uicc ribbon.xml ribbon.bml /header:ids.h /res:ribbonui.rc

pause
