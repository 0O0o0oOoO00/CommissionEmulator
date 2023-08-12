# Commission Emulator
委托模拟器  
请先阅读[注意事项](#注意事项)

## 环境
- Python >= 3.11.4
- Cmake >= 3.25
- MSVC

## 构建
下载源码: 

    git clone https://github.com/0O0o0oOoO00/CommissionEmulator

进入项目目录:

    cd CommissionEmulator

生成配置:

    cmake -B build -DCMAKE_BUILD_TYPE=Release

构建:

    cmake --build build --config Release

构建产物在`CommissionEmulator/bin`

## 使用
这里以模拟天数为`10000`，紧急委托掉率为`0.06`，并且使用以下过滤器为例

    Gem-8 > Gem-4 > Gem-2 > DailyChip-1 > DailyChip-2 > ExtraCube-0:30 > DailyResource-1 > 
    DailyResource-2 > UrgentCube-1:30 > UrgentCube-1:45 > UrgentCube-3 > UrgentCube-2:15 > 
    UrgentBox-6 > UrgentBox-3 > UrgentBox-1 > NightDrill-8 > UrgentCube-4 > NightDrill-7 > 
    ExtraCube-1:30 > ExtraCube-3 > UrgentCube-6 > NightDrill-6 > ExtraOil-8 > NightOil-8 > 
    ExtraDrill-5:20 > ExtraDrill-3:20 > ExtraOil-4 > ExtraDrill-2 > ExtraOil-1 > UrgentDrill-1:30 > 
    ExtraDrill-0:20 > ExtraDrill-1 > UrgentDrill-4 > UrgentDrill-1 > UrgentDrill-2:40 > shortest

### emulator.exe
命令行参数:

    -d  (must)      set emulate days                    设置模拟天数
    -r  (must)      set urgent commission drop rate     设置紧急委托掉率
    -f  (must)      set filter                          设置过滤器
    -v  (option)    verbose                             输出详细结果
    -h              show this help manul                打印帮助信息

运行示例:

    emulator.exe -f "UrgentGem-8 > UrgentGem-4 > UrgentGem-2 > DailyChip-1 > DailyChip-2 > 
    ExtraCube-0:30 > DailyResource-1 > DailyResource-2 > UrgentCube-1:30 > UrgentCube-1:45 > 
    UrgentCube-3 > UrgentCube-2:15 > UrgentBox-6 > UrgentBox-3 > UrgentBox-1 > NightDrill-8 > 
    UrgentCube-4 > NightDrill-7 > ExtraCube-1:30 > ExtraCube-3 > UrgentCube-6 > NightDrill-6 > 
    ExtraOil-8 > NightOil-8 > ExtraDrill-5:20 > ExtraDrill-3:20 > ExtraOil-4 > ExtraDrill-2 > 
    ExtraOil-1 > UrgentDrill-1:30 > ExtraDrill-0:20 > ExtraDrill-1 > UrgentDrill-4 > UrgentDrill-1 > 
    UrgentDrill-2:40 > shortest" -d 10000 -r 0.06

## 注意事项
- 本模拟器不对普通用户开放
- 本项目不提供Release，需要自行编译构建
- 目前无法在Linux系统上运行，不知道为什么(需要C语言大手子)
- 如果不想或者不能自行构建，请移步[Zuosizhu/AzurLaneCommissionSimulator](https://github.com/Zuosizhu/AzurLaneCommissionSimulator)

## 感谢
- [Zuosizhu](https://github.com/Zuosizhu) 提供部分数据以及指导
- [LmeSzinc](https://github.com/LmeSzinc) 开发了[AzurLaneAutoScript
  ](https://github.com/LmeSzinc/AzurLaneAutoScript)让我无法手操，导致对委托的机制不清楚