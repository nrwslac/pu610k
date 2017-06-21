TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build an ioc application
# Use PROD_IOC_RTEMS for RTEMS only
# Use PROD_IOC_Linux for Linux only
PROD_IOC = pu610k

# pu610k.dbd will be created and installed
DBD += pu610k.dbd

# pu610k.dbd will be made up from these files:
pu610k_DBD += base.dbd
pu610k_DBD += iocAdmin.dbd
pu610k_DBD += autosaveSupport.dbd
pu610k_DBD += asyn.dbd
pu610k_DBD += asynRecord.dbd
pu610k_DBD += stream.dbd
pu610k_DBD += drvAsynIPPort.dbd
pu610k_DBD += pu610kSubs.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
pu610k_SRCS += pu610k_registerRecordDeviceDriver.cpp
pu610k_SRCS_DEFAULT += pu610kMain.cpp pu610kSubs.c

pu610k_LIBS += devIocStats
pu610k_LIBS += autosave
pu610k_LIBS += stream
pu610k_LIBS += asyn
pu610k_LIBS += pcre
pu610k_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

