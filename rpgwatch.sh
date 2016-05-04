#!/bin/sh

mkfifo -m 666 tmp/rpgwatch

while true; do
	while IFS="" read -r -d $'\n' line; do
		printf '%s\n' "$line"
	done < tmp/rpgwatch
done

