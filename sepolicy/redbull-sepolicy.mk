PRODUCT_PUBLIC_SEPOLICY_DIRS += device/google/redbull/sepolicy/public
PRODUCT_PRIVATE_SEPOLICY_DIRS += device/google/redbull/sepolicy/private

# confirmationui
BOARD_SEPOLICY_DIRS += hardware/google/pixel-sepolicy/confirmationui_hal

# ramdump
BOARD_SEPOLICY_DIRS += hardware/google/pixel-sepolicy/ramdump

# twoshay
BOARD_SEPOLICY_DIRS += hardware/google/pixel-sepolicy/input

# google_battery service
BOARD_SEPOLICY_DIRS += hardware/google/pixel-sepolicy/googlebattery

# vendors
BOARD_SEPOLICY_DIRS += device/google/redbull/sepolicy/vendor/google
BOARD_SEPOLICY_DIRS += device/google/redbull/sepolicy/vendor/qcom/common
BOARD_SEPOLICY_DIRS += device/google/redbull/sepolicy/vendor/qcom/sm7250
BOARD_SEPOLICY_DIRS += device/google/redbull/sepolicy/tracking_denials
BOARD_SEPOLICY_DIRS += device/google/redbull/sepolicy/vendor/st
BOARD_SEPOLICY_DIRS += device/google/redbull/sepolicy/vendor/verizon

# misc_writer
BOARD_VENDOR_SEPOLICY_DIRS += device/google/redbull/sepolicy/vendor/google/misc_writer

# thermal
BOARD_VENDOR_SEPOLICY_DIRS += device/google/redbull/sepolicy/vendor/google/thermal

# twoshay
BOARD_VENDOR_SEPOLICY_DIRS += device/google/redbull/sepolicy/vendor/google/twoshay

# Pixel-wide sepolicy
BOARD_VENDOR_SEPOLICY_DIRS += hardware/google/pixel-sepolicy/powerstats
BOARD_VENDOR_SEPOLICY_DIRS += hardware/google/pixel-sepolicy/ramdump/common

# system_ext
SYSTEM_EXT_PRIVATE_SEPOLICY_DIRS += device/google/redbull/sepolicy/system_ext/private
SYSTEM_EXT_PUBLIC_SEPOLICY_DIRS += device/google/redbull/sepolicy/system_ext/public
