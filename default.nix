{ pkgs ? import <nixpkgs> { }
, stdenv ? pkgs.stdenv
,
}:
stdenv.mkDerivation rec {
  name = "xca";
  version = "master";

  src = ./.;

  nativeBuildInputs = with pkgs; [
    ccache
    cmake
    gcc
    clang_13
    gdb
    ninja
  ];

  buildInputs = with pkgs; [ gcc qt5.full openssl_1_1 ];

  configurePhase = "cmake -G Ninja -DCMAKE_INSTALL_PREFIX=$out -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .";

  buildPhase = ''
    cmake --build .
  '';
}
