include(defaults.pri)

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = libvle libgvle vle gvle

libvle.file = src/libvle.pro
libgvle.file = src/vle/gvle/libgvle.pro

vle.file = src/apps/vle/vle-cli.pro
gvle.file = src/apps/gvle/gvle.pro

libgvle.depends = libvle
vle.depends = libvle
gvle.depends = libvle libgvle

SUBDIRS += pkg_vle_output_console pkg_vle_output_dummy pkg_vle_output_file pkg_vle_output_storage pkg_gvle_output_storage pkg_gvle_output_file

pkg_vle_output_console.file = src/pkgs/vle.output/vle_output_console.pro
pkg_vle_output_dummy.file = src/pkgs/vle.output/vle_output_dummy.pro
pkg_vle_output_file.file = src/pkgs/vle.output/vle_output_file.pro
pkg_vle_output_storage.file = src/pkgs/vle.output/vle_output_storage.pro
pkg_gvle_output_storage.file = src/pkgs/vle.output/gvle/storage/gvle_output_storage.pro
pkg_gvle_output_file.file = src/pkgs/vle.output/gvle/file/gvle_output_file.pro
pkg_vle_output_console.depends = libvle
pkg_vle_output_dummy.depends = libvle
pkg_vle_output_file.depends = libvle
pkg_vle_output_storage.depends = libvle
pkg_gvle_output_file.depends = libvle libgvle
pkg_gvle_output_storage.depends = libvle libgvle

SUBDIRS += pkg_vle_generic_builder

pkg_vle_generic_builder.file = src/pkgs/vle.generic.builder/vle_generic_builder.pro
pkg_vle_generic_builder.depends = libvle

SUBDIRS += pkg_vle_adapative_qss_adder pkg_vle_adapative_qss_constant pkg_vle_adapative_qss_generator pkg_vle_adapative_qss_adaptative-quantifier pkg_vle_adapative_qss_integrator pkg_vle_adapative_qss_mult pkg_vle_adapative_qss_plot

pkg_vle_adapative_qss_adder.file = src/pkgs/vle.adaptative-qss/vle_adaptative_qss_adder.pro
pkg_vle_adapative_qss_constant.file = src/pkgs/vle.adaptative-qss/vle_adaptative_qss_constant.pro
pkg_vle_adapative_qss_generator.file = src/pkgs/vle.adaptative-qss/vle_adaptative_qss_generator.pro
pkg_vle_adapative_qss_adaptative-quantifier.file = src/pkgs/vle.adaptative-qss/vle_adaptative_qss_adaptative-quantifier.pro
pkg_vle_adapative_qss_integrator.file = src/pkgs/vle.adaptative-qss/vle_adaptative_qss_integrator.pro
pkg_vle_adapative_qss_mult.file = src/pkgs/vle.adaptative-qss/vle_adaptative_qss_mult.pro
pkg_vle_adapative_qss_plot.file = src/pkgs/vle.adaptative-qss/vle_adaptative_qss_plot.pro

pkg_vle_adapative_qss_adder.depends = libvle
pkg_vle_adapative_qss_constant.depends = libvle
pkg_vle_adapative_qss_generator.depends = libvle
pkg_vle_adapative_qss_adaptative-quantifier.depends = libvle
pkg_vle_adapative_qss_integrator.depends = libvle
pkg_vle_adapative_qss_mult.depends = libvle
pkg_vle_adapative_qss_plot.depends = libvle

SUBDIRS += pkg_gvle_datecondition pkg_gvle_default_cpp_panel pkg_gvle_default_data_panel pkg_gvle_default_out_panel pkg_gvle_default_simsub_panel pkg_gvle_default_vpz_panel

pkg_gvle_datecondition.file = src/pkgs/gvle.default/DateCondition/gvle_datecondition.pro
pkg_gvle_datecondition.depends = libvle libgvle

pkg_gvle_default_cpp_panel.file = src/pkgs/gvle.default/DefaultCppPanel/gvle_default_cpp_panel.pro
pkg_gvle_default_cpp_panel.depends = libvle libgvle

pkg_gvle_default_data_panel.file = src/pkgs/gvle.default/DefaultDataPanel/gvle_default_data_panel.pro
pkg_gvle_default_data_panel.depends = libvle libgvle

pkg_gvle_default_out_panel.file = src/pkgs/gvle.default/DefaultOutPanel/gvle_default_out_panel.pro
pkg_gvle_default_out_panel.depends = libvle libgvle

pkg_gvle_default_simsub_panel.file = src/pkgs/gvle.default/DefaultSimSubpanel/gvle_default_simsub_panel.pro
pkg_gvle_default_simsub_panel.depends = libvle libgvle

pkg_gvle_default_vpz_panel.file = src/pkgs/gvle.default/DefaultVpzPanel/gvle_default_vpz_panel.pro
pkg_gvle_default_vpz_panel.depends = libvle libgvle

OTHER_FILES += defaults.pri

RESOURCES += \
    src/vle/gvle/gvle.qrc

share_files.path = $$DATADIR/
share_files.files = share/README.md share/COPYING

share_template_files.path = $$DATADIR/
share_template_files.files = share/template/

share_cmake_files.path = $$DATADIR/cmake
share_cmake_files.files = share/cmake/FindGVLE.cmake share/cmake/FindQwt.cmake share/cmake/FindVLEDEPS.cmake share/cmake/VleUtilsConfig.cmake

share_dtd_files.path = $$DATADIR/dtd
share_dtd_files.files = vle-0.5.0.dtd  vle-0.6.0.dtd  vle-0.7.0.dtd  vle-0.8.0.dtd  vle-0.9.0.dtd  vle-1.0.0.dtd  vle-1.1.0.dtd  vle-1.2.0.dtd  vle-1.3.0.dtd  vle-2.0.dtd

INSTALLS += share_files share_template_files share_cmake_files share_dtd_files
