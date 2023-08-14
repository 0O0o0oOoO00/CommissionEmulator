import argparse
import os
from pathlib import Path
import subprocess
import json

PYTHON_FILE_CURRENT_PATH = Path(os.path.dirname(os.path.abspath(__file__)))

CommandLineArguments = None

with open(file=PYTHON_FILE_CURRENT_PATH / "ftd.json", mode="r", encoding="utf-8") as f:
    COMPOSITE_FILTER_DATA = json.load(f)

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

    def SetParameters(self, EmulatorDays: int, DropRate: float, Filter: str):
        self.EmulatorDays = EmulatorDays
        self.DropRate = DropRate
        self.Filter = Filter.replace(" ", "").split(">")
        return self

    def __ParseRawResult(self):
        for i in self.RawResult.split("\n")[4:16]:
            if not i:
                continue
            DataList = i.split(" ")
            self.Result[DataList[4]] = int(DataList[-1].split("\t")[-1])

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


def EmulateFilter():
    emulator = Emulator().SetParameters(CommandLineArguments.EmulateDays,
                                        CommandLineArguments.DropRate,
                                        CommandLineArguments.EmulateFilter)
    if CommandLineArguments.Verbose:
        emulator.EnableVerbose()
    print(emulator.Run().RawResult)


def CalculateFilter():
    ...


def ParseCommandLineArguments():
    global CommandLineArguments
    parser = argparse.ArgumentParser(description='Commission emulator controller.')
    parser.add_argument("-r", metavar="--drop-rate", type=float, required=True, dest="DropRate",
                        help="Set urgent commission drop rate")
    parser.add_argument("-d", metavar="--days", type=int, required=True, dest="EmulateDays", help="Set emulate days")
    group = parser.add_mutually_exclusive_group()
    group.add_argument("-e", metavar="--emulate-filter", type=str, dest="EmulateFilter", help="Emulate filter")
    group.add_argument("-c", metavar="--calculate-filter", type=str, dest="CalculateFilter", help="Calculate filter")
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
