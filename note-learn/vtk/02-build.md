

cmake   ../   -DVTK_GROUP_ENABLE_Qt=YES  -DVTK_MODULE_ENABLE_VTK_GuiSupportQt=YES  -DVTK_MODULE_ENABLE_VTK_ViewQt=YES  

# -DQt5_DIR=/home/shane/Qt/5.15.2/gcc_64/lib/cmake/Qt5



list(APPEND CMAKE_PREFIX_PATH "/opt/eigen-3.4.0") # 安装路径前缀
