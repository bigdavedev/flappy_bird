set (CPACK_DEBIAN_PACKAGE_SECTION "games")
set (CPACK_PACKAGING_INSTALL_PREFIX "/opt/${PROJECT_NAME}")
set (
	CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
	${PROJECT_BINARY_DIR}/extra/packaging/debian/postinst
	${PROJECT_BINARY_DIR}/extra/packaging/debian/postrm
)

configure_file (
	${PROJECT_SOURCE_DIR}/extra/packaging/debian/template.desktop.in
	${PROJECT_BINARY_DIR}/extra/packaging/debian/FlappyBird-${PROJECT_NAME}.desktop
	@ONLY
)
configure_file (
	${PROJECT_SOURCE_DIR}/extra/packaging/debian/postinst.in
	${PROJECT_BINARY_DIR}/extra/packaging/debian/postinst
	@ONLY
)
configure_file (
	${PROJECT_SOURCE_DIR}/extra/packaging/debian/postrm.in
	${PROJECT_BINARY_DIR}/extra/packaging/debian/postrm
	@ONLY
)

install (
	FILES ${PROJECT_SOURCE_DIR}/extra/packaging/debian/icon.png
	DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps
)
install (
	FILES ${PROJECT_BINARY_DIR}/extra/packaging/debian/FlappyBird-${PROJECT_NAME}.desktop
	DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)
