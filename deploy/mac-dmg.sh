mkdir out

create-dmg \
	--volname "GayTor.rent Upload Utility" \
    --volicon "../src/assets/icon.icns" \
	--icon-size 100 \
	--window-pos 200 120 \
	--window-size 800 400 \
	--icon "GayTor.rent Upload Utility.app" 200 190 \
	--hide-extension "GayTor.rent Upload Utility.app" \
	--app-drop-link 600 185 \
	"out/GUU-Mac-x86_64.dmg" \
	"../build/src/Release"