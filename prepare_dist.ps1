& cargo build --release
cd interface
& npm run build
cd ..

mkdir -Force dist
cp -Force instructions.txt dist/instructions.txt
cp -Force target/release/acoustic_simulator.exe dist/acoustic_simulator.exe
cp -r -Force interface/public dist/interface/public