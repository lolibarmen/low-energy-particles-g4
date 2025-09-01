{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
    buildInputs = [
      pkgs.geant4
      pkgs.geant4.data.G4ABLA
      pkgs.geant4.data.G4INCL 
      pkgs.geant4.data.G4PhotonEvaporation
      pkgs.geant4.data.G4RealSurface
      pkgs.geant4.data.G4EMLOW
      pkgs.geant4.data.G4NDL
      pkgs.geant4.data.G4PII
      pkgs.geant4.data.G4SAIDDATA
      pkgs.geant4.data.G4ENSDFSTATE
      pkgs.geant4.data.G4PARTICLEXS
      pkgs.geant4.data.G4RadioactiveDecay
    ];
    shellHook = ''
      export QT_QPA_PLATFORM="xcb"
    '';
}