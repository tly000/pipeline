cmake_minimum_required(VERSION 3.12)
project(mandelpipeline)

set(CMAKE_CXX_STANDARD 20)

add_executable(mandelpipeline
        src/Actions/BoxedAction.h
        src/Actions/BufferGeneratorAction.h
        src/Actions/FunctionAction.h
        src/Actions/FunctionCallAction.h
        src/Actions/GeneratorAction.h
        src/Actions/ImageGeneratorAction.h
        src/Actions/KernelAction.h
        src/Actions/KernelDefinesAction.h
        src/Actions/KernelGeneratorAction.h
        src/Actions/LazyAction.h
        src/Actions/ParameterAction.h
        src/Actions/SlotAction.h
        src/Kernel/Type/Complex.h
        src/Kernel/Type/CurveSegment.h
        src/Kernel/Type/double.h
        src/Kernel/Type/Fixed16.h
        src/Kernel/Type/Fixed4.h
        src/Kernel/Type/Fixed8.h
        src/Kernel/Type/FixedN.h
        src/Kernel/Type/float.h
        src/Kernel/Type/float128.h
        src/Kernel/Type/longdouble.h
        src/Kernel/Type/Q16_16.h
        src/Kernel/Type/Q32_32.h
        src/Kernel/Type/StandardTypes.h
        src/Kernel/MacroTools.h
        src/Kernel/Multisampling.h
        src/Kernel/Utils.h
        src/MandelPipeline/CalculationActionBase.h
        src/MandelPipeline/ColoringAction.h
        src/MandelPipeline/Enums.h
        src/MandelPipeline/GridbasedCalculationAction.h
        src/MandelPipeline/MandelPipeline.h
        src/MandelPipeline/MarianiSilverAction.h
        src/MandelPipeline/NormalCalculationAction.h
        src/MandelPipeline/ParserAction.cpp
        src/MandelPipeline/ParserAction.h
        src/MandelPipeline/PipelineWrapper.cpp
        src/MandelPipeline/PipelineWrapper.h
        src/MandelPipeline/PositionAction.h
        src/MandelPipeline/ReductionAction.h
        src/MandelPipeline/SuccessiveIterationAction.h
        src/MandelPipeline/SuccessiveRefinementAction.h
        src/Parser/JsonParser.cpp
        src/Parser/JsonParser.h
        src/Parser/Parser.cpp
        src/Parser/Parser.h
        src/Pipeline/AbstractInput.cpp
        src/Pipeline/AbstractInput.h
        src/Pipeline/AbstractOutput.cpp
        src/Pipeline/AbstractOutput.h
        src/Pipeline/AbstractPipelineAction.cpp
        src/Pipeline/AbstractPipelineAction.h
        src/Pipeline/StaticInputOutput.h
        src/Pipeline/StaticPipelineAction.h
        src/Platform/CL/CL.h
        src/Platform/CL/CLBuffer.h
        src/Platform/CL/CLFactory.cpp
        src/Platform/CL/CLFactory.h
        src/Platform/CL/CLImage.h
        src/Platform/CL/CLKernel.h
        src/Platform/CPU/CPUBuffer.h
        src/Platform/CPU/CPUFactory.h
        src/Platform/CPU/CPUImage.h
        src/Platform/CPU/CPUKernel.h
        src/Platform/Buffer.h
        src/Platform/Image.h
        src/Platform/Kernel.h
        src/tests/ExternTemplates.h
        src/tests/ExternTemplates1.cpp
        src/tests/ExternTemplates2.cpp
        src/tests/ExternTemplates3.cpp
        src/tests/fixedconversionTest.cpp
        src/tests/fixedTest.cpp
        src/tests/mapstructTest.cpp
        src/tests/parserTest.cpp
        src/tests/performanceTests.cpp
        src/tests/pipelineTest.cpp
        src/tests/test1.cpp
        src/tests/test2.cpp
        src/Type/Curve.cpp
        src/Type/Curve.h
        src/Type/EnumClass.h
        src/Type/Gradient.cpp
        src/Type/Gradient.h
        src/Type/Range.h
        src/Type/StringConstant.h
        src/Type/StringConstantMap.h
        src/Type/TypeHelper.cpp
        src/Type/TypeHelper.h
        src/Type/Vec.h
        src/UI/CurveEditor.cpp
        src/UI/CurveEditor.h
        src/UI/GtkHeader.h
        src/UI/MandelbrotImageView.cpp
        src/UI/MandelbrotImageView.h
        src/UI/ParameterWidgets.h
        src/UI/PipelineParameterBox.cpp
        src/UI/PipelineParameterBox.h
        src/UI/PipelineUI.cpp
        src/UI/PipelineUI.h
        src/UI/Platform.h
        src/UI/PlatformCL.cpp
        src/UI/PlatformCPU.cpp
        src/UI/UIMain.cpp
        src/UI/UIParameterAction.h
        src/Utility/DynamicLibrary.cpp
        src/Utility/DynamicLibrary.h
        src/Utility/NonCopyable.h
        src/Utility/Timer.cpp
        src/Utility/Timer.h
        src/Utility/Utils.cpp
        src/Utility/Utils.h
        src/Utility/VariadicUtils.h
        src/stb_image_write.c
        src/stb_image_write.h)

if(UNIX OR MINGW)
    find_package(PkgConfig)
    pkg_search_module(GTKMM REQUIRED gtkmm-3.0)
    link_directories(
            ${GTKMM_LIBRARY_DIRS}  )
    target_compile_options(mandelpipeline PUBLIC "-Wno-c++11-narrowing")
else()
    find_path(GTKMM_INCLUDE_DIRS gtkmm/base.h)
    find_library(GTKMM_LIBRARY gtkmm REQUIRED)
    find_library(GLIBMM_LIBRARY glibmm-2.0 REQUIRED)
    find_library(SIGCPP_LIBRARY sigc-2.0 REQUIRED)
    find_library(GDKMM_LIBRARY gdkmm REQUIRED)
    find_library(ATKMM_LIBRARY atkmm REQUIRED)
    find_library(CAIROMM_LIBRARY cairomm-1.0 REQUIRED)
    find_library(CAIRO_LIBRARY cairo REQUIRED)
    set(GTKMM_LIBRARIES ${GTKMM_LIBRARY} ${GLIBMM_LIBRARY} ${SIGCPP_LIBRARY} ${GDKMM_LIBRARY} ${CAIROMM_LIBRARY} ${ATKMM_LIBRARY} ${CAIRO_LIBRARY})

    target_compile_options(mandelpipeline PRIVATE /bigobj)
endif()

message(${GTKMM_LIBRARIES})

find_package(OpenCL REQUIRED)
include_directories(
        ${GTKMM_INCLUDE_DIRS}  )

target_link_libraries(mandelpipeline PUBLIC OpenCL::OpenCL ${GTKMM_LIBRARIES})