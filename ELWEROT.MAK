###############################################################################
#####               This makefile generates ELWEROT.DLL                   #####
###############################################################################
!include $(WGDEV)\src\gcrules.mak

VIRDIR = .\GCVIRDAT

!if $(OS) == WNT
DLLLIB = $(DLLLIB) wgserver.lib galgsbl.lib
MATHDLL =
DEFILE = 
!endif

!if $(OS) == WG2
DLLLIB = $(DLLLIB) wgserver.lib galgsbl.lib $(BORLAND)\lib\mathh.lib ${DOSEXT}\bc4\lib\phapi.lib
MATHDLL = $(DOSEXT)\bc4\lib\fpdmy
DEFILE = $(WGLIBDIR)\mathdef.def
!endif

VIRFILES =          \
     GCVIRDAT\elwerot.vir

OBJFILES =          \
     ${TEMPDIR}\elwerot.obj    \
     ${TEMPDIR}\elwerotr.obj

elwerot.dll:        \
     $(GCBUILD)     \
     $(OBJFILES)    \
     $(DLLLIB)      \
     $(DEFILE)      \
     $(VIRFILES)
     $(LINK) $(LIBCMD) @&&|
$(PRODLL) $(OBJFILES)
$(RUNDIR)\$&
$(RUNDIR)\$&
$(DLLLIB) $(LINKDLL)
$(DEFILE)
|
