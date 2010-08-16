#-------------------------------------------------
# spirit_fcn_test.mak
#-------------------------------------------------

# Paths
STD_INC   = "C:\Program Files\Microsoft Visual Studio 9.0\VC\include"
BOOST_INC = "D:\boost_1_43_0\boost_1_43_0"

LIB_PATH_1 = "C:\Program Files\Microsoft Visual Studio 9.0\VC\lib"
LIB_PATH_2 = "C:\Program Files\Microsoft SDKs\Windows\v6.0a\Lib"
LIB_PATH_3 = "D:\boost_1_43_0\boost_1_43_0\bin.v2\libs"

# Compiler etc
CC     = cl
LINK32 = link

# Options
CXXFLAGS = /c /W3 /O2 /nologo /EHsc /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /I$(STD_INC) /I$(BOOST_INC)
LINK32_FLAGS = /nologo /subsystem:console /incremental:no /machine:I386

fln = spirit_fcn_test

OBJ = $(fln).obj PFunction.obj PFunctionHandler.obj

$(fln): $(OBJ)
	$(LINK32) $(LINK32_FLAGS) /LIBPATH:$(LIB_PATH_1) /LIBPATH:$(LIB_PATH_2) /LIBPATH:$(LIB_PATH_3) $(OBJ) /out:$(fln).exe 

.cpp.obj:
	$(CC) $(CXXFLAGS) $<

clean:
	@del /f *.obj

