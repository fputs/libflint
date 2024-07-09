#!/usr/bin/env sh

set -e

CFLAGS="-I./include -I/usr/local/include"

rm -f compile_commands.json

for f in $(find ./src -type f | grep -v macos); do
  n=$(echo $f | grep -o '[^/]*$' | sed 's/c$/json/')
  cc -MJ $n $CFLAGS -c $f
done

rm *.o
sed -e '1s/^/[/' -e '$s/,$/]/' *.json > compile_commands.out
rm *.json
mv compile_commands.out compile_commands.json
