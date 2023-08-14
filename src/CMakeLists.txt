add_executable(emulator main.c Commission.h CommissionData.h Emulator.c Emulator.h TypeDef.h)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Controller.py" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/../bin")
file(RENAME "${CMAKE_CURRENT_SOURCE_DIR}/../bin/Controller.py" "${CMAKE_CURRENT_SOURCE_DIR}/../bin/ec.py")

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../data/FilterTranslationData.json" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/../bin")
file(RENAME "${CMAKE_CURRENT_SOURCE_DIR}/../bin/FilterTranslationData.json" "${CMAKE_CURRENT_SOURCE_DIR}/../bin/ftd.json")

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../data/AvailableFilterTag.yaml" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/../bin")
file(RENAME "${CMAKE_CURRENT_SOURCE_DIR}/../bin/AvailableFilterTag.yaml" "${CMAKE_CURRENT_SOURCE_DIR}/../bin/aft.yaml")