add_subdirectory(ctkDicomAppWidget2)
add_subdirectory(MedicalImageFinder)
set(
    CMAKE_PROJECT_QRC
    ${CMAKE_SOURCE_DIR}/Resource/Project.qrc
)
set(
    CMAKE_PROJECT_TS
    ${CMAKE_SOURCE_DIR}/Resource/lang_en_US.ts
    ${CMAKE_SOURCE_DIR}/Resource/lang_zh_CN.ts
)