CXX=g++
CFLAGS=`sdl-config --cflags` `pkg-config zziplib libconfuse gl glu lua5.1 bullet assimp --cflags` `freetype-config --cflags` -DGETOPT -Werror -Wall -ggdb -Itools/glm
LIBS=`sdl-config --libs` `pkg-config zziplib libconfuse lua5.1 bullet assimp --libs` `freetype-config --libs` -lGL -lGLU -lGLEW -lSDL_mixer -lSDL_image -lSDL_net -lguichan_sdl -lguichan_opengl -lguichan -L/usr/X11R6/lib -lX11

VERSION := $(shell grep -E --only-matching 'VERSION ".+"' src/rage.h | sed -n 1p | sed "s/VERSION //" | sed 's/"//g')


OBJPATH=build
SRCPATH=src

CPPFILES=$(wildcard \
	$(SRCPATH)/*.cpp \
	$(SRCPATH)/audio/*.cpp \
	$(SRCPATH)/mod/*.cpp \
	$(SRCPATH)/net/*.cpp \
	$(SRCPATH)/render/*.cpp \
	$(SRCPATH)/entity/*.cpp \
	$(SRCPATH)/fx/*.cpp \
	$(SRCPATH)/util/*.cpp \
	$(SRCPATH)/lua/*.cpp \
	$(SRCPATH)/gui/*.cpp \
	$(SRCPATH)/http/*.cpp \
	$(SRCPATH)/weapons/*.cpp \
)

OBJFILES=$(patsubst $(SRCPATH)/%.cpp,$(OBJPATH)/%.o,$(CPPFILES))
OBJMAINS=build/client.o build/server.o

OBJFILES_CLIENT=build/client.o build/linux.o $(filter-out $(OBJMAINS), $(OBJFILES))


default: chaoticrage
.PHONY: all chaoticrage clean
all: chaoticrage


chaoticrage: $(OBJFILES_CLIENT)
	@echo [LINK] $@
	@$(CXX) $(CFLAGS) $(OBJFILES_CLIENT) -o chaoticrage $(LIBS)


install: chaoticrage
	mkdir -p $(DESTPATH)/usr/bin
	install chaoticrage $(DESTPATH)/usr/bin
	
	mkdir -p $(DESTPATH)/usr/share/chaoticrage
	cp -r --no-preserve=ownership data $(DESTPATH)/usr/share/chaoticrage
	cp -r --no-preserve=ownership maps $(DESTPATH)/usr/share/chaoticrage


dist: src data maps
	mkdir -p chaotic-rage-$(VERSION)
	
	cp -r Makefile chaotic-rage-$(VERSION)
	cp -r LICENSE chaotic-rage-$(VERSION)
	cp -r README.md chaotic-rage-$(VERSION)
	
	cp -r src chaotic-rage-$(VERSION)
	cp -r data chaotic-rage-$(VERSION)
	cp -r maps chaotic-rage-$(VERSION)
	
	mkdir -p chaotic-rage-$(VERSION)/tools
	cp -r tools/glm chaotic-rage-$(VERSION)/tools
	
	tar -cvjf chaotic-rage-$(VERSION).tar.bz2 chaotic-rage-$(VERSION)
	rm -rf chaotic-rage-$(VERSION)
	
clean:
	rm -f chaoticrage
	rm -f $(OBJFILES)
	rm -f $(OBJPATH)/linux.o


$(OBJPATH)/%.o: $(SRCPATH)/%.cpp $(SRCPATH)/rage.h Makefile
	@echo [CC] $<
	@mkdir -p `dirname $< | sed "s/src/build/"`
	@$(CXX) $(CFLAGS) -o $@ -c $<
	
$(OBJPATH)/happyhttp.o: $(SRCPATH)/http/happyhttp.cpp $(SRCPATH)/http/happyhttp.h Makefile
	@echo [CC] $<
	@$(CXX) $(CFLAGS) -Wno-error -o $@ -c $< 
	
$(OBJPATH)/linux.o: $(SRCPATH)/platform/linux.cpp $(SRCPATH)/platform/platform.h Makefile
	@echo [CC] $<
	@$(CXX) $(CFLAGS) -o $@ -c $<



ifeq ($(wildcard $(OBJPATH)/),)
$(shell mkdir -p $(OBJPATH))
endif
