#!/bin/bash 
# en la linea 1 el -x es para depurar
# chmod u+x drive_space.sh 

# sysinfo - Un script que informa del estado del sistema
#set -e                   --> significa q los comandos siguientes si uno falla se aborta todo

# stat /dev/sda
#EJERCICIO C
# stat -c %T /dev/sda
# stat -c %t /dev/sda
# ls -l /dev/sda

# 2>/dev/null para crear en temporal
#2b) lsof2>/dev/null | grep "8,3"
#3 lsof2>/dev/null | grep "8,3" | tail-n+2 | tr -s ' ' | cut -d ' ' -f3 | sort -u 
#ejecicio con c++ 
#1. abrir un fichero : open fd_src
#2.crear un desino para excribir: open crear para escriura fd_ds
#3.crear un buffer, std::vector<uin8_t>
#4. bucle 
#->buffer.resize(BUFFER_SIZE)
#->ssize_t byes_read=read(fd_src,buffer.daa(),buffer.size())
#->byes_read<0 ====> error NO UTILAR exit() sino return 1;
#->byes_read>0 ====>
#    buffer.resize(bytes_read)
#    write(fd_dst,buffer) -> write 
#-> byes_read=0 ====> se termina el bucle
#5.close(fd_src)
#6.close(fd_dst)


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

    printf "\n%-10s%-20s%-20s%-10s%-10s%-10s%-10s%-10s%-10s" "TIPO" "MONTAJE" "NOMBRE" "MEMORIA" "PORCENTAJE" "CANTIDAD" "M_USADA" "MAYOR" "MENOR"
    tipos=$(mount | tr -s " " ":" | cut -d ":" -f 5 | sort -u)
    for tipo in $tipos; do
        cerr1=$(df -t $tipo 2>/dev/null)
        if [ "$?" = "0" ]; then
            systema=$tipo
            memoria=$(df -t $tipo | tail -n+2 | tr -s " " | sort -k 3 | cut -d " " -f3 | tail -n-1 )
            nombre=$(df -t $tipo | tail -n+2 | tr -s " " | sort -k 3 | cut -d " " -f1 | tail -n-1 )
            montaje=$(df -t $tipo | tail -n+2 | tr -s " " | sort -k 3 | cut -d " " -f6 | tail -n-1 )
            porcetaje=$(df -t $tipo | tail -n+2 | tr -s " " | sort -k 3 | cut -d " " -f5  | tail -n-1 )
            cantidad=$(df -t $tipo | tail -n+2 | wc -l )
            memoria_usada=$(df -t $tipo | tail -n+2 | awk '{ sum += $3 } END { print sum }')
            cerr2=$(stat $nombre 2>/dev/null)
            if [ "$?" = "0" ];then
                mayor=$(stat -c %t $nombre)
                menor=$(stat -c %T $nombre)
                printf "\n%-10s%-20s%-20s%-10d%-10s%-10d%-10d%-10d%-10d" "$systema" "$montaje" "$nombre" "$memoria" "$porcetaje" "$cantidad" "$memoria_usada" "$mayor" "$menor"
            else
                printf "\n%-10s%-20s%-20s%-10d%-10s%-10d%-10d%-10s%-10s" "$systema" "$montaje" "$nombre" "$memoria" "$porcetaje" "$cantidad" "$memoria_usada" "*" "*"
            fi
        
        else 
            continue
        fi

    done
}

type_sysinfoINV() {

    printf "\n%-10s%-20s%-20s%-10s%-10s%-10s%-10s%-10s%-10s" "TIPO" "MONTAJE" "NOMBRE" "MEMORIA" "PORCENTAJE" "CANTIDAD" "M_USADA" "MAYOR" "MENOR"
    tipos=$(mount | tr -s " " ":" | cut -d ":" -f 5 | sort -ur)
    for tipo in $tipos; do
        cerr1=$(df -t $tipo 2>/dev/null)
        if [ "$?" = "0" ]; then
            systema=$tipo
            memoria=$(df -t $tipo | tail -n+2 | tr -s " " | sort -k 3 | cut -d " " -f3 | tail -n-1 )
            nombre=$(df -t $tipo | tail -n+2 | tr -s " " | sort -k 3 | cut -d " " -f1 | tail -n-1 )
            montaje=$(df -t $tipo | tail -n+2 | tr -s " " | sort -k 3 | cut -d " " -f6 | tail -n-1 )
            porcetaje=$(df -t $tipo | tail -n+2 | tr -s " " | sort -k 3 | cut -d " " -f5  | tail -n-1 )
            cantidad=$(df -t $tipo | tail -n+2 | wc -l )
            memoria_usada=$(df -t $tipo | tail -n+2 | awk '{ sum += $3 } END { print sum }')
            cerr2=$(stat $nombre 2>/dev/null)
            if [ "$?" = "0" ];then
                mayor=$(stat -c %t $nombre)
                menor=$(stat -c %T $nombre)
                printf "\n%-10s%-20s%-20s%-10d%-10s%-10d%-10d%-10d%-10d" "$systema" "$montaje" "$nombre" "$memoria" "$porcetaje" "$cantidad" "$memoria_usada" "$mayor" "$menor"
            else
                printf "\n%-10s%-20s%-20s%-10d%-10s%-10d%-10d%-10s%-10s" "$systema" "$montaje" "$nombre" "$memoria" "$porcetaje" "$cantidad" "$memoria_usada" "*" "*"
            fi
        
        else 
            continue
        fi

    done
}
av() {

    printf "\n%-10s%-20s%-20s%-10s%-20s%-10s%-10s%-10s%-10s%-10s" "TIPO" "MONTAJE" "NOMBRE" "MEMORIA" "PORCENTAJE" "CANTIDAD" "M_USADA" "MAYOR" "MENOR" "M_DISPONIBLE"
    tipos=$(mount | tr -s " " ":" | cut -d ":" -f 5 | sort -ur)
    for tipo in $tipos; do
        cerr1=$(df -t $tipo 2>/dev/null)
        if [ "$?" = "0" ]; then
            systema=$tipo
            memoria=$(df -t $tipo | tail -n+2 | tr -s " " | sort -k 3 | cut -d " " -f3 | tail -n-1 )
            nombre=$(df -t $tipo | tail -n+2 | tr -s " " | sort -k 3 | cut -d " " -f1 | tail -n-1 )
            montaje=$(df -t $tipo | tail -n+2 | tr -s " " | sort -k 3 | cut -d " " -f6 | tail -n-1 )
            porcetaje=$(df -t $tipo | tail -n+2 | tr -s " " | sort -k 3 | cut -d " " -f5  | tail -n-1 )
            cantidad=$(df -t $tipo | tail -n+2 | wc -l )
            memoria_usada=$(df -t $tipo | tail -n+2 | awk '{ sum += $3 } END { print sum }')
            memoria_disponible=$(df -t $tipo | tail -n+2 | awk '{ sum += $4 } END { print sum }')
            cerr2=$(stat $nombre 2>/dev/null)
            if [ "$?" = "0" ];then
                mayor=$(stat -c %t $nombre)
                menor=$(stat -c %T $nombre)
                printf "\n%-10s%-20s%-20s%-10d%-20s%-10d%-10d%-10d%-10d%-10d" "$systema" "$montaje" "$nombre" "$memoria" "$porcetaje" "$cantidad" "$memoria_usada" "$mayor" "$menor" "$memoria_disponible"
            else
                printf "\n%-10s%-20s%-20s%-10d%-20s%-10d%-10d%-10s%-10s%-10d" "$systema" "$montaje" "$nombre" "$memoria" "$porcetaje" "$cantidad" "$memoria_usada" "*" "*" "$memoria_disponible"
            fi
        
        else 
            continue
        fi

    done
}

inv=0
av=0
while [ "$1" != "" ]; do
   case $1 in
       -av)
            av=1
       ;;
       -inv )

            inv=1

            ;;
       -h | --help )

            usage
           exit 1
           ;;
        
   esac
   shift
done

if [ "$av" == "1" ]; then

cat << _EOF_
    $TEXT_BOLD$TITTLES$TEXT_RESET
    $(av)
_EOF_
 
else 

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

fi



