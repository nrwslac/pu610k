#!/reg/g/pcds/epics-dev/nrw/iocs/pu610k/bin/linux-x86_64/pu610k

epicsEnvSet("IOCNAME",    "ioc-mec-pu610k" )
epicsEnvSet("ENGINEER",   "Michael Browne (mcbrowne)")
epicsEnvSet("LOCATION",   "MEC Mezzanine")
epicsEnvSet("IOCSH_PS1",  "$(IOCNAME)> ")
epicsEnvSet("IOC_PV",     "IOC:MEC:PFN")
epicsEnvSet("IOCTOP",     "/reg/g/pcds/epics-dev/nrw/iocs/pu610k")
epicsEnvSet("STREAM_PROTOCOL_PATH", "$(IOCTOP)/app/srcProtocol")
< envPaths
epicsEnvSet("TOP", "/reg/g/pcds/epics-dev/nrw/iocs/pu610k/children/build")
cd("$(IOCTOP)")

# Run common startup commands for linux soft IOC's
< /reg/d/iocCommon/All/pre_linux.cmd

# Register all support components
dbLoadDatabase("dbd/pu610k.dbd")
pu610k_registerRecordDeviceDriver(pdbbase)

# Set this to enable LOTS of stream module diagnostics
#var streamDebug 1

# Configure each device
drvAsynIPPortConfigure("PU_0","moxa-mec-01:4005",0,0,0)
drvAsynIPPortConfigure("PU_1","moxa-mec-01:4011",0,0,0)
drvAsynIPPortConfigure("PU_2","moxa-mec-01:4002",0,0,0)
drvAsynIPPortConfigure("PU_3","moxa-mec-01:4003",0,0,0)
drvAsynIPPortConfigure("PU_4","moxa-mec-01:4004",0,0,0)
drvAsynIPPortConfigure("PU_5","moxa-mec-01:4006",0,0,0)
drvAsynIPPortConfigure("PU_6","moxa-mec-01:4007",0,0,0)
drvAsynIPPortConfigure("PU_7","moxa-mec-01:4008",0,0,0)
drvAsynIPPortConfigure("PU_8","moxa-mec-01:4009",0,0,0)


# Load record instances
dbLoadRecords("db/iocSoft.db", "IOC=$(IOC_PV)")
dbLoadRecords("db/save_restoreStatus.db", "IOC=$(IOC_PV)")
dbLoadRecords("db/pu610k.db", "BASE=MEC:PFN,PORT=PU_0,CHAN=CH0,T0=0,T1=0,NAME=25mm Amps CD")
dbLoadRecords("db/asynRecord.db", "P=MEC:PFN,R=:ASYN,PORT=PU_0,ADDR=0,OMAX=0,IMAX=0")
dbLoadRecords("db/pu610k.db", "BASE=MEC:PFN,PORT=PU_1,CHAN=CH1,T0=0,T1=0,NAME=50mm Amps A")
dbLoadRecords("db/asynRecord.db", "P=MEC:PFN,R=:ASYN,PORT=PU_1,ADDR=0,OMAX=0,IMAX=0")
dbLoadRecords("db/pu610k.db", "BASE=MEC:PFN,PORT=PU_2,CHAN=CH2,T0=0,T1=0,NAME=50mm Amps B")
dbLoadRecords("db/asynRecord.db", "P=MEC:PFN,R=:ASYN,PORT=PU_2,ADDR=0,OMAX=0,IMAX=0")
dbLoadRecords("db/pu610k.db", "BASE=MEC:PFN,PORT=PU_3,CHAN=CH3,T0=0,T1=0,NAME=50mm Amps E")
dbLoadRecords("db/asynRecord.db", "P=MEC:PFN,R=:ASYN,PORT=PU_3,ADDR=0,OMAX=0,IMAX=0")
dbLoadRecords("db/pu610k.db", "BASE=MEC:PFN,PORT=PU_4,CHAN=CH4,T0=0,T1=0,NAME=50mm Amps F")
dbLoadRecords("db/asynRecord.db", "P=MEC:PFN,R=:ASYN,PORT=PU_4,ADDR=0,OMAX=0,IMAX=0")
dbLoadRecords("db/pu610k.db", "BASE=MEC:PFN,PORT=PU_5,CHAN=CH5,T0=0,T1=0,NAME=50mm Amps G")
dbLoadRecords("db/asynRecord.db", "P=MEC:PFN,R=:ASYN,PORT=PU_5,ADDR=0,OMAX=0,IMAX=0")
dbLoadRecords("db/pu610k.db", "BASE=MEC:PFN,PORT=PU_6,CHAN=CH6,T0=0,T1=0,NAME=50mm Amps H")
dbLoadRecords("db/asynRecord.db", "P=MEC:PFN,R=:ASYN,PORT=PU_6,ADDR=0,OMAX=0,IMAX=0")
dbLoadRecords("db/pu610k.db", "BASE=MEC:PFN,PORT=PU_7,CHAN=CH7,T0=0,T1=0,NAME=50mm Amps I")
dbLoadRecords("db/asynRecord.db", "P=MEC:PFN,R=:ASYN,PORT=PU_7,ADDR=0,OMAX=0,IMAX=0")
dbLoadRecords("db/pu610k.db", "BASE=MEC:PFN,PORT=PU_8,CHAN=CH8,T0=0,T1=0,NAME=50mm Amps J")
dbLoadRecords("db/asynRecord.db", "P=MEC:PFN,R=:ASYN,PORT=PU_8,ADDR=0,OMAX=0,IMAX=0")
dbLoadRecords("db/pu610k_master.db", "BASE=MEC:PFN,T0=1,CHILL=MEC:EK9K1:BI1")

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
