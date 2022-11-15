#========================================== variables =========================================
assetsDir       = ./assets/
sourceDir       = ./source/
binDir          = ./bin/
emuFlags		= -keymap de -scale 2 -debug -sdcard $(assetsDir)sdcard.img -run -prg 
compiler		= g++
compilerFlags 	= --output

sourceFiles		= $(sourceDir)tileExtract.cpp $(sourceDir)paletteExtract.cpp 
#
#==============================================================================================


#============================================ make ============================================
.PHONY: all
all: $(binDir)tileExtract $(binDir)paletteExtract

$(binDir)tileExtract: $(sourceFiles)
	$(compiler) $(compilerFlags) $(binDir)tileExtract $(sourceDir)tileExtract.cpp

$(binDir)paletteExtract: $(sourceFiles)
	$(compiler) $(compilerFlags) $(binDir)paletteExtract $(sourceDir)paletteExtract.cpp
#==============================================================================================


#=========================================== testing ==========================================
.PHONY: run
run:
	$(binDir)bmpconv
#==============================================================================================



#============================================ clean ===========================================
.PHONY: clean
clean:
	rm -Rf $(binDir)*.exe
#==============================================================================================



#============================================== git ===========================================
.PHONY: push
push:
	git add *
	git commit -m "commit"
	git push origin main

.PHONY: pull
pull:
	git pull origin main
#==============================================================================================
