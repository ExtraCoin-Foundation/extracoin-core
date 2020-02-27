CONFIG += c++1z
QT += concurrent network
linux: QMAKE_CXXFLAGS += -Wall -Werror=return-type -Werror=implicit-fallthrough -Wno-unused-function # -Wno-unused-value -Wno-unused-parameter -Wno-unused-variable

QMAKE_SPEC_T = $$[QMAKE_SPEC]
contains(QMAKE_SPEC_T,.*win32.*) {
    COMPILE_DATE=$$system(date /t)
} else {
    COMPILE_DATE=$$system(date +%m.%d)
}

GIT_COMMIT_CORE = $$system(git --git-dir .git --work-tree $$PWD describe --always --tags)
GIT_BRANCH_CORE = $$system(git --git-dir .git --work-tree $$PWD symbolic-ref --short HEAD)

lessThan(QT_MAJOR_VERSION, 5): error("requires Qt 5.14+")
lessThan(QT_MINOR_VERSION, 14): error("requires Qt 5.14+")
lessThan(QT_PATCH_VERSION, 0): error("requires Qt 5.14+")
