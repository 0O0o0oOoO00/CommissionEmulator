import openpyxl


class Commission:
    Category = ""
    Genre = ""
    FilterTag = ""
    NameForC = ""
    TimeLimit = 0
    CommissionLevel = 0
    Duration = 0
    MinCoin = 0
    MaxCoin = 0
    MinMind = 0
    MaxMind = 0
    MinOil = 0
    MaxOil = 0
    MinHomeCoin = 0
    MaxHomeCoin = 0
    EXP = 0
    Book = 0
    Box = 0
    MaxCube = 0
    MinCube = 0
    Part = 0
    Retrofit = 0
    Drill = 0
    MaxGem = 0
    MinGem = 0
    Ship = 0
    ShipLevel = 0
    BigSuccess = False

    def __init__(self, Cell):
        self.Category = Cell[2].value
        self.Genre = Cell[3].value
        self.FilterTag = Cell[4].value
        self.CommissionLevel = int(Cell[8].value)
        Time = Cell[7].value
        self.Duration = Time.hour * 60 + Time.minute
        self.ShipLevel = Cell[9].value
        self.NameForC = Cell[4].value.replace("-", "_").replace(":", "_").upper()
        Limit = Cell[12].value
        if Limit is not None:
            self.TimeLimit = Time.hour * 60 + Time.minute
        self.EXP = int(Cell[13].value)
        if Cell[14].value is not None:
            MindRange = Cell[14].value.split("~")
            self.MinMind = int(MindRange[0])
            self.MaxMind = int(MindRange[1])
        if Cell[15].value is not None:
            CoinRange = Cell[15].value.split("~")
            self.MinCoin = int(CoinRange[0])
            self.MaxCoin = int(CoinRange[1])
        if Cell[16].value is not None:
            OilRange = Cell[16].value.split("~")
            self.MinOil = int(OilRange[0])
            self.MaxOil = int(OilRange[1])
        if Cell[17].value is not None:
            HomeCoinRange = Cell[17].value.split("~")
            self.MinHomeCoin = int(HomeCoinRange[0])
            self.MaxHomeCoin = int(HomeCoinRange[1])
        if Cell[18].value is not None:
            if "~" in str(Cell[18].value):
                CubeRange = Cell[18].value.split("~")
                self.MinCube = int(CubeRange[0])
                self.MaxCube = int(CubeRange[1])
            else:
                self.MinCube = Cell[18].value
                self.MaxCube = Cell[18].value
            self.BigSuccess = True
        if Cell[19].value is not None:
            self.Book = Cell[19].value
            self.BigSuccess = True
        if Cell[20].value is not None:
            GemRange = Cell[20].value.split("~")
            self.MinGem = int(GemRange[0])
            self.MaxGem = int(GemRange[1])
            self.BigSuccess = True
        if Cell[21].value is not None:
            self.Retrofit = Cell[21].value
            self.BigSuccess = True
        if Cell[22].value is not None:
            self.Part = Cell[22].value
            self.BigSuccess = True
        if Cell[23].value is not None:
            self.Drill = Cell[23].value
            self.BigSuccess = True
        if Cell[24].value is not None:
            self.Ship = Cell[24].value
            self.BigSuccess = True
        if Cell[25].value is not None:
            self.Box = Cell[25].value
            self.BigSuccess = True
        if self.Category != "Urgent":
            if self.Genre == "Cube":
                self.MaxCube = 1
                self.MinCube = 1
            elif self.Genre == "Retrofit":
                self.Retrofit = 1
            elif self.Genre == "Book":
                self.Book = 1
            elif self.Genre == "Drill":
                self.Drill = 1
            elif self.Genre == "Box":
                self.Box = 1
            elif self.Genre == "Part":
                self.Part = 1
            self.BigSuccess = True


def GenerateCommissionData(File, DataList):
    for i in DataList:
        File.write(f"static COMMISSION {i.NameForC} = {{\n"
                   f"       .Duration = {i.Duration},\n"
                   f"       .TimeLimit = {i.TimeLimit},\n"
                   f"       .Type = {i.Category.upper()}_COMMISSION,\n"
                   f"       .Id = {i.NameForC}_ID,\n"
                   f"       .FilterTag = {i.FilterTag.replace('-', '_').replace(':', '_').upper()}_FILTER,\n"
                   f"       .NormalIncome.Coin.Max = {i.MaxCoin},\n"
                   f"       .NormalIncome.Coin.Min = {i.MinCoin},\n"
                   f"       .NormalIncome.Mind.Max = {i.MaxMind},\n"
                   f"       .NormalIncome.Mind.Min = {i.MinMind},\n"
                   f"       .NormalIncome.Oil.Max = {i.MaxOil},\n"
                   f"       .NormalIncome.Oil.Min = {i.MinOil},\n"
                   f"       .NormalIncome.HomeCoin.Max = {i.MaxHomeCoin},\n"
                   f"       .NormalIncome.HomeCoin.Min = {i.MinHomeCoin},\n"
                   f"       .NormalIncome.EXP = {i.EXP},\n"
                   f"       .NormalIncome.Book = 0,\n"
                   f"       .NormalIncome.Box = 0,\n"
                   f"       .NormalIncome.Cube.Max = 0,\n"
                   f"       .NormalIncome.Cube.Min = 0,\n"
                   f"       .NormalIncome.Drill = 0,\n"
                   f"       .NormalIncome.Gem.Max = 0,\n"
                   f"       .NormalIncome.Gem.Min = 0,\n"
                   f"       .NormalIncome.Part = 0,\n"
                   f"       .NormalIncome.Retrofit = 0,\n"
                   f"       .NormalIncome.Ship = 0,\n"
                   f"       .BigSuccess.Flag = {str(i.BigSuccess).upper()},\n"
                   f"       .BigSuccess.Income.Coin.Max = 0,\n"
                   f"       .BigSuccess.Income.Coin.Min = 0,\n"
                   f"       .BigSuccess.Income.Mind.Max = 0,\n"
                   f"       .BigSuccess.Income.Mind.Min = 0,\n"
                   f"       .BigSuccess.Income.Oil.Max = 0,\n"
                   f"       .BigSuccess.Income.Oil.Min = 0,\n"
                   f"       .BigSuccess.Income.HomeCoin.Max = 0,\n"
                   f"       .BigSuccess.Income.HomeCoin.Min = 0,\n"
                   f"       .BigSuccess.Income.EXP = 0,\n"
                   f"       .BigSuccess.Income.Book = {i.Book},\n"
                   f"       .BigSuccess.Income.Box = {i.Box},\n"
                   f"       .BigSuccess.Income.Cube.Min = {i.MinCube},\n"
                   f"       .BigSuccess.Income.Cube.Max = {i.MaxCube},\n"
                   f"       .BigSuccess.Income.Drill = {i.Drill},\n"
                   f"       .BigSuccess.Income.Gem.Max = {i.MaxGem},\n"
                   f"       .BigSuccess.Income.Gem.Min = {i.MinGem},\n"
                   f"       .BigSuccess.Income.Part = {i.Part},\n"
                   f"       .BigSuccess.Income.Retrofit = {i.Retrofit},\n"
                   f"       .BigSuccess.Income.Ship = {i.Ship}\n"
                   f"}};\n")


def GenerateCommissionDataList(File, DataList):
    File.write("static PCOMMISSION CommissionList[] = {\n")
    for i in range(len(DataList)):
        File.write(f"       &{DataList[i].NameForC}")
        if i != len(DataList) - 1:
            File.write(",\n")
        else:
            File.write("\n")
    File.write("};\n")


def GenerateDailyCommissionIdList(File, DataList):
    File.write("static INT DailyCommissionIdList[] = {\n")
    for i in range(len(DataList)):
        if DataList[i].Category != "Daily":
            continue
        File.write(f"       {DataList[i].NameForC}_ID")
        if i != len(DataList) - 1:
            File.write(",\n")
        else:
            File.write("\n")
    File.write("};\n")


def GenerateExtraCommissionIdList(File, DataList):
    File.write("static INT ExtraCommissionIdList[] = {\n")
    for i in range(len(DataList)):
        if DataList[i].Category != "Extra":
            continue
        File.write(f"       {DataList[i].NameForC}_ID")
        if i != len(DataList) - 1:
            File.write(",\n")
        else:
            File.write("\n")
    File.write("};\n")


def GenerateNightCommissionIdList(File, DataList):
    File.write("static INT NightCommissionIdList[] = {\n")
    for i in range(len(DataList)):
        if DataList[i].Category != "Night":
            continue
        File.write(f"       {DataList[i].NameForC}_ID")
        if i != len(DataList) - 1:
            File.write(",\n")
        else:
            File.write("\n")
    File.write("};\n")


def GenerateUrgentCommissionIdList(File, DataList):
    File.write("static INT UrgentCommissionIdList[] = {\n")
    for i in range(len(DataList)):
        if DataList[i].Category != "Urgent":
            continue
        File.write(f"       {DataList[i].NameForC}_ID")
        if i != len(DataList) - 1:
            File.write(",\n")
        else:
            File.write("\n")
    File.write("};\n")


def GengrateCommissionFilterTagList(File, DataList):
    File.write("static PCHAR CommissionFilterTagList[] = {\n")
    for i in range(len(DataList)):
        File.write(f"       \"{DataList[i].FilterTag}-{DataList[i].CommissionLevel}\"")
        if i != len(DataList) - 1:
            File.write(",\n")
        else:
            File.write("\n")
    File.write("};\n")


def GenerateCommissionIdDefine(File, DataList):
    for i in range(len(DataList)):
        File.write("#define {:25}   {}\n".format(
            f"{DataList[i].NameForC}_ID", i))


def GenerateCommissionFilterTagIdDefine(File, DataList):
    FilterTagList = []
    for i in DataList:
        FilterTagList.append(i.FilterTag.upper() + "_FILTER")
    FilterTagList = list(set(FilterTagList))
    FilterTagList.append("SHORTEST_FILTER")
    for i in range(len(FilterTagList)):
        File.write("#define {:25}   {}\n".format(
            FilterTagList[i].replace("-", "_").replace(":", "_"), i))


def HandleRepeatCommissionData(DataList):
    IdDataDict = {}
    for i in DataList:
        CommissionIdName = i.NameForC
        if CommissionIdName in IdDataDict:
            i.NameForC += f"_{IdDataDict[CommissionIdName]}"
            IdDataDict[CommissionIdName] += 1
        else:
            IdDataDict[CommissionIdName] = 1


xlsx = openpyxl.load_workbook(filename="../data/CommissionData.xlsx")
sheet = xlsx["Data"]
commissionDataList = []
for i in range(3, 122):
    cell = sheet[f"A{i}:AA{i}"]
    if cell[0][0].value is None:
        continue
    commissionDataList.append(Commission(cell[0]))

HandleRepeatCommissionData(commissionDataList)

File = open(file="../src/CommissionData.h", mode="w", encoding="utf-8")
for _ in range(3):
    File.write("// This file is generated by python script, don\'t edit manually ! ! !\n")
File.write("#include <windef.h>\n")
File.write("#include \"Commission.h\"\n")
File.write("#ifndef COMMISSION_EMULATOR_COMMISSION_DATA_H\n")
File.write("#define COMMISSION_EMULATOR_COMMISSION_DATA_H\n")

GenerateCommissionIdDefine(File, commissionDataList)
GenerateCommissionFilterTagIdDefine(File, commissionDataList)
GenerateCommissionData(File, commissionDataList)
GengrateCommissionFilterTagList(File, commissionDataList)

GenerateCommissionDataList(File, commissionDataList)
GenerateDailyCommissionIdList(File, commissionDataList)
GenerateExtraCommissionIdList(File, commissionDataList)
GenerateNightCommissionIdList(File, commissionDataList)
GenerateUrgentCommissionIdList(File, commissionDataList)

File.write("#endif //COMMISSION_EMULATOR_COMMISSION_DATA_H\n")
File.close()
