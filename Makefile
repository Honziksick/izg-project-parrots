################################################################################
#                                                                              #
# Project:      IZG Parrtos Project - Implementation of own graphics card      #
#                                                                              #
# University:   Faculty of Information Technology, BUT                         #
# Subject:      IZG: Computer Graphics Principles                              #
#                                                                              #
# File:         Makefile                                                       #
# Author:       Jan Kalina <xkalinj00>                                         #
#                                                                              #
# Created:      04.06.2025                                                     #
# Last edit:    04.06.2025                                                     #
#                                                                              #
# Description:  This Makefile is used for compiling the IZG Parrtos project    #
#               which implements a custom graphics card. Besides building      #
#               the project, this Makefile also automates other tasks such     #
#               as generating documentation, cleaning project directories,     #
#               packaging the project for submission, etc. This Makefile is    #
#               inspired by Makefiles created for previous projects at BUT     #
#               FIT (e.g., for the IVS, IFJ and IPK courses).                  #
#                                                                              #
################################################################################

################################################################################
#                                                                              #
#                 BASIC SETTINGS AND DEFINITIONS FOR MAKEFILE                  #
#                                                                              #
################################################################################

###                                   ###
#  Basic configuration of the Makefile  #
###                                   ###

# Project name
EXECUTABLE = izgProject

# Name of the ZIP archive for project submission
PACK_NAME = proj
ARCHIVE_NAME = $(PACK_NAME).zip

# ANSI sequences for colors
COLOR_RESET = \033[0m
COLOR_RED = \033[0;31m
COLOR_GREEN = \033[0;32m
COLOR_BLUE = \033[0;36m
COLOR_YELLOW = \033[0;33m
COLOR_MAGENTA = \033[0;35m


###                                        ###
#  Switches for running the $(MAKE) command  #
###                                        ###

# Run 'make' in silent mode (without event output)
$(VERBOSE)SILENTOPT = -s

# Definition of a constant to disable selected targets (for submission)
#SUBMISSION_MODE ?= true


###                   ###
#  Definition of paths  #
###                   ###

# Directories for placing built files
BUILD_DIR = build

# Student solution files
STUDENT_SOLUTION_FILES = studentSolution/src/studentSolution/gpu.cpp \
                         studentSolution/src/studentSolution/prepareModel.cpp


################################################################################
#                                                                              #
#                                MAIN COMMANDS                                 #
#                                                                              #
################################################################################

# The '.PHONY' command indicates that the following commands are never considered as files
.PHONY: all build clean doc help pack run run-help clean-all clean-build  clean-exec \
        clean-pack install-dev-dep install-help-dep install-test-dep install-doc-dep \
        install-pack-dep update-dep

### MC # all: # Builds the 'fsmtool'
all: build

### MC # build: # Builds the 'fsmtool' via CMake in developer version and Make in submission version
build:
	@mkdir -p build
	@cd build && cmake .. && make -j8

### MC # run: # Runs the executable
run:
	@if [ ! -f "$(EXECUTABLE)" ]; then \
		$(MAKE) build; \
	fi
	./$(EXECUTABLE)

### MC # run-help: # Runs the executable with print help argument
run-help:
	@if [ ! -f "$(EXECUTABLE)" ]; then \
		$(MAKE) build; \
	fi
	./$(EXECUTABLE) -h

# Definition of shortcuts for command categories
CATEGORIES := MC C DEV

### MC # help: # Prints help for using the Makefile
help:
ifndef SUBMISSION_MODE
	@$(MAKE) $(SILENTOPT) install-help-dep
endif
	@{ \
	for CATEGORY in $(CATEGORIES); do \
		case $$CATEGORY in \
		"MC") FULL_CAT="Main Commands";; \
		"C") FULL_CAT="Clean (special)";; \
		"DEV") FULL_CAT="Install Dependencies";; \
		esac; \
		echo "$(COLOR_YELLOW)$$FULL_CAT:$(COLOR_RESET)"; \
		grep -E "^### $$CATEGORY # [a-zA-Z0-9_\-]+:.*?# .*$$" $(MAKEFILE_LIST) | \
		sort -f | \
		awk 'BEGIN {FS = ":.*?# "}; \
		{ \
			gsub(/^### [A-Z]+ # /, "", $$1); \
			split($$2, lines, "\\\\n"); \
			printf "$(COLOR_BLUE)%-30s$(COLOR_RESET) %s\n", $$1, lines[1]; \
			for (i = 2; i <= length(lines); i++) { \
				printf "$(COLOR_BLUE)%-30s$(COLOR_RESET) %s\n", "", lines[i]; \
			} \
		}'; \
		echo ""; \
	done; \
	} | less -R

### MC # clean: # Runs 'clean-all' in developer / submission mode (different versions)
ifndef SUBMISSION_MODE
clean: clean-all
else
clean: clean-build clean-exec
endif

### MC # pack: # Creates a ZIP archive with files intended for submission (not allowed for submission)
ifndef SUBMISSION_MODE
pack:
	zip -j $(ARCHIVE_NAME) $(STUDENT_SOLUTION_FILES)
else
pack:
	@echo "$(COLOR_RED)The 'pack' target is disabled for project submission.$(COLOR_RESET)"
endif


################################################################################
#                                                                              #
#                        SPECIALIZED 'CLEAN' COMMANDS                          #
#                                                                              #
################################################################################

### C # clean-all: # Removes all created files (build, executable, archive, ...)
clean-all: clean-build clean-exec clean-pack

### C # clean-build: # Removes the 'build' directory
clean-build:
	rm -rf $(BUILD_DIR)

### C # clean-exec: # Removes the executable
clean-exec:
	rm -f $(EXECUTABLE)

### C # clean-pack: # Removes the 'pack' directory including the archive (not allowed for submission)
ifndef SUBMISSION_MODE
clean-pack:
	rm -rf $(ARCHIVE_NAME)
else
clean-pack:
	@echo "$(COLOR_RED)The 'clean-pack' target is disabled for project submission.$(COLOR_RESET)"
endif


################################################################################
#                                                                              #
#                    TARGETS FOR INSTALLING NECESSARY TOOLS                    #
#                                                                              #
################################################################################

### DEV # developer-mode: # Switches the Makefile to developer mode
developer-mode:
	@sed -i '0,/SUBMISSION_MODE/ {/^[^#]*SUBMISSION_MODE/ s/^/#/}' Makefile
	@echo "$(COLOR_MAGENTA)The Makefile has been switched to:$(COLOR_RESET) $(COLOR_YELLOW)DEVELOPER MODE$(COLOR_RESET)"

### DEV # submission-mode: # Switches the Makefile to submission mode
submission-mode:
	@sed -i '0,/SUBMISSION_MODE/ {/SUBMISSION_MODE/ s|#||g}' Makefile
	@sed -i '0,/SUBMISSION_MODE/ s|^\s*\(.*SUBMISSION_MODE.*\)$$|\1|' Makefile
	@echo "$(COLOR_MAGENTA)The Makefile has been switched to:$(COLOR_RESET) $(COLOR_YELLOW)SUBMISSION MODE$(COLOR_RESET)"

### DEV # install-dev-dep: # Installs dependencies needed for using all 'Makefile' functions (not allowed for submission)
ifndef SUBMISSION_MODE
install-dev-dep: update-dep install-help-dep install-doc-dep install-pack-dep install-test-dep
else
install-dev-dep:
	@echo "$(COLOR_RED)The 'install-dev-dep' target is disabled for project submission.$(COLOR_RESET)"
endif

### DEV # install-help-dep: # Installs dependencies needed for printing 'Makefile' help - 'less' (not allowed for submission)
ifndef SUBMISSION_MODE
install-help-dep:
	@dpkg -s less >/dev/null 2>&1 || (echo "Installing less" && sudo apt-get install less)
else
install-help-dep:
	@echo "$(COLOR_RED)The 'install-help-dep' target is disabled for project submission.$(COLOR_RESET)"
endif

### DEV # install-doc-dep: # Installs dependencies needed for generating documentation - 'doxygen' (not allowed for submission)
ifndef SUBMISSION_MODE
install-doc-dep:
	@dpkg -s doxygen >/dev/null 2>&1 || (echo "Installing doxygen" && sudo apt-get install doxygen)
else
install-doc-dep:
	@echo "$(COLOR_RED)The 'install-doc-dep' target is disabled for project submission.$(COLOR_RESET)"
endif

### DEV # install-pack-dep: # Installs dependencies needed for project packaging - 'zip' (not allowed for submission)
ifndef SUBMISSION_MODE
install-pack-dep:
	@dpkg -s zip >/dev/null 2>&1 || (echo "Installing zip" && sudo apt-get install zip)
else
install-pack-dep:
	@echo "$(COLOR_RED)The 'install-pack-dep' target is disabled for project submission.$(COLOR_RESET)"
endif

### DEV # install-test-dep: # Installs dependencies needed for project testing - 'cmake' (not allowed for submission)
ifndef SUBMISSION_MODE
install-test-dep:
	@dpkg -s cmake >/dev/null 2>&1 || (echo "Installing cmake" && sudo apt-get install cmake)
else
install-test-dep:
	@echo "$(COLOR_RED)The 'install-pack-dep' target is disabled for project submission.$(COLOR_RESET)"
endif

### DEV # update-dep: # Updates the list of available packages (not allowed for submission)
ifndef SUBMISSION_MODE
update-dep:
	sudo apt-get update -y
else
update-dep:
	@echo "$(COLOR_RED)The 'update-dep' target is disabled for project submission.$(COLOR_RESET)"
endif

### end of file Makefile ###
