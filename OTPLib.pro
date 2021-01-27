TEMPLATE = subdirs
SUBDIRS += OTPLib_TESTS OTPLib

OTPLib_TESTS.subdir = tests
OTPLib.subdir  = src

OTPLib_TESTS.depends = OTPLib
