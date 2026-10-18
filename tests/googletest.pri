# -----------------------------------------------------------------------------
# Google Test & Google Mock QMake Integration
# -----------------------------------------------------------------------------

GTEST_DIR = $$PWD/googletest/googletest
GMOCK_DIR = $$PWD/googletest/googlemock

INCLUDEPATH += \
    $$GTEST_DIR/include \
    $$GTEST_DIR \
    $$GMOCK_DIR/include \
    $$GMOCK_DIR

SOURCES += \
    $$GTEST_DIR/src/gtest-all.cc \
    $$GMOCK_DIR/src/gmock-all.cc
