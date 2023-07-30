#!/bin/bash

# chmod u+x drive_space.sh 

# sysinfo - Un script que informa del estado del sistema

TITLE_BOLD=$(tput bold)
TEXT_GREEN=$(tput setaf 2)
TEXT_RESET=$(tput sgr0)

nombre=tipo
montaje=
memoria=

usage()
{
   echo "usage: ./filesysteminfo.sh [-h] [-inv]"

}

type_sysinfo() {

    printf "\n%-30s%-30s%-30s%-30s%-30s" "TIPO" "MONTAJE" "NOMBRE" "MEMORIA" " PORCENTAJE "
    tipos=$(mount | tr -s " " ":" | cut -d ":" -f 5 | sort -u)
    for tipo in $tipos; do
        systema=$tipo
        nombre=$(df -at $tipo | tail -n+2 | tr -s " " ":" | cut -d ":" -f1 | sort -n | tail -n-1 )
        memoria=$(df -at $tipo | tail -n+2 | tr -s " " ":" | cut -d ":" -f3 | sort -n | tail -n-1 )
        montaje=$(df -at $tipo | tail -n+2 | tr -s " " ":" | cut -d ":" -f6 | sort -n | tail -n-1 )
        porcetaje=$(df -at $tipo | tail -n+2 | tr -s " " ":" | cut -d ":" -f5 | sort -n | tail -n-1 )
        printf "\n%-30s%-30s%-30s%-30d%-30s" "$systema" "$montaje" "$nombre" "$memoria" "$porcetaje"
    done
}

type_sysinfoINV() {

    printf "\n%-30s%-30s%-30s%-30s%-30s" "TIPO" "MONTAJE" "NOMBRE" "MEMORIA" " PORCENTAJE "
    tipos=$(mount | tr -s " " ":" | cut -d ":" -f 5 | sort -ur)
    for tipo in $tipos; do
        systema=$tipo
        nombre=$(df -at $tipo | tail -n+2 | tr -s " " ":" | cut -d ":" -f1 | sort -n | tail -n-1 )
        memoria=$(df -at $tipo | tail -n+2 | tr -s " " ":" | cut -d ":" -f3 | sort -n | tail -n-1 )
        montaje=$(df -at $tipo | tail -n+2 | tr -s " " ":" | cut -d ":" -f6 | sort -n | tail -n-1 )
        porcetaje=$(df -at $tipo | tail -n+2 | tr -s " " ":" | cut -d ":" -f5 | sort -n | tail -n-1 )
        printf "\n%-30s%-30s%-30s%-30d%-30s" "$systema" "$montaje" "$nombre" "$memoria" "$porcetaje"
    done
}
inv=0
while [ "$1" != "" ]; do
   case $1 in
       -inv )

            inv=1

            ;;
       -h )

            usage
           exit 1
           ;;
   esac
   shift
done

if [ "$inv" == "1" ]; then

cat << _EOF_
    $TEXT_BOLD$TITTLES$TEXT_RESET
    $(type_sysinfoINV)
_EOF_
 
else 

cat << _EOF_
    $TEXT_BOLD$TITTLES$TEXT_RESET
    $(type_sysinfo)
_EOF_

fi


