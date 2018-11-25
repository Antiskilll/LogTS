"C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" G-Key.sln /p:Configuration=Release /p:Platform="Win32"
"C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" G-Key.sln /p:Configuration=Release /p:Platform="x64"
del G-Key_0.6.2.ts3_plugin
.\Tools\7zip\x64\7za.exe -tzip a G-Key_0.6.2.ts3_plugin .\Release\*