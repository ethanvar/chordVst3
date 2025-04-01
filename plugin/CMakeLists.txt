cmake_minimum_required(VERSION 3.22)

project(ChordVst3 VERSION 0.1.0)

juce_add_plugin(${PROJECT_NAME}
    PRODUCT_NAME "chordVst3"
    COMPANY_NAME "ethanvarghese"
    BUNDLE_ID "com.ethanvarghese.chordvst3"
    VERSION ${PROJECT_VERSION}
    PLUGIN_MANUFACTURER_CODE "PLAI"
    PLUGIN_CODE "ETHV"
    FORMATS VST3 Standalone
)


target_sources(${PROJECT_NAME}
    PRIVATE
        source/PluginProcessor.cpp
        source/PluginEditor.cpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        include
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        juce::juce_core
        juce::juce_gui_basics
        juce::juce_audio_processors
        juce::juce_audio_devices
        juce::juce_audio_utils
        juce::juce_audio_formats
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST_CAN_REPLACE_VST2=0
)