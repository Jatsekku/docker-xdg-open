{
  config,
  lib,
  pkgs,
  ...
}:
let
  cfg = config.programs.docker-xdg-open;

  wrappedServer = pkgs.symlinkJoin {
    name = "docker-xdg-open-server";
    paths = [ cfg.package ];
    nativeBuildInputs = [ pkgs.makeWrapper ];
    postBuild = ''
      wrapProgram $out/bin/docker-xdg-open-server \
        --set DOCKER_XDG_OPEN_PORT ${toString cfg.server.port}
    '';
  };
in
{
  options.programs.docker-xdg-open = {
    server = {
      enable = lib.mkEnableOption "docker-xdg-open server";
      port = lib.mkOption {
        type = lib.types.port;
        default = 9000;
        description = "Port on which docker-xdg-open server listens";
      };
    };

    package = lib.mkPackageOption pkgs "docker-xdg-open" { };
  };

  config = lib.mkIf cfg.server.enable {
    environment.systemPackages = [ wrappedServer ];
  };
}
