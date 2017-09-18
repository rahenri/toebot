#/bin/sh

OUTPUT=toebot.zip

rm -rf ${OUTPUT}

FILES=$(find .  | grep '\.\(h\|cpp\)$' | grep -v '_test\.cpp$')

zip ${OUTPUT} ${FILES}
