cmake_minimum_required(VERSION 3.22)

project(bufywave VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------------------------------------------------------
# Pull in JUCE. By default we download it automatically so the project builds
# from a clean clone with no manual setup. If you already have JUCE locally,
# point -DJUCE_PATH=/path/to/JUCE at it to skip the download.
# ----------------------------------------------------------------------------
option(JUCE_PATH "Path to a local JUCE checkout (skips the download)" "")

if(JUCE_PATH)
    add_subdirectory(${JUCE_PATH} ${CMAKE_BINARY_DIR}/JUCE)
else()
    include(FetchContent)
    FetchContent_Declare(
        JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG        8.0.4
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(JUCE)
endif()

# ----------------------------------------------------------------------------
# The plugin target
# ----------------------------------------------------------------------------
juce_add_plugin(bufywave
    COMPANY_NAME                "bufyyy"
    PLUGIN_MANUFACTURER_CODE    Bufy
    PLUGIN_CODE                 Bfyw
    IS_SYNTH                    TRUE
    NEEDS_MIDI_INPUT            TRUE
    NEEDS_MIDI_OUTPUT           FALSE
    IS_MIDI_EFFECT              FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD     FALSE
    FORMATS                     VST3 Standalone
    PRODUCT_NAME                "bufywave")

target_sources(bufywave PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp)

target_compile_definitions(bufywave PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0)

# juce_generate_juce_header lets the sources keep using #include <JuceHeader.h>
juce_generate_juce_header(bufywave)

target_link_libraries(bufywave
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
