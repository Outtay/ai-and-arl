#LIBS=-lsfml-graphics -lsfml-window -lsfml-system
LIBS = 
CXX = clang++
CXXFLAGS = -c -ggdb -O0 -Wall -Wextra
LDFLAGS = -ggdb -O0

EXECUTABLE=iris
BUILDDIR = ./build
SOURCEDIR = ./src

.PHONY: clean

all: dir $(BUILDDIR)/$(EXECUTABLE)

dir: 
	mkdir -p $(BUILDDIR)

SOURCES = $(wildcard $(SOURCEDIR)/*.cpp)
OBJECTS = $(patsubst $(SOURCEDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))

#Would love to understand this a bit better, to make this easier
#this is called static pattern and enables "make foo", which 
#I don't need
$(OBJECTS): $(BUILDDIR)/%.o : $(SOURCEDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(BUILDDIR)/$(EXECUTABLE): $(OBJECTS)
	@echo "** Building the game"
	$(CXX) $^ -o $(BUILDDIR)/$(EXECUTABLE) $(LIBS) $(LDFLAGS)

clean:
	@echo "** Removing object files and executable..."
	rm -f $(BUILDDIR)/*.o $(BUILDDIR)/$(EXECUTABLE)

#install:
#        @echo '** Installing...'
#        cp thegame /usr/bin/
#
#uninstall:
#        @echo '** Uninstalling...'
#        $(RM) /usr/bin/thegame

