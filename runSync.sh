#!/bin/bash
#The program need to exit with code 0 so it won't restart
while ! ./builddir/main
do
  echo "Restarting program..."
done
