# ----------------------------------------------------------
# 1️⃣ Configurações Iniciais
# ----------------------------------------------------------
cmake_minimum_required(VERSION 3.30)
project(m VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# ----------------------------------------------------------
# 2️⃣ Módulos Oficiais do Qt6
# ----------------------------------------------------------
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql PrintSupport Network AxContainer)

# ----------------------------------------------------------
# 3️⃣ Adicionar a Biblioteca QtXlsx (Pasta Local)
# ----------------------------------------------------------
# Isso vai ler o CMakeLists.txt que você criou dentro da pasta QtXlsx
#add_subdirectory(QtXlsx)


# ----------------------------------------------------------
# 4️⃣ Arquivos do seu Sistema
# ----------------------------------------------------------
set(PROJECT_SOURCES
    main.cpp
    orcamento_ti.cpp
    orcamento_ti.h
    orcamento_ti.ui
    orcamento_novo.cpp
    orcamento_novo.h
    orcamento_novo.ui
    editar_item.cpp
    editar_item.h
    editar_item.ui

)

# ----------------------------------------------------------
# 5️⃣ Criar o Executável
# ----------------------------------------------------------
qt_add_executable(orcamentoweb
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    dados_do_cliente.h dados_do_cliente.cpp dados_do_cliente.ui
    struct_comum.h
    atividades.h atividades.cpp atividades.ui
    prospect_ti.h prospect_ti.cpp prospect_ti.ui
    prospect_novo.h prospect_novo.cpp prospect_novo.ui
    prospect_editar.h prospect_editar.cpp prospect_editar.ui
    pedidos.h pedidos.cpp pedidos.ui
    pedido_itens.h pedido_itens.cpp pedido_itens.ui
    orcamento_editar.h orcamento_editar.cpp orcamento_editar.ui
)

# ----------------------------------------------------------
# 6️⃣ Linkar Bibliotecas (Onde a mágica acontece)
# ----------------------------------------------------------
target_link_libraries(orcamentoweb PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    Qt6::PrintSupport
    Qt6::Network
    Qt6::AxContainer

)

# Importante para o compilador achar os .h do Excel no seu código
target_include_directories(orcamentoweb PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/QtXlsx/src/xlsx"
)



# ----------------------------------------------------------
# 7️⃣ Configurações de Windows e Deploy
# ----------------------------------------------------------
if(WIN32)
    set_property(TARGET orcamentoweb PROPERTY WIN32_EXECUTABLE TRUE)
    find_package(Qt6 REQUIRED COMPONENTS AxContainer)
        target_link_libraries(orcamentoweb PRIVATE Qt6::AxContainer)
endif()

include(GNUInstallDirs)
install(TARGETS orcamentoweb
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
