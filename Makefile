#---------------------------------------------------------------------------------------------------------------------
# TARGET is the name of the output.
# BUILD is the directory where object files & intermediate files will be placed.
# LIBBUTANO is the main directory of butano library.
#---------------------------------------------------------------------------------------------------------------------
TARGET          :=  $(notdir $(CURDIR))
BUILD           :=  build
LIBBUTANO       :=  ../gba_development/butano/butano
PYTHON          :=  python
SOURCES         :=  src src/blockudoku
INCLUDES        :=  include ../gba_development/butano/common/include
DATA            :=
GRAPHICS        :=  graphics ../gba_development/butano/common/graphics
AUDIO           :=  audio
AUDIOBACKEND    :=  maxmod
AUDIOTOOL       :=
DMGAUDIO        :=  dmg_audio
DMGAUDIOBACKEND :=  default
ROMTITLE        :=  BLOCKUDOKU
ROMCODE         :=  BKDK
USERFLAGS       :=
USERCXXFLAGS    :=
USERASFLAGS     :=
USERLDFLAGS     :=
USERLIBDIRS     :=
USERLIBS        :=
DEFAULTLIBS     :=
STACKTRACE      :=
USERBUILD       :=
EXTTOOL         :=

ifndef LIBBUTANOABS
	export LIBBUTANOABS := $(realpath $(LIBBUTANO))
endif

include $(LIBBUTANOABS)/butano.mak
