echo $VULKAN_SDK

${VULKAN_SDK}/Bin/glslangValidator -V Shader.vert -o Shader.vert.spv

${VULKAN_SDK}/Bin/glslangValidator -V Shader.frag -o Shader.frag.spv