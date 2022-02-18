# CMakeBase.cmake functions

## Project layout
* CMakeLists.txt
* src
    + /c          - C source files
    + /cpp        - C++ Source files
    + /public     - Public headers
    + /headers    - Private headers
---
# Main project functions

>## addProject()
>This function sets up a basic executable or shared/static library project, the value of `${PROJECT_NAME}` is used as the name of library or executable.
>In case the project uses a different layout than described above, or in case you want to add the sources to the library or executable by hand, pass the `NO_SOURCES` parameter.
>
>In the case of a shared library, the `withExportHeaders` macro is called, which creates an export header and includes it in the project.
>If this header needs to be in a sub-folder, it can be defined with `EXPORT_HEADER_DIR`
> 
>In the case of a shared library or an executable, the runtime dependencies will be copied to the build folder if the OS is `WIN32` to ensure it can be run straight from the IDE.
>
>If the project is a subproject of a larger project, a target alias of `ROOT_PROJ_NAME::PROJ_NAME` will be added to ensure the dependency names are the same after an installation with exported CMake files.
>
>#### Usage
>>addProject({LIB | LIBRARY} {SHARED | STATIC} [NO_SOURCES] [EXPORT_HEADER_DIR <<g>dir>])
> 
>>addProject(EXECUTABLE [NO_SOURCES])

>## findLibrary(name)
>This function combines many forms of finding dependencies available in CMake. If it is found it will always be added as an imported target.
>
>`name` - Argument which is required. This will be the name used in `find_package` and the main target name if other finding methods are used.
>In case the name of the dependency is different when e.g. `pkg_search_module` is used, the `NAME` parameter can be used to provide an alternate name.
>
>#### Usage
>>findLibrary(<name<name>> \
>> [NAME <altername_name>] \
>> [FIND_HEADER <header_loc>] \
>> [HEADER_DIR <header_subdirectory>] \
>> [COMPONENTS <components>]\
>> [FILE <file_name>]\
>> [REQUIRED]\
>> [QUIET]\
>> )
>
>#### How it works
>The function starts by seeing if the package is available with `find_package`, 
>if this is the case it is also tested for the presence of the components defined in the `COMPONENTS` parameter.
>
>If this fails, and the `FIND_HEADER` parameter is defined, the function will look if the defined header is present.
>If this header is found, the `FOUND_DIR/HEADER_DIR` will be used as the header directory. if `HEADER_DIR` is not defined, `FOUND_DIR` will be used for this.
>After this the corresponding library will be searched, with the `name` parameter as it's base. If the `FILE` parameter is defined, this name will be searched as well.
>If both these actions succeeded a new target will be created and the function will return. The runtime library will also be found if `WIN32`.
>
>If this has also failed an PkgConfig is present `pkg_search_module` will be run on the provided `name`
>If it is found a new target will be created and the function will return.
>
>>### If the compiler is MSVC
>> If the compiler is identified as MSVC, and the target still has not been found, the function will check if the `./libs/${name}` folder exists.
>> If this is the case the `./libs/${name}/include` folder will be used as include dir. The DLL in `./libs/${name}/bin` will be added as runtime libraries and the .lib in `./libs/${name}/lib` will be used to link against
>> 
>> Usage of defining dependencies like this and uploading to e.g. a git is discouraged


> ## useLibraries(name)
> Tells CMake that a project depends on the given target. 
> If you want it to depend on multiple targets with the same parameters, call the function as `useLibraries(NAMES <name1> <name2> ....)` instead of `useLibraries(<name> ....)`
> 
> #### Usage
>>useLibrary({<<t>name> | NAMES <<t>name1> <<t>name2> ...}\
>> {PUBLIC | PRIVATE} \
>> [STATIC_HEADERS] \
>>)
> 
>>useLibrary({<<t>name> | NAMES <<t>name1> <<t>name2> ...}\
>> {PUBLIC | PRIVATE} \
>> [{INSTALL | WIN_INSTALL} <install_location>] \
>> [HEADER_ROOT_DIR <header_root_dir>] \
>> [HEADER_INCLUDE_DIRS <header_dir1> <header_dir2> ...] \
>> [HEADER_INCLUDE_FILES <include_file1> <include_file2> ...] \
>>)
> 
> #### How it works
> I case you just want to depend on the library, the syntax of the first usage is pretty self-explanatory. Insert the name(s) of the target the project depends on and define if they are public or private dependencies.
> If the target is a static library, the dependency is `PUBLIC` and `STATIC_HEADERS` is provided, it's headers will be added to this project's public headers
> 
> In case you also want to install the dependency's runtime, the `INSTALL` or `WIN_INSTAll` parameters must be provided. The difference is that a `INSTALL` will always be installed, and a `WIN_INSTALL` win only be installed if `WIN32` (You will want this one most of the time)
>
> If you also want to install a dependency's dev elements, make sure that besides an `(WIN_)INSTALL` either of the following parameters is defined: `HEADER_ROOT_DIR, HEADER_INCLUDE_DIRS or HEADER_INCLUDE_FILES`
> Since a library's include files are dumped flat in /include, these parameters allow you to filter the headers that are actually installed. \
>- `HEADER_ROOT_DIR` defines a subdirectory where the copying and searching will start \
>- `HEADER_INCLUDE_DIRS` define which directories will be completely copied over (Starting from `${root_include}/HEADER_ROOT_DIR`) \
>- `HEADER_INCLUDE_FILES` define which files will be copied over (Starting from `${root_include}/HEADER_ROOT_DIR`) \
> 
> The link-library will also be found and installed

> ## useSourceLibrary(name)
> Projects sometimes directly depend on sources from another project, and in order to easily update these file you might not want to dump them directly in your main project structure.
> 
> This function allows the user to easily adds source-dependencies. This function will check if the `./libs/${name}` folder exists. If this is the case, the `/libs/${name}/cpp` and `/libs/${name}/c` folders will be added as cpp and c source folders respectively. \
> The `/libs/${name}/public` and `/libs/${name}/include` directories will be added as public include directories, and the `/libs/${name}/headers` folder will be added as a private include directory.
> 
> If the `CLOSED` parameter is not defined, the `/libs/${name}/cpp` and `/libs/${name}/c` will be added as private include directories as well, so any headers in those folders can be properly included aswell.

> ## includeDirectory(folder)
> Wrapper for the `target_include_directories` function
> Define the `PUBLIC` or `PRIVATE` parameter to indicate public or private include directories. \
> Automatically adds the directories to the current project and defines `$<BUILD_INTERFACE:${folder}>` where necessary


> ## getRemoteLibrary(name)
> Fetches a remote repository and build the contained project if needed. If `GIT_TAG` is not provided it will default to `master`
> #### Usage
>>getRemoteLibrary(<<t>name> \
>> GIT_REPO <git_repo_url> \
>> [GIT_TAG <git_tag_or_branch>] \
>> [NO_BUILD] \
>> [BUILD_ARGS <build_arg1> <build_arg2> ...] \
>> [FIND <find_directive>]
>>)
>>
>>getRemoteLibrary(<<t>name> \
>> FILE <zip_location> \
>> [NO_BUILD] \
>> [BUILD_ARGS <build_arg1> <build_arg2> ...] \
>> [FIND <find_directive>]
>>)
> 
> In case a `FILE` is defined, the provided zipped repo will be unzipped and used
> 
> After the remote repo is fetched, the parameters `${name}_SOURCE_DIR` and `${name}_BINARY_DIR` will be populated. \
> If `NO_BUILD` is defined, the function will return immediately.
> If this is not the case, the function will build the target, adding the build parameters defined by `BUILD_ARGS` to the argument list. \
> If `FIND` is defined, the `find_package` command will be run after the build with the `FIND` value as it's parameter(s)

> ## useRemoteSource(remoteLibrary name)
> Adds the sources provided by the remote library defined by `remoteLibrary` as a new source library under the name `name`, in a similar fashion to the `useSourceLibrary(name)` function;
> 
> #### Usage
>> useRemoteSource(<<g>remoteLibrary> <<g>name> \
>> [ROOT <root_dir>] \
>> [PUBLIC_HEADER_DIR <header_dir_prefix>] \
>> [PUBLIC_HEADER_FILES <name_or_regex1> <name_or_regex1> ...] \
>> [PRIVATE_HEADER_FILES <name_or_regex1> <name_or_regex1> ...] \
>> [SOURCE_FILES <name_or_regex1> <name_or_regex1> ...] \
>> [REPLACE "old1|new1" "old2|new2" ...] \
>> [CLOSED] \
>>)
> 
> If `ROOT` is defined it will be used as the base search folder for finding all files
> `PUBLIC_HEADER_DIR` is for giving public header files an offset which wasn't in the original repo. All other files will be checked and have the place to search for these headers altered.
> 
> The `PUBLIC_HEADER_FILES`, `PRIVATE_HEADER_FILES` and `SOURCE_FILES` are for defining which files belong to which type. They are also handled in this order, so if you include 1 header into the public headers, and then you use the regex `*.h` for the private header, the 1 header already used as public will be skipped. \
> This function does not check recursively. Examples for valid entries: `import.h` `*glfw*.*` `project/sources/*.c`
> 
> If the `CLOSED` parameter is not defined, the source directory will be added as a private include directory, so any headers in that folder can be properly included.

---
# Utility functions

>## setCentralOutput()
>Ensures the build targets from all projects end up in the same folders instead of all ending up in their own sub-folders.
>(All targets go to the `/bin/` and `/lib/` folders depending on the target type)

>## forceX64()
>Forces that the project is build in 64-bit. Tries to set flags to force this on MSVC, errors on other compilers if the `sizeof(pointer) != 8`

>## processAssets()
>Called in addProject()
>
>Macro with as optional parameter the installation folder\
>In case of an installation, this function makes sure that all files in src/assets are installed and updated correctly

>## withExportHeaders()
>Called in addProject()
>
>No-arg macro which creates an export header, adds it to the project and makes sure it's installed correctly

>## importSharedLibraries()
>Called in addProject()
> 
>No-arg macro which copies runtime-dependencies of the project to the build folder so it can be run directly from the IDE without first being installed if running on `WIN32` (Checked within the macro)

---
# Python stuff
Sometimes your, or external projects, may use python scripts to configure certain elements. To ensure the python env is correct the following python utility functions are available.

>## findPython()
> This function ensures Python3 is present in the system, and also sets `PYTHON_EXECUTABLE` and `PYTHON3_EXECUTABLE` to the executable.

>## checkPythonModule(description package cmd result_var)
> This function checks if the provided python package is present.
> 
> The `description` is merely used in the logs. \
> The `package` is the name of the package you want to check. \
> The `cmd` the python command that needs to be true in order for this function to be satisfied. \
> The `result_var` is the name of the variable that will be set to either `TRUE` or `FALSE` depending on whether the package is present.
> 
> #### Example
>> ensurePythonModule("mako >= 0.4.2" mako "mako.\_\_version__ >= '0.4.2'")

>## checkPythonModule(description package cmd)
> The parameters are the same as in `checkPythonModule(description package cmd result_var)` \
> This function first checks if the required module is present. If this is not the case it will attempt to install it with pip and then re-check.
> If the package is still missing, the function will error. If the package is found along the way the function will pass.

---
# GTest stuff
Functions for easily adding GTest tests to your project

>## fetchGTest()
> This function downloads and builds GTest, it is recommended that this function is run in the root project so it will be available for all subprojects.
>
>#### Usage
>>fetchGTest(VERSION <gtest_version>)
>
>>fetchGTest(FILE <relative_zip_location>)
> 
> In case a `VERSION` is defined, GTest will be pulled from GitHub. \
> In case a `FILE` is defined, the provided zipped GTest repo will be unzipped and used

>## addTest()
> If `${ENABLE_UNIT_TESTS}` is TRUE (default value) and a `test` folder is present, the sources in the `test` folder will be compiled, linked against GTest and the current project and added as a target.


---
# Install functions
Functions for easily installing your project

>##checkInstallFiles()
> By default, if you install a project, cmake will only update the files that it needs to update. If files are no longer part of the project, cmake won't remove these by default. \
> This function ensures all files that were installed before but are no longer needed will be removed

>## installRuntime()
> Installs all runtime files from this project.
> 
> If `DESTINATION` is provided, the runtime files will be installed in that subdirectory. \
> If `DEV` is provided, this project's public headers and it's link libraries will be installed as well.
> A cmake file for importing this project will also be generated by CMake.

>## makeRootCMakeConfig()
> This function is for the root project. It will create a master-cmake that will be installed and make sure all sub-projects are included.
> 
> If `DESTINATION` is provided, it will be installed the provided folder instead of the `cmake` folder. \
> If `FILES` is provided, the provided cmake files will be installed alongside the master cmake file.
