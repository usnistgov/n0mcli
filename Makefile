PRJ_DIR        = .
PROG	= n0mcli

SRCDIR = src
INCDIR := include

# Set search paths for source files
# Add any subdirs here
#------------------------------------
ADDITIONALSOURCE = 

MKINCLUDES =

BUILD_SPEC     = default

INCLUDES = -I. -Iinclude

# basic compiler flags
CFLAGS = -MD -D__WIN32__

-include $(MKINCLUDES)

SOURCES =  \
		n0mcli.c \
		serio.c \
		c1219.c \
		diag.c \
		lputils.c \
		xmlgen.c \
		
OBJS= $(SOURCES:.c=.o) \
		
OBJ_FILES = $(patsubst %,$(OBJDIR)/%,$(subst :, ,$(OBJS)))
		
INCLUDE_FILES = \
  customtypes.h \
  se240mtr.h \
  psem.h \
  serio.h \
  c1219.h \
  diag.h \
  lputils.h \
  xmlgen.h \
  		
#Directory Search Paths
vpath %.c $(SRCDIR) $(ADDITIONALSOURCE)
vpath %.h $(INCDIR)


ifeq ($(BUILD_SPEC),release)
##	CFLAGS += -static -O2 -g0
	CFLAGS += -O2 -g0
	OBJDIR        = binrelease
else
#	CFLAGS += -static -O0 -g3
	CFLAGS += -O0 -g3
	OBJDIR        = bindebug
endif

AR               = ar
AS               = gcc
CC               = gcc
CPP              = gcc -E
LD               = ld
NM               = nm
STRIP            = strip

LDFLAGS = -lrpcrt4

# Generic Rules
${OBJDIR}/%.o: %.c
	$(CC) -c $(CDEFINES) $(CFLAGS) $(INCLUDES) -o $@ $<

all:	$(PROG) $(INCLUDE_FILES)	


# bin directory 
# creates the bin directory if it doesn't exist
#------------------------------------
$(OBJDIR):
	$(MKDIR) $(OBJDIR) 

-include $(OBJDIR)/*.d

$(PROG): Makefile $(LDSCRIPT) $(OBJ_FILES)
	@echo ======================  Link  ================================	
	$(CC) -o ${OBJDIR}/$(PROG) $(OBJ_FILES) $(LDFLAGS) 


clean:
	rm -rf $(OBJDIR)/*.o $(OBJDIR)/*.d 
	rm -rf $(OBJDIR)/$(PROG)
