# ff-modmenu

Mod menu Free Fire — ARM64, Dobby + KittyMemory.

## Requisitos
- Android Studio Hedgehog+ / NDK 26.1 / JDK 17
- apktool, zipalign, apksigner, python3 no PATH

## Setup
1. Clone KittyMemory no path do CMake:
   git clone https://github.com/MJx0/KittyMemory app/src/main/cpp/external/KittyMemory
2. Keystore de debug:
   keytool -genkey -v -keystore debug.keystore -alias androiddebugkey \
     -storepass android -keypass android -keyalg RSA -keysize 2048 -validity 10000

## Build
   cd app && ./gradlew :assembleRelease

## Offsets
Rode Zygisk-Il2CppDumper no com.dts.freefireth, pegue dump.cs,
preencha OFF_* em Main.cpp. Probe de sanidade rejeita offset invalido
no JNI_OnLoad (sem crash loop).

## Injecao
   bash inject.sh freefire.apk app/build/intermediates/cmake/release/obj/arm64-v8a/libModMenu.so ff_mod.apk

## Limites
- Offsets mudam por patch.
- APK reassinado detectado por integridade.
- Conta secundaria.
