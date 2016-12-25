#/bin/sh

output=toebot.tar.gz

rm -rf ${output}

FILES=$(find .  | grep '\.\(h\|cpp\)$' | grep -v '_test\.cpp$')

tar -czf ${output} ${FILES}
