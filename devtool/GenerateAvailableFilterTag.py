from ExtractData import GetCommissionDataList, Commission
import yaml
from pathlib import Path
import os

PYTHON_FILE_CURRENT_PATH = Path(os.path.dirname(os.path.abspath(__file__)))


def main():
    AvailableFilterTagList = []
    CommissionDataList = GetCommissionDataList()
    for i in CommissionDataList:
        if i.AlasFilterTag not in AvailableFilterTagList:
            AvailableFilterTagList.append(i.AlasFilterTag)
    with open(file=PYTHON_FILE_CURRENT_PATH / ".." / "data" / "AvailableFilterTag.yaml", mode="w", encoding="utf-8") as f:
        yaml.dump(AvailableFilterTagList, f)


if __name__ == "__main__":
    main()