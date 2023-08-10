# Commission Emulator
委托模拟器

## 环境
- Python >= 3.11.4
- Cmake >= 3.25

## 构建
下载源码: 

    git clone https://github.com/0O0o0oOoO00/CommissionEmulator

进入项目目录:

    cd CommissionEmulator

生成配置:

    cmake -B build -DCMAKE_BUILD_TYPE=Release

构建:

    cmake --build build --config Release

## 注意
- 默认模拟为`1000000`天，`每分钟委托获取数量`默认为`0.05`
