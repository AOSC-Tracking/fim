#
# All of this is in balance as an obese elephant on thin ice..
# 

GCC=gcc -O3
GPP=g++ -fpermissive -fno-default-inline -O3
LD=ld
LIBS=-lreadline -lfl -lm
FBI=src/*o  src/*/*o
FBI=src/bmp.o   src/dither.o    src/fb-gui.o  src/fbtools.o  src/fs.o    src/jpegtools.o  src/op.o   src/ppm.o       src/filter.o   src/jpeg.o  src/loader.o     src/png.o  src/tiff.o src/jpeg/transupp.o src/fbi.o

FBILIBS=-lm -ltiff -lcurl -ljpeg -lpng -lgif -lFS -lz
FIM=fim.cpp CommandConsole.cpp Image.cpp Command.cpp Browser.cpp Var.cpp Arg.cpp string.cpp interpreter.cpp common.c
FIMEXTRA=yacc.tab.cpp lex.yy.cc

#The following flags, if set, enable certain optional features.
DEFINES=-D FIM_DEFAULT_CONFIG -D FIM_DEFAULT_KEY_CONFIG #-D FIM_NOFB
# -D FIM_PATCHED

O=-O3

#all:
#	@ #$(GPP) -o fim $(LIBS) fim.cpp interpreter.c yacc.tab.cpp lex.yy.cc
#	$(GPP) -o fim  fim.cpp interpreter.cpp yacc.tab.cpp lex.yy.cc $(LIBS)
	
hack:	clean patch 
	#cd src ;  cc -c fbi-rl.c ; cd ..
	flex -+ lex.lex
	bison -v  -d yacc.ypp
	@ ( cd src ;  ( make clean -i ; make  -i ) 2>&1 > /dev/null ; cd .. ) 
	$(GPP) -o fim $(FBI) $(FBILIBS) $(DEFINES) $(O) $(FIM) $(FIMEXTRA) $(LIBS) 
	@ strip fim

clean:
	@ rm *.o fim -f src/*.o src/*/*.o yacc.output yacc.tab.cpp yacc.tab.hpp lex.yy.cc  src/exiftran

patch:

wc:
	wc $(FIM) 	# missing headers..



test:
	@./fim #~/M*s/*g

edit:
	vim fim.cpp +':split fim.h'

# WARNING 

tgz:	clean
	#rm -f ../fim*.tgz && 
	tar -czf ../fim.`date +%Y%m%d%H%M`.tgz ../fim/*
	ls -l ../fim*.tgz  -v
	@#tar -tzf ../fim*.tgz  -v





report:
	@flex -V  
	@echo '-' ; 
	@bison -V
	@echo '-' ; 
	@gcc -v
	@echo '-' ; 
	@echo 'now please report the bug with this information to the author via email' ; 

exec:	test
	

.PHONY:
	@true
