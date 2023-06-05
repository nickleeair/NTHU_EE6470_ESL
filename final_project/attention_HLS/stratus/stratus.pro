#################################################
# Stratus IDE project file generated;
# Stratus IDE project file format version: 2
#################################################
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   -= qt

STRATUSHOME = $$(STRATUS_HOME)
SYSTEMCHOME = $$(STRATUS_HOME)/tools.lnx86/stratus/systemc/2.3.3

TEMPLATE = app
INCLUDEPATH += $${SYSTEMCHOME}/include
INCLUDEPATH += $${SYSTEMCHOME}/include/tlm
INCLUDEPATH += $${STRATUSHOME}/share/stratus/include
INCLUDEPATH += ./bdw_work/wrappers
INCLUDEPATH += ./

SOURCES += \ 
		../main.cpp \ 
		../Testbench.cpp \ 
		../System.cpp \ 
		../softmax.cpp \ 

HEADERS += \ 
		../Testbench.h \ 
		../System.h \ 
		../softmax.h \ 

OTHER_FILES += \ 
		Makefile \ 
		project.tcl \ 

