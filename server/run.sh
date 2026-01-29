#!/bin/bash

#!/bin/bash

# 脚本名称：auto_restart.sh
# 功能：循环10次执行 ./build/a.out，每次运行2秒后杀掉进程

# 设置循环次数
LOOP_COUNT=10

# 循环执行
for ((i=1; i<=LOOP_COUNT; i++)); do
    echo "========== 执行第 $i 次 =========="
    
    # 启动程序并获取PID
    ./build/a.out &
    PID=$!
    echo "启动程序 PID: $PID"
    
    # 等待2秒
    sleep 8
    
    # 杀掉进程
    echo "正在终止进程 $PID..."
    kill -9 $PID 2>/dev/null
    
    # 检查是否成功终止
    if ps -p $PID > /dev/null; then
        echo "警告: 未能终止进程 $PID"
    else
        echo "成功终止进程"
    fi
    
    echo "完成第 $i 次执行"
    echo ""
    
    # 短暂间隔，便于观察
    sleep 0.5
done

echo "========== 已完成 $LOOP_COUNT 次循环执行 =========="
