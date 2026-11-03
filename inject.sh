#!/usr/bin/env bash
set -euo pipefail
APK="${1:-freefire.apk}"
LIB="${2:-app/build/intermediates/cmake/release/obj/arm64-v8a/libModMenu.so}"
OUT="${3:-ff_mod.apk}"
W="ff_mod_work"
apktool d -f "$APK" -o "$W"
mkdir -p "$W/lib/arm64-v8a"
cp "$LIB" "$W/lib/arm64-v8a/"
APP=$(grep -rl "android/app/Application" "$W/smali" 2>/dev/null | head -n1 || true)
if [ -n "$APP" ]; then
python3 - "$APP" <<'PY'
import sys,re
p=sys.argv[1]; s=open(p).read()
inj='    const-string v0, "ModMenu"\n    invoke-static {v0}, Ljava/lang/System;->loadLibrary(Ljava/lang/String;)V\n'
s=re.sub(r'(\.method public constructor <init>\(\)V\n)', r'\1'+inj, s, count=1)
open(p,'w').write(s); print("injected", p)
PY
else
echo "aviso: Application smali nao achado — injete manualmente"
fi
apktool b "$W" -o "$OUT.unsigned"
zipalign -f 4 "$OUT.unsigned" "$OUT.aligned"
if [ -f debug.keystore ]; then
apksigner sign --ks debug.keystore --ks-pass pass:android --out "$OUT" "$OUT.aligned"
else
echo "aviso: sem debug.keystore. Gere com:"
echo "  keytool -genkey -v -keystore debug.keystore -alias androiddebugkey \\"
echo "    -storepass android -keypass android -keyalg RSA -keysize 2048 -validity 10000"
cp "$OUT.aligned" "$OUT"
fi
rm -f "$OUT.unsigned" "$OUT.aligned"
echo "ok: $OUT"
