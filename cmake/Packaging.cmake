if(NOT AERO_DESKTOP)
  return()
endif()

set(AERO_PACKAGE_TARGETS)
foreach(_package_target AeroEditor Sandbox ServerSandbox)
  if(TARGET ${_package_target})
    list(APPEND AERO_PACKAGE_TARGETS ${_package_target})
  endif()
endforeach()

if(NOT AERO_PACKAGE_TARGETS)
  return()
endif()

include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "GameEngine")
  set(CPACK_PACKAGE_VENDOR "YourStudio")
  set(CPACK_PACKAGE_VERSION "1.0.0")
  set(CPACK_PACKAGE_VERSION_MAJOR "1")
  set(CPACK_PACKAGE_VERSION_MINOR "0")
  set(CPACK_PACKAGE_VERSION_PATCH "0")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Professional Game Engine")
  set(CPACK_PACKAGE_DESCRIPTION
      "A professional cross-platform game engine with editor, scripting, and audio support."
  )
  set(CPACK_PACKAGE_CONTACT "support@yourwebsite.com")
  set(CPACK_PACKAGE_HOMEPAGE_URL "https://yourwebsite.com")

  install(TARGETS ${AERO_PACKAGE_TARGETS}
          RUNTIME DESTINATION bin
          BUNDLE DESTINATION .)

  if(AERO_ENGINE_ASSETS_ENABLED)
    install(FILES "${AERO_ASSETS_PAK}" DESTINATION bin)
  endif()

  if(EXISTS "${CMAKE_SOURCE_DIR}/imgui.ini")
    install(FILES "${CMAKE_SOURCE_DIR}/imgui.ini" DESTINATION bin)
  endif()

  foreach(doc README.md LICENSE.txt CHANGELOG.md)
    if(EXISTS "${CMAKE_SOURCE_DIR}/${doc}")
      install(FILES "${CMAKE_SOURCE_DIR}/${doc}" DESTINATION bin)
    endif()
  endforeach()

  if(WIN32)
    if(EXISTS "${CMAKE_SOURCE_DIR}/icon.ico")
      set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/icon.ico")
      set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/icon.ico")
      set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/icon.ico")
    endif()

    set(CMAKE_INSTALL_PREFIX
        "C:/Program Files/GameEngine"
        CACHE PATH "Install path" FORCE)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_PACKAGE_NAME "Game Engine")
    set(CPACK_NSIS_DISPLAY_NAME "Game Engine v${CPACK_PACKAGE_VERSION}")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\AeroEditor.exe")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "Game Engine")
    set(CPACK_NSIS_MODIFY_PATH OFF)
    set(CPACK_NSIS_HELP_LINK "https://github.com/yourname/game-engine")
    set(CPACK_NSIS_URL_INFO_ABOUT "https://yourwebsite.com")
    set(CPACK_NSIS_CONTACT "support@yourwebsite.com")
    set(CPACK_NSIS_COMPRESSOR "/SOLID lzma")
    set(CPACK_NSIS_COMPRESSOR_SET_THREADS 3)
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

    set(_nsis_install_commands "")
    set(_nsis_uninstall_commands "")
    set(_nsis_display_exe "")

    foreach(_shortcut_entry IN ITEMS "Aero Editor|AeroEditor" "Sandbox|Sandbox"
                                   "ServerSandbox|ServerSandbox")
      string(REPLACE "|" ";" _shortcut_parts "${_shortcut_entry}")
      list(GET _shortcut_parts 0 _shortcut_label)
      list(GET _shortcut_parts 1 _shortcut_exe)

      if(NOT TARGET ${_shortcut_exe})
        continue()
      endif()
      if(NOT _nsis_display_exe)
        set(_nsis_display_exe "${_shortcut_exe}")
      endif()

      string(
        APPEND
        _nsis_install_commands
        "            CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\${_shortcut_label}.lnk'    '$INSTDIR\\\\bin\\\\${_shortcut_exe}.exe'    '' '$INSTDIR\\\\bin\\\\${_shortcut_exe}.exe'    0\n"
        "            CreateShortCut '$DESKTOP\\\\${_shortcut_label}.lnk'   '$INSTDIR\\\\bin\\\\${_shortcut_exe}.exe'    '' '$INSTDIR\\\\bin\\\\${_shortcut_exe}.exe'    0\n"
      )
      string(
        APPEND
        _nsis_uninstall_commands
        "            Delete '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\${_shortcut_label}.lnk'\n"
        "            Delete '$DESKTOP\\\\${_shortcut_label}.lnk'\n")
    endforeach()

    if(_nsis_display_exe)
      set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\${_nsis_display_exe}.exe")
    endif()

    string(
      APPEND
      _nsis_install_commands
      "            CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Uninstall.lnk'      '$INSTDIR\\\\Uninstall.exe'\n"
      "\n"
      "            WriteRegStr   HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\GameEngine' 'DisplayName'     'Game Engine'\n"
      "            WriteRegStr   HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\GameEngine' 'UninstallString' '$INSTDIR\\\\Uninstall.exe'\n"
      "            WriteRegStr   HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\GameEngine' 'InstallLocation' '$INSTDIR'\n"
      "            WriteRegStr   HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\GameEngine' 'DisplayVersion'  '${CPACK_PACKAGE_VERSION}'\n"
      "            WriteRegStr   HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\GameEngine' 'Publisher'       '${CPACK_PACKAGE_VENDOR}'\n"
      "            WriteRegStr   HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\GameEngine' 'DisplayIcon'     '$INSTDIR\\\\bin\\\\${_nsis_display_exe}.exe'\n"
      "            WriteRegDWORD HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\GameEngine' 'NoModify' 1\n"
      "            WriteRegDWORD HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\GameEngine' 'NoRepair' 1\n"
      "\n"
      "            CreateDirectory '$DOCUMENTS\\\\GameEngine Projects'\n")

    string(
      APPEND
      _nsis_uninstall_commands
      "            Delete '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Uninstall.lnk'\n"
      "\n"
      "            MessageBox MB_YESNO 'Remove sample projects from Documents folder?' IDNO skip_samples\n"
      "            RMDir /r '$DOCUMENTS\\\\GameEngine Projects'\n"
      "            skip_samples:\n"
      "\n"
      "            DeleteRegKey HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\GameEngine'\n"
    )

    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "${_nsis_install_commands}")
    set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "${_nsis_uninstall_commands}")

  elseif(UNIX AND NOT APPLE)

    set(_desktop_files)
    foreach(_desktop_target AeroEditor Sandbox ServerSandbox)
      if(NOT TARGET ${_desktop_target})
        continue()
      endif()

      string(TOLOWER "${_desktop_target}" _desktop_name)
      set(_desktop_in
          "${CMAKE_SOURCE_DIR}/packaging/linux/${_desktop_name}.desktop.in")

      if(EXISTS "${_desktop_in}")
        configure_file("${_desktop_in}"
                       "${CMAKE_BINARY_DIR}/${_desktop_name}.desktop" @ONLY)
        list(APPEND _desktop_files
             "${CMAKE_BINARY_DIR}/${_desktop_name}.desktop")
      elseif(EXISTS "${CMAKE_SOURCE_DIR}/packaging/linux")
        message(
          WARNING "Missing ${_desktop_in} — skipping .desktop installation")
      endif()
    endforeach()

    if(_desktop_files)
      install(FILES ${_desktop_files} DESTINATION share/applications)
    endif()

    if(EXISTS "${CMAKE_SOURCE_DIR}/packaging/linux/gameengine.png")
      install(
        FILES "${CMAKE_SOURCE_DIR}/packaging/linux/gameengine.png"
        DESTINATION share/icons/hicolor/256x256/apps
        RENAME gameengine.png)
    endif()

    set(_cpack_generators "TGZ")
    find_program(DPKG_FOUND dpkg)
    if(DPKG_FOUND)
      list(APPEND _cpack_generators "DEB")
      message(STATUS "dpkg found — will generate .deb package")
    endif()
    find_program(RPMBUILD_FOUND rpmbuild)
    if(RPMBUILD_FOUND)
      list(APPEND _cpack_generators "RPM")
      message(STATUS "rpmbuild found — will generate .rpm package")
    endif()
    set(CPACK_GENERATOR "${_cpack_generators}")
    message(STATUS "Linux CPack generators: ${_cpack_generators}")

    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "YourStudio <support@yourwebsite.com>")
    set(CPACK_DEBIAN_PACKAGE_SECTION "games")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS
        "libgtk-3-0, libgl1, libx11-6, libxrandr2, libxi6, libasound2 | libpulse0"
    )
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION
        "Professional Game Engine\n A cross-platform game engine with editor, Lua scripting,\n Box2D physics, OpenAL audio, and OpenGL rendering."
    )

    set(CPACK_RPM_PACKAGE_LICENSE "Proprietary")
    set(CPACK_RPM_PACKAGE_GROUP "Amusements/Games")
    set(CPACK_RPM_PACKAGE_REQUIRES
        "gtk3, mesa-libGL, libX11, libXrandr, libXi, alsa-lib")
    set(CPACK_RPM_PACKAGE_DESCRIPTION
        "A cross-platform game engine with editor, Lua scripting, Box2D physics, OpenAL audio, and OpenGL rendering."
    )

    set(CPACK_PACKAGE_FILE_NAME
        "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-Linux-x86_64")
    set(CPACK_STRIP_FILES TRUE)

  elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_VOLUME_NAME "Game Engine ${CPACK_PACKAGE_VERSION}")
    set(CPACK_PACKAGE_FILE_NAME
        "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-macOS")
    if(EXISTS "${CMAKE_SOURCE_DIR}/packaging/macos/DS_Store")
      set(CPACK_DMG_DS_STORE "${CMAKE_SOURCE_DIR}/packaging/macos/DS_Store")
    endif()
    if(EXISTS "${CMAKE_SOURCE_DIR}/packaging/macos/background.png")
      set(CPACK_DMG_BACKGROUND_IMAGE
          "${CMAKE_SOURCE_DIR}/packaging/macos/background.png")
    endif()
  endif()

  include(CPack)
