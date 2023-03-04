mkdir .\bin\web
emcc main.c -O2 -s USE_SDL=2 -o .\bin\web\index.html