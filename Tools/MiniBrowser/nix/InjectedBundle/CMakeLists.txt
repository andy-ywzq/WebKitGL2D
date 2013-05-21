set(MiniBrowserInjectedBundle_SOURCES
    ${MiniBrowser_DIR}/InjectedBundle/Bundle.cpp
)

set(MiniBrowserInjectedBundle_LIBRARIES
    WebKit2
)

add_library(MiniBrowserInjectedBundle SHARED ${MiniBrowserInjectedBundle_SOURCES})
target_link_libraries(MiniBrowserInjectedBundle ${MiniBrowserInjectedBundle_LIBRARIES})
