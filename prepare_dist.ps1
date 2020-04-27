# Create directory and copy files for distribution
New-Item dist -Force -ItemType Directory;

# Build legacy stuff
Set-Location legacy
& cargo build --release
Set-Location interface
& npm run build
Set-Location ../..

Copy-Item legacy/target/release/acoustic_simulator.exe dist/acoustic_simulator.exe -Force
Copy-Item legacy/interface/public dist/interface/public -Recurse -Force

# Copy ComputeEngine
Copy-Item ./cmake-build-release/ComputeEngine/*.exe ./dist
Copy-Item ./cmake-build-release/ComputeEngine/*.dll ./dist