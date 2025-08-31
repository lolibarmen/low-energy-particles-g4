{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  nativeBuildInputs = with pkgs; [
    cmake
    clhep
    geant4
  ];

  # Указываем CMake путь к CLHEP
  CLHEP_DIR = "${pkgs.clhep}";

    shellHook = ''
    export CMAKE_PREFIX_PATH=$(nix-build '<nixpkgs>' -A clhep --no-out-link):$CMAKE_PREFIX_PATH
    '';
}