#!/bin/bash

 
for((i=1;i<=10;i++));
do 
ping -c 1 -t $i www.4399.com
done
