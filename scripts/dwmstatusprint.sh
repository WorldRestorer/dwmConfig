#!/bin/zsh

function get_bytes {
	# Find active network interface
	interface=$(ip route get 8.8.8.8 2>/dev/null| awk '{print $5}')
	line=$(grep $interface /proc/net/dev | cut -d ':' -f 2 | awk '{print "received_bytes="$1, "transmitted_bytes="$9}')
	eval $line
	now=$(date +%s%N)
}

function get_velocity {
	value=$1
	old_value=$2
	now=$3

	timediff=$(($now - $old_time))
	velKB=$(echo "1000000000*($value-$old_value)/1024/$timediff" | bc)
	if test "$velKB" -gt 1024
	then
		echo $(echo "$velKB/1024" | bc)Mbps
	else
		echo ${velKB}Kbps
	fi
}

get_bytes
old_received_bytes=$received_bytes
old_transmitted_bytes=$transmitted_bytes
old_time=$now

check_connect(){
    interface=$(ip route get 8.8.8.8 2>/dev/null| awk '{print $5}')
    line=$(grep $interface /proc/net/dev | cut -d ':' -f 2 | awk '{print "received_bytes="$1, "transmitted_bytes="$9}')
    sped=$(echo $line | sed -r "s/.*=(.*).*/\1/")
    if [ "$sped" -eq 0 ]; then
        echo ""
    else
        echo ""
    fi
}

print_date(){
	date '+%Y-%m-%d %H:%M:%S'
}

dwm_alsa () {
    VOL=$(amixer get Master | tail -n1 | sed -r "s/.*\[(.*)%\].*/\1/")
	VOLSTAT=$(amixer get Master | tail -n1 | sed -r "s/.*\[(.*)%\] \[(.*)\]/\2/")
    printf "%s" "$SEP1"
    if [ "$VOL" -eq 0 ] || [ "$VOLSTAT" = "off" ]; then
        printf "󰖁"
    elif [ "$VOL" -gt 0 ] && [ "$VOL" -le 33 ]; then
	printf "󰖀 $VOL%%"
    elif [ "$VOL" -gt 33 ] && [ "$VOL" -le 66 ]; then
	printf "󰕾 $VOL%%"
    else
	printf " $VOL%%"
    fi
    printf "%s\n" "$SEP2"
}

light () {
    Light=$(xbacklight -get)
    printf "󰌵 $Light"
} 

#memfree=$(($(grep -m1 'MemAvailable:' /proc/meminfo | awk '{print $2}')))
#memtotal=$(($(grep -m1 'MemTotal:' /proc/meminfo | awk '{print $2}')))
#useage=$(echo "scale=2;100 * ($memfree/$memtotal)" | bc)
get_battery_combined_percent() {
    total_charge=$(expr $(acpi -b | awk '{print $4}' | grep -Eo "[0-9]+" | paste -sd+ | bc))
    battery_number=$(acpi -b | wc -l)
    percent=$(expr $total_charge / $battery_number)

    if [ "$percent" -le 33 ]; then
        if $(acpi -b | grep --quit Discharging); then
           printf "󰁻 %s%%" "$percent"
        else
           printf " %s%%" "$percent"
        fi
    elif [ "$percent" -ge 33 ] && [ "$percent" -le 66 ]; then
        if $(acpi -b | grep --quit Discharging); then
            printf "󰁾 %s%%" "$percent"
        else
            printf "󰢝 %s%%" "$percent"
        fi
    else
        if $(acpi -b | grep --quit Discharging); then
            printf "󰁹 %s%%" "$percent"
        else
            printf "󰂅 %s%%" "$percent"
        fi
    fi
}
print_CPU(){
#CPU时间计算公式：CPU_TIME=user+system+nice+idle+iowait+irq+softirq
#CPU使用率计算公式：cpu_usage=(idle2-idle1)/(cpu2-cpu1)*100
#默认时间间隔
    TIME_INTERVAL=5
    time=$(date "+%Y-%m-%d %H:%M:%S")
    LAST_CPU_INFO=$(cat /proc/stat | grep -w cpu | awk '{print $2,$3,$4,$5,$6,$7,$8}')
    LAST_SYS_IDLE=$(echo $LAST_CPU_INFO | awk '{print $4}')
    LAST_TOTAL_CPU_T=$(echo $LAST_CPU_INFO | awk '{print $1+$2+$3+$4+$5+$6+$7}')
    sleep ${TIME_INTERVAL}
    NEXT_CPU_INFO=$(cat /proc/stat | grep -w cpu | awk '{print $2,$3,$4,$5,$6,$7,$8}')
    NEXT_SYS_IDLE=$(echo $NEXT_CPU_INFO | awk '{print $4}')
    NEXT_TOTAL_CPU_T=$(echo $NEXT_CPU_INFO | awk '{print $1+$2+$3+$4+$5+$6+$7}')
    #系统空闲时间
    SYSTEM_IDLE=`echo ${NEXT_SYS_IDLE} ${LAST_SYS_IDLE} | awk '{print $1-$2}'`
    #CPU总时间
    TOTAL_TIME=`echo ${NEXT_TOTAL_CPU_T} ${LAST_TOTAL_CPU_T} | awk '{print $1-$2}'`
    echo ${SYSTEM_IDLE} ${TOTAL_TIME} | awk '{printf "󰻟 %.1f%", 100-$1/$2*100}'
}
print_mem(){
    memfree=$(free -h | awk '/^内存/ { print $3"/"$2 }' | sed 's/i//g')
    echo "󰍛 $memfree"
}
dwm_network () {
    CONNAME=$(nmcli -a | grep '已连接 到' | awk 'NR==1{print $3}')
    if [ "$CONNAME" = "" ]; then
        CONNAME=$(nmcli -t -f active,ssid dev wifi | grep '^yes' | cut -c 5-)
    fi

    PRIVATE=$(nmcli -a | grep 'inet4 192' | awk '{print $2}')
    PUBLIC=$(curl -s https://api.ipify.org)

    printf "%s" "$SEP1"
    if [ "$IDENTIFIER" = "unicode" ]; then
        printf "🌐%s %s | %s" "$CONNAME" "$PRIVATE" "$PUBLIC"
    else
        printf "󰖩 %s""$CONNAME"
    fi
    printf "%s\n" "$SEP2"
}
get_bytes
vel_recv=$(get_velocity $received_bytes $old_received_bytes $now)
vel_trans=$(get_velocity $transmitted_bytes $old_transmitted_bytes $now)

xsetroot -name " $vel_recv  $vel_trans|$(light)%|$(dwm_alsa)|$(get_battery_combined_percent)|$(print_mem)|$(print_CPU)|$(dwm_network)|$(print_date)|$(check_connect) "

exit 0
