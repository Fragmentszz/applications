#!/bin/bash
  
# 检查是否提供了参数
if [ "$1" = "" ]; then
    echo "Usage: script.sh png_name"
    exit 1
fi

# 设置变量
png_name="$1"
current_path="$(dirname "$0")"
dot_path="dot"

# 构建命令
command="$dot_path -Tpng NFA.dot -o $png_name"

# 执行命令
echo "run $command"

$command

# 检查命令是否成功执行
if [ $? -eq 0 ]; then
    echo "$png_name created."
else
    echo "Failed to create $png_name."
fi
