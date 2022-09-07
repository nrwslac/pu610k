#!$$IOCTOP/bin/linux-x86_64/pu610k

epicsEnvSet("IOCNAME",    "$$IOCNAME" )
epicsEnvSet("ENGINEER",   "$$ENGINEER")
epicsEnvSet("LOCATION",   "$$LOCATION")
epicsEnvSet("IOCSH_PS1",  "$(IOCNAME)> ")
epicsEnvSet("IOC_PV",     "$$IOC_PV")
epicsEnvSet("IOCTOP",     "$$IOCTOP")
epicsEnvSet("STREAM_PROTOCOL_PATH", "$(IOCTOP)/app/srcProtocol")
< envPaths
epicsEnvSet("TOP", "$$TOP")
cd("$(IOCTOP)")

# Run common startup commands for linux soft IOC's
< /reg/d/iocCommon/All/pre_linux.cmd

# Register all support components
dbLoadDatabase("dbd/pu610k.dbd")
pu610k_registerRecordDeviceDriver(pdbbase)

# Set this to enable LOTS of stream module diagnostics
#var streamDebug 1

# Configure each device
$$LOOP(PU610K)
drvAsynIPPortConfigure("PU_$$INDEX","$$IP:$$PORT",0,0,0)
$$IF(DEBUG)
asynSetTraceMask(  "PU_$$INDEX",	0,		9)
asynSetTraceIOMask("PU_$$INDEX",	0,		2)

$$ENDIF(DEBUG)
$$ENDLOOP(PU610K)


# Load record instances
dbLoadRecords("db/iocSoft.db", "IOC=$(IOC_PV)")
dbLoadRecords("db/save_restoreStatus.db", "IOC=$(IOC_PV)")
$$LOOP(PU610K)
dbLoadRecords("db/pu610k.db", "BASE=$$BASE,PORT=PU_$$INDEX,CHAN=CH$$INDEX,T0=$$IF(T0,$$T0,0),T1=$$IF(T1,$$T1,0),NAME=$$NAME")
dbLoadRecords("db/asynRecord.db", "P=$$BASE,R=:ASYN,PORT=PU_$$INDEX,ADDR=0,OMAX=0,IMAX=0")
$$ENDLOOP(PU610K)
dbLoadRecords("db/pu610k_master.db", "BASE=$$BASE,T0=1,CHILL=MEC:EK9K1:BI1")

# Setup autosave
set_savefile_path("$(IOC_DATA)/$(IOC)/autosave")
set_requestfile_path("$(TOP)/autosave")
save_restoreSet_status_prefix("$(IOC_PV):")
save_restoreSet_IncompleteSetsOk(1)
save_restoreSet_DatedBackupFiles(1)

# Just restore the settings
set_pass0_restoreFile("$(IOC).sav")
set_pass1_restoreFile("$(IOC).sav")

# Initialize the IOC and start processing records
iocInit()

# Start autosave backups
create_monitor_set("$(IOC).req", 5, "")

# All IOCs should dump some common info after initial startup.
< /reg/d/iocCommon/All/post_linux.cmd
