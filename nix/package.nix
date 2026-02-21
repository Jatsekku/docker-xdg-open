{
  stdenv,
  cmake,
  glibc,
}:
stdenv.mkDerivation {
  pname = "docker-xdg-open";
  version = "0.1.0";

  # Point to project root
  src = ../.;

  nativeBuildInputs = [
    cmake
    glibc
    glibc.static
  ];

  cmakeFlags = [
    "-DBUILD_STATIC_CLIENT=OFF"
    "-DBUILD_STATIC_SERVER=OFF"
  ];

  installPhase = ''
    cmake --install . --prefix $out --component server
  '';
}
