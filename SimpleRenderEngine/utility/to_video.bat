REM CD "..\Temp"
ffmpeg -r 60 -i ..\Screenshots\Image%%03d.bmp -c:v libx264 -qp 1 -profile:v high444 -preset fast -pix_fmt yuv420p result.mp4
@ECHO OFF
EXIT
