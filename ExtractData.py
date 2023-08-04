import openpyxl


class Commission:
    Category = ""
    Genre = ""
    FilterTag = ""
    NameForC = ""
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
    Cube = 0
    Part = 0
    Retrofit = 0
    Drill = 0
    MaxGem = 0
    MinGem = 0
    BigSuccess = False

    def __init__(self, Cell):
        self.Category = Cell[2].value
        self.Genre = Cell[3].value
        self.FilterTag = Cell[4].value
        self.NameForC = Cell[4].value.replace("-", "_").replace(":", "_")
        self.CommissionLevel = int(Cell[8].value)
        Time = Cell[7].value
        self.Duration = Time.hour * 60 + Time.minute
        self.EXP = int(Cell[14].value)
        if Cell[15].value is not None:
            MindRange = Cell[15].value.split("~")
            self.MinMind = int(MindRange[0])
            self.MaxMind = int(MindRange[1])
        if Cell[17].value is not None:
            CoinRange = Cell[17].value.split("~")
            self.MinCoin = int(CoinRange[0])
            self.MaxCoin = int(CoinRange[1])
        if Cell[20].value is not None:
            OilRange = Cell[20].value.split("~")
            self.MinOil = int(OilRange[0])
            self.MaxOil = int(OilRange[1])
        if Cell[21].value is not None:
            HomeCoinRange = Cell[21].value.split("~")
            self.MinHomeCoin = int(HomeCoinRange[0])
            self.MaxHomeCoin = int(HomeCoinRange[1])
        if self.Category != "Urgent":
            if self.Genre == "Cube":
                self.Cube = 1
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
        else:
            if self.NameForC == "UrgentCube-1:30" or self.NameForC == "UrgentCube-2:15" or self.NameForC == "UrgentCube-1:45":
                self.Cube = 2
                self.BigSuccess = True
            elif self.NameForC == "UrgentCube-4" or self.NameForC == "UrgentCube-3":
                self.Cube = 3
                self.BigSuccess = True
            elif self.NameForC == "UrgentCube-6":
                self.Cube = 4
                self.BigSuccess = True
            elif self.Genre == "Box":
                self.Box = 1
                self.BigSuccess = True
            elif self.Genre == "Book":
                self.Book = 2
                self.BigSuccess = True
            elif self.NameForC == "UrgentDrill-1" or self.NameForC == "UrgentDrill-1:30" or self.NameForC == "UrgentDrill-1:10":
                self.Drill = 2
                self.BigSuccess = True
            elif self.NameForC == "UrgentDrill-2:40" or self.NameForC == "UrgentDrill-2":
                self.Drill = 3
                self.BigSuccess = True
            elif self.NameForC == "UrgentDrill-4":
                self.Drill = 4
                self.BigSuccess = True
            elif self.FilterTag == "Gem-2":
                self.MinGem = 10
                self.MaxGem = 20
            elif self.FilterTag == "Gem-4":
                self.MinGem = 25
                self.MaxGem = 40
            elif self.FilterTag == "Gem-8":
                self.MinGem = 50
                self.MaxGem = 80


def GenerateCommissionData(File, DataList):
    for i in DataList:
        File.write(f"static COMMISSION {i.NameForC}_{i.CommissionLevel} = {{\n"
                   f"       .Duration = {i.Duration},\n"
                   f"       .Type = {i.Category.upper()}_COMMISSION,\n"
                   f"       .Id = {i.NameForC.upper()}_{i.CommissionLevel}_ID,\n"
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
                   f"       .NormalIncome.Cube = 0,\n"
                   f"       .NormalIncome.Drill = 0,\n"
                   f"       .NormalIncome.Gem.Max = 0,\n"
                   f"       .NormalIncome.Gem.Min = 0,\n"
                   f"       .NormalIncome.Part = 0,\n"
                   f"       .NormalIncome.Retrofit = 0,\n"
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
                   f"       .BigSuccess.Income.Cube = {i.Cube},\n"
                   f"       .BigSuccess.Income.Drill = {i.Drill},\n"
                   f"       .BigSuccess.Income.Gem.Max = {i.MaxGem},\n"
                   f"       .BigSuccess.Income.Gem.Min = {i.MinGem},\n"
                   f"       .BigSuccess.Income.Part = {i.Part},\n"
                   f"       .BigSuccess.Income.Retrofit = {i.Retrofit}\n"
                   f"}};\n")


def GenerateCommissionDataList(File, DataList):
    File.write("static PCOMMISSION CommissionList[] = {\n")
    for i in range(len(DataList)):
        File.write(f"       &{DataList[i].NameForC}_{DataList[i].CommissionLevel}")
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
        File.write(f"       {DataList[i].NameForC.upper()}_{DataList[i].CommissionLevel}_ID")
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
        File.write(f"       {DataList[i].NameForC.upper()}_{DataList[i].CommissionLevel}_ID")
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
        File.write(f"       {DataList[i].NameForC.upper()}_{DataList[i].CommissionLevel}_ID")
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
        File.write(f"       {DataList[i].NameForC.upper()}_{DataList[i].CommissionLevel}_ID")
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
            f"{DataList[i].NameForC.upper()}_{DataList[i].CommissionLevel}_ID", hex(i).upper()))


xlsx = openpyxl.load_workbook(filename="./CommissionData.xlsx")
sheet = xlsx["Data"]
commissionDataList = []
for i in range(3, 130):
    cell = sheet[f"A{i}:AG{i}"]
    if cell[0][0].value is None:
        continue
    commissionDataList.append(Commission(cell[0]))

File = open(file="./CommissionData.h", mode="w", encoding="utf-8")
for _ in range(3):
    File.write("// This file is generated by python script, don\'t edit manually ! ! !\n")
File.write("#include <windef.h>\n")
File.write("#include \"Commission.h\"\n")
File.write("#ifndef COMMISSION_EMULATOR_COMMISSION_DATA_H\n")
File.write("#define COMMISSION_EMULATOR_COMMISSION_DATA_H\n")

GenerateCommissionIdDefine(File, commissionDataList)
GenerateCommissionData(File, commissionDataList)
GengrateCommissionFilterTagList(File, commissionDataList)

GenerateCommissionDataList(File, commissionDataList)
GenerateDailyCommissionIdList(File, commissionDataList)
GenerateExtraCommissionIdList(File, commissionDataList)
GenerateNightCommissionIdList(File, commissionDataList)
GenerateUrgentCommissionIdList(File, commissionDataList)

File.write("#endif //COMMISSION_EMULATOR_COMMISSION_DATA_H\n")
File.close()
