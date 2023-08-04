#include <windef.h>
#include <stdio.h>
#include <time.h>
#include "Config.h"
#include "Commission.h"
#include "CommissionData.h"
#include "Emulator.h"
#define DOUBLE double

#define NONE_DATA (~0)

#define MAXIMUM_DOING_COMMISSION_COUNT 4

#define DAILY_COMMISSION_LIST_COUNT 4
#define URGENT_COMMISSION_LIST_COUNT 500

#define BIG_SUCCESS_PERCENTAGE 33

#define DAILY_COMMISSION_TYPE 0
#define URGENT_COMMISSION_TYPE 1
#define SELECT_DAILY_COMMISSION 0
#define SELECT_URGENT_COMMISSION 1

static INT FilterTagCount = sizeof(FilterTag) / sizeof(INT);

static INT CommissionListCount = sizeof(CommissionList) / sizeof(PCOMMISSION);
static INT DailyCommissionIdListCount = sizeof(DailyCommissionIdList) / sizeof(INT);
static INT ExtraCommissionIdListCount = sizeof(ExtraCommissionIdList) / sizeof(INT);
static INT NightCommissionIdListCount = sizeof(NightCommissionIdList) / sizeof(INT);
static INT UrgentCommissionIdListCount = sizeof(UrgentCommissionIdList) / sizeof(INT);


INT Random(_In_ INT Min, _In_ INT Max){
    // [ Min, Max )
    if (Min == 0 && Max == 0) {
        return 0;
    }
    srand((unsigned)time(NULL) + rand());
    return rand() % (Max - Min + 1) + Min;
}

VOID SelectCommission(_In_ PCOMMISSION DailyList[], _In_ PCOMMISSION UrgentList[],
                      _Out_ PINT pCommissionIndex, _Out_ PINT pSelectType){
    PCOMMISSION pCommission = (PCOMMISSION)NONE_DATA;
    for (int i = 0; i < FilterTagCount; ++i) {
        for (int j = 0; j < URGENT_COMMISSION_LIST_COUNT; ++j) {
            pCommission = *(PCOMMISSION*)((ULONGLONG)UrgentList + j * sizeof(PCOMMISSION));
            if (pCommission != (PCOMMISSION)NONE_DATA) {
                if (pCommission->Id == FilterTag[i]) {
                    *pSelectType = SELECT_URGENT_COMMISSION;
                    *pCommissionIndex = j;
                    return;
                }
            }
        }
        for (int j = 0; j < DAILY_COMMISSION_LIST_COUNT; ++j) {
            pCommission = *(PCOMMISSION*)((ULONGLONG)DailyList + j * sizeof(PCOMMISSION));
            if (pCommission != (PCOMMISSION)NONE_DATA) {
                if (pCommission->Id == FilterTag[i]) {
                    *pSelectType = SELECT_DAILY_COMMISSION;
                    *pCommissionIndex = j;
                    return;
                }
            }
        }
        // TODO : To be finished ! ! !
    }
}

BOOL IsCommissionBigSuccess(){
    if (Random(0, 100) < BIG_SUCCESS_PERCENTAGE) {
        return TRUE;
    }
    return FALSE;
}

INT FindBlank(_In_ PCOMMISSION List[], _In_ INT Length){
    PCOMMISSION pCommission = (PCOMMISSION)NONE_DATA;
    for (int i = 0; i < Length; ++i) {
        pCommission = *(PCOMMISSION*)((ULONGLONG)List + i * sizeof(PCOMMISSION));
        if (pCommission == (PCOMMISSION)NONE_DATA) {
            return i;
        }
    }
    return NONE_DATA;
}

VOID RefreshCommissionTime(_In_ INT CommissionTimeList[], _In_ INT Length, _In_ INT Minutes){
    PINT pCommissionTime = (PINT)NONE_DATA;
    for (int i = 0; i < Length; ++i) {
        pCommissionTime = (PINT)((ULONGLONG)CommissionTimeList + i * sizeof(INT));
        if (*pCommissionTime != NONE_DATA && *pCommissionTime > 0) {
            *pCommissionTime -= Minutes;
        }
    }
}

VOID EmulatorMain(){
    INT DoingCommissionTimeList[MAXIMUM_DOING_COMMISSION_COUNT] = {0};
    PCOMMISSION DoingCommissionList[MAXIMUM_DOING_COMMISSION_COUNT] = {0};

    INT DailyCommissionTimeList[DAILY_COMMISSION_LIST_COUNT] = {0};
    PCOMMISSION DailyCommissionList[DAILY_COMMISSION_LIST_COUNT] = {0};

    INT UrgentCommissionTimeList[URGENT_COMMISSION_LIST_COUNT] = {0};
    PCOMMISSION UrgentCommissionList[URGENT_COMMISSION_LIST_COUNT] = {0};

    RtlFillMemory(DoingCommissionTimeList, sizeof(DoingCommissionTimeList), 0xFF);
    RtlFillMemory(DoingCommissionList, sizeof(DoingCommissionList), 0xFF);
    RtlFillMemory(DailyCommissionTimeList, sizeof(DailyCommissionTimeList), 0xFF);
    RtlFillMemory(DailyCommissionList, sizeof(DailyCommissionList), 0xFF);
    RtlFillMemory(UrgentCommissionTimeList, sizeof(UrgentCommissionTimeList), 0xFF);
    RtlFillMemory(UrgentCommissionList, sizeof(UrgentCommissionList), 0xFF);

    INCOME TotalIncome = {0};

    for (int Day = 0; Day < EMULATE_DAYS; ++Day) {
        INT DailyCommissionFinishedCount = 0;
        INT CommissionIsDoingCount = 0;
        DOUBLE UrgentCommission = 0.0;
        for (int i = 0; i < 4; ++i) {
            DailyCommissionList[i] = CommissionList[DailyCommissionIdList[Random(0, DailyCommissionIdListCount)]];
            DailyCommissionTimeList[i] = DailyCommissionList[i]->Duration;
        }
        for (int Minute = 0; Minute < MINUTES_A_DAY; ++Minute) {
            for (int i = 0; i < MAXIMUM_DOING_COMMISSION_COUNT - CommissionIsDoingCount; ++i) {
                INT CommissionIndex = NONE_DATA;
                INT SelectType = NONE_DATA;
                SelectCommission(DailyCommissionList, UrgentCommissionList,
                                 &CommissionIndex, &SelectType);
                INT BlankIndex = FindBlank(DoingCommissionList, MAXIMUM_DOING_COMMISSION_COUNT);
                switch (SelectType) {
                    case SELECT_DAILY_COMMISSION:
                        DoingCommissionList[BlankIndex] = DailyCommissionList[CommissionIndex];
                        DoingCommissionTimeList[BlankIndex] = DailyCommissionList[CommissionIndex]->Duration;
                        DailyCommissionList[CommissionIndex] = (PCOMMISSION)NONE_DATA;
                        DailyCommissionTimeList[CommissionIndex] = NONE_DATA;
                        break;
                    case SELECT_URGENT_COMMISSION:
                        DoingCommissionList[BlankIndex] = UrgentCommissionList[CommissionIndex];
                        DoingCommissionTimeList[BlankIndex] = UrgentCommissionList[CommissionIndex]->Duration;
                        UrgentCommissionList[CommissionIndex] = (PCOMMISSION)NONE_DATA;
                        UrgentCommissionTimeList[CommissionIndex] = NONE_DATA;
                        break;
                    case NONE_DATA:
                        break;
                }
                if (SelectType != NONE_DATA) {
                    CommissionIsDoingCount++;
                }

            }

            RefreshCommissionTime(DoingCommissionTimeList, MAXIMUM_DOING_COMMISSION_COUNT, 1);
            RefreshCommissionTime(DailyCommissionTimeList, DAILY_COMMISSION_LIST_COUNT, 1);
            RefreshCommissionTime(UrgentCommissionTimeList, URGENT_COMMISSION_LIST_COUNT, 1);

            INT DailyCommissionCount = 0;
            for (int i = 0; i < MAXIMUM_DOING_COMMISSION_COUNT; ++i) {
                if (DoingCommissionTimeList[i] == 0) {
                    if (DoingCommissionList[i]->Type != URGENT_COMMISSION) {
                        DailyCommissionFinishedCount++;
                        DailyCommissionCount++;
                    }
                    TotalIncome.Coin += Random(DoingCommissionList[i]->NormalIncome.Coin.Min, DoingCommissionList[i]->NormalIncome.Coin.Max + 1);
                    TotalIncome.Mind += Random(DoingCommissionList[i]->NormalIncome.Mind.Min, DoingCommissionList[i]->NormalIncome.Mind.Max + 1);
                    TotalIncome.Oil += Random(DoingCommissionList[i]->NormalIncome.Oil.Min, DoingCommissionList[i]->NormalIncome.Oil.Max + 1);
                    TotalIncome.HomeCoin += Random(DoingCommissionList[i]->NormalIncome.HomeCoin.Min, DoingCommissionList[i]->NormalIncome.HomeCoin.Max + 1);
                    TotalIncome.EXP += DoingCommissionList[i]->NormalIncome.EXP;
                    if (DoingCommissionList[i]->BigSuccess.Flag == TRUE) {
                        if (IsCommissionBigSuccess() == TRUE) {
                            TotalIncome.Gem += Random(DoingCommissionList[i]->BigSuccess.Income.Gem.Min, DoingCommissionList[i]->BigSuccess.Income.Gem.Max + 1);
                            TotalIncome.Book += DoingCommissionList[i]->BigSuccess.Income.Book;
                            TotalIncome.Box += DoingCommissionList[i]->BigSuccess.Income.Box;
                            TotalIncome.Cube += DoingCommissionList[i]->BigSuccess.Income.Cube;
                            TotalIncome.Drill += DoingCommissionList[i]->BigSuccess.Income.Drill;
                            TotalIncome.Part += DoingCommissionList[i]->BigSuccess.Income.Part;
                            TotalIncome.Retrofit += DoingCommissionList[i]->BigSuccess.Income.Retrofit;
                        }
                    }
                    DoingCommissionList[i] = (PCOMMISSION)NONE_DATA;
                    DoingCommissionTimeList[i] = NONE_DATA;
                    CommissionIsDoingCount--;
                }

            }

            for (int i = 0; i < DailyCommissionCount; ++i) {
                INT BlankIndex = FindBlank(DailyCommissionList, DAILY_COMMISSION_LIST_COUNT);
                DailyCommissionList[BlankIndex] = CommissionList[DailyCommissionIdList[Random(0, DailyCommissionIdListCount)]];
                DailyCommissionTimeList[BlankIndex] = DailyCommissionList[BlankIndex]->Duration;
            }

            UrgentCommission += URGENT_COMMISSION_GET_PER_MINUTE;
            if (UrgentCommission >= 1.0) {
                INT BlankIndex = FindBlank(UrgentCommissionList, URGENT_COMMISSION_LIST_COUNT);
                UrgentCommissionList[BlankIndex] = CommissionList[UrgentCommissionIdList[Random(0, UrgentCommissionIdListCount)]];
                UrgentCommissionTimeList[BlankIndex] = UrgentCommissionList[BlankIndex]->Duration;
                UrgentCommission -= 1.0;
            }
        }
        RtlFillMemory(DoingCommissionTimeList, sizeof(DoingCommissionTimeList), 0xFF);
        RtlFillMemory(DoingCommissionList, sizeof(DoingCommissionList), 0xFF);
        RtlFillMemory(DailyCommissionTimeList, sizeof(DailyCommissionTimeList), 0xFF);
        RtlFillMemory(DailyCommissionList, sizeof(DailyCommissionList), 0xFF);
        RtlFillMemory(UrgentCommissionTimeList, sizeof(UrgentCommissionTimeList), 0xFF);
        RtlFillMemory(UrgentCommissionList, sizeof(UrgentCommissionList), 0xFF);
    }
    printf("Total :\n"
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
           "    Retrofit    : %llu\n",
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
           TotalIncome.Retrofit
           );
}