#!/bin/bash
echo -----------------------------
echo "Number of active users"
who | wc -l
echo -----------------------------
echo "System has been runing sice"
last reboot | sed -n '1p'
echo -----------------------------
cat /proc/cpuinfo | grep -i "Model" | sed -n '1p'
echo ---------------------------
cat /proc/meminfo | grep -i "MemTotal"
echo -----------------------------

temp=$( cat nanpa | wc -l)
echo nanpa file contains $temp lines.

echo The first 17 lines of nanpa are:
head -n 17 nanpa
echo The last 42 lines of nanpa are:
tail -n 42 nanpa

##This next part is to test something, plis don't erase it
#dd if=/dev/zero bs=512 count=10000 | tr '\0' 'A' > file1
#./head -n 5 < file1 > file2
#diff file1 file2
