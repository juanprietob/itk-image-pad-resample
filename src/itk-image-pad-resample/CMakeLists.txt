

cmake_minimum_required(VERSION 3.3)
include(ExternalProject)

SET(FLAGS "--bind -Wno-warn-absolute-paths --memory-init-file 0 -s ALLOW_MEMORY_GROWTH=1 -s TOTAL_MEMORY=1.5GB -s MODULARIZE=1 -s EXPORT_NAME=\"'ImgPadResampleModule'\"" )

SET (CMAKE_CXX_FLAGS ${FLAGS} CACHE STRING "compile flags" FORCE)
SET (CMAKE_C_FLAGS ${FLAGS} CACHE STRING "compile flags" FORCE)

set(ITK_IO_MODULES_USED 
		# ITKIONIFTI
		# ITKIONRRD
		# MGHIO
		# ITKIOGDCM
		# ITKIOImageBase
	)
find_package(ITK COMPONENTS
  ITKCommon
  ITKImageFunction
  ITKImageGrid
  ITKIOImageBase
  ${ITK_IO_MODULES_USED}
)
IF(${ITK_FOUND})
	include(${ITK_USE_FILE})
ENDIF(${ITK_FOUND})


INCLUDE_DIRECTORIES($ENV{EMSCRIPTEN}/system/include/emscripten/)

SET(EXE_NAME ImgPadResample)
SET(${EXE_NAME}_LIBRARIES  ${ITK_LIBRARIES})
SET(${EXE_NAME}_SOURCE ImgPadResample.cxx)
add_executable(${EXE_NAME} ${${EXE_NAME}_SOURCE})
target_link_libraries(${EXE_NAME} ${${EXE_NAME}_LIBRARIES})
