@call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"

@rem Compiling Shaders
dxc.exe -T vs_6_0 -E VS -Fo output\default_vs.cso shaders\default.hlsl
dxc.exe -T ps_6_0 -E PS -Fo output\default_ps.cso shaders\default.hlsl

pause