#include "Commission.h"
#include "CommissionData.h"
#include "Emulator.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>

double UrgentCommissionDropPerMinute = 0.0;
INT EmulateDays = 0;
INT AvailableFilterTagCount = 0;
INT FilterTag[ALL_FILTER_TAG_COUNT] = {0};
BOOL IsVerbose = FALSE;

VOID ResolveFilterTag(_In_ PCHAR FilterTagString){
    PCHAR SymbolPositionList[ALL_FILTER_TAG_COUNT] = {0};
    PCHAR Filter[ALL_FILTER_TAG_COUNT] = {0};
    INT SymbolCount = 0;
    for (int i = 0; i < strlen(FilterTagString); ++i) {
        if (FilterTagString[i] == '>') {
            SymbolPositionList[SymbolCount] = FilterTagString + i;
            SymbolCount++;
        }
    }
    Filter[0] = FilterTagString;
    AvailableFilterTagCount++;
    for (int i = 0; i < SymbolCount; ++i) {
        PCHAR SymbolPosition = SymbolPositionList[i];
        for (int j = 1;; ++j) {
            if (*(SymbolPosition + j) != ' ') {
                Filter[i + 1] = SymbolPosition + j;
                AvailableFilterTagCount++;
                break;
            }
        }
        for (int j = 1;; ++j) {
            if (*(SymbolPosition - j) != ' ') {
                *(SymbolPosition - j + 1) = '\0';
                break;
            }
        }
    }
    for (int i = 0; i < AvailableFilterTagCount; ++i) {
        for (int j = 0; j < FilterTagCount; ++j) {
            if (strcmp(Filter[i], FilterTagName[j]) == 0) {
                FilterTag[i] = j;
            }
        }
    }
}

VOID ShowHelpManual(){
    printf("-d  (must)      set emulate days\n"
           "-r  (must)      set urgent commission drop rate\n"
           "-f  (must)      set filter\n"
           "-v  (option)    verbose\n"
           "-h              show this help manul\n");
    exit(0);
}

VOID ResolveCommandLineArguments(int argc, char* argv[]){
    INT Option = 0;
    while ((Option = getopt(argc, argv, "f:d:r:vh")) != -1) {
        switch (Option) {
            case 'f':
                ResolveFilterTag(optarg);
                break;
            case 'd':
                EmulateDays = atoi(optarg);
                break;
            case 'r':
                UrgentCommissionDropPerMinute = atof(optarg);
                break;
            case 'v':
                IsVerbose = TRUE;
                break;
            case 'h':
                ShowHelpManual();
                break;
        }
    }
}

VOID CheckConfig(){
    if (EmulateDays == 0 || AvailableFilterTagCount == 0 || UrgentCommissionDropPerMinute == 0) {
        ShowHelpManual();
    }
}

int main(int argc, char* argv[]){
    ResolveCommandLineArguments(argc, argv);
    CheckConfig();
    EmulatorMain();
    return 0;
}