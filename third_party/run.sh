#/bin/sh

path=$(dirname $0)
${path}/tcpclient.py frogherd.com 2083 "$1" toebot putaquepariu "$2"
