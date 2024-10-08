objlist := puzzle utility game random levelgeneration gamedata player actors input
program_title = puzzle
 
CC := gcc
LD := gcc
 
objdir := obj
srcdir := src
objlisto := $(foreach o,$(objlist),$(objdir)/$(o).o)
 
# FL4SHK updated this makefile to work on Linux.  Date of update:  Jun 1, 2016
ifeq ($(OS),Windows_NT)
  CFLAGS := -Wall -O2 -std=gnu99 -ggdb
  LDLIBS := -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
  LDFLAGS := -Wl,-subsystem,windows
else
  CFLAGS := -Wall -O2 -std=gnu99 `sdl2-config -ggdb
  LDLIBS := -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
  #LDFLAGS := -Wl
endif
 
puzzle: $(objlisto)
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)
 
$(objdir)/%.o: $(srcdir)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@
 
.PHONY: clean
 
clean:
	-rm $(objdir)/*.o
