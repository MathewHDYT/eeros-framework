{
  description = "BehaviourTreeSample dev shell";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };

      btcpp = pkgs.stdenv.mkDerivation {
        pname = "behaviortree_cpp";
        version = "4.7.1";

        src = pkgs.fetchFromGitHub {
          owner = "BehaviorTree";
          repo = "BehaviorTree.CPP";
          rev = "4.7.1";
          sha256 = "sha256-pqqbQphHTgnn6xrltwapY7BoT66wU5E2ri1tqnZ/ieY=";
        };

        nativeBuildInputs = [ pkgs.cmake pkgs.pkg-config ];
        buildInputs = [
          pkgs.zeromq
          pkgs.sqlite
        ];

        cmakeFlags = [
          "-DBUILD_EXAMPLES=OFF"
          "-DBUILD_TESTING=OFF"
          "-DBUILD_GROOT=OFF"
          "-DUSE_CONAN=OFF"
        ];
      };
    in {
      devShells.${system}.default = pkgs.mkShell {
        packages = [
          pkgs.cmake
          pkgs.gcc
          pkgs.gnumake
          btcpp
        ];

        shellHook = ''
          export CMAKE_PREFIX_PATH=${btcpp}/lib/cmake:$CMAKE_PREFIX_PATH
        '';
      };
    };
}
