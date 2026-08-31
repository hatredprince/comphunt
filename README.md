Install:
- Download MinGW: https://sourceforge.net/projects/mingw/
- Select `mingw32-gcc-g++` → Apply Changes
- Add `C:\MinGW\bin` to system PATH

Compile:
```bash
g++ -O2 -static -o comphunt.exe comp.cpp -lws2_32
```

Run:
```bash
comphunt.exe
```