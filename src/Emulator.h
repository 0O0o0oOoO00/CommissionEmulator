#include <windef.h>

#ifndef COMMISSION_EMULATOR_EMULATOR_H
#define COMMISSION_EMULATOR_EMULATOR_H

#define MINUTES_A_DAY (24 * 60)
#define DOUBLE double

static INT FilterTagCount = sizeof(FilterTagName) / sizeof(PCHAR);

extern double UrgentCommissionDropPerMinute;
extern INT EmulateDays;
extern INT AvailableFilterTagCount;
extern INT FilterTag[ALL_FILTER_TAG_COUNT];
extern BOOL IsVerbose;

typedef struct _COMMISSION_RECORD{
    INT CommissionIsDoingCount;
    INT WaitingDailyCommissionCount;
    INT WaitingUrgentCommissionCount;
    DOUBLE ProcessRateOfUrgentCommissionGeneration;
    INT GeneratedDailyCommission;
    INT UrgentCommissionCount;

    INT DailyCount;
    INT ExtraCount;
    INT NightCount;
    INT UrgentCount;
}COMMISSION_RECORD, * PCOMMISSION_RECORD;

VOID EmulatorMain();

#endif //COMMISSION_EMULATOR_EMULATOR_H
