#!/bin/sh

echo Copiando...
cp -R /media/discretaboy/tp-2014-1c-the-discreta-boy/ ~/tp
echo Archivos copiados
cd ~/tp/CPU/Debug/
sudo make all
cd ~/tp/Kernel/Debug/
sudo make all
cd ~/tp/ProcesoPrograma/Debug/
sudo make all
cd ~/tp/UMV/Debug/
sudo make all
sudo cp ~/tp/ProcesoPrograma/Debug/ProcesoPrograma /usr/bin/ansisop
echo Finalizado