from typing import Dict, List
import datetime
from ExtractData import GetCommissionDataList, Commission
import json
from pathlib import Path
import os

PYTHON_FILE_CURRENT_PATH = Path(os.path.dirname(os.path.abspath(__file__)))


def GenerateCompositeFilter(DataDict: Dict[str, List[str]], DataList: List[Commission], Category="", Genre="",
                            Duration=""):
    FilterTagAdded = []
    HorizontalLine = ""
    for i in DataList:
        if Duration:
            HorizontalLine = "-"
        Name = f"{Category}{Genre}{HorizontalLine}{Duration}"
        if Name not in DataDict:
            DataDict[Name] = []

        if Category:
            if i.Category != Category:
                continue
        if Genre:
            if i.Genre != Genre:
                continue
        if Duration:
            DataTime = datetime.time(hour=int(i.Duration / 60), minute=i.Duration % 60)
            if ":" in Duration:
                Tmp = Duration.split(":")
                TargetTime = datetime.time(hour=int(Tmp[0]), minute=int(Tmp[1]))
            else:
                TargetTime = datetime.time(hour=int(Duration))
            if DataTime != TargetTime:
                continue

        if i.AlasFilterTag not in FilterTagAdded:
            DataDict[Name].append(i.AlasFilterTag)
            FilterTagAdded.append(i.AlasFilterTag)


def BatchGeneration(DataDict: Dict[str, List[str]], DataList: List[Commission], Category="", Genre="", Duration=True):
    DurationList = ["0:30",
                    "1", "1:00", "1:10", "1:20", "1:30", "1:40", "1:45",
                    "2", "2:00", "2:15", "2:30", "2:40",
                    "3", "3:00", "3:20",
                    "4", "4:00",
                    "5", "5:00", "5:20",
                    "6", "6:00",
                    "7", "7:00",
                    "8", "8:00",
                    "9", "9:00",
                    "10", "10:00",
                    "12", "12:00"]
    if Duration:
        for i in DurationList:
            GenerateCompositeFilter(DataDict, DataList, Category, Genre, i)
    else:
        GenerateCompositeFilter(DataDict, DataList, Category, Genre)


def main():
    CompositeFilterDict = {}
    CommissionDataList = GetCommissionDataList()
    GenerateCompositeFilter(CompositeFilterDict, CommissionDataList, Genre="Gem", Duration="8")
    GenerateCompositeFilter(CompositeFilterDict, CommissionDataList, Genre="Gem", Duration="4")
    GenerateCompositeFilter(CompositeFilterDict, CommissionDataList, Genre="Gem", Duration="2")
    BatchGeneration(CompositeFilterDict, CommissionDataList, "Extra")
    BatchGeneration(CompositeFilterDict, CommissionDataList, "Daily", "Chip", False)
    BatchGeneration(CompositeFilterDict, CommissionDataList, "Daily", "Resource", False)
    BatchGeneration(CompositeFilterDict, CommissionDataList, "Daily")
    BatchGeneration(CompositeFilterDict, CommissionDataList, "Night", "Oil", False)
    BatchGeneration(CompositeFilterDict, CommissionDataList, "Night", "Cube", False)
    with open(file=PYTHON_FILE_CURRENT_PATH / ".." / "data" / "FilterTranslationData.json", mode="w",
              encoding="utf-8") as f:
        json.dump(CompositeFilterDict, f, indent=4)


if __name__ == "__main__":
    main()
