################################################
#                                              #
# Makefile generated for Broadway Project      #
#                                              #
################################################
# Project name     : MediaCenter
# Target plateform : Linux 
# Build mode       : Debug 
# Version          : 1.2

#
# last Modif :
# Moved ffmpeg_compiled dir to Extern folder
#


#TOOLCHAIN	:=/usr/
CC		:= gcc-4.9
CXX       	:= g++-4.9

CFLAGS+= -g 
CXXFLAGS+= -g -DTARGET_RASPBERRY_PI
################################################################################################
#
# User sources to build 
#


TargetName = JSCenter

UserSRC = main.cpp \
	Controller.cpp \

INCLUDES+= -I/opt/vc/include/ -I/opt/vc/include/interface/vcos/pthreads/ -I/opt/vc/include/interface/vmcs_host/linux/ 

LDFLAGS+=  -loscpack -lpthread -L/opt/vc/lib -lbcm_host -lbcm2835 
CXXFLAGS+= -std=c++14   -Wall  

# add GX

LDFLAGS+= -L/opt/vc/lib -lGLESv2 -lEGL -ljpeg -lfreetype
INCLUDES+= -I/usr/include/freetype2/

################################################################################################
# AJOUTS OMX
CXXFLAGS+= -D_LARGEFILE64_SOURCE 
CXXFLAGS+= -D_FILE_OFFSET_BITS=64  
CXXFLAGS+= -D__VIDEOCORE4__
CXXFLAGS+= -U_FORTIFY_SOURCE -Wall  

CXXFLAGS+= -fPIC
CXXFLAGS+= -DPIC
CXXFLAGS+= -DTARGET_POSIX
CXXFLAGS+= -D_REENTRANT
CXXFLAGS+= -D__STDC_CONSTANT_MACROS 
CXXFLAGS+= -D__STDC_LIMIT_MACROS 

CXXFLAGS+= -DUSE_OPENMAXIL
CXXFLAGS+= -DTARGET_LINUX 
CXXFLAGS+= -DUSE_EXTERNAL_FFMPEG
CXXFLAGS+= -DHAVE_LIBAVUTIL_OPT_H
CXXFLAGS+= -DHAVE_LIBAVUTIL_MEM_H
CXXFLAGS+= -DHAVE_LIBAVFORMAT_AVFORMAT_H
CXXFLAGS+= -DHAVE_LIBSWRESAMPLE_SWRESAMPLE_H
CXXFLAGS+= -DHAVE_LIBAVUTIL_AVUTIL_H
CXXFLAGS+= -DHAVE_LIBAVFILTER_AVFILTER_H
CXXFLAGS+= -DHAVE_OMXLIB

CXXFLAGS+= -DUSE_EXTERNAL_LIBBCM_HOST

CXXFLAGS+= -DUSE_EXTERNAL_OMX -DOMX -DOMX_SKIP64BIT 

INCLUDES+= -I../Core/GXExtra/OMX/linux

## MODIF
INCLUDES+= -I../Core/GXExtra/OMX/ -I../Extern/ffmpeg_compiled/usr/local/include/

# dbus - a virer 
INCLUDES+= -I /usr/include/dbus-1.0 -I /usr/lib/arm-linux-gnueabihf/dbus-1.0/include

## MODIF
LDFLAGS+= -L../Extern/ffmpeg_compiled/usr/local/lib/

LDFLAGS+= -lrt -ldbus-1 -lavutil -lavcodec -lavformat -lpcre -lswresample
LDFLAGS+= -lopenmaxil

################################################################################################


CXXFLAGS+= -DENABLE_ELEMENT_SELECTOR -DUSE_WEB_SERVER  -DUSE_NETWORK -DUSE_GPIO -DUSE_GRAPHICS -DUSE_JAVA_INTERPRETER -DUSE_GRAPHICS_HELPERS -DUSE_JSON_PARSER
CXXFLAGS+= -DUSE_GRAPHICS_EXTRA -DHAVE_JPEG_LIB
 

# BASE CORE SOURES

SRC+= ../Core/Internal/AbstractController.cpp \
		../Core/Internal/AbstractDelegate.cpp \
		../Core/Internal/Element.cpp \
		../Core/Internal/Object.cpp \
		../Core/Internal/Thread.cpp \
		../Core/Internal/FileSystem.cpp \
		../Core/Internal/Utils.cpp \
		../Core/Data/Variant.cpp \
		../Core/Data/Database.cpp \
		../Core/Data/StringOperations.cpp \
		../Core/Log/Log.cpp \
		../Core/Scheduler/Event.cpp \
		../Core/Scheduler/Dispatch.cpp \
		../Core/Scheduler/Timecode.cpp \
		../Core/Application/ApplicationBase.cpp \
		
# Web Component

SRC+= ../Core/Web/WebServer.cpp \
		../Core/Web/web/mongoose.c \
		
# JSON Parser
SRC+= ../Core/Parsers/JSONParser.cpp \
		../Core/Parsers/cJSON/cJSON.c \	
# Network Component

SRC+= ../Core/Network/NetworkController.cpp \
		
# GPIO Component

SRC+= ../Core/Interfaces/InterfaceController.cpp \
		../Core/Interfaces/InterfaceEvent.cpp \
		../Core/Interfaces/SerialInterface.cpp \
		../Core/Interfaces/CanEvent.cpp \
		../Core/Interfaces/GpioInterface.cpp \
		../Core/Interfaces/Impl/GpioPlateformImpl.cpp \
		../Core/Interfaces/Impl/CanPlateformImpl.cpp \

# GX

SRC+= ../Core/GX/DisplayController.cpp \
		../Core/GX/Impl/DisplayImpl.cpp \
		../Core/GX/GXElement.cpp \
		../Core/GX/GXPath.cpp \
		../Core/GX/GXAnimation.cpp \
		../Core/GX/Animations.cpp \
		../Core/GX/GXImage.cpp \
		../Core/GX/GXScene.cpp \
		../Core/GX/GXAsyncPainter.cpp \
		../Core/GX/Painter.cpp \
		../Core/GX/GXTextRenderer.cpp \
		../Core/GX/GXFont.cpp \
		../Core/Parsers/FontParser.cpp \

# later move to another module

SRC+= ../Core/GXDataType/Curve.cpp \
		../Core/GXDataType/GXColors.cpp \
		../Core/GXDataType/GXGeometry.cpp \
		../Core/GXExtra/GraphicsComponents.cpp \
		../Core/GXExtra/GXVideo.cpp \

# JS

SRC+= ../Core/JSMachine/JSMachine.cpp \
		../Core/JSMachine/JSMachineDelegate.cpp \
		../Core/JSMachine/TinyJS/TinyJS.cpp \
		../Core/JSMachine/TinyJS/TinyJS_Functions.cpp \
		../Core/JSMachine/TinyJS/TinyJS_MathFunctions.cpp \

################################################################################################
# AJOUTS OMX
# use OMX
SRC+= ../Core/GXExtra/OMX/linux/XMemUtils.cpp \
		../Core/GXExtra/OMX/utils/log.cpp \
		../Core/GXExtra/OMX/DynamicDll.cpp \
		../Core/GXExtra/OMX/utils/PCMRemap.cpp \
		../Core/GXExtra/OMX/utils/RegExp.cpp \
		../Core/GXExtra/OMX/OMXSubtitleTagSami.cpp \
		../Core/GXExtra/OMX/OMXOverlayCodecText.cpp \
		../Core/GXExtra/OMX/BitstreamConverter.cpp \
		../Core/GXExtra/OMX/linux/RBP.cpp \
		../Core/GXExtra/OMX/OMXThread.cpp \
		../Core/GXExtra/OMX/OMXReader.cpp \
		../Core/GXExtra/OMX/OMXStreamInfo.cpp \
		../Core/GXExtra/OMX/OMXAudioCodecOMX.cpp \
		../Core/GXExtra/OMX/OMXCore.cpp \
		../Core/GXExtra/OMX/OMXVideo.cpp \
		../Core/GXExtra/OMX/OMXAudio.cpp \
		../Core/GXExtra/OMX/OMXClock.cpp \
		../Core/GXExtra/OMX/File.cpp \
		../Core/GXExtra/OMX/OMXPlayerVideo.cpp \
		../Core/GXExtra/OMX/OMXPlayerAudio.cpp \
		../Core/GXExtra/OMX/OMXPlayerSubtitles.cpp \
		../Core/GXExtra/OMX/SubtitleRenderer.cpp \
		../Core/GXExtra/OMX/Unicode.cpp \
		../Core/GXExtra/OMX/Srt.cpp \
		../Core/GXExtra/OMX/KeyConfig.cpp \
		../Core/GXExtra/OMX/OMXControl.cpp \
		../Core/GXExtra/OMX/Keyboard.cpp \
		../Core/GXExtra/OMX/MainPlayer.cpp \

################################################################################################


		
OBJS+=$(filter %.o,$(SRC:.cpp=.o))

OBJS+=$(filter %.o,$(SRC:.c=.o))

UserOBJS+=$(filter %.o,$(UserSRC:.cpp=.o))


%.o: %.cpp
	@rm -f $@
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@ -Wno-deprecated-declarations

%.o: %.c
	@rm -f $@
	$(CXX) $(CFLAGS) $(INCLUDES) -x c -c $< -o $@ -Wno-deprecated-declarations


all: $(OBJS) $(UserOBJS) 
	$(CXX) $(CXXFLAGS) -o $(TargetName) $(OBJS) $(UserOBJS) $(LDFLAGS)

clean:
	for i in $(OBJS); do (if test -e "$$i"; then ( rm $$i ); fi ); done
	rm $(TargetName)
