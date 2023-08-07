#include <windef.h>
#include <stdio.h>
#include <time.h>
#include "Config.h"
#include "Commission.h"
#include "CommissionData.h"
#include "Emulator.h"


#define NONE_DATA (~0)

#define NIGHT_COMMISSION_REFRESH_TIME (21 * 60)

#define MAXIMUM_DOING_COMMISSION_COUNT 4
#define NIGHT_COMMISSION_COUNT 4
#define MAXIMUM_DAILY_COMMISSION 7
#define MAXIMUM_DAILY_COMMISSION_LIST_COUNT 4
#define MAXIMUM_URGENT_COMMISSION_LIST_COUNT 40

#define BIG_SUCCESS_PERCENTAGE 33


static INT FilterTagCount = sizeof(FilterTag) / sizeof(INT);

static INT CommissionListCount = sizeof(CommissionList) / sizeof(PCOMMISSION);
static INT DailyCommissionIdListCount = sizeof(DailyCommissionIdList) / sizeof(INT);
static INT ExtraCommissionIdListCount = sizeof(ExtraCommissionIdList) / sizeof(INT);
static INT NightCommissionIdListCount = sizeof(NightCommissionIdList) / sizeof(INT);
static INT UrgentCommissionIdListCount = sizeof(UrgentCommissionIdList) / sizeof(INT);

COMMISSION_RECORD CommissionRecord = {0};

ULONGLONG RandomSeedOffset = 0;

INT Random(_In_ INT Min, _In_ INT Max){
    // [ Min, Max )
    if (Min == 0 && Max == 0) {
        return 0;
    }
    if (Min == Max) {
        return Max;
    }
    RandomSeedOffset += clock();
    srand((unsigned)time(NULL) + RandomSeedOffset + clock());
    return rand() % (Max - Min) + Min;
}

BOOL IsCommissionBigSuccess(){
    return Random(0, 100) < BIG_SUCCESS_PERCENTAGE ? TRUE : FALSE;
}

BOOL IsCommissionRepeated(_In_ INT Length, _In_ PCOMMISSION List[], _In_ PCOMMISSION pCommission){
    PCOMMISSION pCommissionInList = (PCOMMISSION)NONE_DATA;
    for (int i = 0; i < Length; ++i) {
        pCommissionInList = List[i];
        if (pCommissionInList == (PCOMMISSION)NONE_DATA) { continue;}
        if (pCommissionInList == pCommission) {
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

VOID PutCommissionIntoWaitingList(_In_ INT Length, _In_ PCOMMISSION List[], _In_ INT TimeList[], _In_ PCOMMISSION pCommission){
    INT BlankIndex = FindBlankIndexOfList(Length, List);
    if (BlankIndex != NONE_DATA) {
        List[BlankIndex] = pCommission;
        TimeList[BlankIndex] = pCommission->TimeLimit;
        if (pCommission->Type == DAILY_COMMISSION || pCommission->Type == EXTRA_COMMISSION) {
            CommissionRecord.WaitingDailyCommissionCount++;
        } else {
            CommissionRecord.WaitingUrgentCommissionCount++;
        }
    }
}

VOID GenerateDailyCommission(_In_ INT Length, _In_ PCOMMISSION List[], _In_ INT TimeList[], _In_ INT GenerateCount){
    PCOMMISSION pCommission = (PCOMMISSION)NONE_DATA;
    for (int i = 0; i < GenerateCount; ++i) {
        pCommission = RandomGenerateCommissionFromIdList(DailyCommissionIdListCount, DailyCommissionIdList);
        PutCommissionIntoWaitingList(Length, List, TimeList, pCommission);
    }
}

VOID SelectCommission(_In_ INT DailyListLength, _In_ PCOMMISSION DailyList[],
                      _In_ INT UrgentListLength, _In_ PCOMMISSION UrgentList[],
                      _Out_ PINT pSelectType, _Out_ PINT pIndex){
    INT SelectType = NONE_DATA;
    INT TargetIndex = NONE_DATA;
    PCOMMISSION pCommission = (PCOMMISSION)NONE_DATA;

    for (int i = 0; i < FilterTagCount; ++i) {
        INT UrgentCount = 0;
        INT DailyCount = 0;
        if (FilterTag[i] == SHORTEST_FILTER) {
            INT ShortestTime = MINUTES_A_DAY;
            INT IndexOfShortest = NONE_DATA;
            for (int j = 0; j < DailyListLength; ++j) {
                pCommission = DailyList[j];
                if (pCommission == (PCOMMISSION)NONE_DATA) {continue;}
                if (pCommission->Duration < ShortestTime) {
                    ShortestTime = pCommission->Duration;
                    IndexOfShortest = j;
                }
                DailyCount++;
                if (DailyCount >= CommissionRecord.WaitingDailyCommissionCount) {
                    break;
                }
            }
            SelectType = DAILY_COMMISSION;
            TargetIndex = IndexOfShortest;
            goto Return;
        }
        for (int j = 0; j < UrgentListLength; ++j) {
            pCommission = UrgentList[j];
            if (pCommission == (PCOMMISSION)NONE_DATA) {continue;}
            if (pCommission->FilterTag == FilterTag[i]) {
                SelectType = URGENT_COMMISSION;
                TargetIndex = j;
                goto Return;
            }
            UrgentCount++;
            if (UrgentCount >= CommissionRecord.WaitingUrgentCommissionCount) {
                break;
            }
        }
        for (int j = 0; j < DailyListLength; ++j) {
            pCommission = DailyList[j];
            if (pCommission == (PCOMMISSION)NONE_DATA) {continue;}
            if (pCommission->FilterTag == FilterTag[i]) {
                SelectType = DAILY_COMMISSION;
                TargetIndex = j;
                goto Return;
            }
            DailyCount++;
            if (DailyCount >= CommissionRecord.WaitingDailyCommissionCount) {
                break;
            }
        }
    }
    Return:
    *pSelectType = SelectType;
    *pIndex = TargetIndex;
}

INT GetBlankCountOfCommissionList(_In_ INT Length, _In_ PCOMMISSION List[]){
    INT Count = 0;
    for (int i = 0; i < Length; ++i) {
        if (List[i] == (PCOMMISSION)NONE_DATA) {
            Count++;
        }
    }
    return Count;
}

VOID MoveCommissionFromWaitingListToDoingList(_In_ PCOMMISSION WaitingList[], _In_ INT WaitingTimeList[],
                                              _In_ INT DoingListLength, _In_ PCOMMISSION DoingList[], _In_ INT DoingTimeList[],
                                              _In_ INT IndexOfWaitingCommission){
    INT BlankIndex = FindBlankIndexOfList(DoingListLength, DoingList);
    PCOMMISSION pCommissionInWaitingList = WaitingList[IndexOfWaitingCommission];
    DoingList[BlankIndex] = pCommissionInWaitingList;
    DoingTimeList[BlankIndex] = pCommissionInWaitingList->Duration;
    WaitingList[IndexOfWaitingCommission] = (PCOMMISSION)NONE_DATA;
    WaitingTimeList[IndexOfWaitingCommission] = NONE_DATA;
    if (pCommissionInWaitingList->Type == DAILY_COMMISSION || pCommissionInWaitingList->Type == EXTRA_COMMISSION) {
        CommissionRecord.WaitingDailyCommissionCount--;
    } else {
        CommissionRecord.WaitingUrgentCommissionCount--;
    }
}

VOID SelectAndDoCommission(_In_ INT DoingListLength, _In_ PCOMMISSION DoingList[], _In_ INT DoingTimeList[],
                           _In_ INT DailyListLength, _In_ PCOMMISSION DailyList[], _In_ INT DailyTimeList[],
                           _In_ INT UrgentListLength, _In_ PCOMMISSION UrgentList[], _In_ INT UrgentTimeList[]){
    INT SelectType = NONE_DATA;
    INT TargetIndex = NONE_DATA;
    for (int i = 0; i < MAXIMUM_DOING_COMMISSION_COUNT - CommissionRecord.CommissionIsDoingCount; ++i) {
        SelectCommission(DailyListLength, DailyList, UrgentListLength, UrgentList,
                         &SelectType, &TargetIndex);
        if (SelectType == NONE_DATA || TargetIndex == NONE_DATA) {
            continue;
        }
        switch (SelectType) {
            case DAILY_COMMISSION:
                MoveCommissionFromWaitingListToDoingList(DailyList, DailyTimeList, DoingListLength, DoingList, DoingTimeList, TargetIndex);
                break;
            case URGENT_COMMISSION:
                MoveCommissionFromWaitingListToDoingList(UrgentList, UrgentTimeList, DoingListLength, DoingList, DoingTimeList, TargetIndex);
                break;
            default:
                break;
        }
        CommissionRecord.CommissionIsDoingCount++;
    }
}

VOID MinusCommissionTime(_In_ INT WaitingListLength, _In_ INT WaitingTimeList[]){
    for (int i = 0; i < WaitingListLength; ++i) {
        if (WaitingTimeList[i] != NONE_DATA && WaitingTimeList[i] != 0) {
            WaitingTimeList[i]--;
        }
    }
}

BOOL IsCommissionFinished(_In_ INT CommissionTime){
    return CommissionTime == 0 ? TRUE : FALSE;
}

VOID FinishCommission(_In_ PCOMMISSION DoingList[], _In_ INT DoingTimeList[], _In_ INT IndexOfFinishedCommission){
    PCOMMISSION pCommission = DoingList[IndexOfFinishedCommission];
    DoingList[IndexOfFinishedCommission] = (PCOMMISSION)NONE_DATA;
    DoingTimeList[IndexOfFinishedCommission] = NONE_DATA;
    if (pCommission->Type == DAILY_COMMISSION) {
        CommissionRecord.DailyCommissionFinishedCount++;
        CommissionRecord.FinishDailyCommissionCount++;
    }
    if (pCommission->Type == DAILY_COMMISSION || pCommission->Type == EXTRA_COMMISSION) {
        CommissionRecord.WaitingDailyCommissionCount--;
    } else {
        CommissionRecord.WaitingUrgentCommissionCount--;
    }
    CommissionRecord.CommissionIsDoingCount--;
}

VOID CalculateIncome(_In_ PCOMMISSION pCommission, _Inout_ PINCOME pIncome){
    pIncome->Coin += Random(pCommission->NormalIncome.Coin.Min, pCommission->NormalIncome.Coin.Max + 1);
    pIncome->Mind += Random(pCommission->NormalIncome.Mind.Min, pCommission->NormalIncome.Mind.Max + 1);
    pIncome->Oil += Random(pCommission->NormalIncome.Oil.Min, pCommission->NormalIncome.Oil.Max + 1);
    pIncome->HomeCoin += Random(pCommission->NormalIncome.HomeCoin.Min, pCommission->NormalIncome.HomeCoin.Max + 1);
    pIncome->EXP += pCommission->NormalIncome.EXP;
    if (pCommission->BigSuccess.Flag == TRUE && IsCommissionBigSuccess() == TRUE) {
        pIncome->Gem += Random(pCommission->BigSuccess.Income.Gem.Min, pCommission->BigSuccess.Income.Gem.Max + 1);
        pIncome->Book += pCommission->BigSuccess.Income.Book;
        pIncome->Box += pCommission->BigSuccess.Income.Box;
        pIncome->Cube += Random(pCommission->BigSuccess.Income.Cube.Min, pCommission->BigSuccess.Income.Cube.Max + 1);
        pIncome->Drill += pCommission->BigSuccess.Income.Drill;
        pIncome->Part += pCommission->BigSuccess.Income.Part;
        pIncome->Retrofit += pCommission->BigSuccess.Income.Retrofit;
        pIncome->Ship += pCommission->BigSuccess.Income.Ship;
    }
}

VOID FinishCommissionAndCalculateIncome(_In_ PCOMMISSION DoingList[], _In_ INT DoingTimeList[], _In_ INT IndexOfFinishedCommission,
                                        _Inout_ PINCOME pIncome){
    PCOMMISSION pCommission = DoingList[IndexOfFinishedCommission];
    FinishCommission(DoingList, DoingTimeList, IndexOfFinishedCommission);
    CalculateIncome(pCommission, pIncome);
}

VOID GenerateExtraCommission(_In_ INT Length, _In_ PCOMMISSION List[], _In_ INT TimeList[], _In_ INT GenerateCount){
    PCOMMISSION pCommission = (PCOMMISSION)NONE_DATA;
    for (int i = 0; i < GenerateCount; ++i) {
        pCommission = RandomGenerateCommissionFromIdList(ExtraCommissionIdListCount, ExtraCommissionIdList);
        PutCommissionIntoWaitingList(Length, List, TimeList, pCommission);
    }
}

BOOL IsTimeToGenerateNightCommission(_In_ ULONGLONG Minute){
    return Minute % NIGHT_COMMISSION_REFRESH_TIME == 0 && Minute != 0 ? TRUE : FALSE;
}

VOID GenerateNightCommission(_In_ INT Length, _In_ PCOMMISSION List[], _In_ INT TimeList[]){
    PCOMMISSION pCommission = (PCOMMISSION)NONE_DATA;
    for (int i = 0; i < NIGHT_COMMISSION_COUNT; ++i) {
        pCommission = RandomGenerateCommissionFromIdList(NightCommissionIdListCount, NightCommissionIdList);
        PutCommissionIntoWaitingList(Length, List, TimeList, pCommission);
    }
}

BOOL IsGenerateUrgentCommission(_In_ DOUBLE Rate){
    return Rate >= 1.0 ? TRUE : FALSE;
}

VOID GenerateUrgentCommission(_In_ INT Length, _In_ PCOMMISSION List[], _In_ INT TimeList[]){
    PCOMMISSION pCommission = (PCOMMISSION)NONE_DATA;
    for (int i = 0; i < 1; ++i) {
        Start:
        pCommission = RandomGenerateCommissionFromIdList(UrgentCommissionIdListCount, UrgentCommissionIdList);
        if (IsCommissionRepeated(Length, List, pCommission) == TRUE) {goto Start;}
        PutCommissionIntoWaitingList(Length, List, TimeList, pCommission);
    }
    CommissionRecord.ProcessRateOfUrgentCommissionGeneration -= 1.0;
}

VOID GenerateNewCommission(_In_ INT DailyListLength, _In_ PCOMMISSION DailyList[], _In_ INT DailyTimeList[],
                           _In_ INT UrgentListLength, _In_ PCOMMISSION UrgentList[], _In_ INT UrgentTimeList[],
                           _In_ ULONGLONG Minute){
    if (CommissionRecord.DailyCommissionFinishedCount <= MAXIMUM_DAILY_COMMISSION) {
        GenerateDailyCommission(DailyListLength, DailyList, DailyTimeList, MAXIMUM_DAILY_COMMISSION_LIST_COUNT - CommissionRecord.FinishDailyCommissionCount);
        CommissionRecord.FinishDailyCommissionCount = 0;
    } else {
        GenerateExtraCommission(DailyListLength, DailyList, DailyTimeList, MAXIMUM_DAILY_COMMISSION_LIST_COUNT - CommissionRecord.FinishDailyCommissionCount);
    }

    if (IsTimeToGenerateNightCommission(Minute) == TRUE) {
        GenerateNightCommission(UrgentListLength, UrgentList, UrgentTimeList);
    }

    CommissionRecord.ProcessRateOfUrgentCommissionGeneration += URGENT_COMMISSION_GET_PER_MINUTE;
    if (IsGenerateUrgentCommission(CommissionRecord.ProcessRateOfUrgentCommissionGeneration) == TRUE) {
        GenerateUrgentCommission(UrgentListLength, UrgentList, UrgentTimeList);
    }
}

BOOL IsDayFinished(_In_ ULONGLONG Minute){
    return Minute % MINUTES_A_DAY == 0 && Minute != 0 ? TRUE : FALSE;
}

VOID ClearCommission(_In_ INT DailyListLength, _In_ PCOMMISSION DailyList[], _In_ INT DailyTimeList[],
                     _In_ INT UrgentListLength, _In_ PCOMMISSION UrgentList[], _In_ INT UrgentTimeList[]){
    RtlFillMemory(DailyList, DailyListLength * sizeof(PCOMMISSION), NONE_DATA);
    RtlFillMemory(DailyTimeList, DailyListLength * sizeof(INT), NONE_DATA);
    CommissionRecord.WaitingDailyCommissionCount = 0;

    for (int i = 0; i < UrgentListLength; ++i) {
        if (UrgentList[i] == (PCOMMISSION)NONE_DATA) {continue;}
        if (UrgentList[i]->Type != NIGHT_COMMISSION) {
            UrgentList[i] = (PCOMMISSION)NONE_DATA;
            UrgentTimeList[i] = NONE_DATA;
            CommissionRecord.WaitingUrgentCommissionCount--;
        }
    }
}

BOOL IsCommissionReachedTimeLimit(_In_ INT Time){
    return Time == 0 ? TRUE : FALSE;
}

VOID ClearOutdatedUrgentCommission(_In_ PCOMMISSION List[], _In_ INT TimeList[], _In_ INT IndexOfOutdated){
    List[IndexOfOutdated] = (PCOMMISSION)NONE_DATA;
    TimeList[IndexOfOutdated] = NONE_DATA;
    CommissionRecord.WaitingUrgentCommissionCount--;
}

VOID EmulatorMain(){
    INT DoingCommissionTimeList[MAXIMUM_DOING_COMMISSION_COUNT] = {0};
    PCOMMISSION DoingCommissionList[MAXIMUM_DOING_COMMISSION_COUNT] = {0};

    INT DailyCommissionWaitingTimeList[MAXIMUM_DAILY_COMMISSION_LIST_COUNT] = {0};
    PCOMMISSION DailyCommissionWaitingList[MAXIMUM_DAILY_COMMISSION_LIST_COUNT] = {0};

    INT UrgentCommissionWaitingTimeList[MAXIMUM_URGENT_COMMISSION_LIST_COUNT] = {0};
    PCOMMISSION UrgentCommissionWaitingList[MAXIMUM_URGENT_COMMISSION_LIST_COUNT] = {0};

    RtlFillMemory(DoingCommissionTimeList, sizeof(DoingCommissionTimeList), NONE_DATA);
    RtlFillMemory(DoingCommissionList, sizeof(DoingCommissionList), NONE_DATA);
    RtlFillMemory(DailyCommissionWaitingTimeList, sizeof(DailyCommissionWaitingTimeList), NONE_DATA);
    RtlFillMemory(DailyCommissionWaitingList, sizeof(DailyCommissionWaitingList), NONE_DATA);
    RtlFillMemory(UrgentCommissionWaitingTimeList, sizeof(UrgentCommissionWaitingTimeList), NONE_DATA);
    RtlFillMemory(UrgentCommissionWaitingList, sizeof(UrgentCommissionWaitingList), NONE_DATA);

    INCOME TotalIncome = {0};

    GenerateDailyCommission(MAXIMUM_DAILY_COMMISSION_LIST_COUNT, DailyCommissionWaitingList, DailyCommissionWaitingTimeList, MAXIMUM_DAILY_COMMISSION_LIST_COUNT);
    CommissionRecord.WaitingDailyCommissionCount = MAXIMUM_DAILY_COMMISSION_LIST_COUNT;

    for (ULONGLONG Minute = 0; Minute < EMULATE_DAYS * MINUTES_A_DAY; ++Minute) {
        
        SelectAndDoCommission(MAXIMUM_DOING_COMMISSION_COUNT, DoingCommissionList, DoingCommissionTimeList,
                              MAXIMUM_DAILY_COMMISSION_LIST_COUNT, DailyCommissionWaitingList, DailyCommissionWaitingTimeList,
                              MAXIMUM_URGENT_COMMISSION_LIST_COUNT, UrgentCommissionWaitingList, UrgentCommissionWaitingTimeList);
        
        MinusCommissionTime(MAXIMUM_DOING_COMMISSION_COUNT, DoingCommissionTimeList);
        MinusCommissionTime(MAXIMUM_DAILY_COMMISSION_LIST_COUNT, DailyCommissionWaitingTimeList);
        MinusCommissionTime(MAXIMUM_URGENT_COMMISSION_LIST_COUNT, UrgentCommissionWaitingTimeList);

        for (int i = 0; i < MAXIMUM_URGENT_COMMISSION_LIST_COUNT; ++i) {
            if (UrgentCommissionWaitingTimeList[i] == NONE_DATA) {continue;}
            if (IsCommissionReachedTimeLimit(UrgentCommissionWaitingTimeList[i])) {
                ClearOutdatedUrgentCommission(UrgentCommissionWaitingList, UrgentCommissionWaitingTimeList, i);
            }
        }

        for (int i = 0; i < MAXIMUM_DOING_COMMISSION_COUNT; ++i) {
            if (DoingCommissionTimeList[i] == NONE_DATA) {continue;}
            if (IsCommissionFinished(DoingCommissionTimeList[i]) == TRUE) {
                FinishCommissionAndCalculateIncome(DoingCommissionList, DoingCommissionTimeList, i,
                                                   &TotalIncome);
            }
        }

        GenerateNewCommission(MAXIMUM_DAILY_COMMISSION_LIST_COUNT, DailyCommissionWaitingList, DailyCommissionWaitingTimeList,
                              MAXIMUM_URGENT_COMMISSION_LIST_COUNT, UrgentCommissionWaitingList, UrgentCommissionWaitingTimeList,
                              Minute);

        if (IsDayFinished(Minute)) {
            ClearCommission(MAXIMUM_DAILY_COMMISSION_LIST_COUNT, DailyCommissionWaitingList, DailyCommissionWaitingTimeList,
                            MAXIMUM_URGENT_COMMISSION_LIST_COUNT, UrgentCommissionWaitingList, UrgentCommissionWaitingTimeList);
            GenerateDailyCommission(MAXIMUM_DAILY_COMMISSION_LIST_COUNT, DailyCommissionWaitingList, DailyCommissionWaitingTimeList, MAXIMUM_DAILY_COMMISSION_LIST_COUNT);
                                    CommissionRecord.WaitingDailyCommissionCount = MAXIMUM_DAILY_COMMISSION_LIST_COUNT;
        }
    }

    printf("Emulate days    : %d\n"
           "Total :\n"
           "    Coin        : %llu\n"
           "    Mind        : %llu\n"
           "    Oil         : %llu\n"
           "    HomeCoin    : %llu\n"
           "    EXP         : %llu\n"
           "    Book        : %llu\n"
           "    Box         : %llu\n"
           "    Cube        : %llu\n"
           "    Drill       : %llu\n"
           "    Gem         : %llu\n"
           "    Part        : %llu\n"
           "    Retrofit    : %llu\n"
           "    Ship        : %llu\n",
           EMULATE_DAYS,
           TotalIncome.Coin,
           TotalIncome.Mind,
           TotalIncome.Oil,
           TotalIncome.HomeCoin,
           TotalIncome.EXP,
           TotalIncome.Book,
           TotalIncome.Box,
           TotalIncome.Cube,
           TotalIncome.Drill,
           TotalIncome.Gem,
           TotalIncome.Part,
           TotalIncome.Retrofit,
           TotalIncome.Ship
           );
}