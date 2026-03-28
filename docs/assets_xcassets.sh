# Creates an Xcode asset catalog.

cd ../../system/assets
mkdir -p Assets.xcassets
cd Assets.xcassets

# Read folder names and check if they end with .icon
for folder in ../*/; do
    folder_name=$(basename "$folder")
    if [[ "${folder_name: -5}" == ".icon" ]]; then
        trimmed_name="${folder_name%.icon}"
    fi
done

mkdir -p AccentColor.colorset
mkdir -p $trimmed_name.appiconset

touch Contents.json
echo '{
  "info" : {
    "author" : "xcode",
    "version" : 1
  }
}' > Contents.json

cd AccentColor.colorset
touch Contents.json
echo '{
  "colors" : [
    {
      "color" : {
        "color-space" : "srgb",
        "components" : {
          "alpha" : "1.000",
          "blue" : "255",
          "green" : "255",
          "red" : "255"
        }
      },
      "idiom" : "universal"
    }
  ],
  "info" : {
    "author" : "xcode",
    "version" : 1
  }
}' > Contents.json

cd ../$trimmed_name.appiconset
touch Contents.json
echo '
{
  "images" : [
    {
      "idiom" : "mac",
      "scale" : "1x",
      "size" : "16x16"
    },
    {
      "idiom" : "mac",
      "scale" : "2x",
      "size" : "16x16"
    },
    {
      "idiom" : "mac",
      "scale" : "1x",
      "size" : "32x32"
    },
    {
      "idiom" : "mac",
      "scale" : "2x",
      "size" : "32x32"
    },
    {
      "idiom" : "mac",
      "scale" : "1x",
      "size" : "128x128"
    },
    {
      "idiom" : "mac",
      "scale" : "2x",
      "size" : "128x128"
    },
    {
      "idiom" : "mac",
      "scale" : "1x",
      "size" : "256x256"
    },
    {
      "idiom" : "mac",
      "scale" : "2x",
      "size" : "256x256"
    },
    {
      "idiom" : "mac",
      "scale" : "1x",
      "size" : "512x512"
    },
    {
      "idiom" : "mac",
      "scale" : "2x",
      "size" : "512x512"
    }
  ],
  "info" : {
    "author" : "xcode",
    "version" : 1
  }
}' > Contents.json
