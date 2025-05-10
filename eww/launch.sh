#!/bin/bash

# Terminate already running eww instances
eww kill

# Launch EWW daemon
eww daemon

# Open EWW window(s)
eww open time0
eww open canvas0
eww open time1
eww open canvas1

echo "Eww launched"
