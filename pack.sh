#/bin/sh

output=toebot.tar.gz

rm -rf ${output}

tar -czf ${output} *.h *.cpp
