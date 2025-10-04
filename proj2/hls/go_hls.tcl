
# Copyright (c) 2016-2019, NVIDIA CORPORATION.  All rights reserved.
# 
# Licensed under the Apache License, Version 2.0 (the "License")
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

source nvhls_exec.tcl

proc nvhls::usercmd_post_assembly {} {
    upvar TOP_NAME TOP_NAME


 #  directive set /$TOP_NAME/run/while -PIPELINE_INIT_INTERVAL 2
    # Set pipeline initialization interval for loops
    directive set /$TOP_NAME/run/while -PIPELINE_STALL_MODE flush

    # Add loop unrolling for FIR computation
    directive set /$TOP_NAME/run/optimize1 -UNROLL 16
   # directive set /$TOP_NAME/run/optimize2 -PIPELINE_INIT_INTERVAL 2

    # Partition arrays to remove memory bottlenecks
    # directive set /$TOP_NAME/run/input_data_buffer -PARTITION cyclic factor=16
    # directive set /$TOP_NAME/run/weight_data_buffer -PARTITION complete
    # directive set /$TOP_NAME/run/output_data_buffer -PARTITION cyclic factor=4
    #     # Apply array partitioning directives
#     # directive set /$TOP_NAME/weight_data -ARRAY_PARTITION cyclic 4
#     # directive set /$TOP_NAME/input_data -ARRAY_PARTITION cyclic 4
#     # directive set /$TOP_NAME/output_data -ARRAY_PARTITION cyclic 4

#     # Explicitly map arrays to RAM
#     #directive set /$TOP_NAME/weight_data -RESOURCE RAM_1P
#     #directive set /$TOP_NAME/input_data -RESOURCE RAM_1P
#     #directive set /$TOP_NAME/output_data -RESOURCE RAM_1P
}

nvhls::run
