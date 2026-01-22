#!/bin/bash
# Script to run the GPU test with local OpenCL environment

# Set path to the local vendors configuration so the loader finds our extracted driver
export OCL_ICD_VENDORS=$(pwd)/config/vendors

# Add local lib directory to library path so the executable finds libOpenCL.so and the driver finds libamdocl-orca64.so
export LD_LIBRARY_PATH=$(pwd)/libs:$LD_LIBRARY_PATH

echo "Environment configured."
echo "Running gpu_test..."
./minirt
