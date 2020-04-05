CXX ?= g++

CXXFLAGS := -Wall -Werror -std=c++11 -O2

SRCS := main.cpp MultAndDrum.cpp RomToRom.cpp copyDatSize.cpp
	crc32.cpp getDat.cpp importOft.cpp playSample.cpp showDat.cpp
	showSam.cpp showTra.cpp subTabSrh.cpp subTrackInfoGet.cpp
	tabSearch.cpp

HEADERS := main.h MultAndDrum.h RomToRom.h copyDatSize.h
	crc32.h getDat.h importOft.h playSample.h showDat.h
	showSam.h showTra.h subTabSrh.h subTrackInfoGet.h
	tabSearch.h

.PHONY: all clean 

all: DiwaRipper
	@:

DiwaRipper: $(SRCS) $(HEADERS)
		$(CXX) $(CXXFLAGS) $(SRCS) -o $@ $(LDFLAGS)
		
clean:
	$(RM) DiwaRipper DiwaRipper.exe
	