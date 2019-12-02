# 根据文件夹名称。确定项目名称
set(Project_Source_Dir ${CMAKE_CURRENT_SOURCE_DIR})
string(FIND ${Project_Source_Dir} "/" sub_source_directory_length) 
# 如果字符串长度不等于-1,进行处理
if(${sub_source_directory_length} EQUAL -1)		
	message(FATAL_ERROR  "sub_source_directory_length: " ${sub_source_directory_length}) 
else()
	while(NOT (${sub_source_directory_length} EQUAL -1))		
		string(SUBSTRING ${Project_Source_Dir} ${sub_source_directory_length} -1 Project_Source_Dir)  
		string(SUBSTRING ${Project_Source_Dir} 1 -1 Project_Source_Dir) 	
		#message(STATUS "    Project_Source_Dir: " ${Project_Source_Dir})			
		string(FIND ${Project_Source_Dir} "/" sub_source_directory_length) 
	endwhile()
endif()  	
string(LENGTH ${Project_Source_Dir} Project_Source_Dir_Length)  
if(${sub_source_directory_length} EQUAL -1 AND ${Project_Source_Dir_Length} GREATER 0)
	project(${Project_Source_Dir})
else()
	message(FATAL_ERROR  "Project_Source_Dir: " ${Project_Source_Dir}) 
endif()

find_package(Qt5Widgets)

# 实现qt中moc功能
set(CMAKE_AUTOMOC ON)
# 实现qt中ui编译功能
set(CMAKE_AUTOUIC ON)

file(GLOB_RECURSE FORMS *.ui)
qt5_wrap_ui(FORMS_UIC ${FORMS})

# 在相应的生成目录中查找头文件
set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB_RECURSE project_headers *.h)
file(GLOB_RECURSE project_cpps *.cpp)
file(GLOB_RECURSE project_txt *.txt)
file(GLOB_RECURSE project_ui *.ui)

set(SRC ${project_headers} ${project_cpps} ${project_txt} ${project_ui})

# 搜索当前文件路径

set(is_static "AnXin_${PROJECT_NAME}_static")
option(${is_static} "Is ${PROJECT_NAME} Static" ON)
if(${is_static})
	ADD_DEFINITIONS(-D${PROJECT_NAME}_LIBRARY_STATIC)
	add_library(${PROJECT_NAME} STATIC ${SRC})
	if (AnXin_display_lib_info)
		MESSAGE (STATUS "${PROJECT_NAME} library is static")
	endif()
else()
	ADD_DEFINITIONS(-D${PROJECT_NAME}_LIBRARY_SHARED)
	add_library(${PROJECT_NAME} SHARED ${SRC})
	if (AnXin_display_lib_info)
		MESSAGE (STATUS "${PROJECT_NAME} library is shared")
	endif()
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
endif()

add_dependencies(${PROJECT_NAME} ots otsServer otsXML)
target_link_libraries( ${PROJECT_NAME}  
						ots
						otsServer
						otsXML
						Qt5::Widgets
						)


#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")

#set (CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} /SAFESEH:NO")

#set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /SAFESEH:NO")
#set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_DEBUG /SAFESEH:NO)

#SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /SAFESEH:NO")
#SET(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL} /SAFESEH:NO")
#SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SAFESEH:NO")

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Ui")