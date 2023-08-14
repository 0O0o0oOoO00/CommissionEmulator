import argparse
import os
from operator import attrgetter
from pathlib import Path
import subprocess
import json
import yaml
import threading

PYTHON_FILE_CURRENT_PATH = Path(os.path.dirname(os.path.abspath(__file__)))

CommandLineArguments = None

with open(file=PYTHON_FILE_CURRENT_PATH / "ftd.json", mode="r", encoding="utf-8") as f:
    COMPOSITE_FILTER_DATA = json.load(f)
with open(file=PYTHON_FILE_CURRENT_PATH / "aft.yaml", mode="r", encoding="utf-8") as f:
    AVAILABLE_FILTER_TAG = yaml.load(f, yaml.Loader)


class Result:
    def __init__(self, DataDict: dict):
        self.Oil = DataDict["Oil"]
        self.Coin = DataDict["Coin"]
        self.DecorCoin = DataDict["DecorCoin"]
        self.Cube = DataDict["Cube"]
        self.Part = DataDict["Part"]
        self.Drill = DataDict["Drill"]
        self.CognitiveChip = DataDict["CognitiveChip"]
        self.Retrofit = DataDict["Retrofit"]
        self.Gem = DataDict["Gem"]
        self.Box = DataDict["Box"]
        self.Book = DataDict["Book"]
        self.Ship = DataDict["Ship"]
        self.Filter = DataDict["Filter"]

    def __add__(self, other):
        self.Oil = other.Oil
        self.Coin = other.Coin
        self.DecorCoin = other.DecorCoin
        self.Cube = other.Cube
        self.Part = other.Part
        self.Drill = other.Drill
        self.CognitiveChip = other.CognitiveChip
        self.Retrofit = other.Retrofit
        self.Gem = other.Gem
        self.Box = other.Box
        self.Book = other.Book
        self.Ship = other.Ship
        return self

    def __iadd__(self, other):
        return self.__add__(other)

class Emulator:
    def __init__(self):
        self.EmulatorPath = PYTHON_FILE_CURRENT_PATH
        self.EmulatorName = "emulator.exe"
        self.EmulatorDays = int()
        self.DropRate = float()
        self.Filter = list()

        self.IsVerbose = False

        self.RawResult = str()
        self.Result = dict()

    def __TranslateFilter(self) -> list[str]:
        Result = []
        for i in self.Filter:
            if i == "DailyEvent":
                continue
            if i in COMPOSITE_FILTER_DATA:
                Result += COMPOSITE_FILTER_DATA[i]
            else:
                Result.append(i)

        return Result

    def __GetCommandList(self) -> list:
        CommandList = [str(self.EmulatorPath / self.EmulatorName),
                       "-d", str(self.EmulatorDays),
                       "-r", str(self.DropRate),
                       "-f", "\"{}\"".format(" > ".join(self.__TranslateFilter()))]
        if self.IsVerbose:
            CommandList.append("-v")
        return CommandList

    def EnableVerbose(self):
        self.IsVerbose = True
        return self

    def SetParameters(self, EmulatorDays: int, DropRate: float, Filter: str = ""):
        self.EmulatorDays = EmulatorDays
        self.DropRate = DropRate
        if Filter:
            self.Filter = Filter.replace(" ", "").split(">")
        return self

    def SetFilter(self, Filter: list | str):
        if isinstance(Filter, list):
            self.Filter = Filter
        else:
            self.Filter = Filter.replace(" ", "").split(">")
        return self

    def AddFilterTag(self, Tag: str):
        self.Filter.append(Tag)
        return self

    def __ParseRawResult(self):
        for i in self.RawResult.split("\n")[4:16]:
            if not i:
                continue
            DataList = i.split(" ")
            self.Result[DataList[4]] = int(DataList[-1].split("\t")[-1])
        self.Result["Filter"] = " > ".join(self.Filter)

    def Run(self):
        with subprocess.Popen(" ".join(self.__GetCommandList()), stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                              encoding="utf-8") as proc:
            try:
                self.RawResult = proc.stdout.read()
            except AttributeError:
                pass
        self.__ParseRawResult()
        return self

    def GetResult(self) -> dict[str, int]:
        pass


def EmulateFilter(Filter: str = ""):
    if not Filter:
        emulator = Emulator().SetParameters(CommandLineArguments.EmulateDays,
                                            CommandLineArguments.DropRate,
                                            CommandLineArguments.EmulateFilter)
        if CommandLineArguments.Verbose:
            emulator.EnableVerbose()
    else:
        emulator = Emulator().SetParameters(CommandLineArguments.EmulateDays, CommandLineArguments.DropRate, Filter)
    print(emulator.Run().RawResult)


def CalculateFilter():
    EmulatorList = []
    EmulatorPool = []
    CalculateData: dict[str, Result] = {}
    CalculatedFilterTag = []
    CalculateFilter = CommandLineArguments.CalculateFilter.replace(" ", "").split(">")
    while len(CalculatedFilterTag) != len(AVAILABLE_FILTER_TAG):
        for i in AVAILABLE_FILTER_TAG:
            if i in CalculatedFilterTag:
                continue
            for _ in range(CommandLineArguments.ProcessesNumber):
                EmulatorList.append(
                    Emulator().SetParameters(CommandLineArguments.EmulateDays, CommandLineArguments.DropRate, " > ".join(CalculatedFilterTag))
                    .AddFilterTag(i).AddFilterTag("shortest"))
        for i in EmulatorList:
            EmulatorPool.append(threading.Thread(target=i.Run))
        for i in EmulatorPool:
            i.start()
        for i in EmulatorPool:
            i.join()
        for i in EmulatorList:
            Filter = i.Result["Filter"]
            if Filter in CalculateData:
                CalculateData[Filter] += Result(i.Result)
            else:
                CalculateData[Filter] = Result(i.Result)
        SortedCalculateData: list[Result] = list(sorted(CalculateData.values(), key=attrgetter(*CalculateFilter), reverse=True))
        SelectTag = SortedCalculateData[0].Filter.replace(" ", "").split(">")[-2]
        print("Select : " + SelectTag)
        CalculatedFilterTag.append(SelectTag)

        CalculateData.clear()
        EmulatorList.clear()
        EmulatorPool.clear()
    print("\nResult :\n\t", end="")
    print(" > ".join(CalculatedFilterTag))
    EmulateFilter(" > ".join(CalculatedFilterTag))


def ParseCommandLineArguments():
    global CommandLineArguments
    parser = argparse.ArgumentParser(description='Commission emulator controller.')
    parser.add_argument("-r", metavar="--drop-rate", type=float, required=True, dest="DropRate",
                        help="Set urgent commission drop rate")
    parser.add_argument("-d", metavar="--days", type=int, required=True, dest="EmulateDays", help="Set emulate days")
    group = parser.add_mutually_exclusive_group()
    group.add_argument("-e", metavar="--emulate-filter", type=str, dest="EmulateFilter", help="Emulate filter")
    group.add_argument("-c", metavar="--calculate-filter", type=str, dest="CalculateFilter", help="Calculate filter")
    parser.add_argument("-n", metavar="--process-number", type=int, default=3, dest="ProcessesNumber",
                        help="Number of processes for emulator when calculating")
    parser.add_argument("-v", dest="Verbose", action='store_true', help="Verbose")
    CommandLineArguments = parser.parse_args()


def main():
    if CommandLineArguments.EmulateFilter:
        EmulateFilter()
    if CommandLineArguments.CalculateFilter:
        CalculateFilter()


if __name__ == "__main__":
    ParseCommandLineArguments()
    main()
