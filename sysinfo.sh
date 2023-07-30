#!/bin/bash

# chmod u+x drive_space.sh 

# sysinfo - Un script que informa del estado del sistema

##### Constantes

TITLE="Información del sistema para $HOSTNAME"

RIGHT_NOW=$(date +"%x %r%Z")
TIME_STAMP="Actualizada el $RIGHT_NOW por $USER"

##### Estilos

TEXT_BOLD=$(tput bold)
TEXT_ULINE=$(tput sgr 0 1)
TEXT_GREEN=$(tput setaf 2)
TEXT_RESET=$(tput sgr0)

##### Funciones

system_info()
{
   # Función de stub temporal
   echo "función system_info"
   echo "${TEXT_ULINE}Versión del sistema${TEXT_RESET}"

    echo
   uname -a
}

show_uptime()
{
   # Función de stub temporal
   echo "función show_uptime"
   echo "${TEXT_ULINE}Tiempo de encendido del sistema$TEXT_RESET"
   echo 
   uptime
}


drive_space()

{
   # Función de stub temporal
   echo "${TEXT_ULINE}Espacio ocupado en las particiones del disco duro$TEXT_RESET"

   echo "función drive_space"
   echo 
   df -h
}


home_space()
{
    # Función de stub temporal
   echo "${TEXT_ULINE}Espacio ocupado en las particiones del disco duro$TEXT_RESET"

   echo "función home_space"
   if [ "$USER" != root ]; then

    echo "No tienes permisos de superusuario"
    echo 
    du -s $HOME
    echo
    echo "Archivo en HOME"
    echo 
    find $HOME -maxdepth 1 -mindepth 1 -type f 
    echo
    echo "Cantidad de archivos .out (ejecutables)"
    echo    
    ls $HOME/*.out | wc -w 
   else
    echo 
    echo -e "${TEXT_ULINE}USADO \tDIRECTORIO$TEXT_RESET"
    du -s /home/* | sort -r
   fi
}

##### Programa principal

cat << _EOF_

$TEXT_BOLD$TITLE$TEXT_RESET

$TEXT_GREEN$TIME_STAMP$TEXT_RESET

_EOF_

system_info
show_uptime
drive_space
home_space