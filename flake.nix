{
  inputs.nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";

  outputs = {
    self,
    nixpkgs,
  }: let
    supportedSystems = ["x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin"];
    forEachSupportedSystem = f:
      nixpkgs.lib.genAttrs supportedSystems (system:
        f {
          pkgs = import nixpkgs {
            inherit system;
            #   overlays = [self.overlays.default];
          };
        });
  in {
    #    overlays.default = final: prev: {
    #      platformio-core = prev.platformio-core.overridePythonAttrs (oldAttrs: {
    #        version = "6.1.16";
    #      });
    #    };

    devShells = forEachSupportedSystem ({pkgs}: {
      default = pkgs.mkShell {
        packages = with pkgs;
          [
            clang-tools
            cmake
            codespell
            conan
            cppcheck
            doxygen
            gtest
            lcov
            platformio
            vcpkg
            vcpkg-tool

            # node
            node2nix
            nodejs
            nodePackages.pnpm
          ]
          ++ pkgs.lib.optionals (system != "aarch64-darwin") [gdb];

        shellHook = ''
          export PLATFORMIO_CORE_DIR=$PWD/.platformio
        '';
      };
    });
  };
}
