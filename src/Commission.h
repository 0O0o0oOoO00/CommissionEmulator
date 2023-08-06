#include <windef.h>

#ifndef COMMISSION_EMULATOR_COMMISSION_H
#define COMMISSION_EMULATOR_COMMISSION_H

typedef struct _INCOME{
    ULONGLONG Coin;
    ULONGLONG Mind;
    ULONGLONG Oil;
    ULONGLONG HomeCoin;
    ULONGLONG EXP;
    ULONGLONG Book;
    ULONGLONG Box;
    ULONGLONG Cube;
    ULONGLONG Drill;
    ULONGLONG Gem;
    ULONGLONG Part;
    ULONGLONG Retrofit;
    ULONGLONG Ship;
}INCOME, * PINCOME;

typedef struct _VALUE_RANGE{
    INT Max;
    INT Min;
}VALUE_RANGE, * PVALUE_RANGE;

typedef struct _COMMISSION_INCOME{
    VALUE_RANGE Coin;
    VALUE_RANGE Mind;
    VALUE_RANGE Oil;
    VALUE_RANGE HomeCoin;
    INT EXP;
    INT Book;
    INT Box;
    VALUE_RANGE Cube;
    INT Drill;
    VALUE_RANGE Gem;
    INT Part;
    INT Retrofit;
    INT Ship;
}COMMISSION_INCOME, * PCOMMISSION_INCOME;

typedef enum _COMMISSION_TYPE{
    MAJOR_COMMISSION,
    DAILY_COMMISSION,
    EXTRA_COMMISSION,
    URGENT_COMMISSION,
    NIGHT_COMMISSION
}COMMISSION_TYPE, * PCOMMISSION_TYPE;

typedef struct _COMMISSION{
    INT Duration;
    INT TimeLimit;
    COMMISSION_TYPE Type;
    INT FilterTag;
    INT Id;
    COMMISSION_INCOME NormalIncome;
    struct {
        BOOL Flag;
        COMMISSION_INCOME Income;
    }BigSuccess;
}COMMISSION, * PCOMMISSION;

#endif //COMMISSION_EMULATOR_COMMISSION_H
