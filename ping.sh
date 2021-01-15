#!/bin/bash
for((i=1;i<=11;i++));
do
echo $i
ping -c 1 -t $i www.4399.com
done
