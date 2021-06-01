#!/bin/bash
# declare an array called array and define 3 vales


gpio=( 3 4 17 27 22 23 24 5 6 13 19 26 16 20 21)
relay=( 5 6 13 19 26 16 20 21)

for i in "${gpio[@]}"
do
	echo "############################################################"
	echo  -n "GPIO"$i":"
	echo -n "Unexport  - > "
	echo $i  > /sys/class/gpio/unexport
	echo "Export"$i
	echo $i  > /sys/class/gpio/export
	echo "Set direction OUT"
	echo out  > /sys/class/gpio/gpio$i/direction
done

sleep 1

#turnoff relays
echo "############################################################"
echo "............................................................"
echo "############################################################"
echo "Turn off relays"
echo "Set GPIO"$i" value HIGH"
for i in "${relay[@]}"
do
		echo 1 > /sys/class/gpio/gpio$i/value
done

#GPIO 24 - _mux2_SIG - Configured as Input
echo "############################################################"
echo "Set GPIO"$i" (_mux2_SIG) direction IN"
echo in > /sys/class/gpio/gpio24/direction
echo "############################################################"
