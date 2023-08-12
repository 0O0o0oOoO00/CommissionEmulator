#include "TypeDef.h"
#include <stdio.h>
#include <time.h>
#include "Commission.h"
#include "CommissionData.h"
#include "Emulator.h"
#include <stdlib.h>
#include <string.h>


#define NONE_DATA (~0)

#define NIGHT_COMMISSION_REFRESH_TIME (21 * 60)

#define MAXIMUM_DOING_COMMISSION_COUNT 4
#define NIGHT_COMMISSION_COUNT 4
#define MAXIMUM_DAILY_COMMISSION_COUNT 10
#define MAXIMUM_DAILY_COMMISSION_LIST_COUNT 4
#define MAXIMUM_URGENT_COMMISSION_LIST_COUNT 45

#define BIG_SUCCESS_PERCENTAGE 33

static INT CommissionListCount = sizeof(CommissionList) / sizeof(PCOMMISSION);
static INT DailyCommissionIdListCount = sizeof(DailyCommissionIdList) / sizeof(INT);
static INT ExtraCommissionIdListCount = sizeof(ExtraCommissionIdList) / sizeof(INT);
static INT NightCommissionIdListCount = sizeof(NightCommissionIdList) / sizeof(INT);
static INT UrgentCommissionIdListCount = sizeof(UrgentCommissionIdList) / sizeof(INT);
static const INT CommissionNameListCount = sizeof(CommissionNameList) / sizeof(PCHAR);

PCOMMISSION DoingCommissionList[MAXIMUM_DOING_COMMISSION_COUNT] = {0};
INT DoingCommissionTimeList[MAXIMUM_DOING_COMMISSION_COUNT] = {0};

PCOMMISSION DailyCommissionWaitingList[MAXIMUM_DAILY_COMMISSION_LIST_COUNT] = {0};
INT DailyCommissionWaitingTimeList[MAXIMUM_DAILY_COMMISSION_LIST_COUNT] = {0};

PCOMMISSION UrgentCommissionWaitingList[MAXIMUM_URGENT_COMMISSION_LIST_COUNT] = {0};
INT UrgentCommissionWaitingTimeList[MAXIMUM_URGENT_COMMISSION_LIST_COUNT] = {0};

INT FinishedCommissionCount[sizeof(CommissionNameList) / sizeof(PCHAR)] = {0};

COMMISSION_RECORD CommissionRecord = {0};

ULONGLONG RandomSeedOffset = 0;

INT Random(_In_ INT Min, _In_ INT Max){
    // [ Min, Max )
    if (Min == 0 && Max == 0) {
        return 0;
    }
    if (Min == Max - 1) {
        return Min;
    }
    RandomSeedOffset += clock();
    srand((unsigned)time(NULL) + RandomSeedOffset + rand());
    return rand() % (Max - Min) + Min;
}

BOOL IsCommissionBigSuccess(){
    return Random(0, 100) < BIG_SUCCESS_PERCENTAGE ? TRUE : FALSE;
}

BOOL IsCommissionRepeated(_In_ PCOMMISSION pCommission){
    PCOMMISSION pCommissionInList = (PCOMMISSION)NONE_DATA;
    INT Length = NONE_DATA;
    COMMISSION_TYPE CommissionType = pCommission->Type;
    switch (CommissionType) {
        case NIGHT_COMMISSION:
        case URGENT_COMMISSION:
            Length = MAXIMUM_URGENT_COMMISSION_LIST_COUNT;
            break;
        case EXTRA_COMMISSION:
        case DAILY_COMMISSION:
            Length = MAXIMUM_DAILY_COMMISSION_LIST_COUNT;
            break;
    }
    if (pCommission->Type == URGENT_COMMISSION) {
        for (int i = 0; i < MAXIMUM_DOING_COMMISSION_COUNT; ++i) {
            pCommissionInList = DoingCommissionList[i];
            if (pCommissionInList == (PCOMMISSION) NONE_DATA) { continue; }
            if (pCommissionInList->NameId == pCommission->NameId) {
                return TRUE;
            }
        }
    }
    for (int i = 0; i < Length; ++i) {
        switch (CommissionType) {
            case NIGHT_COMMISSION:
            case URGENT_COMMISSION:
                pCommissionInList = UrgentCommissionWaitingList[i];
                break;
            case EXTRA_COMMISSION:
            case DAILY_COMMISSION:
                pCommissionInList = DailyCommissionWaitingList[i];
                break;
        }
        if (pCommissionInList == (PCOMMISSION)NONE_DATA) { continue;}
        if (pCommissionInList->NameId == pCommission->NameId) {
            return TRUE;
        }
    }
    return FALSE;
}

INT FindBlankIndexOfList(_In_ INT Length, _In_ PCOMMISSION List[]){
    PCOMMISSION pCommissionInList = (PCOMMISSION)NONE_DATA;
    for (int i = 0; i < Length; ++i) {
        pCommissionInList = List[i];
        if (pCommissionInList == (PCOMMISSION)NONE_DATA) {
            return i;
        }
    }
    return NONE_DATA;
}

PCOMMISSION GetCommissionById(_In_ INT CommissionId){
    return CommissionList[CommissionId];
}

PCOMMISSION RandomGenerateCommissionFromIdList(_In_ INT Length, _In_ INT IdList[]){
    return GetCommissionById(IdList[Random(0, Length)]);
}

VOID PutCommissionIntoWaitingList(_In_ PCOMMISSION pCommission){
    INT BlankIndex = NONE_DATA;
    switch (pCommission->Type) {
        case NIGHT_COMMISSION:
        case URGENT_COMMISSION:
            BlankIndex = FindBlankIndexOfList(MAXIMUM_URGENT_COMMISSION_LIST_COUNT, UrgentCommissionWaitingList);
            if (BlankIndex == NONE_DATA) {break;}
            UrgentCommissionWaitingList[BlankIndex] = pCommission;
            UrgentCommissionWaitingTimeList[BlankIndex] = pCommission->TimeLimit;
            CommissionRecord.WaitingUrgentCommissionCount++;
            break;
        case DAILY_COMMISSION:
            CommissionRecord.GeneratedDailyCommission++;
        case EXTRA_COMMISSION:
            BlankIndex = FindBlankIndexOfList(MAXIMUM_DAILY_COMMISSION_LIST_COUNT, DailyCommissionWaitingList);
            if (BlankIndex == NONE_DATA) {break;}
            DailyCommissionWaitingList[BlankIndex] = pCommission;
            DailyCommissionWaitingTimeList[BlankIndex] = pCommission->TimeLimit;
            CommissionRecord.WaitingDailyCommissionCount++;
            break;
    }
}

VOID GenerateDailyCommission(_In_ INT GenerateCount){
    PCOMMISSION pCommission = (PCOMMISSION)NONE_DATA;
    for (int i = 0; i < GenerateCount; ++i) {
        pCommission = RandomGenerateCommissionFromIdList(DailyCommissionIdListCount, DailyCommissionIdList);
        PutCommissionIntoWaitingList(pCommission);
    }
}

VOID SelectCommission(_Out_ PINT pSelectType, _Out_ PINT pIndex){
    INT SelectType = NONE_DATA;
    INT TargetIndex = NONE_DATA;
    PCOMMISSION pCommission = (PCOMMISSION)NONE_DATA;

    for (int i = 0; i < AvailableFilterTagCount; ++i) {
        if (FilterTag[i] == SHORTEST_FILTER) {
            INT ShortestTime = MINUTES_A_DAY;
            INT IndexOfShortest = NONE_DATA;
            for (int j = 0; j < MAXIMUM_DAILY_COMMISSION_LIST_COUNT; ++j) {
                pCommission = DailyCommissionWaitingList[j];
                if (pCommission == (PCOMMISSION)NONE_DATA) {continue;}
                if (pCommission->Duration < ShortestTime) {
                    ShortestTime = pCommission->Duration;
                    IndexOfShortest = j;
                }
            }
            SelectType = DAILY_COMMISSION;
            TargetIndex = IndexOfShortest;
            goto Return;
        }
        for (int j = 0; j < MAXIMUM_URGENT_COMMISSION_LIST_COUNT; ++j) {
            pCommission = UrgentCommissionWaitingList[j];
            if (pCommission == (PCOMMISSION)NONE_DATA) {continue;}
            if (pCommission->FilterTag == FilterTag[i]) {
                SelectType = URGENT_COMMISSION;
                TargetIndex = j;
                goto Return;
            }
        }
        for (int j = 0; j < MAXIMUM_DAILY_COMMISSION_LIST_COUNT; ++j) {
            pCommission = DailyCommissionWaitingList[j];
            if (pCommission == (PCOMMISSION)NONE_DATA) {continue;}
            if (pCommission->FilterTag == FilterTag[i]) {
                SelectType = DAILY_COMMISSION;
                TargetIndex = j;
                goto Return;
            }
        }
    }
    Return:
    *pSelectType = SelectType;
    *pIndex = TargetIndex;
}

VOID MoveCommissionFromWaitingListToDoingList(_In_ COMMISSION_TYPE CommissionType, _In_ INT IndexOfWaitingCommission){
    INT BlankIndex = FindBlankIndexOfList(MAXIMUM_DOING_COMMISSION_COUNT, DoingCommissionList);
    PCOMMISSION pCommissionInWaitingList = (PCOMMISSION)NONE_DATA;
    switch (CommissionType) {
        case URGENT_COMMISSION:
            pCommissionInWaitingList = UrgentCommissionWaitingList[IndexOfWaitingCommission];
            UrgentCommissionWaitingList[IndexOfWaitingCommission] = (PCOMMISSION)NONE_DATA;
            UrgentCommissionWaitingTimeList[IndexOfWaitingCommission] = NONE_DATA;
            CommissionRecord.WaitingUrgentCommissionCount--;
            break;
        case DAILY_COMMISSION:
            pCommissionInWaitingList = DailyCommissionWaitingList[IndexOfWaitingCommission];
            DailyCommissionWaitingList[IndexOfWaitingCommission] = (PCOMMISSION)NONE_DATA;
            DailyCommissionWaitingTimeList[IndexOfWaitingCommission] = NONE_DATA;
            CommissionRecord.WaitingDailyCommissionCount--;
            break;
    }
    CommissionRecord.CommissionIsDoingCount++;
    DoingCommissionList[BlankIndex] = pCommissionInWaitingList;
    DoingCommissionTimeList[BlankIndex] = pCommissionInWaitingList->Duration;
}

INT GetBlankCountCountOfDoingList(){
    INT Count = 0;
    for (int i = 0; i < MAXIMUM_DOING_COMMISSION_COUNT; ++i) {
        if (DoingCommissionList[i] == (PCOMMISSION)NONE_DATA) {Count++;}
    }
    return Count;
}

VOID SelectAndDoCommission(){
    INT SelectType = NONE_DATA;
    INT TargetIndex = NONE_DATA;
    INT BlankCount = GetBlankCountCountOfDoingList();
    for (int i = 0; i < BlankCount; ++i) {
        SelectCommission(&SelectType, &TargetIndex);
        if (SelectType == NONE_DATA || TargetIndex == NONE_DATA) {
            break;
        }
        switch (SelectType) {
            case DAILY_COMMISSION:
                MoveCommissionFromWaitingListToDoingList(DAILY_COMMISSION, TargetIndex);
                break;
            case URGENT_COMMISSION:
                MoveCommissionFromWaitingListToDoingList(URGENT_COMMISSION, TargetIndex);
                break;
            default:
                break;
        }
    }
}

VOID MinusCommissionTime(){
    for (int i = 0; i < MAXIMUM_DOING_COMMISSION_COUNT; ++i) {
        if (DoingCommissionTimeList[i] == NONE_DATA && DoingCommissionTimeList[i] != 0) {continue;}
        DoingCommissionTimeList[i]--;
    }
    for (int i = 0; i < MAXIMUM_DAILY_COMMISSION_LIST_COUNT; ++i) {
        if (DailyCommissionWaitingTimeList[i] == NONE_DATA && DailyCommissionWaitingTimeList[i] != 0) {continue;}
        DailyCommissionWaitingTimeList[i]--;
    }
    for (int i = 0; i < MAXIMUM_URGENT_COMMISSION_LIST_COUNT; ++i) {
        if (UrgentCommissionWaitingTimeList[i] == NONE_DATA && UrgentCommissionWaitingTimeList[i] != 0) {continue;}
        UrgentCommissionWaitingTimeList[i]--;
    }
}

BOOL IsCommissionFinished(_In_ INT CommissionTime){
    return CommissionTime == 0 ? TRUE : FALSE;
}

VOID CalculateIncome(_In_ PCOMMISSION pCommission, _Inout_ PINCOME pIncome){
    DROP_TYPE DropType = pCommission->DropType;
    if ((DropType & DROP_COIN) == DROP_COIN) {
        pIncome->Coin += Random(pCommission->Normal.Coin.Min, pCommission->Normal.Coin.Max + 1);
    }
    if ((DropType & DROP_OIL) == DROP_OIL) {
        pIncome->Oil += Random(pCommission->Normal.Oil.Min, pCommission->Normal.Oil.Max + 1);
    }
    if ((DropType & DROP_DECOR_COIN) == DROP_DECOR_COIN) {
        pIncome->DecorCoin += Random(pCommission->Normal.DecorCoin.Min, pCommission->Normal.DecorCoin.Max + 1);
    }
    if ((DropType & DROP_COGNITIVE_CHIP) == DROP_COGNITIVE_CHIP) {
        pIncome->CognitiveChip += Random(pCommission->BigSuccess.CognitiveChip.Min, pCommission->BigSuccess.CognitiveChip.Max + 1);
    }

    if ((DropType & ~(DROP_COIN | DROP_OIL | DROP_DECOR_COIN)) != 0) {
        if (IsCommissionBigSuccess() == TRUE) {
            if ((DropType & DROP_BOOK) == DROP_BOOK) {
                pIncome->Book += Random(pCommission->BigSuccess.Book.Min, pCommission->BigSuccess.Book.Max + 1);
            }
            if ((DropType & DROP_BOX) == DROP_BOX) {
                pIncome->Box += Random(pCommission->BigSuccess.Box.Min, pCommission->BigSuccess.Box.Max + 1);
            }
            if ((DropType & DROP_CUBE) == DROP_CUBE) {
                pIncome->Cube += Random(pCommission->BigSuccess.Cube.Min, pCommission->BigSuccess.Cube.Max + 1);
            }
            if ((DropType & DROP_DRILL) == DROP_DRILL) {
                pIncome->Drill += Random(pCommission->BigSuccess.Drill.Min, pCommission->BigSuccess.Drill.Max + 1);
            }
            if ((DropType & DROP_GEM) == DROP_GEM) {
                pIncome->Gem += Random(pCommission->BigSuccess.Gem.Min, pCommission->BigSuccess.Gem.Max + 1);
            }
            if ((DropType & DROP_PART) == DROP_PART) {
                pIncome->Part += Random(pCommission->BigSuccess.Part.Min, pCommission->BigSuccess.Part.Max + 1);
            }
            if ((DropType & DROP_RETROFIT) == DROP_RETROFIT) {
                pIncome->Retrofit += Random(pCommission->BigSuccess.Retrofit.Min, pCommission->BigSuccess.Retrofit.Max + 1);
            }
            if ((DropType & DROP_SHIP) == DROP_SHIP) {
                pIncome->Ship += Random(pCommission->BigSuccess.Ship.Min, pCommission->BigSuccess.Ship.Max + 1);
            }
        }
    }
}

VOID GenerateExtraCommission(_In_ INT GenerateCount){
    PCOMMISSION pCommission = (PCOMMISSION)NONE_DATA;
    for (int i = 0; i < GenerateCount; ++i) {
        pCommission = RandomGenerateCommissionFromIdList(ExtraCommissionIdListCount, ExtraCommissionIdList);
        PutCommissionIntoWaitingList(pCommission);
    }
}

BOOL IsTimeToGenerateNightCommission(_In_ ULONGLONG Minute){
    return Minute % NIGHT_COMMISSION_REFRESH_TIME == 0 && Minute != 0 ? TRUE : FALSE;
}

VOID GenerateNightCommission(){
    PCOMMISSION pCommission = (PCOMMISSION)NONE_DATA;
    for (int i = 0; i < NIGHT_COMMISSION_COUNT; ++i) {
        pCommission = RandomGenerateCommissionFromIdList(NightCommissionIdListCount, NightCommissionIdList);
        PutCommissionIntoWaitingList(pCommission);
    }
}

BOOL IsGenerateUrgentCommission(_In_ DOUBLE Rate){
    return Rate >= 1.0 ? TRUE : FALSE;
}

VOID GenerateUrgentCommission(){
    PCOMMISSION pCommission = (PCOMMISSION)NONE_DATA;
    CommissionRecord.ProcessRateOfUrgentCommissionGeneration -= 1.0;
    INT Count = 0;
    Start:
    if (Count > ALL_URGENT_COMMISSION_COUNT) {
        return;
    }
    pCommission = RandomGenerateCommissionFromIdList(UrgentCommissionIdListCount, UrgentCommissionIdList);
    if (IsCommissionRepeated(pCommission) == TRUE) {
        Count++;
        goto Start;
    }
    PutCommissionIntoWaitingList(pCommission);
    CommissionRecord.UrgentCommissionCount++;
}

VOID GenerateNewCommission(_In_ ULONGLONG Minute){

    if (IsTimeToGenerateNightCommission(Minute) == TRUE) {
        GenerateNightCommission();
    }
    CommissionRecord.ProcessRateOfUrgentCommissionGeneration += UrgentCommissionDropPerMinute;
    for (int i = 0; i < (INT)(CommissionRecord.ProcessRateOfUrgentCommissionGeneration); ++i) {
        if (IsGenerateUrgentCommission(CommissionRecord.ProcessRateOfUrgentCommissionGeneration) == TRUE && CommissionRecord.UrgentCommissionCount < ALL_URGENT_COMMISSION_COUNT) {
            GenerateUrgentCommission();
        }
    }
}

BOOL IsDayFinished(_In_ ULONGLONG Minute){
    return Minute % MINUTES_A_DAY == 0 && Minute != 0 ? TRUE : FALSE;
}

VOID ClearCommission(){
    memset(DailyCommissionWaitingList, NONE_DATA, MAXIMUM_DAILY_COMMISSION_LIST_COUNT * sizeof(PCOMMISSION));
    memset(DailyCommissionWaitingTimeList, NONE_DATA, MAXIMUM_DAILY_COMMISSION_LIST_COUNT * sizeof(INT));
    CommissionRecord.WaitingDailyCommissionCount = 0;
}

BOOL IsCommissionReachedTimeLimit(_In_ INT Time){
    return Time == 0 ? TRUE : FALSE;
}

VOID ClearOutdatedUrgentCommission(_In_ INT IndexOfOutdated){
    UrgentCommissionWaitingList[IndexOfOutdated] = (PCOMMISSION)NONE_DATA;
    UrgentCommissionWaitingTimeList[IndexOfOutdated] = NONE_DATA;
    CommissionRecord.UrgentCommissionCount--;
    CommissionRecord.WaitingUrgentCommissionCount--;
}

VOID FinishCommission(_In_ INT IndexOfFinishedCommission){
    PCOMMISSION pCommission = DoingCommissionList[IndexOfFinishedCommission];
    DoingCommissionList[IndexOfFinishedCommission] = (PCOMMISSION)NONE_DATA;
    DoingCommissionTimeList[IndexOfFinishedCommission] = NONE_DATA;
    switch (pCommission->Type) {
        case DAILY_COMMISSION:
            if (CommissionRecord.GeneratedDailyCommission < MAXIMUM_DAILY_COMMISSION_COUNT) {
                GenerateDailyCommission(1);
            } else {
                GenerateExtraCommission(1);
            }
            CommissionRecord.DailyCount++;
            break;
        case EXTRA_COMMISSION:
            GenerateExtraCommission(1);
            CommissionRecord.ExtraCount++;
            break;
        case NIGHT_COMMISSION:
            CommissionRecord.NightCount++;
            break;
        case URGENT_COMMISSION:
            CommissionRecord.UrgentCommissionCount--;
            CommissionRecord.UrgentCount++;
            break;
    }
    CommissionRecord.CommissionIsDoingCount--;
    FinishedCommissionCount[pCommission->NameId]++;
}

VOID ClearCounterWhenCrossDay(){
    CommissionRecord.GeneratedDailyCommission = 0;
    CommissionRecord.WaitingDailyCommissionCount = 0;
}

VOID ShowResult(_In_ PINCOME pIncome){
    ULONGLONG TotalCoin = pIncome->Coin;
    ULONGLONG TotalCognitiveChip = pIncome->CognitiveChip;
    ULONGLONG TotalOil = pIncome->Oil;
    ULONGLONG TotalDecorCoin = pIncome->DecorCoin;
    ULONGLONG TotalBook = pIncome->Book;
    ULONGLONG TotalBox = pIncome->Box;
    ULONGLONG TotalCube = pIncome->Cube;
    ULONGLONG TotalDrill = pIncome->Drill;
    ULONGLONG TotalGem = pIncome->Gem;
    ULONGLONG TotalPart = pIncome->Part;
    ULONGLONG TotalRetrofit = pIncome->Retrofit;
    ULONGLONG TotalShip = pIncome->Ship;

    printf("Emulate days : %d\n"
           "Drop rate    : %f\n"
           "Income :\n"
           "    Item              Average(Day)\tAverage(Month)\t\tTotal\n"
           "    Coin              %11f\t%13f\t\t%llu\n"
           "    CognitiveChip     %11f\t%13f\t\t%llu\n"
           "    Oil               %11f\t%13f\t\t%llu\n"
           "    DecorCoin         %11f\t%13f\t\t%llu\n"
           "    Book              %11f\t%13f\t\t%llu\n"
           "    Box               %11f\t%13f\t\t%llu\n"
           "    Cube              %11f\t%13f\t\t%llu\n"
           "    Drill             %11f\t%13f\t\t%llu\n"
           "    Gem               %11f\t%13f\t\t%llu\n"
           "    Part              %11f\t%13f\t\t%llu\n"
           "    Retrofit          %11f\t%13f\t\t%llu\n"
           "    Ship              %11f\t%13f\t\t%llu\n",
           EmulateDays,
           UrgentCommissionDropPerMinute,
           (DOUBLE)TotalCoin / EmulateDays,             (DOUBLE)TotalCoin / EmulateDays * 30,             TotalCoin,
           (DOUBLE)TotalCognitiveChip / EmulateDays,    (DOUBLE)TotalCognitiveChip / EmulateDays * 30,    TotalCognitiveChip,
           (DOUBLE)TotalOil / EmulateDays,              (DOUBLE)TotalOil / EmulateDays * 30,              TotalOil,
           (DOUBLE)TotalDecorCoin / EmulateDays,        (DOUBLE)TotalDecorCoin / EmulateDays * 30,        TotalDecorCoin,
           (DOUBLE)TotalBook / EmulateDays,             (DOUBLE)TotalBook / EmulateDays * 30,             TotalBook,
           (DOUBLE)TotalBox / EmulateDays,              (DOUBLE)TotalBox / EmulateDays * 30,              TotalBox,
           (DOUBLE)TotalCube / EmulateDays,             (DOUBLE)TotalCube / EmulateDays * 30,             TotalCube,
           (DOUBLE)TotalDrill / EmulateDays,            (DOUBLE)TotalDrill / EmulateDays * 30,            TotalDrill,
           (DOUBLE)TotalGem / EmulateDays,              (DOUBLE)TotalGem / EmulateDays * 30,              TotalGem,
           (DOUBLE)TotalPart / EmulateDays,             (DOUBLE)TotalPart / EmulateDays * 30,             TotalPart,
           (DOUBLE)TotalRetrofit / EmulateDays,         (DOUBLE)TotalRetrofit / EmulateDays * 30,         TotalRetrofit,
           (DOUBLE)TotalShip / EmulateDays,             (DOUBLE)TotalShip / EmulateDays * 30,             TotalShip
           );
    if (IsVerbose) {
        printf("\nFinish commission count :\n");
        printf("    Daily    : %d\n"
               "    Extra    : %d\n"
               "    Night    : %d\n"
               "    Urgent   : %d\n",
               CommissionRecord.DailyCount,
               CommissionRecord.ExtraCount,
               CommissionRecord.NightCount,
               CommissionRecord.UrgentCount);

        printf("\nDetailed :\n");
        for (int i = 0; i < CommissionNameListCount; ++i) {
            if (FinishedCommissionCount[i] != 0) {
                printf("    %-25s : %d\n", CommissionNameList[i], FinishedCommissionCount[i]);
            }
        }
    }
}

VOID EmulatorMain(){
    memset(DoingCommissionTimeList, NONE_DATA, sizeof(DoingCommissionTimeList));
    memset(DoingCommissionList, NONE_DATA, sizeof(DoingCommissionList));
    memset(DailyCommissionWaitingTimeList, NONE_DATA, sizeof(DailyCommissionWaitingTimeList));
    memset(DailyCommissionWaitingList, NONE_DATA, sizeof(DailyCommissionWaitingList));
    memset(UrgentCommissionWaitingTimeList, NONE_DATA, sizeof(UrgentCommissionWaitingTimeList));
    memset(UrgentCommissionWaitingList, NONE_DATA, sizeof(UrgentCommissionWaitingList));

    INCOME TotalIncome = {0};

    GenerateDailyCommission(MAXIMUM_DAILY_COMMISSION_LIST_COUNT);

    for (ULONGLONG Minute = 0; Minute < EmulateDays * MINUTES_A_DAY; ++Minute) {
        GenerateNewCommission(Minute);
        if (CommissionRecord.CommissionIsDoingCount < MAXIMUM_DOING_COMMISSION_COUNT) {
            SelectAndDoCommission();
        }
        
        MinusCommissionTime();

        for (int i = 0; i < MAXIMUM_URGENT_COMMISSION_LIST_COUNT; ++i) {
            if (UrgentCommissionWaitingTimeList[i] == NONE_DATA) {continue;}
            if (IsCommissionReachedTimeLimit(UrgentCommissionWaitingTimeList[i])) {
                ClearOutdatedUrgentCommission(i);
            }
        }

        for (int i = 0; i < MAXIMUM_DOING_COMMISSION_COUNT; ++i) {
            if (DoingCommissionTimeList[i] == NONE_DATA) {continue;}
            if (IsCommissionFinished(DoingCommissionTimeList[i]) == TRUE) {
                CalculateIncome(DoingCommissionList[i], &TotalIncome);
                FinishCommission(i);
            }
        }



        if (IsDayFinished(Minute)) {
            ClearCommission();
            ClearCounterWhenCrossDay();
            GenerateDailyCommission(MAXIMUM_DAILY_COMMISSION_LIST_COUNT);
        }
    }

    ShowResult(&TotalIncome);
}