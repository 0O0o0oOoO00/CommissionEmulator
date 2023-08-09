#include <windef.h>

#ifndef COMMISSION_EMULATOR_COMMISSION_H
#define COMMISSION_EMULATOR_COMMISSION_H

typedef struct _INCOME{
    ULONGLONG Coin;
    ULONGLONG Oil;
    ULONGLONG DecorCoin;
    ULONGLONG Book;
    ULONGLONG Box;
    ULONGLONG Cube;
    ULONGLONG Drill;
    ULONGLONG Gem;
    ULONGLONG Part;
    ULONGLONG Retrofit;
    ULONGLONG Ship;
    ULONGLONG CognitiveChip;
}INCOME, * PINCOME;

typedef struct _VALUE_RANGE{
    INT Max;
    INT Min;
}VALUE_RANGE, * PVALUE_RANGE;

typedef enum _DROP_TYPE{
    DROP_OIL = 1 << 0,
    DROP_COIN = 1 << 1,
    DROP_DECOR_COIN = 1 << 2,
    DROP_COGNITIVE_CHIP = 1 << 3,
    DROP_BOOK = 1 << 4,
    DROP_BOX = 1 << 5,
    DROP_CUBE = 1 << 6,
    DROP_DRILL = 1 << 7,
    DROP_GEM = 1 << 8,
    DROP_PART = 1 << 9,
    DROP_RETROFIT = 1 << 10,
    DROP_SHIP = 1 << 11,
}DROP_TYPE, * PDROP_TYPE;

typedef struct _NORMAL_DROP{
    VALUE_RANGE Oil;
    VALUE_RANGE Coin;
    VALUE_RANGE DecorCoin;
}NORMAL_DROP, * PNORMAL_DROP;

typedef struct _BIG_SUCCESS_DROP{
    VALUE_RANGE CognitiveChip;
    VALUE_RANGE Book;
    VALUE_RANGE Box;
    VALUE_RANGE Cube;
    VALUE_RANGE Drill;
    VALUE_RANGE Gem;
    VALUE_RANGE Part;
    VALUE_RANGE Retrofit;
    VALUE_RANGE Ship;
}BIG_SUCCESS_DROP, * PBIG_SUCCESS_DROP;

typedef enum _COMMISSION_TYPE{
    DOING_COMMISSION,
    MAJOR_COMMISSION,
    DAILY_COMMISSION,
    EXTRA_COMMISSION,
    URGENT_COMMISSION,
    NIGHT_COMMISSION
}COMMISSION_TYPE, * PCOMMISSION_TYPE;

typedef struct _COMMISSION{
    INT Id;
    INT NameId;
    COMMISSION_TYPE Type;
    INT FilterTag;
    INT Duration;
    INT TimeLimit;
    DROP_TYPE DropType;
    NORMAL_DROP Normal;
    BIG_SUCCESS_DROP BigSuccess;
}COMMISSION, * PCOMMISSION;

#endif //COMMISSION_EMULATOR_COMMISSION_H
