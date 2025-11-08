#!/bin/bash

# Kill any running Stellarium instances
pkill -9 stellarium 2>/dev/null

# Wait a moment for cleanup
sleep 2

# Create timestamped log filename
LOGFILE=~/stellarium_debug_$(date +%Y%m%d_%H%M%S).log

echo "Starting Stellarium with logging to: $LOGFILE"
echo "Press Ctrl+C to stop"
echo ""

# Run Stellarium and log everything
/Applications/Stellarium.app/Contents/MacOS/stellarium 2>&1 | tee "$LOGFILE"
