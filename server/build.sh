#!/bin/bash

# 确保ServerLog目录存在
if [ ! -d "ServerLog" ]; then
    if ! mkdir -p "ServerLog"; then
        echo "错误：无法创建ServerLog目录" >&2
        exit 1
    fi
    echo "已创建ServerLog目录"
fi

# 清理并构建项目
rm -rf build
mkdir build || exit 1
cd build || exit 1
cmake .. || exit 1
make || exit 1
cd build || exit 1
# ./a.out
