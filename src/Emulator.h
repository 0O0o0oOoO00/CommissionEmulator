#include <windef.h>

#ifndef COMMISSION_EMULATOR_EMULATOR_H
#define COMMISSION_EMULATOR_EMULATOR_H

#define MINUTES_A_DAY (24 * 60)
#define DOUBLE double

typedef struct _COMMISSION_RECORD{
    INT CommissionIsDoingCount;
    INT DailyCommissionFinishedCount;
    INT FinishDailyCommissionCount;
    DOUBLE ProcessRateOfUrgentCommissionGeneration;
}COMMISSION_RECORD, * PCOMMISSION_RECORD;

VOID EmulatorMain();

#endif //COMMISSION_EMULATOR_EMULATOR_H
