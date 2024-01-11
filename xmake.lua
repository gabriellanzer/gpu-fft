add_rules("plugin.vsxmake.autoupdate")
add_rules("plugin.compile_commands.autoupdate")

add_rules("mode.debug", "mode.release")

add_requires("glfw", "glad", "stb")

add_requires("imgui v1.89.9-docking", {configs = {glfw_opengl3 = true}})

target("GPU-FFT-0.0.1")
    set_kind("binary")
	add_packages("glfw", "glad", "imgui", "stb")

	add_headerfiles("src/**.h")
    add_headerfiles("src/**.hpp")

    add_files("src/**.cpp", "src/**.c*")

	set_languages("c++20")

	add_defines("USE_STL_LIB")
	-- add_defines("USE_EASTL_LIB");

	if is_mode("debug") then
		set_targetdir("bin/Debug/")
	elseif is_mode("release") then
		set_targetdir("bin/Release/")
	end
target_end()