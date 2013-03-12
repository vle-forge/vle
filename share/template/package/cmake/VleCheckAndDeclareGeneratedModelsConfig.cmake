# VleCheckAndDeclareGeneratedModelsConfig.cmake
# =============================================
#
# Check the dependencies of VLE's generated models
# Also Declare and Configure
#
# Copyright (c) 2012 INRA
# Patrick Chabrier <patrick.chabrier@toulouse.inra.fr>
#
# Distributed under the OS-approved BSD License (the "License");
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#
# ===========================================================================
#
# Changelog
# ---------
#
# 1 Initial version.
#
# ===========================================================================
#
# Usage
# -----
#
# CheckAndDeclareGeneratedModels()
#
#
# Example (inside a src/CMakeLists.txt)
# -------------------------------------
#
#SET(INCLUDIR ${CMAKE_SOURCE_DIR}/src ${VLE_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
#
#SET(LINKDIR ${VLE_LIBRARY_DIRS})
#
#link_directories(${LINKDIR})
#
#CheckAndDeclareGeneratedModels()
#
# ===========================================================================

##
## Define a macro to check if an item is present inside a list
##

MACRO(LIST_CONTAINS var value)
  SET(${var})
  FOREACH (value2 ${ARGN})
    IF (${value} STREQUAL ${value2})
      SET(${var} TRUE)
    ENDIF (${value} STREQUAL ${value2})
  ENDFOREACH (value2)
ENDMACRO(LIST_CONTAINS)

##
## Define a macro to check and declare generated models.
##

MACRO(CheckAndDeclareGeneratedModels)

  # the list of C++ files inside the local directory
  FILE(GLOB CPPList "*.cpp")

  FOREACH(CPPFile ${CPPList})

    FILE(READ "${CPPFile}" CPPcontents)

    # Filter the generated files
    STRING(REGEX MATCH "@@tag([^@])+" TAGLine ${CPPcontents})
    IF(NOT ${TAGLine} STREQUAL "")

      # loocking for the plugin name
      STRING(REGEX MATCH " ([^ ])+" EXT ${TAGLine})
      STRING(REGEX MATCH "([^ ])+" cleanEXT ${EXT})

      # name of the file without the path
      GET_FILENAME_COMPONENT(SHORTName ${CPPFile} NAME)

      # name of the file without the extension
      GET_FILENAME_COMPONENT(BASEName ${CPPFile} NAME_WE)

      # DECISION
      IF(${cleanEXT} STREQUAL "Decision")

	VLE_CHECK_PACKAGE(DECISION vle.extension.decision)
	if (NOT DECISION_FOUND)
	  message(SEND_ERROR "Missing vle.extension.decision")
	endif (NOT DECISION_FOUND)

	DeclareDecisionDynamics(${BASEName} ${SHORTName})

	# adding the path if needed
	LIST_CONTAINS(alreadythere ${DECISION_INCLUDE_DIRS} ${INCLUDIR})
	IF (NOT alreadythere)
	  SET(INCLUDIR ${INCLUDIR} ${DECISION_INCLUDE_DIRS})
	ENDIF(NOT alreadythere)

	# DIFFERENCE EQUATION
      ELSEIF(${cleanEXT} STREQUAL "DifferenceEquationMultiple"
	  OR ${cleanEXT} STREQUAL "DifferenceEquationGeneric"
	  OR ${cleanEXT} STREQUAL "DifferenceEquationSimple")

	VLE_CHECK_PACKAGE(DIFFERENCE_EQU vle.extension.difference-equation)
	if (NOT DIFFERENCE_EQU_FOUND)
	  message(SEND_ERROR "Missing vle.extension.difference-equation")
	endif (NOT DIFFERENCE_EQU_FOUND)

	DeclareDifferenceEquationDynamics(${BASEName} ${SHORTName})

	# adding the path if needed
	LIST_CONTAINS(alreadythere ${DIFFERENCE_EQU_INCLUDE_DIRS} ${INCLUDIR})
	IF (NOT alreadythere)
	  SET(INCLUDIR ${INCLUDIR} ${DIFFERENCE_EQU_INCLUDE_DIRS})
	ENDIF(NOT alreadythere)

	# DIFFRERENTIAL
      ELSEIF(${cleanEXT} STREQUAL "Forrester")

	VLE_CHECK_PACKAGE(DIFFERENTIAL_EQU vle.extension.differential-equation)
	if (NOT DIFFERENTIAL_EQU_FOUND)
	  message(SEND_ERROR "Missing vle.extension.differential-equation")
	endif (NOT DIFFERENTIAL_EQU_FOUND)

	DeclareDifferentialEquationDynamics(${BASEName} ${SHORTName})

	# adding the path if needed
	LIST_CONTAINS(alreadythere ${DIFFERENTIAL_EQU_INCLUDE_DIRS} ${INCLUDIR})
	IF (NOT alreadythere)
	  SET(INCLUDIR ${INCLUDIR} ${DIFFERENTIAL_EQU_INCLUDE_DIRS})
	ENDIF(NOT alreadythere)

	# FSA
      ELSEIF(${cleanEXT} STREQUAL "Statechart")

	VLE_CHECK_PACKAGE(FSA vle.extension.fsa)
	if (NOT FSA_FOUND)
	  message(SEND_ERROR "Missing vle.extension.fsa")
	endif (NOT FSA_FOUND)

	DeclarefsaDynamics(${BASEName} ${SHORTName})

	# adding the path if needed
	LIST_CONTAINS(alreadythere ${FSA_INCLUDE_DIRS} ${INCLUDIR})
	IF (NOT alreadythere)
	  SET(INCLUDIR ${INCLUDIR} ${FSA_INCLUDE_DIRS})
	ENDIF(NOT alreadythere)

	# PETRINET
      ELSEIF(${cleanEXT} STREQUAL "Petrinet")

	VLE_CHECK_PACKAGE(PETRINET vle.extension.petrinet)
	if (NOT PETRINET_FOUND)
	  message(SEND_ERROR "Missing vle.extension.petrinet")
	endif (NOT PETRINET_FOUND)

	DeclarePetrinetDynamics(${BASEName} ${SHORTName})

	# adding the path if needed
	LIST_CONTAINS(alreadythere ${PETRINET_INCLUDE_DIRS} ${INCLUDIR})
	IF (NOT alreadythere)
	  SET(INCLUDIR ${INCLUDIR} ${PETRINET_INCLUDE_DIRS})
	ENDIF(NOT alreadythere)

      ENDIF()
    ENDIF(NOT ${TAGLine} STREQUAL "")
  ENDFOREACH(CPPFile)

  include_directories(${INCLUDIR})

endmacro(CheckAndDeclareGeneratedModels)

CheckAndDeclareGeneratedModels()