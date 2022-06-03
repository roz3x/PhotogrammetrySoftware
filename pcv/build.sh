g++ -c  main.cpp -o main.o -lglfw -lGLEW -lGL -lX11 -lGLU -lOpenGL -limgui
g++ -o bin main.o imgui/backends/imgui_impl_glfw.o imgui/imgui.o
