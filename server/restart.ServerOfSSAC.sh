#!/bin/bash

com=$1
appName=serverOfSSAC
lastPath=$( pwd )
rootPath=/opt/serverOfSSAC

sudo service iptables stop	#每次启动都关闭防火墙.CentOS7.0以下版本
sudo chkconfig iptables off #禁止防火墙开机启动
sudo systemctl stop firewalld.service	#每次启动都关闭防火墙.CentOS7及以上版本
sudo systemctl disable firewalld.service #禁止防火墙开机启动


if [[ s$com == s"" ]]
then
	echo "You have not input restart/start/stop for restart/start/stop $appName!"
	echo "Application will start or restart default..."
	com="start"
fi

#get process id
pidd=$(  ps -ax | grep $appName | awk '{print $1}' )

if  [[ s$com == s"restart" || s$com == s"start" ]]
then
	sudo kill -KILL $pidd
	cd $rootPath/
	sudo nohup ./$appName 2>/dev/null &
elif [[ s$com == s"stop" ]]
then
	sudo kill -KILL $pidd
fi
cd $lastPath

echo -e "Process $appName $com \033[40;31mSuccess\033[0m!"
