from typing import TextIO, List
from pathlib import Path
import os

FILE_PATH = Path(os.path.abspath(os.path.dirname(__file__)))


class Range:
    Max = 0
    Min = 0


class Commission:

    def __init__(self, DataList: list[str]):
        self.Id = str()
        self.Name = str()
        self.NameId = str()
        self.FilterTag = str()
        self.BigSuccessDrop = str()
        self.Category = str()
        self.Genre = str()
        self.ChineseName = str()
        self.EnglishName = str()

        self.Duration = int()
        self.TimeLimit = int()
        self.BigSuccessRate = str()

        self.Oil = Range()
        self.Coin = Range()
        self.DecorCoin = Range()

        self.Cube = Range()
        self.Part = Range()
        self.Drill = Range()
        self.CognitiveChip = Range()
        self.Retrofit = Range()
        self.Gem = Range()
        self.Box = Range()
        self.Book = Range()
        self.Ship = Range()

        self.CommissionType = str()

        self.Id = DataList[0]
        self.FilterTag = DataList[1]
        self.Name = DataList[2]
        self.NameId = DataList[3]
        self.BigSuccessDrop = DataList[4]
        self.Category = DataList[7]
        self.Genre = DataList[8]
        self.ChineseName = DataList[9]
        self.EnglishName = DataList[10]

        OilData = DataList[11]
        if OilData:
            OilRange = OilData.split("~")
            self.Oil.Min = OilRange[0]
            self.Oil.Max = OilRange[1]
        CoinsData = DataList[12]
        if CoinsData:
            CoinsRange = CoinsData.split("~")
            self.Coin.Min = CoinsRange[0]
            self.Coin.Max = CoinsRange[1]
        DecorCoinsData = DataList[13]
        if DecorCoinsData:
            DecorCoinsRange = DecorCoinsData.split("~")
            self.DecorCoin.Min = DecorCoinsRange[0]
            self.DecorCoin.Max = DecorCoinsRange[1]
        CubeData = DataList[14]
        if CubeData:
            CubeRange = CubeData.split("~")
            self.Cube.Min = CubeRange[0]
            self.Cube.Max = CubeRange[1]
        PartData = DataList[15]
        if PartData:
            PartRange = PartData.split("~")
            self.Part.Min = PartRange[0]
            self.Part.Max = PartRange[1]
        DrillData = DataList[16]
        if DrillData:
            DrillRange = DrillData.split("~")
            self.Drill.Min = DrillRange[0]
            self.Drill.Max = DrillRange[1]
        CognitiveChipsData = DataList[17]
        if CognitiveChipsData:
            CognitiveChipsRange = CognitiveChipsData.split("~")
            self.CognitiveChip.Min = CognitiveChipsRange[0]
            self.CognitiveChip.Max = CognitiveChipsRange[1]
        RetrofitData = DataList[18]
        if RetrofitData:
            RetrofitRange = RetrofitData.split("~")
            self.Retrofit.Min = RetrofitRange[0]
            self.Retrofit.Max = RetrofitRange[1]
        GemData = DataList[19]
        if GemData:
            GemRange = GemData.split("~")
            self.Gem.Min = GemRange[0]
            self.Gem.Max = GemRange[1]
        BoxData = DataList[20]
        if BoxData:
            BoxRange = BoxData.split("~")
            self.Box.Min = BoxRange[0]
            self.Box.Max = BoxRange[1]
        BookData = DataList[21]
        if BookData:
            BookRange = BookData.split("~")
            self.Book.Min = BookRange[0]
            self.Book.Max = BookRange[1]
        ShipData = DataList[22]
        if ShipData:
            ShipRange = ShipData.split("~")
            self.Ship.Min = ShipRange[0]
            self.Ship.Max = ShipRange[1]

        self.Duration = int(DataList[23])
        self.TimeLimit = int(DataList[24])
        self.BigSuccessRate = DataList[25]

        self.CommissionType = self.Category.upper() + "_COMMISSION"


def GenerateCommissionIdDefine(File: TextIO, DataList: List[Commission]):
    for i in range(len(DataList)):
        File.write("#define {:40} {}\n".format(DataList[i].Id, i))


def GenerateFilterTagDefine(File: TextIO, DataList: List[Commission]):
    FilterTagList = []
    for i in DataList:
        FilterTagList.append(i.FilterTag)
    FilterTagList = list(set(FilterTagList))
    Length = len(FilterTagList)
    for i in range(Length):
        File.write("#define {:40} {}\n".format(FilterTagList[i], i))
    File.write("#define {:40} {}\n".format("SHORTEST_FILTER", Length + 0))


def GenerateNameIdDefine(File: TextIO, DataList: List[Commission]):
    NameIdList = []
    for i in DataList:
        if i.NameId in NameIdList:
            continue
        NameIdList.append(i.NameId)

    Length = len(NameIdList)
    for i in range(Length):
        File.write("#define {:40} {}\n".format(NameIdList[i], i))


def GenerateCommissionNameList(File: TextIO, DataList: List[Commission]):
    NameList = []
    for i in DataList:
        Name = i.ChineseName.replace(" ", "_")\
            .replace(".", "_")\
            .replace("'", "_")\
            .replace("-", "_")\
            .replace("Ⅰ", "I")\
            .replace("Ⅱ", "II")\
            .replace("Ⅲ", "III")\
            .replace("Ⅳ", "IV")\
            .replace("Ⅴ", "V")\
            .replace("Ⅵ", "VI")
        if Name in NameList:
            continue
        NameList.append(Name)

    Length = len(NameList)
    File.write("static PCHAR CommissionNameList[] = {\n")
    for i in range(Length):
        File.write(f"       \"{NameList[i]}\"")
        if i != Length - 1:
            File.write(",")
        File.write("\n")
    File.write("};\n")


def GenerateCommissionDate(File: TextIO, DataList: List[Commission]):
    for i in DataList:
        DropTagList = []
        if i.Oil.Max:
            DropTagList.append("DROP_OIL")
        if i.Coin.Max:
            DropTagList.append("DROP_COIN")
        if i.DecorCoin.Max:
            DropTagList.append("DROP_DECOR_COIN")
        if i.CognitiveChip.Max:
            DropTagList.append("DROP_COGNITIVE_CHIP")
        if i.Book.Max:
            DropTagList.append("DROP_BOOK")
        if i.Box.Max:
            DropTagList.append("DROP_BOX")
        if i.Cube.Max:
            DropTagList.append("DROP_CUBE")
        if i.Drill.Max:
            DropTagList.append("DROP_DRILL")
        if i.Gem.Max:
            DropTagList.append("DROP_GEM")
        if i.Part.Max:
            DropTagList.append("DROP_PART")
        if i.Retrofit.Max:
            DropTagList.append("DROP_RETROFIT")
        if i.Ship.Max:
            DropTagList.append("DROP_SHIP")
        File.write(f"static COMMISSION {i.Name} = {{\n"
                   f"       .Type = {i.CommissionType},\n"
                   f"       .NameId = {i.NameId},\n"
                   f"       .FilterTag = {i.FilterTag},\n"
                   f"       .Duration = {i.Duration},\n"
                   f"       .TimeLimit = {i.TimeLimit},\n"
                   f"       .DropType = {' | '.join(DropTagList)},\n"
                   f"       .Normal.Oil.Min = {i.Oil.Min},\n"
                   f"       .Normal.Oil.Max = {i.Oil.Max},\n"
                   f"       .Normal.Coin.Min = {i.Coin.Min},\n"
                   f"       .Normal.Coin.Max = {i.Coin.Max},\n"
                   f"       .Normal.DecorCoin.Min = {i.DecorCoin.Min},\n"
                   f"       .Normal.DecorCoin.Max = {i.DecorCoin.Max},\n"
                   f"       .BigSuccess.CognitiveChip.Min = {i.CognitiveChip.Min},\n"
                   f"       .BigSuccess.CognitiveChip.Max = {i.CognitiveChip.Max},\n"
                   f"       .BigSuccess.Book.Min = {i.Book.Min},\n"
                   f"       .BigSuccess.Book.Max = {i.Book.Max},\n"
                   f"       .BigSuccess.Box.Min = {i.Box.Min},\n"
                   f"       .BigSuccess.Box.Max = {i.Box.Max},\n"
                   f"       .BigSuccess.Cube.Min = {i.Cube.Min},\n"
                   f"       .BigSuccess.Cube.Max = {i.Cube.Max},\n"
                   f"       .BigSuccess.Drill.Min = {i.Drill.Min},\n"
                   f"       .BigSuccess.Drill.Max = {i.Drill.Max},\n"
                   f"       .BigSuccess.Gem.Min = {i.Gem.Min},\n"
                   f"       .BigSuccess.Gem.Max = {i.Gem.Max},\n"
                   f"       .BigSuccess.Part.Min = {i.Part.Min},\n"
                   f"       .BigSuccess.Part.Max = {i.Part.Max},\n"
                   f"       .BigSuccess.Retrofit.Min = {i.Retrofit.Min},\n"
                   f"       .BigSuccess.Retrofit.Max = {i.Retrofit.Max},\n"
                   f"       .BigSuccess.Ship.Min = {i.Ship.Min},\n"
                   f"       .BigSuccess.Ship.Max = {i.Ship.Max},\n"
                   f"}};\n"
                   )


def GenerateCommissionList(File: TextIO, DataList: List[Commission]):
    NameList: List[str] = []
    for i in DataList:
        NameList.append(i.Name)
    File.write("static PCOMMISSION CommissionList[] = {\n")
    for i in range(len(NameList)):
        File.write(f"       &{NameList[i]}")
        if i != len(NameList) - 1:
            File.write(",")
        File.write("\n")
    File.write("};\n")


def GenerateTargetCommissionList(File: TextIO, DataList: List[Commission], ListName: str, Type: str):
    TargetList: List[str] = []
    for i in DataList:
        if i.CommissionType == Type:
            TargetList.append(i.Id)
    File.write(f"static INT {ListName}[] = {{\n")
    for i in range(len(TargetList)):
        File.write(f"       {TargetList[i]}")
        if i != len(TargetList) - 1:
            File.write(",")
        File.write("\n")
    File.write("};\n")


def main():
    CommissionDataList: List[Commission] = []

    with open(file=FILE_PATH / ".." / "data" / "DetailedData.csv", mode="r", encoding="utf-8-sig") as f:
        DetailedDataStr = f.read()
        DetailedDataList = DetailedDataStr.split("\n")
        for i in DetailedDataList[1:]:
            if i:
                CommissionDataList.append(Commission(i.split(",")))

    File = open(file=FILE_PATH / ".." / "src" / "CommissionData.h", mode="w", encoding="gbk")
    for _ in range(3):
        File.write("// This file is generated by python script, don\'t edit manually ! ! !\n")
    File.write("#include <windef.h>\n")
    File.write("#include \"Commission.h\"\n")
    File.write("#ifndef COMMISSION_EMULATOR_COMMISSION_DATA_H\n")
    File.write("#define COMMISSION_EMULATOR_COMMISSION_DATA_H\n")

    GenerateCommissionIdDefine(File, CommissionDataList)
    GenerateFilterTagDefine(File, CommissionDataList)
    GenerateNameIdDefine(File, CommissionDataList)

    GenerateCommissionDate(File, CommissionDataList)
    GenerateCommissionList(File, CommissionDataList)
    GenerateTargetCommissionList(File, CommissionDataList, "DailyCommissionIdList", "DAILY_COMMISSION")
    GenerateTargetCommissionList(File, CommissionDataList, "MajorCommissionIdList", "MAJOR_COMMISSION")
    GenerateTargetCommissionList(File, CommissionDataList, "ExtraCommissionIdList", "EXTRA_COMMISSION")
    GenerateTargetCommissionList(File, CommissionDataList, "NightCommissionIdList", "NIGHT_COMMISSION")
    GenerateTargetCommissionList(File, CommissionDataList, "UrgentCommissionIdList", "URGENT_COMMISSION")
    GenerateCommissionNameList(File, CommissionDataList)
    File.write("#endif //COMMISSION_EMULATOR_COMMISSION_DATA_H\n")
    File.close()


if __name__ == "__main__":
    main()
