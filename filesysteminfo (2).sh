#!/bin/bash 
# en la linea 1 el -x es para depurar
# chmod u+x drive_space.sh 


TITLE_BOLD=$(tput bold)
TEXT_GREEN=$(tput setaf 2)
TEXT_RESET=$(tput sgr0)

nombre=tipo
montaje=
memoria=
catidad_abiertos=0
sum_catidad_abiertos=0
array=()


error_exit()
{
    echo "$1" 1>&2
    exit 1
}


usage()
{
   echo "usage: ./fileysteminfo.sh [-h] [-inv] [-noheader] [devicefiles] [-u]"

}


type_sysinfo() {
    echo
    echo "           ${TEXT_GREEN}Información sobre los sistemas de archivos montados${TEXT_RESET}"

    # printf '\n%s\n' "${array[@]}"
    # HEADER O NO
    if [ "$nohead" = "0" ];then
        printf "\n%-10s%-20s%-20s%-10s%-10s%-10s%-10s%-10s" "TIPO" "MONTAJE" "NOMBRE" "MEMORIA"  "CANTIDAD" "M_USADA" "MAYOR" "MENOR"
    fi
    # INVERTIDO O NO
    if [ "$inv" = "0" ];then
        tipos=$(mount | tr -s " " ":" | cut -d ":" -f 5 | sort -u)
    else 
        tipos=$(mount | tr -s " " ":" | cut -d ":" -f 5 | sort -ur)
    fi
    if [[ "$df" = "1" && "$nohead" = "0" ]];then
        printf "%-10s" "ARCHV. ARBIERTOS"
    fi


    for tipo in $tipos; do
        cerr1=$(df -t $tipo 2>/dev/null)
        if [ "$?" = "0" ]; then
            systema=$tipo
            memoria=$(df -t $tipo | tail -n+2 | tr -s " " | sort -k 3 | cut -d " " -f3 | tail -n-1 )
            nombre=$(df -t $tipo | tail -n+2 | tr -s " " | sort -k 3 | cut -d " " -f1 | tail -n-1 )
            montaje=$(df -t $tipo | tail -n+2 | tr -s " " | sort -k 3 | cut -d " " -f6 | tail -n-1 )
            porcetaje=$(df -t $tipo | tail -n+2 | tr -s " " | sort -k 3 | cut -d " " -f5  | tail -n-1 )
            cantidad_=$(df -t $tipo | tail -n+2 | wc -l )
            memoria_usada=$(df -t $tipo | tail -n+2 | awk '{ sum += $3 } END { print sum }')
            cerr2=$(stat $nombre 2>/dev/null)
            if [ "$?" = "0" ];then
                mayor=$(stat -c %t $nombre)
                menor=$(stat -c %T $nombre)
                mayor=$(echo "obase=10; ibase=16; $mayor;" | bc )
                menor=$(echo "obase=10; ibase=16; $menor;" | bc )
                if [ "$df" = "0" ];then
                    printf "\n%-10s%-20s%-20s%-10d%-10s%-10d%-10d%-10d" "$systema" "$montaje" "$nombre" "$memoria"  "$cantidad_" "$memoria_usada" "$mayor" "$menor"
                else 
                    if [ "$activoarray" = "0" ];then
                        catidad_abiertos=$(lsof $nombre | wc -l )
                        printf "\n%-10s%-20s%-20s%-10d%-10s%-10d%-10d%-10d%-10d" "$systema" "$montaje" "$nombre" "$memoria"  "$cantidad_" "$memoria_usada" "$mayor" "$menor" "$catidad_abiertos"
                    else
                        for i in ${array[@]}; do
                            catidad_abiertos=$(lsof -a -u $i $nombre | wc -l)
                            sum_catidad_abiertos=$(("$catidad_abiertos" + "$sum_catidad_abiertos"))
                        done
                        catidad_abiertos="$sum_catidad_abiertos"
                        printf "\n%-10s%-20s%-20s%-10d%-10s%-10d%-10d%-10d%-10d" "$systema" "$montaje" "$nombre" "$memoria"  "$cantidad_" "$memoria_usada" "$mayor" "$menor" "$catidad_abiertos"
                        sum_catidad_abiertos=0
                    fi
                fi
            else
                if [ "$df" = "0" ];then
                    printf "\n%-10s%-20s%-20s%-10d%-10s%-10d%-10s%-10s" "$systema" "$montaje" "$nombre" "$memoria"  "$cantidad_" "$memoria_usada" "*" "*"
                fi
            fi
        fi

    done
}


nohead=0
inv=0
df=0
u=0
activoarray=0
s_open=0
s_device=0
while [ "$1" != "" ]; do
   case $1 in
        -inv )
            u=0
            inv=1
            ;;
        -u )
            if [ "$df" = 1 ];then
                error_exit "No se puede utilizar -devicefile y -u a la vez"
            fi
            activoarray=1
            u=1
            df=1
            ;;
        -devicefiles )
            if [ "$u" = 1 ];then
                error_exit "No se puede utilizar -devicefile y -u a la vez"
            fi
            df=1
            ;;
        -noheader )
            u=0
            nohead=1
            ;;
        -sopen )
            s_open=1
            ;;
        -sdevice )
            s_device=1
            ;;
        -h | --help )
            if [[ "$inv" = "1" || "$df" = "1" || "$u" = "1" ]]; then
                error_exit "--help o -h deben estar solos"
            fi
            u=0
            usage
            exit 1
            ;;
        -*) 
            u=0
            error_exit "$1 no fue reconocido"
            ;;
        *)
            if [ "$u" = "1" ];then
                array+=("$1")
                actual=$(cat /etc/passwd | cut -d ":" -f1 | grep "$1" 2>/dev/null)
                if [ "$actual" = "" ]; then
                    error_exit "El usuario $1 no existe"
                fi
            else
                error_exit "$1 no es una opción"
            fi
            ;;
   esac
   shift
done




cat << _EOF_
    $TEXT_BOLD$TITTLES$TEXT_RESET
    $(type_sysinfo)
_EOF_







