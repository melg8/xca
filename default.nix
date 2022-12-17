{ pkgs ? import <nixpkgs> { }
, stdenv ? pkgs.stdenv
}:
pkgs.libsForQt5.mkDerivation rec {
  name = "xca";
  version = "master";

  src = ./.;

  nativeBuildInputs = with pkgs; [
    libsForQt5.qttools
    ccache
    cmake
    gcc12
    clang_14
    gdb
    flawfinder
    ninja
  ];

  buildInputs = with pkgs; [ libtool openssl libsForQt5.qtbase ];
  QT_PLUGIN_PATH = "${pkgs.libsForQt5.qtbase}/${pkgs.libsForQt5.qtbase.qtPluginPrefix}";

  configurePhase = "cmake -G Ninja -DCMAKE_INSTALL_PREFIX=$out -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .";

  buildPhase = ''
    cmake --build .
  '';
}
